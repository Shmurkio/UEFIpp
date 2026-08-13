#pragma once

#include <intrin.h>

#include <UEFIpp/Foundation/Atomic.hpp>
#include <UEFIpp/Foundation/Utility.hpp>

namespace UEFIpp::Foundation {
class SpinLock final {
public:
  constexpr SpinLock() noexcept = default;

  SpinLock(const SpinLock &) = delete;
  auto operator=(const SpinLock &) -> SpinLock & = delete;

  [[nodiscard]] auto TryLock() noexcept -> Bool;

  auto Lock() noexcept -> Void;

  auto Unlock() noexcept -> Void;

  [[nodiscard]] auto Locked() const noexcept -> Bool;

private:
  Atomic<Uint32> State_{};
};

class SpinLockGuard final {
public:
  explicit SpinLockGuard(SpinLock &Lock) noexcept;

  SpinLockGuard(const SpinLockGuard &) = delete;
  auto operator=(const SpinLockGuard &) -> SpinLockGuard & = delete;

  SpinLockGuard(SpinLockGuard &&Other) noexcept;

  ~SpinLockGuard() noexcept;

private:
  SpinLock *Lock_{};
};
} // namespace UEFIpp::Foundation
