#include <UEFIpp/Architecture/X64/Instruction/DecodedInstruction.hpp>

namespace UEFIpp::Architecture::X64::InstructionSet {
auto DecodedInstruction::ToInstruction() const noexcept -> Instruction {
  Instruction Result{Mnemonic_};
  Result.Options().Mode = Mode_;
  Result.Options().Prefixes = Prefixes_;
  Result.Options().Branch = BranchType_;

  switch (Encoding_) {
  case InstructionEncoding::Legacy:
    Result.Options().AllowedEncodings = EncodableEncoding::Legacy;
    break;
  case InstructionEncoding::Amd3DNow:
    Result.Options().AllowedEncodings = EncodableEncoding::Amd3DNow;
    break;
  case InstructionEncoding::Xop:
    Result.Options().AllowedEncodings = EncodableEncoding::Xop;
    break;
  case InstructionEncoding::Vex:
    Result.Options().AllowedEncodings = EncodableEncoding::Vex;
    break;
  case InstructionEncoding::Evex:
    Result.Options().AllowedEncodings = EncodableEncoding::Evex;
    break;
  case InstructionEncoding::Mvex:
    Result.Options().AllowedEncodings = EncodableEncoding::Mvex;
    break;
  default:
    break;
  }

  Result.Options().Evex.Broadcast = VectorEncoding_.Broadcast;
  Result.Options().Evex.Rounding = VectorEncoding_.Rounding;
  Result.Options().Evex.SuppressAllExceptions =
      VectorEncoding_.SuppressAllExceptions;
  Result.Options().Evex.ZeroingMask =
      VectorEncoding_.Mask == MaskMode::Zeroing ||
      VectorEncoding_.Mask == MaskMode::ControlZeroing;

  Result.Options().Mvex.Broadcast = VectorEncoding_.Broadcast;
  Result.Options().Mvex.Conversion = VectorEncoding_.Conversion;
  Result.Options().Mvex.Rounding = VectorEncoding_.Rounding;
  Result.Options().Mvex.Swizzle = VectorEncoding_.Swizzle;
  Result.Options().Mvex.SuppressAllExceptions =
      VectorEncoding_.SuppressAllExceptions;
  Result.Options().Mvex.EvictionHint = VectorEncoding_.EvictionHint;

  for (const auto &Immediate : RawEncoding_.Immediates) {
    if (!Immediate.IsRelative) {
      continue;
    }

    switch (Immediate.SizeInBits) {
    case 8:
      Result.Options().BranchSize = BranchWidth::Width8;
      break;
    case 16:
      Result.Options().BranchSize = BranchWidth::Width16;
      break;
    case 32:
      Result.Options().BranchSize = BranchWidth::Width32;
      break;
    case 64:
      Result.Options().BranchSize = BranchWidth::Width64;
      break;
    default:
      break;
    }
    break;
  }

  for (Foundation::Size Index = 0;
       Index < VisibleOperandCount_ && Index < Instruction::MaximumOperandCount;
       ++Index) {
    (void)Result.AddOperand(Operands_[Index]);
  }

  return Result;
}
} // namespace UEFIpp::Architecture::X64::InstructionSet
