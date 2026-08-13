#pragma once

#include <UEFIpp/Architecture/X64/Assembly/AssembledCode.hpp>
#include <UEFIpp/Architecture/X64/Assembly/Encoder.hpp>
#include <UEFIpp/Architecture/X64/Assembly/Program.hpp>
#include <UEFIpp/Architecture/X64/Instruction/Condition.hpp>

namespace UEFIpp::Architecture::X64::Assembly {
class Assembler {
public:
  explicit Assembler(Memory::AllocatorStub Allocator = {});

  [[nodiscard]] auto GetProgram() noexcept -> Program &;

  [[nodiscard]] auto GetProgram() const noexcept -> const Program &;

  [[nodiscard]] auto CreateSection(SectionKind Kind, Library::StringView Name,
                                   Foundation::Size Alignment = 1)
      -> Library::Expected<SectionId, AssemblyError>;

  [[nodiscard]] auto SwitchSection(SectionId Section) noexcept
      -> Foundation::Bool;

  [[nodiscard]] auto CreateLabel() -> Library::Expected<Label, AssemblyError>;

  [[nodiscard]] auto Bind(Label Value)
      -> Library::Expected<Foundation::Void, AssemblyError>;

  [[nodiscard]] auto Emit(const InstructionSet::Instruction &Value)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  template <typename... TOperands>
  [[nodiscard]] auto Emit(InstructionSet::Mnemonic Mnemonic,
                          const TOperands &...Operands)
      -> Library::Expected<Foundation::Size, AssemblyError> {
    static_assert(sizeof...(Operands) <=
                  InstructionSet::Instruction::MaximumOperandCount);

    InstructionSet::Instruction Value{Mnemonic};
    const Foundation::Bool Added =
        (Value.AddOperand(ToOperand(Operands)) && ... && true);

    if (!Added) {
      return Library::MakeUnexpected(AssemblyError::InvalidArgument);
    }

    return Emit(Value);
  }

  [[nodiscard]] auto EmitData(Library::Span<const Foundation::Byte> Data)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Align(Foundation::Size Alignment,
                           Foundation::Byte Fill = 0x90)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Mov(const InstructionSet::Operand &Destination,
                         const InstructionSet::Operand &Source)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Lea(InstructionSet::Register Destination,
                         const InstructionSet::MemoryOperand &Source)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Add(const InstructionSet::Operand &Destination,
                         const InstructionSet::Operand &Source)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Sub(const InstructionSet::Operand &Destination,
                         const InstructionSet::Operand &Source)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto And(const InstructionSet::Operand &Destination,
                         const InstructionSet::Operand &Source)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Or(const InstructionSet::Operand &Destination,
                        const InstructionSet::Operand &Source)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Xor(const InstructionSet::Operand &Destination,
                         const InstructionSet::Operand &Source)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Cmp(const InstructionSet::Operand &Left,
                         const InstructionSet::Operand &Right)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Test(const InstructionSet::Operand &Left,
                          const InstructionSet::Operand &Right)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Push(const InstructionSet::Operand &Value)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Pop(const InstructionSet::Operand &Value)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Inc(const InstructionSet::Operand &Value)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Dec(const InstructionSet::Operand &Value)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Call(Foundation::Uint64 Target)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Call(Label Target)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Jmp(Foundation::Uint64 Target,
                         InstructionSet::BranchWidth Width =
                             InstructionSet::BranchWidth::Automatic)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Jmp(Label Target,
                         InstructionSet::BranchWidth Width =
                             InstructionSet::BranchWidth::Automatic)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Jcc(InstructionSet::Condition Condition,
                         Foundation::Uint64 Target,
                         InstructionSet::BranchWidth Width =
                             InstructionSet::BranchWidth::Automatic)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Jcc(InstructionSet::Condition Condition, Label Target,
                         InstructionSet::BranchWidth Width =
                             InstructionSet::BranchWidth::Automatic)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Ret()
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Nop()
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Assemble(Foundation::Uint64 BaseAddress = 0) const
      -> Library::Expected<AssembledCode, AssemblyError>;

