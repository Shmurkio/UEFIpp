#include <UEFIpp/IO/Terminal/Terminal.hpp>
#include <UEFIpp/IO/Text/Writer.hpp>

namespace UEFIpp::IO {
namespace {
inline constexpr Foundation::Uint32 TimerEvent = 0x80000000;
inline constexpr UEFI::Tpl ApplicationTpl = 4;

[[nodiscard]] constexpr auto MapScanCode(Protocols::ScanCodeValue Value)
    -> KeyCode {
  using S = Protocols::ScanCode;
  switch (Value) {
  case S::Null: return KeyCode::None;
  case S::Up: return KeyCode::Up;
  case S::Down: return KeyCode::Down;
  case S::Right: return KeyCode::Right;
  case S::Left: return KeyCode::Left;
  case S::Home: return KeyCode::Home;
  case S::End: return KeyCode::End;
  case S::Insert: return KeyCode::Insert;
  case S::Delete: return KeyCode::Delete;
  case S::PageUp: return KeyCode::PageUp;
  case S::PageDown: return KeyCode::PageDown;
  case S::Function1: return KeyCode::Function1;
  case S::Function2: return KeyCode::Function2;
  case S::Function3: return KeyCode::Function3;
  case S::Function4: return KeyCode::Function4;
  case S::Function5: return KeyCode::Function5;
  case S::Function6: return KeyCode::Function6;
  case S::Function7: return KeyCode::Function7;
  case S::Function8: return KeyCode::Function8;
  case S::Function9: return KeyCode::Function9;
  case S::Function10: return KeyCode::Function10;
  case S::Esc: return KeyCode::Escape;
  default: return KeyCode::Unknown;
  }
}
} // namespace

auto Terminal::Reset(UEFI::Table::BootServices *BootServices,
                     Protocols::SimpleTextInputEx *InputEx,
                     Protocols::SimpleTextInput *Input, WriterRef Output,
                     ConsoleSink *Console) -> Foundation::Void {
  BootServices_ = BootServices;
  InputEx_ = InputEx;
  Input_ = Input;
  Output_ = Output;
  Console_ = Console;
  PendingHighSurrogate_ = 0;
  QueuedEvent_.Reset();
}

auto Terminal::PollKey() -> Result<KeyEvent> {
  if (!Valid()) {
    return Failure(Error::Semantic(ErrorCode::Closed, Operation::Read));
  }
  if (QueuedEvent_) {
    auto Event = QueuedEvent_.Value();
    QueuedEvent_.Reset();
    OnKey.Emit(Event);
    return Event;
  }
  Protocols::KeyData Native{};
  const auto Status = InputEx_
                          ? InputEx_->ReadKey(Native)
                          : Input_->ReadKeyStroke(Input_, &Native.Key);
  if (Status == UEFI::ToStatusValue(UEFI::StatusCode::NotReady)) {
    return Failure(Error::Semantic(ErrorCode::WouldBlock, Operation::Read));
  }
  if (UEFI::Status{Status}.Failed()) {
    return Failure(Error::FromStatus(Operation::Read, Status));
  }
  auto Event = Convert(Native);
  if (!Event) return Failure(Event.Error());
  OnKey.Emit(Event.Value());
  return Event;
}

auto Terminal::WaitKey(WaitOptions Options) -> Result<KeyEvent> {
  if (!Valid()) {
    return Failure(Error::Semantic(ErrorCode::Closed, Operation::Wait));
  }
  if (QueuedEvent_) return PollKey();

  UEFI::Event DeadlineTimer{};
  UEFI::Event CancellationTimer{};
  UEFI::Event Events[3]{InputEx_ ? InputEx_->WaitForKeyEx
                                 : Input_->WaitForKey};
  Foundation::Size Count{1};

  const auto CloseTimers = [&]() {
    if (DeadlineTimer) (void)BootServices_->CloseEvent(DeadlineTimer);
    if (CancellationTimer) (void)BootServices_->CloseEvent(CancellationTimer);
  };

  if (Options.Timeout100ns) {
    auto Status = BootServices_->CreateEvent(TimerEvent, ApplicationTpl, nullptr,
                                             nullptr, &DeadlineTimer);
    if (UEFI::IsError(Status)) {
      return Failure(Error::FromStatus(Operation::Wait, Status));
    }
    Status = BootServices_->SetTimer(DeadlineTimer,
                                     UEFI::Table::TimerDelay::Relative,
                                     Options.Timeout100ns.Value());
    if (UEFI::IsError(Status)) {
      CloseTimers();
      return Failure(Error::FromStatus(Operation::Wait, Status));
    }
    Events[Count++] = DeadlineTimer;
  }
  if (Options.Cancellation) {
    auto Status = BootServices_->CreateEvent(TimerEvent, ApplicationTpl, nullptr,
                                             nullptr, &CancellationTimer);
    if (UEFI::IsError(Status)) {
      CloseTimers();
      return Failure(Error::FromStatus(Operation::Wait, Status));
    }
    Status = BootServices_->SetTimer(CancellationTimer,
                                     UEFI::Table::TimerDelay::Periodic, 100000);
    if (UEFI::IsError(Status)) {
      CloseTimers();
      return Failure(Error::FromStatus(Operation::Wait, Status));
    }
    Events[Count++] = CancellationTimer;
  }

  for (;;) {
    if (Options.Cancellation && Options.Cancellation->Cancelled()) {
      CloseTimers();
      return Failure(Error::Semantic(ErrorCode::Cancelled, Operation::Wait));
    }
    Foundation::UintN Selected{};
    const auto Status = BootServices_->WaitForEvent(Count, Events, &Selected);
    if (UEFI::IsError(Status)) {
      CloseTimers();
      return Failure(Error::FromStatus(Operation::Wait, Status));
    }
    if (DeadlineTimer && Events[Selected] == DeadlineTimer) {
      CloseTimers();
      return Failure(Error::Semantic(ErrorCode::Timeout, Operation::Wait));
    }
    if (CancellationTimer && Events[Selected] == CancellationTimer) {
      continue;
    }
    auto Event = PollKey();
    if (Event || Event.Error().Code != ErrorCode::WouldBlock) {
      CloseTimers();
      return Event;
    }
  }
}

auto Terminal::WriteSome(Library::Span<const Foundation::Byte> Bytes)
    -> Result<Foundation::Size> {
  return Output_.WriteSome(Bytes);
}

auto Terminal::Write(Library::U8StringView Text) -> Result<> {
  return WriteText(Output_, Text);
}

auto Terminal::Flush() -> Result<> { return Output_.Flush(); }

auto Terminal::Clear() -> Result<> {
  auto Result = Flush();
  if (!Result) return Result;
  if (!Console_)
    return Failure(Error::Semantic(ErrorCode::Unsupported, Operation::Write));
  return Console_->ClearTerminal();
}

auto Terminal::SetStyle(ConsoleStyle Style) -> Result<> {
  auto Result = Flush();
  if (!Result) return Result;
  if (!Console_)
    return Failure(Error::Semantic(ErrorCode::Unsupported, Operation::Write));
  return Console_->SetStyle(Style);
}

auto Terminal::ResetStyle() -> Result<> {
  return SetStyle({ConsoleColor::LightGray, ConsoleColor::Black});
}

auto Terminal::SetCursorVisible(Foundation::Bool Visible) -> Result<> {
  auto Result = Flush();
  if (!Result) return Result;
  if (!Console_)
    return Failure(Error::Semantic(ErrorCode::Unsupported, Operation::Write));
  return Console_->SetCursorVisible(Visible);
}

auto Terminal::Geometry() const -> Result<ConsoleGeometry> {
  if (!Console_) {
    return Failure(
        Error::Semantic(ErrorCode::Unsupported, Operation::Read));
  }
  return Console_->Geometry();
}

auto Terminal::SetCursor(Foundation::UintN Column, Foundation::UintN Row)
    -> Result<> {
  auto Result = Flush();
  if (!Result) return Result;
  if (!Console_) {
    return Failure(
        Error::Semantic(ErrorCode::Unsupported, Operation::Write));
  }
  return Console_->SetCursor(Column, Row);
}

auto Terminal::KeyAwaitable::await_suspend(std::coroutine_handle<> Coroutine)
    -> Foundation::Bool {
  if (!Owner_ || !Owner_->Valid()) {
    Registration_ = Failure(
        Error::Semantic(ErrorCode::Closed, Operation::Wait));
    return false;
  }
  Registration_ = Loop_->Register(Owner_->InputEx_
                                      ? Owner_->InputEx_->WaitForKeyEx
                                      : Owner_->Input_->WaitForKey,
                                  Coroutine);
  return Registration_.HasValue();
}

auto Terminal::KeyAwaitable::await_resume() -> Result<KeyEvent> {
  if (!Registration_) return Failure(Registration_.Error());
  return Owner_->PollKey();
}

auto Terminal::Convert(const Protocols::KeyData &Native) -> Result<KeyEvent> {
  Foundation::Char32 Character{};
  const auto Code = MapScanCode(Native.Key.ScanCode);
  const KeyModifiers Modifiers{Native.KeyState.KeyShiftState,
                               Native.KeyState.KeyToggleState};
  const auto Unit =
      Foundation::Cast::Auto<Foundation::Char16>(Native.Key.UnicodeChar);
  if (Unit >= 0xD800 && Unit <= 0xDBFF) {
    if (PendingHighSurrogate_) Character = Utf8Decoder::ReplacementCharacter;
    PendingHighSurrogate_ = Unit;
  } else if (Unit >= 0xDC00 && Unit <= 0xDFFF) {
    if (!PendingHighSurrogate_) {
      Character = Utf8Decoder::ReplacementCharacter;
    } else {
      Character = 0x10000 +
                  ((PendingHighSurrogate_ - 0xD800) << 10) +
                  (Unit - 0xDC00);
      PendingHighSurrogate_ = 0;
    }
  } else {
    if (PendingHighSurrogate_) {
      PendingHighSurrogate_ = 0;
      QueuedEvent_.Emplace(KeyEvent{Code, Unit, Modifiers, Native});
      return KeyEvent{KeyCode::None, Utf8Decoder::ReplacementCharacter,
                      {}, {}};
    } else {
      Character = Unit;
    }
  }
  return KeyEvent{Code, Character, Modifiers, Native};
}
} // namespace UEFIpp::IO
