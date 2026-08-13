#include <UEFIpp/Foundation/SpinLock.hpp>

namespace UEFIpp::Foundation {
auto SpinLock::TryLock() noexcept -> Bool {
  return State_.Exchange(1, MemoryOrder::Acquire) == 0;
}

auto SpinLock::Lock() noexcept -> Void {
  while (!TryLock()) {
    while (State_.Load(MemoryOrder::Relaxed) != 0) {
      _mm_pause();
    }
  }
}

auto SpinLock::Unlock() noexcept -> Void {
  UEFIPP_ASSERT(State_.Load(MemoryOrder::Relaxed) != 0);
  State_.Store(0, MemoryOrder::Release);
}

auto SpinLock::Locked() const noexcept -> Bool {
  return State_.Load(MemoryOrder::Relaxed) != 0;
}

SpinLockGuard::SpinLockGuard(SpinLock &Lock) noexcept : Lock_(&Lock) {
  Lock_->Lock();
}

SpinLockGuard::SpinLockGuard(SpinLockGuard &&Other) noexcept
    : Lock_(Utility::Exchange(Other.Lock_, nullptr)) {}

SpinLockGuard::~SpinLockGuard() noexcept {
  if (Lock_) {
    Lock_->Unlock();
  }
}
} // namespace UEFIpp::Foundation
