#pragma once

#include <UEFIpp/Stream/Output/Output.hpp>
#include <intrin.h>

namespace UEFIpp::Stream {
class SerialSink {
public:
  static constexpr auto LineStatusOffset = Foundation::Uint16{5};
  static constexpr auto TransmitterHoldingRegisterEmpty =
      Foundation::Bit::Mask<Foundation::Uint8>(5);

  constexpr SerialSink() = default;

  constexpr explicit SerialSink(Foundation::Uint16 Port)
      : Port_(Port), Enabled_(true) {}

  auto Write(const Foundation::Char *Data, Foundation::Size Length) -> void;

  auto Write(const Foundation::WChar *Data, Foundation::Size Length) -> void;

  auto Clear() -> void;

private:
  Foundation::Uint16 Port_{};
  Foundation::Bool Enabled_{};
};

namespace Out {
extern Output<SerialSink> Serial;
}
} // namespace UEFIpp::Stream
