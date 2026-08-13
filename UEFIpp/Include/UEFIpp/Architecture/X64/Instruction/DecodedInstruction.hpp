#pragma once

#include <UEFIpp/Architecture/X64/Instruction/Attributes.hpp>
#include <UEFIpp/Architecture/X64/Instruction/Instruction.hpp>
#include <UEFIpp/Architecture/X64/Instruction/RegisterSet.hpp>
#include <UEFIpp/Library/Containers/Optional.hpp>

namespace UEFIpp::Architecture::X64::InstructionSet {
namespace Detail {
struct DecodedInstructionAccess;
}

struct CpuFlagAccess {
  Foundation::Uint32 Read{};
  Foundation::Uint32 Modified{};
  Foundation::Uint32 SetToZero{};
  Foundation::Uint32 SetToOne{};
  Foundation::Uint32 Undefined{};

  [[nodiscard]] constexpr auto Written() const noexcept -> Foundation::Uint32 {
    return Modified | SetToZero | SetToOne | Undefined;
  }
};

struct VectorEncodingInfo {
  Foundation::Uint16 VectorLength{};
  MaskMode Mask{MaskMode::Invalid};
  Register MaskRegister{};
  Foundation::Bool StaticBroadcast{};
  BroadcastMode Broadcast{BroadcastMode::Invalid};
  RoundingMode Rounding{RoundingMode::Invalid};
  SwizzleMode Swizzle{SwizzleMode::Invalid};
  ConversionMode Conversion{ConversionMode::Invalid};
  Foundation::Bool SuppressAllExceptions{};
  Foundation::Bool EvictionHint{};
};

struct RawEncodingInfo {
  struct LegacyPrefixInfo {
    PrefixType Type{PrefixType::Ignored};
    Foundation::Uint8 Value{};
  };

  struct RexInfo {
    Foundation::Uint8 W{};
    Foundation::Uint8 R{};
    Foundation::Uint8 X{};
    Foundation::Uint8 B{};
    Foundation::Uint8 Offset{};
  };

  struct XopInfo {
    Foundation::Uint8 R{};
    Foundation::Uint8 X{};
    Foundation::Uint8 B{};
    Foundation::Uint8 Map{};
    Foundation::Uint8 W{};
    Foundation::Uint8 Vvvv{};
    Foundation::Uint8 L{};
    Foundation::Uint8 Pp{};
    Foundation::Uint8 Offset{};
  };

  struct VexInfo : XopInfo {
    Foundation::Uint8 Size{};
  };

  struct EvexInfo {
    Foundation::Uint8 R{};
    Foundation::Uint8 X{};
    Foundation::Uint8 B{};
    Foundation::Uint8 R2{};
    Foundation::Uint8 Map{};
    Foundation::Uint8 W{};
    Foundation::Uint8 Vvvv{};
    Foundation::Uint8 Pp{};
    Foundation::Uint8 Z{};
    Foundation::Uint8 L2{};
    Foundation::Uint8 L{};
    Foundation::Uint8 BcstRcSae{};
    Foundation::Uint8 V2{};
    Foundation::Uint8 Aaa{};
    Foundation::Uint8 Offset{};
  };

  struct MvexInfo {
    Foundation::Uint8 R{};
    Foundation::Uint8 X{};
    Foundation::Uint8 B{};
    Foundation::Uint8 R2{};
    Foundation::Uint8 Map{};
    Foundation::Uint8 W{};
    Foundation::Uint8 Vvvv{};
    Foundation::Uint8 Pp{};
    Foundation::Uint8 Eviction{};
    Foundation::Uint8 Sss{};
    Foundation::Uint8 V2{};
    Foundation::Uint8 Kkk{};
    Foundation::Uint8 Offset{};
  };

  struct ModRmInfo {
    Foundation::Uint8 Mod{};
    Foundation::Uint8 Reg{};
    Foundation::Uint8 Rm{};
    Foundation::Uint8 Offset{};
  };

  struct SibInfo {
    Foundation::Uint8 Scale{};
    Foundation::Uint8 Index{};
    Foundation::Uint8 Base{};
    Foundation::Uint8 Offset{};
  };

  struct DisplacementInfo {
    Foundation::Int64 Value{};
    Foundation::Uint8 Offset{};
    Foundation::Uint8 SizeInBits{};
  };

