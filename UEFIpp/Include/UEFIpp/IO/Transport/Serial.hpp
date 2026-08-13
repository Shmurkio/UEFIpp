#pragma once

#include <intrin.h>

#include <UEFIpp/IO/Core/Concepts.hpp>

namespace UEFIpp::IO {
class SerialSink final {
public:
  static constexpr Foundation::Uint16 LineStatusOffset{5};
  static constexpr Foundation::Uint8 TransmitterHoldingRegisterEmpty{1u << 5};

  constexpr SerialSink() noexcept = default;
  constexpr explicit SerialSink(Foundation::Uint16 Port,
                                Foundation::Size MaximumPolls = 1'000'000)
      : Port_(Port), MaximumPolls_(MaximumPolls), Enabled_(true) {}

  constexpr auto Reset(Foundation::Uint16 Port,
                       Foundation::Size MaximumPolls = 1'000'000) noexcept
      -> Foundation::Void {
    Port_ = Port;
    MaximumPolls_ = MaximumPolls;
    Enabled_ = true;
  }

  constexpr auto Disable() noexcept -> Foundation::Void { Enabled_ = false; }

  [[nodiscard]] auto WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size>;
  [[nodiscard]] constexpr auto Flush() -> Result<> { return {}; }
  [[nodiscard]] auto ClearTerminal() -> Result<>;

private:
  Foundation::Uint16 Port_{};
  Foundation::Size MaximumPolls_{1'000'000};
  Foundation::Bool Enabled_{};
};
} // namespace UEFIpp::IO