  template <typename TDestination, typename TSource>
  [[nodiscard]] auto Mov(const TDestination &Destination, const TSource &Source)
      -> Library::Expected<Foundation::Size, AssemblyError> {
    return Mov(ToOperand(Destination), ToOperand(Source));
  }

  template <typename TDestination, typename TSource>
  [[nodiscard]] auto Add(const TDestination &Destination, const TSource &Source)
      -> Library::Expected<Foundation::Size, AssemblyError> {
    return Add(ToOperand(Destination), ToOperand(Source));
  }

  template <typename TDestination, typename TSource>
  [[nodiscard]] auto Sub(const TDestination &Destination, const TSource &Source)
      -> Library::Expected<Foundation::Size, AssemblyError> {
    return Sub(ToOperand(Destination), ToOperand(Source));
  }

  template <typename TDestination, typename TSource>
  [[nodiscard]] auto And(const TDestination &Destination, const TSource &Source)
      -> Library::Expected<Foundation::Size, AssemblyError> {
    return And(ToOperand(Destination), ToOperand(Source));
  }

  template <typename TDestination, typename TSource>
  [[nodiscard]] auto Or(const TDestination &Destination, const TSource &Source)
      -> Library::Expected<Foundation::Size, AssemblyError> {
    return Or(ToOperand(Destination), ToOperand(Source));
  }

  template <typename TDestination, typename TSource>
  [[nodiscard]] auto Xor(const TDestination &Destination, const TSource &Source)
      -> Library::Expected<Foundation::Size, AssemblyError> {
    return Xor(ToOperand(Destination), ToOperand(Source));
  }

  template <typename TLeft, typename TRight>
  [[nodiscard]] auto Cmp(const TLeft &Left, const TRight &Right)
      -> Library::Expected<Foundation::Size, AssemblyError> {
    return Cmp(ToOperand(Left), ToOperand(Right));
  }

  template <typename TLeft, typename TRight>
  [[nodiscard]] auto Test(const TLeft &Left, const TRight &Right)
      -> Library::Expected<Foundation::Size, AssemblyError> {
    return Test(ToOperand(Left), ToOperand(Right));
  }

  template <typename TValue>
  [[nodiscard]] auto Push(const TValue &Value)
      -> Library::Expected<Foundation::Size, AssemblyError> {
    return Push(ToOperand(Value));
  }

  template <typename TValue>
  [[nodiscard]] auto Pop(const TValue &Value)
      -> Library::Expected<Foundation::Size, AssemblyError> {
    return Pop(ToOperand(Value));
  }

  template <typename TValue>
  [[nodiscard]] auto Inc(const TValue &Value)
      -> Library::Expected<Foundation::Size, AssemblyError> {
    return Inc(ToOperand(Value));
  }

  template <typename TValue>
  [[nodiscard]] auto Dec(const TValue &Value)
      -> Library::Expected<Foundation::Size, AssemblyError> {
    return Dec(ToOperand(Value));
  }

private:
  [[nodiscard]] static constexpr auto
  ToOperand(const InstructionSet::Operand &Value) noexcept
      -> InstructionSet::Operand {
    return Value;
  }

  [[nodiscard]] static constexpr auto
  ToOperand(InstructionSet::Register Value) noexcept
      -> InstructionSet::Operand {
    return InstructionSet::Reg(Value);
  }

  [[nodiscard]] static constexpr auto
  ToOperand(const InstructionSet::MemoryOperand &Value) noexcept
      -> InstructionSet::Operand {
    return InstructionSet::Mem(Value);
  }

  [[nodiscard]] static constexpr auto
  ToOperand(const InstructionSet::ImmediateOperand &Value) noexcept
      -> InstructionSet::Operand {
    return InstructionSet::Operand::FromImmediate(Value);
  }

  [[nodiscard]] static auto
  ConditionMnemonic(InstructionSet::Condition Condition) noexcept
      -> InstructionSet::Mnemonic;

  Program Program_;
  Encoder Encoder_{};
};
} // namespace UEFIpp::Architecture::X64::Assembly