  struct ImmediateInfo {
    Foundation::Bool IsSigned{};
    Foundation::Bool IsRelative{};
    Foundation::Uint64 UnsignedValue{};
    Foundation::Int64 SignedValue{};
    Foundation::Uint8 Offset{};
    Foundation::Uint8 SizeInBits{};
  };

  Foundation::Uint8 PrefixCount{};
  Library::Array<LegacyPrefixInfo, 15> Prefixes{};
  RexInfo Rex{};
  XopInfo Xop{};
  VexInfo Vex{};
  EvexInfo Evex{};
  MvexInfo Mvex{};
  ModRmInfo ModRm{};
  SibInfo Sib{};
  DisplacementInfo Displacement{};

  // Convenience offsets retained for fast relocation code.
  Foundation::Uint8 ModRmOffset{};
  Foundation::Uint8 SibOffset{};
  Foundation::Uint8 DisplacementOffset{};
  Foundation::Uint8 DisplacementSizeInBits{};
  Library::Array<ImmediateInfo, 2> Immediates{};
};

class DecodedInstruction {
public:
  static constexpr Foundation::Size MaximumLength = 15;
  static constexpr Foundation::Size MaximumOperandCount = 10;

public:
  constexpr DecodedInstruction() noexcept = default;

  [[nodiscard]] constexpr auto Address() const noexcept -> Foundation::Uint64 {
    return Address_;
  }

  [[nodiscard]] constexpr auto Length() const noexcept -> Foundation::Uint8 {
    return Length_;
  }

  [[nodiscard]] constexpr auto Bytes() const noexcept
      -> Library::Span<const Foundation::Byte> {
    return {Bytes_.Data(), Length_};
  }

  [[nodiscard]] constexpr auto MnemonicValue() const noexcept -> Mnemonic {
    return Mnemonic_;
  }

  [[nodiscard]] constexpr auto Mode() const noexcept -> MachineMode {
    return Mode_;
  }

  [[nodiscard]] constexpr auto Encoding() const noexcept
      -> InstructionEncoding {
    return Encoding_;
  }

  [[nodiscard]] constexpr auto BranchKind() const noexcept -> BranchType {
    return BranchType_;
  }

  [[nodiscard]] constexpr auto Prefixes() const noexcept
      -> InstructionPrefixes {
    return Prefixes_;
  }

  [[nodiscard]] constexpr auto Category() const noexcept
      -> InstructionCategory {
    return Category_;
  }

  [[nodiscard]] constexpr auto Isa() const noexcept -> IsaSet { return Isa_; }

  [[nodiscard]] constexpr auto IsaExtensionValue() const noexcept
      -> IsaExtension {
    return IsaExtension_;
  }

  [[nodiscard]] constexpr auto ControlFlow() const noexcept -> ControlFlowType {
    return ControlFlow_;
  }

  [[nodiscard]] constexpr auto Attributes() const noexcept
      -> InstructionAttributes {
    return Attributes_;
  }

  [[nodiscard]] constexpr auto RawAttributes() const noexcept
      -> Foundation::Uint64 {
    return RawAttributes_;
  }

  [[nodiscard]] constexpr auto OperandWidth() const noexcept
      -> Foundation::Uint8 {
    return OperandWidth_;
  }

  [[nodiscard]] constexpr auto AddressWidth() const noexcept
      -> Foundation::Uint8 {
    return AddressWidth_;
  }

  [[nodiscard]] constexpr auto StackWidth() const noexcept
      -> Foundation::Uint8 {
    return StackWidth_;
  }

  [[nodiscard]] constexpr auto Operands() const noexcept
      -> Library::Span<const Operand> {
    return {Operands_.Data(), OperandCount_};
  }

  [[nodiscard]] constexpr auto VisibleOperands() const noexcept
      -> Library::Span<const Operand> {
    return {Operands_.Data(), VisibleOperandCount_};
  }

  [[nodiscard]] constexpr auto OperandAt(Foundation::Size Index) const noexcept
      -> const Operand * {
    return Index < OperandCount_ ? &Operands_[Index] : nullptr;
  }

  [[nodiscard]] constexpr auto RegistersRead() const noexcept
      -> const RegisterSet & {
    return RegistersRead_;
  }

  [[nodiscard]] constexpr auto RegistersWritten() const noexcept
      -> const RegisterSet & {
    return RegistersWritten_;
  }

  [[nodiscard]] constexpr auto Flags() const noexcept -> const CpuFlagAccess & {
    return Flags_;
  }

