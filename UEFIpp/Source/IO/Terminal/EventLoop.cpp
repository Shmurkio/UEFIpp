#include <UEFIpp/IO/Terminal/EventLoop.hpp>

namespace UEFIpp::IO {
namespace {
inline constexpr Foundation::Uint32 TimerEvent = 0x80000000;
inline constexpr UEFI::Tpl ApplicationTpl = 4;
} // namespace

auto EventLoop::Register(UEFI::Event Event,
                         std::coroutine_handle<> Coroutine) -> Result<> {
  if (!BootServices_ || !Event || !Coroutine) {
    return Failure(
        Error::Semantic(ErrorCode::InvalidArgument, Operation::Wait));
  }
  if (Count_ == MaximumRegistrations) {
    return Failure(Error::Semantic(ErrorCode::Overflow, Operation::Wait));
  }
  Registrations_[Count_++] = {Event, Coroutine};
  return {};
}

auto EventLoop::RunOnce(WaitOptions Options) -> Result<> {
  if (!BootServices_ || !Count_) {
    return Failure(
        Error::Semantic(ErrorCode::InvalidArgument, Operation::Wait));
  }
  if (Options.Cancellation && Options.Cancellation->Cancelled()) {
    return Failure(Error::Semantic(ErrorCode::Cancelled, Operation::Wait));
  }

  Library::Array<UEFI::Event, MaximumRegistrations + 2> Events{};
  for (Foundation::Size Index{}; Index < Count_; ++Index) {
    Events[Index] = Registrations_[Index].Event;
  }

  UEFI::Event Timer{};
  UEFI::Event CancellationTimer{};
  auto EventCount = Count_;
  if (Options.Timeout100ns) {
    auto Status = BootServices_->CreateEvent(TimerEvent, ApplicationTpl, nullptr,
                                             nullptr, &Timer);
    if (UEFI::IsError(Status)) {
      return Failure(Error::FromStatus(Operation::Wait, Status));
    }
    Status = BootServices_->SetTimer(Timer, UEFI::Table::TimerDelay::Relative,
                                     Options.Timeout100ns.Value());
    if (UEFI::IsError(Status)) {
      (void)BootServices_->CloseEvent(Timer);
      return Failure(Error::FromStatus(Operation::Wait, Status));
    }
    Events[EventCount++] = Timer;
  }

  if (Options.Cancellation) {
    auto Status = BootServices_->CreateEvent(TimerEvent, ApplicationTpl, nullptr,
                                             nullptr, &CancellationTimer);
    if (UEFI::IsError(Status)) {
      if (Timer) (void)BootServices_->CloseEvent(Timer);
      return Failure(Error::FromStatus(Operation::Wait, Status));
    }
    Status = BootServices_->SetTimer(CancellationTimer,
                                     UEFI::Table::TimerDelay::Periodic,
                                     100000);
    if (UEFI::IsError(Status)) {
      (void)BootServices_->CloseEvent(CancellationTimer);
      if (Timer) (void)BootServices_->CloseEvent(Timer);
      return Failure(Error::FromStatus(Operation::Wait, Status));
    }
    Events[EventCount++] = CancellationTimer;
  }

  Foundation::UintN Selected{};
  UEFI::StatusCode Status{};
  for (;;) {
    Status = BootServices_->WaitForEvent(EventCount, Events.Data(), &Selected);
    if (UEFI::IsError(Status) ||
        !CancellationTimer || Events[Selected] != CancellationTimer ||
        Options.Cancellation->Cancelled()) {
      break;
    }
  }
  if (Timer) {
    (void)BootServices_->CloseEvent(Timer);
  }
  if (CancellationTimer) {
    (void)BootServices_->CloseEvent(CancellationTimer);
  }
  if (UEFI::IsError(Status)) {
    return Failure(Error::FromStatus(Operation::Wait, Status));
  }
  if (Timer && Events[Selected] == Timer) {
    return Failure(Error::Semantic(ErrorCode::Timeout, Operation::Wait));
  }
  if (Options.Cancellation && Options.Cancellation->Cancelled()) {
    return Failure(Error::Semantic(ErrorCode::Cancelled, Operation::Wait));
  }

  auto Coroutine = Registrations_[Selected].Coroutine;
  for (Foundation::Size Index = Selected + 1; Index < Count_; ++Index) {
    Registrations_[Index - 1] = Registrations_[Index];
  }
  --Count_;
  Coroutine.resume();
  return {};
}
} // namespace UEFIpp::IO
