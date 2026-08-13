#pragma once

#include <UEFIpp/Protocols/SimpleTextInputEx.hpp>
#include <UEFIpp/Stream/Input/Input.hpp>
#include <UEFIpp/Stream/Output/ConsoleSink.hpp>
#include <UEFIpp/UEFI/BootServices.hpp>
#include <UEFIpp/UEFI/Status.hpp>

namespace UEFIpp::Stream {
class ConsoleInputSource {
public:
  constexpr ConsoleInputSource() = default;

  constexpr ConsoleInputSource(UEFI::Table::BootServices *BootServices,
                               Protocols::SimpleTextInputEx *Input)
      : BootServices_(BootServices), Input_(Input) {}

  [[nodiscard]] constexpr auto Valid() const -> Foundation::Bool {
    return BootServices_ && Input_;
  }

  [[nodiscard]] constexpr explicit operator Foundation::Bool() const {
    return Valid();
  }

  auto Reset(UEFI::Table::BootServices *BootServices,
             Protocols::SimpleTextInputEx *Input) -> Foundation::Void;

  [[nodiscard]] auto ReadKey() -> Protocols::KeyData;

  [[nodiscard]] auto ReadKeyEcho() -> Protocols::KeyData;

  [[nodiscard]] auto ReadKey(Foundation::Bool Echo) -> Protocols::KeyData;

private:
  auto EchoKey(const Protocols::KeyData &Key) -> Foundation::Void;

private:
  UEFI::Table::BootServices *BootServices_{};
  Protocols::SimpleTextInputEx *Input_{};
  Foundation::Size LineLength_{};
};

namespace In {
extern Input<ConsoleInputSource> Console;
}
} // namespace UEFIpp::Stream