  [[nodiscard]] constexpr auto RawEncoding() const noexcept
      -> const RawEncodingInfo & {
    return RawEncoding_;
  }

  [[nodiscard]] constexpr auto VectorEncoding() const noexcept
      -> const VectorEncodingInfo & {
    return VectorEncoding_;
  }

  [[nodiscard]] constexpr auto IsBranch() const noexcept -> Foundation::Bool {
    return ControlFlow_ == ControlFlowType::ConditionalBranch ||
           ControlFlow_ == ControlFlowType::UnconditionalBranch;
  }

  [[nodiscard]] constexpr auto IsConditionalBranch() const noexcept
      -> Foundation::Bool {
    return ControlFlow_ == ControlFlowType::ConditionalBranch;
  }

  [[nodiscard]] constexpr auto IsUnconditionalBranch() const noexcept
      -> Foundation::Bool {
    return ControlFlow_ == ControlFlowType::UnconditionalBranch;
  }

  [[nodiscard]] constexpr auto IsCall() const noexcept -> Foundation::Bool {
    return ControlFlow_ == ControlFlowType::Call;
  }

  [[nodiscard]] constexpr auto IsReturn() const noexcept -> Foundation::Bool {
    return ControlFlow_ == ControlFlowType::Return;
  }

  [[nodiscard]] constexpr auto IsControlFlow() const noexcept
      -> Foundation::Bool {
    return ControlFlow_ != ControlFlowType::None;
  }

  [[nodiscard]] constexpr auto ReadsMemory() const noexcept
      -> Foundation::Bool {
    for (const auto &Value : Operands()) {
      if (Value.Type() == OperandType::Memory && Value.Reads()) {
        return true;
      }
    }

    return false;
  }

  [[nodiscard]] constexpr auto WritesMemory() const noexcept
      -> Foundation::Bool {
    for (const auto &Value : Operands()) {
      if (Value.Type() == OperandType::Memory && Value.Writes()) {
        return true;
      }
    }

    return false;
  }

  [[nodiscard]] constexpr auto HasRipRelativeOperand() const noexcept
      -> Foundation::Bool {
    for (const auto &Value : Operands()) {
      if (Value.Type() == OperandType::Memory &&
          Value.MemoryValue().Base == Registers::Rip) {
        return true;
      }
    }

    return false;
  }

  [[nodiscard]] constexpr auto DirectTarget() const noexcept
      -> const Library::Optional<Foundation::Uint64> & {
    return DirectTarget_;
  }

  [[nodiscard]] constexpr auto StackDelta() const noexcept
      -> const Library::Optional<Foundation::Int64> & {
    return StackDelta_;
  }

  [[nodiscard]] auto ToInstruction() const noexcept -> Instruction;

private:
  friend struct Detail::DecodedInstructionAccess;

  Foundation::Uint64 Address_{};
  Library::Array<Foundation::Byte, MaximumLength> Bytes_{};
  Foundation::Uint8 Length_{};

  Mnemonic Mnemonic_{Mnemonic::Invalid};
  MachineMode Mode_{MachineMode::Long64};
  InstructionEncoding Encoding_{InstructionEncoding::Unknown};
  BranchType BranchType_{BranchType::Automatic};
  InstructionPrefixes Prefixes_{};
  InstructionCategory Category_{InstructionCategory::Invalid};
  IsaSet Isa_{IsaSet::Invalid};
  IsaExtension IsaExtension_{IsaExtension::Invalid};
  ControlFlowType ControlFlow_{ControlFlowType::None};
  InstructionAttributes Attributes_{};
  Foundation::Uint64 RawAttributes_{};

  Foundation::Uint8 StackWidth_{};
  Foundation::Uint8 OperandWidth_{};
  Foundation::Uint8 AddressWidth_{};

  Library::Array<Operand, MaximumOperandCount> Operands_{};
  Foundation::Uint8 OperandCount_{};
  Foundation::Uint8 VisibleOperandCount_{};

  RegisterSet RegistersRead_{};
  RegisterSet RegistersWritten_{};
  CpuFlagAccess Flags_{};
  RawEncodingInfo RawEncoding_{};
  VectorEncodingInfo VectorEncoding_{};

  Library::Optional<Foundation::Uint64> DirectTarget_{};
  Library::Optional<Foundation::Int64> StackDelta_{};
};
} // namespace UEFIpp::Architecture::X64::InstructionSet
