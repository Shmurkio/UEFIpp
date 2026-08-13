#include <UEFIpp/Stream/Output/SerialSink.hpp>

namespace UEFIpp::Stream {
namespace Out {
Output<SerialSink> Serial{};
}

auto SerialSink::Write(const Foundation::Char *Data, Foundation::Size Length)
    -> void {
  if (!Enabled_) {
    return;
  }

  for (Foundation::Size Index = 0; Index < Length; ++Index) {
    while (!Foundation::Bit::IsSet(__inbyte(Port_ + LineStatusOffset),
                                   TransmitterHoldingRegisterEmpty)) {
    }

    __outbyte(Port_, Data[Index]);
  }
}

auto SerialSink::Write(const Foundation::WChar *Data, Foundation::Size Length)
    -> void {
  if (!Data) {
    return;
  }

  for (Foundation::Size Index = 0; Index < Length; ++Index) {
    const auto C = Data[Index];

    if (C <= 0x7F) {
      const auto Narrow = Foundation::Cast::Auto<Foundation::Char>(C);
      Write(&Narrow, 1);
    } else {
      const auto Replacement = Foundation::Char{'?'};
      Write(&Replacement, 1);
    }
  }
}

auto SerialSink::Clear() -> void { Write("\x1B[2J\x1B[H", 7); }
} // namespace UEFIpp::Stream
