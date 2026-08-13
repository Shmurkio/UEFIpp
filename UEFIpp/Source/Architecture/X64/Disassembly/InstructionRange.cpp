#include <UEFIpp/Architecture/X64/Disassembly/InstructionRange.hpp>

namespace UEFIpp::Architecture::X64::Disassembly {
InstructionRange::Iterator::Iterator(const Decoder *DecoderInstance,
                                     Library::Span<const Foundation::Byte> Code,
                                     Foundation::Uint64 BaseAddress,
                                     Foundation::Size Offset) noexcept
    : Decoder_(DecoderInstance), Code_(Code), BaseAddress_(BaseAddress),
      Offset_(Offset) {
  DecodeCurrent();
}

auto InstructionRange::Iterator::operator*() const noexcept
    -> const InstructionSet::DecodedInstruction & {
  return Current_;
}

auto InstructionRange::Iterator::operator->() const noexcept
    -> const InstructionSet::DecodedInstruction * {
  return &Current_;
}

auto InstructionRange::Iterator::operator++() noexcept -> Iterator & {
  if (!AtEnd_) {
    Offset_ += Current_.Length();
    DecodeCurrent();
  }

  return *this;
}

auto InstructionRange::Iterator::DecodeCurrent() noexcept -> Foundation::Void {
  if (!Decoder_ || Offset_ >= Code_.Size()) {
    AtEnd_ = true;
    return;
  }

  auto Result =
      Decoder_->Decode(Code_.Subspan(Offset_), BaseAddress_ + Offset_);

  if (!Result) {
    Error_ = Result.Error();
    AtEnd_ = true;
    return;
  }

  Current_ = Foundation::Utility::Move(Result.Value());
  AtEnd_ = false;
}

auto InstructionRange::begin() const noexcept -> Iterator {
  return Iterator{Decoder_, Code_, BaseAddress_, 0};
}
} // namespace UEFIpp::Architecture::X64::Disassembly
