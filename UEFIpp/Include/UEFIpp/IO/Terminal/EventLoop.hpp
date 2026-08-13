#pragma once

#include <coroutine>

#include <UEFIpp/IO/Core/Error.hpp>
#include <UEFIpp/IO/Core/Options.hpp>
#include <UEFIpp/Library/Containers/Array.hpp>
#include <UEFIpp/UEFI/BootServices.hpp>

namespace UEFIpp::IO {
class EventLoop final {
public:
  static constexpr Foundation::Size MaximumRegistrations{32};

  constexpr EventLoop() noexcept = default;
  constexpr explicit EventLoop(UEFI::Table::BootServices *BootServices) noexcept
      : BootServices_(BootServices) {}

  constexpr auto Reset(UEFI::Table::BootServices *BootServices) noexcept
      -> Foundation::Void {
    BootServices_ = BootServices;
    Count_ = 0;
  }

  [[nodiscard]] auto Register(UEFI::Event Event,
                              std::coroutine_handle<> Coroutine) -> Result<>;
  [[nodiscard]] auto RunOnce(WaitOptions Options = {}) -> Result<>;
  [[nodiscard]] constexpr auto Empty() const noexcept -> Foundation::Bool {
    return Count_ == 0;
  }

private:
  struct Registration final {
    UEFI::Event Event{};
    std::coroutine_handle<> Coroutine{};
  };

  UEFI::Table::BootServices *BootServices_{};
  Library::Array<Registration, MaximumRegistrations> Registrations_{};
  Foundation::Size Count_{};
};
} // namespace UEFIpp::IO
