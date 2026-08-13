#include <UEFIpp/IO/Transport/Serial.hpp>
#include <UEFIpp/IO/Core/Operations.hpp>

namespace UEFIpp::IO {
auto SerialSink::WriteSome(Library::Span<const Foundation::Byte> Bytes)
    -> Result<Foundation::Size> {
  if (!Enabled_) {
    return Failure(Error::Semantic(ErrorCode::Closed, Operation::Write));
  }

  Foundation::Size Written{};
  for (; Written < Bytes.Size(); ++Written) {
    Foundation::Size Polls{};
    while ((__inbyte(Port_ + LineStatusOffset) &
            TransmitterHoldingRegisterEmpty) == 0) {
      if (++Polls >= MaximumPolls_) {
        return Failure(Error::Semantic(ErrorCode::Timeout, Operation::Write,
                                       Written, Written));
      }
      _mm_pause();
    }
    __outbyte(Port_, Bytes[Written]);
  }
  return Written;
}

auto SerialSink::ClearTerminal() -> Result<> {
  constexpr Foundation::Byte Sequence[] = {'\x1B', '[', '2', 'J', '\x1B', '[',
                                            'H'};
  return WriteAll(*this, Sequence);
}
} // namespace UEFIpp::IO
