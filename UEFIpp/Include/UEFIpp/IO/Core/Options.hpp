#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Optional.hpp>

namespace UEFIpp::IO {
class Timeout final {
public:
  [[nodiscard]] static constexpr auto From100Nanoseconds(
      Foundation::Uint64 Value) noexcept -> Foundation::Uint64 {
    return Value;
  }
  [[nodiscard]] static constexpr auto Milliseconds(
      Foundation::Uint64 Value) noexcept -> Foundation::Uint64 {
    return Value > Foundation::Uint64(-1) / 10'000
               ? Foundation::Uint64(-1)
               : Value * 10'000;
  }
  [[nodiscard]] static constexpr auto Seconds(
      Foundation::Uint64 Value) noexcept -> Foundation::Uint64 {
    return Value > Foundation::Uint64(-1) / 10'000'000
               ? Foundation::Uint64(-1)
               : Value * 10'000'000;
  }
};

class CancellationToken final {
public:
  constexpr CancellationToken() noexcept = default;

  auto Cancel() noexcept -> Foundation::Void {
    Cancelled_.Store(1, Foundation::MemoryOrder::Release);
  }

  auto Reset() noexcept -> Foundation::Void {
    Cancelled_.Store(0, Foundation::MemoryOrder::Release);
  }

  [[nodiscard]] auto Cancelled() const noexcept -> Foundation::Bool {
    return Cancelled_.Load(Foundation::MemoryOrder::Acquire) != 0;
  }

private:
  Foundation::Atomic<Foundation::Uint32> Cancelled_{};
};

struct WaitOptions final {
  // UEFI timer units (100 ns). NullOpt means no deadline.
  Library::Optional<Foundation::Uint64> Timeout100ns{};
  const CancellationToken *Cancellation{};
};
} // namespace UEFIpp::IO
