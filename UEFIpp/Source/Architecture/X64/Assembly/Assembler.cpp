#include <UEFIpp/Architecture/X64/Assembly/Assembler.hpp>
#include <UEFIpp/Architecture/X64/Disassembly/Decoder.hpp>

namespace UEFIpp::Architecture::X64::Assembly {
namespace {
struct NodeLayout {
  Foundation::Size Offset{};
  Foundation::Size Size{};
};

struct SectionLayout {
  Foundation::Size Offset{};
  Foundation::Size Size{};
  Foundation::Size FirstNode{};
};

static constexpr Foundation::Uint64 InvalidAddress =
    static_cast<Foundation::Uint64>(-1);

[[nodiscard]] auto RelativeRelocationKind(Foundation::Uint8 SizeInBits) noexcept
    -> RelocationKind {
  switch (SizeInBits) {
  case 8:
    return RelocationKind::Relative8;
  case 16:
    return RelocationKind::Relative16;
  case 64:
    return RelocationKind::Relative64;
  default:
    return RelocationKind::Relative32;
  }
}

[[nodiscard]] auto AbsoluteRelocationKind(Foundation::Uint8 SizeInBits) noexcept
    -> RelocationKind {
  switch (SizeInBits) {
  case 8:
    return RelocationKind::Absolute8;
  case 16:
    return RelocationKind::Absolute16;
  case 32:
    return RelocationKind::Absolute32;
  default:
    return RelocationKind::Absolute64;
  }
}

[[nodiscard]] auto
ResolveInstruction(const Node &NodeValue,
                   Library::Span<const Foundation::Uint64> LabelAddresses)
    -> Library::Expected<InstructionSet::Instruction, AssemblyError> {
  auto Instruction = NodeValue.InstructionValue();

  if (!NodeValue.HasLabelTarget()) {
    return Instruction;
  }

  const auto LabelValue = NodeValue.TargetLabel();

  if (!LabelValue.IsValid() || LabelValue.Id() >= LabelAddresses.Size() ||
      LabelAddresses[LabelValue.Id()] == InvalidAddress) {
    return Library::MakeUnexpected(AssemblyError::UnboundLabel);
  }

  auto *TargetOperand = Instruction.OperandAt(NodeValue.TargetOperandIndex());

  if (!TargetOperand ||
      TargetOperand->Type() != InstructionSet::OperandType::Immediate) {
    return Library::MakeUnexpected(AssemblyError::InternalError);
  }

  const auto SizeInBits = TargetOperand->SizeInBits();

  *TargetOperand = InstructionSet::Operand::FromImmediate(
      InstructionSet::ImmediateOperand::Relative(
          LabelAddresses[LabelValue.Id()]),
      SizeInBits);

  return Instruction;
}

[[nodiscard]] auto
FindMaximumLabelId(Library::Span<const Section> Sections) noexcept
    -> Foundation::Uint32 {
  Foundation::Uint32 Maximum{};

  for (const auto &SectionValue : Sections) {
    for (const auto &NodeValue : SectionValue.Nodes()) {
      if (NodeValue.Type() == NodeType::Label &&
          NodeValue.LabelValue().IsValid()) {
        Maximum =
            Foundation::Utility::Max(Maximum, NodeValue.LabelValue().Id() + 1);
      }

      if (NodeValue.HasLabelTarget() && NodeValue.TargetLabel().IsValid()) {
        Maximum =
            Foundation::Utility::Max(Maximum, NodeValue.TargetLabel().Id() + 1);
      }
    }
  }

  return Maximum;
}
} // namespace

auto Assembler::Mov(const InstructionSet::Operand &Destination,
                    const InstructionSet::Operand &Source)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  return Emit(InstructionSet::Mnemonic::Mov, Destination, Source);
}

auto Assembler::Lea(InstructionSet::Register Destination,
                    const InstructionSet::MemoryOperand &Source)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  return Emit(InstructionSet::Mnemonic::Lea, Destination, Source);
}

auto Assembler::Add(const InstructionSet::Operand &Destination,
                    const InstructionSet::Operand &Source)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  return Emit(InstructionSet::Mnemonic::Add, Destination, Source);
}

auto Assembler::Sub(const InstructionSet::Operand &Destination,
                    const InstructionSet::Operand &Source)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  return Emit(InstructionSet::Mnemonic::Sub, Destination, Source);
}

auto Assembler::And(const InstructionSet::Operand &Destination,
                    const InstructionSet::Operand &Source)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  return Emit(InstructionSet::Mnemonic::And, Destination, Source);
}

auto Assembler::Or(const InstructionSet::Operand &Destination,
                   const InstructionSet::Operand &Source)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  return Emit(InstructionSet::Mnemonic::Or, Destination, Source);
}

auto Assembler::Xor(const InstructionSet::Operand &Destination,
                    const InstructionSet::Operand &Source)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  return Emit(InstructionSet::Mnemonic::Xor, Destination, Source);
}

auto Assembler::Cmp(const InstructionSet::Operand &Left,
                    const InstructionSet::Operand &Right)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  return Emit(InstructionSet::Mnemonic::Cmp, Left, Right);
}

auto Assembler::Test(const InstructionSet::Operand &Left,
                     const InstructionSet::Operand &Right)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  return Emit(InstructionSet::Mnemonic::Test, Left, Right);
}

auto Assembler::Push(const InstructionSet::Operand &Value)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  return Emit(InstructionSet::Mnemonic::Push, Value);
}

auto Assembler::Pop(const InstructionSet::Operand &Value)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  return Emit(InstructionSet::Mnemonic::Pop, Value);
}

auto Assembler::Inc(const InstructionSet::Operand &Value)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  return Emit(InstructionSet::Mnemonic::Inc, Value);
}

auto Assembler::Dec(const InstructionSet::Operand &Value)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  return Emit(InstructionSet::Mnemonic::Dec, Value);
}

auto Assembler::Call(Foundation::Uint64 Target)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  InstructionSet::Instruction Value{InstructionSet::Mnemonic::Call};

  (void)Value.AddOperand(InstructionSet::Relative(Target));

  return Emit(Value);
}

auto Assembler::Call(Label Target)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  InstructionSet::Instruction Value{InstructionSet::Mnemonic::Call};

  (void)Value.AddOperand(InstructionSet::Operand::FromImmediate(
      InstructionSet::ImmediateOperand{.UnsignedValue = 0,
                                       .SignedValue = 0,
                                       .IsSigned = true,
                                       .IsRelative = true}));

  return Program_.AppendInstruction(Value, Target, 0);
}

auto Assembler::Jmp(Foundation::Uint64 Target,
                    InstructionSet::BranchWidth Width)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  InstructionSet::Instruction Value{InstructionSet::Mnemonic::Jmp};

  Value.Options().BranchSize = Width;
  (void)Value.AddOperand(InstructionSet::Relative(Target));

  return Emit(Value);
}

auto Assembler::Jmp(Label Target, InstructionSet::BranchWidth Width)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  InstructionSet::Instruction Value{InstructionSet::Mnemonic::Jmp};

  Value.Options().BranchSize = Width;
  (void)Value.AddOperand(InstructionSet::Operand::FromImmediate(
      InstructionSet::ImmediateOperand{.UnsignedValue = 0,
                                       .SignedValue = 0,
                                       .IsSigned = true,
                                       .IsRelative = true}));

  return Program_.AppendInstruction(Value, Target, 0);
}

auto Assembler::Jcc(InstructionSet::Condition Condition,
                    Foundation::Uint64 Target,
                    InstructionSet::BranchWidth Width)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  InstructionSet::Instruction Value{ConditionMnemonic(Condition)};

  Value.Options().BranchSize = Width;
  (void)Value.AddOperand(InstructionSet::Relative(Target));

  return Emit(Value);
}

auto Assembler::Jcc(InstructionSet::Condition Condition, Label Target,
                    InstructionSet::BranchWidth Width)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  InstructionSet::Instruction Value{ConditionMnemonic(Condition)};

  Value.Options().BranchSize = Width;
  (void)Value.AddOperand(InstructionSet::Operand::FromImmediate(
      InstructionSet::ImmediateOperand{.UnsignedValue = 0,
                                       .SignedValue = 0,
                                       .IsSigned = true,
                                       .IsRelative = true}));

  return Program_.AppendInstruction(Value, Target, 0);
}

auto Assembler::Ret() -> Library::Expected<Foundation::Size, AssemblyError> {
  return Emit(InstructionSet::Instruction{InstructionSet::Mnemonic::Ret});
}

auto Assembler::Nop() -> Library::Expected<Foundation::Size, AssemblyError> {
  return Emit(InstructionSet::Instruction{InstructionSet::Mnemonic::Nop});
}

auto Assembler::ConditionMnemonic(
    InstructionSet::Condition AsmCondition) noexcept
    -> InstructionSet::Mnemonic {
  using InstructionSet::Condition;
  using InstructionSet::Mnemonic;

  switch (AsmCondition) {
  case Condition::Overflow:
    return Mnemonic::Jo;
  case Condition::NotOverflow:
    return Mnemonic::Jno;
  case Condition::Below:
    return Mnemonic::Jb;
  case Condition::AboveEqual:
    return Mnemonic::Jnb;
  case Condition::Equal:
    return Mnemonic::Jz;
  case Condition::NotEqual:
    return Mnemonic::Jnz;
  case Condition::BelowEqual:
    return Mnemonic::Jbe;
  case Condition::Above:
    return Mnemonic::Jnbe;
  case Condition::Sign:
    return Mnemonic::Js;
  case Condition::NotSign:
    return Mnemonic::Jns;
  case Condition::Parity:
    return Mnemonic::Jp;
  case Condition::NotParity:
    return Mnemonic::Jnp;
  case Condition::Less:
    return Mnemonic::Jl;
  case Condition::GreaterEqual:
    return Mnemonic::Jnl;
  case Condition::LessEqual:
    return Mnemonic::Jle;
  case Condition::Greater:
    return Mnemonic::Jnle;
  default:
    return Mnemonic::Invalid;
  }
}

auto Assembler::Assemble(Foundation::Uint64 BaseAddress) const
    -> Library::Expected<AssembledCode, AssemblyError> {
  const auto Sections = Program_.Sections();

  if (Sections.Empty()) {
    return Library::MakeUnexpected(AssemblyError::InvalidSection);
  }

  const auto Allocator = Program_.Allocator();

  Foundation::Size TotalNodeCount{};

  for (const auto &SectionValue : Sections) {
    TotalNodeCount += SectionValue.Nodes().Size();
  }

  Library::Vector<SectionLayout> SectionLayouts{Sections.Size(), Allocator};

  Library::Vector<NodeLayout> NodeLayouts{TotalNodeCount, Allocator};

  Library::Vector<Foundation::Size> NodeSizes{TotalNodeCount,
                                              Foundation::Size{}, Allocator};

  Library::Vector<Foundation::Size> SectionNodeStarts{Sections.Size(),
                                                      Allocator};

  const auto LabelCount =
      static_cast<Foundation::Size>(FindMaximumLabelId(Sections));

  Library::Vector<Foundation::Uint64> LabelAddresses{LabelCount, InvalidAddress,
                                                     Allocator};

  if (SectionLayouts.Size() != Sections.Size() ||
      NodeLayouts.Size() != TotalNodeCount ||
      NodeSizes.Size() != TotalNodeCount ||
      SectionNodeStarts.Size() != Sections.Size() ||
      LabelAddresses.Size() != LabelCount) {
    return Library::MakeUnexpected(AssemblyError::AllocationFailure);
  }

  Foundation::Size FlatIndex{};

  for (Foundation::Size SectionIndex = 0; SectionIndex < Sections.Size();
       ++SectionIndex) {
    SectionNodeStarts[SectionIndex] = FlatIndex;

    for (const auto &NodeValue : Sections[SectionIndex].Nodes()) {
      switch (NodeValue.Type()) {
      case NodeType::Instruction:
        NodeSizes[FlatIndex] =
            InstructionSet::DecodedInstruction::MaximumLength;
        break;
      case NodeType::Data:
        NodeSizes[FlatIndex] = NodeValue.Data().Size();
        break;
      default:
        NodeSizes[FlatIndex] = 0;
        break;
      }

      ++FlatIndex;
    }
  }

  auto ComputeLayout = [&]() -> Foundation::Size {
    for (auto &Address : LabelAddresses) {
      Address = InvalidAddress;
    }

    Foundation::Size Cursor{};
    Foundation::Size NodeIndex{};

    for (Foundation::Size SectionIndex = 0; SectionIndex < Sections.Size();
         ++SectionIndex) {
      const auto &SectionValue = Sections[SectionIndex];

      Cursor = Foundation::Bit::AlignUp(Cursor, SectionValue.Alignment());

      auto &SectionLayoutValue = SectionLayouts[SectionIndex];

      SectionLayoutValue.Offset = Cursor;
      SectionLayoutValue.FirstNode = NodeIndex;

      for (const auto &NodeValue : SectionValue.Nodes()) {
        auto &Layout = NodeLayouts[NodeIndex];
        Layout.Offset = Cursor;

        switch (NodeValue.Type()) {
        case NodeType::Label:
          Layout.Size = 0;

          if (NodeValue.LabelValue().IsValid() &&
              NodeValue.LabelValue().Id() < LabelAddresses.Size()) {
            LabelAddresses[NodeValue.LabelValue().Id()] = BaseAddress + Cursor;
          }
          break;

        case NodeType::Align: {
          const auto Aligned =
              Foundation::Bit::AlignUp(Cursor, NodeValue.Alignment());

          Layout.Size = Aligned - Cursor;
          break;
        }

        default:
          Layout.Size = NodeSizes[NodeIndex];
          break;
        }

        Cursor += Layout.Size;
        ++NodeIndex;
      }

      SectionLayoutValue.Size = Cursor - SectionLayoutValue.Offset;
    }

    return Cursor;
  };

  constexpr Foundation::Size MaximumPasses = 32;
  Foundation::Bool Converged{};

  for (Foundation::Size Pass = 0; Pass < MaximumPasses; ++Pass) {
    ComputeLayout();

    Foundation::Bool Changed{};
    Foundation::Size NodeIndex{};

    for (const auto &SectionValue : Sections) {
      for (const auto &NodeValue : SectionValue.Nodes()) {
        if (NodeValue.Type() != NodeType::Instruction) {
          ++NodeIndex;
          continue;
        }

        auto Resolved = ResolveInstruction(NodeValue, LabelAddresses.View());

        if (!Resolved) {
          return Library::MakeUnexpected(Resolved.Error());
        }

        auto EncodedSize = Encoder_.EncodeSize(
            Resolved.Value(), BaseAddress + NodeLayouts[NodeIndex].Offset);

        if (!EncodedSize) {
          return Library::MakeUnexpected(AssemblyError::EncodingFailure);
        }

        if (NodeSizes[NodeIndex] != EncodedSize.Value()) {
          NodeSizes[NodeIndex] = EncodedSize.Value();
          Changed = true;
        }

        ++NodeIndex;
      }
    }

    if (!Changed) {
      Converged = true;
      break;
    }
  }

  if (!Converged) {
    return Library::MakeUnexpected(AssemblyError::LayoutDidNotConverge);
  }

  const auto TotalSize = ComputeLayout();

  AssembledCode Result{Allocator};
  Result.BaseAddress_ = BaseAddress;

  if (!Result.Buffer_.Resize(TotalSize, 0)) {
    return Library::MakeUnexpected(AssemblyError::AllocationFailure);
  }

  if (!Result.Sections_.Reserve(Sections.Size())) {
    return Library::MakeUnexpected(AssemblyError::AllocationFailure);
  }

  for (Foundation::Size SectionIndex = 0; SectionIndex < Sections.Size();
       ++SectionIndex) {
    if (!Result.Sections_.PushBack(AssembledSection{
            Sections[SectionIndex].Id().Value(), Sections[SectionIndex].Kind(),
            SectionLayouts[SectionIndex].Offset,
            SectionLayouts[SectionIndex].Size,
            Sections[SectionIndex].Alignment()})) {
      return Library::MakeUnexpected(AssemblyError::AllocationFailure);
    }
  }

  Disassembly::Decoder Decoder{};
  Foundation::Size NodeIndex{};

  for (const auto &SectionValue : Sections) {
    for (const auto &NodeValue : SectionValue.Nodes()) {
      const auto &Layout = NodeLayouts[NodeIndex];

      switch (NodeValue.Type()) {
      case NodeType::Instruction: {
        auto Resolved = ResolveInstruction(NodeValue, LabelAddresses.View());

        if (!Resolved) {
          return Library::MakeUnexpected(Resolved.Error());
        }

        auto Destination =
            Result.Buffer_.Bytes().Subspan(Layout.Offset, Layout.Size);

        auto Encoded = Encoder_.Encode(Resolved.Value(), Destination,
                                       BaseAddress + Layout.Offset);

        if (!Encoded || Encoded.Value() != Layout.Size) {
          return Library::MakeUnexpected(AssemblyError::EncodingFailure);
        }

        auto Decoded = Decoder.Decode(
            Library::Span<const Foundation::Byte>{Destination.Data(),
                                                  Destination.Size()},
            BaseAddress + Layout.Offset);

        if (Decoded) {
          const auto &Raw = Decoded->RawEncoding();

          if (NodeValue.HasLabelTarget()) {
            for (const auto &Immediate : Raw.Immediates) {
              if (Immediate.IsRelative && Immediate.SizeInBits) {
                if (!Result.Relocations_.PushBack(Relocation{
                        Layout.Offset + Immediate.Offset,
                        RelativeRelocationKind(Immediate.SizeInBits),
                        RelocationTargetKind::InternalLabel,
                        LabelAddresses[NodeValue.TargetLabel().Id()], 0})) {
                  return Library::MakeUnexpected(
                      AssemblyError::AllocationFailure);
                }

                break;
              }
            }
          } else {
            Foundation::Size ImmediateIndex{};

            for (const auto &Operand : Resolved->Operands()) {
              if (Operand.Type() == InstructionSet::OperandType::Immediate) {
                const auto &Immediate = Operand.ImmediateValue();

                if (Immediate.HasAbsoluteAddress &&
                    ImmediateIndex < Raw.Immediates.Size()) {
                  const auto &RawImmediate = Raw.Immediates[ImmediateIndex];

                  if (RawImmediate.SizeInBits) {
                    const auto Kind =
                        Immediate.IsRelative
                            ? RelativeRelocationKind(RawImmediate.SizeInBits)
                            : AbsoluteRelocationKind(RawImmediate.SizeInBits);

                    if (!Result.Relocations_.PushBack(Relocation{
                            Layout.Offset + RawImmediate.Offset, Kind,
                            RelocationTargetKind::AbsoluteAddress,
                            Immediate.AbsoluteAddress, 0})) {
                      return Library::MakeUnexpected(
                          AssemblyError::AllocationFailure);
                    }
                  }
                }

                ++ImmediateIndex;
              } else if (Operand.Type() ==
                             InstructionSet::OperandType::Memory &&
                         Operand.MemoryValue().HasAbsoluteAddress &&
                         Raw.DisplacementSizeInBits) {
                const auto &Memory = Operand.MemoryValue();
                RelocationKind Kind{};
                Foundation::Bool Relocatable{};

                if (Memory.Base == InstructionSet::Registers::Rip ||
                    Memory.Base == InstructionSet::Registers::Eip) {
                  Kind = RelocationKind::RipRelative32;
                  Relocatable = true;
                } else if (!Memory.Base.IsValid() && !Memory.Index.IsValid()) {
                  Kind = AbsoluteRelocationKind(Raw.DisplacementSizeInBits);
                  Relocatable = true;
                }

                if (Relocatable &&
                    !Result.Relocations_.PushBack(
                        Relocation{Layout.Offset + Raw.DisplacementOffset, Kind,
                                   RelocationTargetKind::AbsoluteAddress,
                                   Memory.AbsoluteAddress, 0})) {
                  return Library::MakeUnexpected(
                      AssemblyError::AllocationFailure);
                }
              }
            }
          }
        }

        break;
      }

      case NodeType::Data:
        for (Foundation::Size Index = 0; Index < NodeValue.Data().Size();
             ++Index) {
          Result.Buffer_.Bytes()[Layout.Offset + Index] =
              NodeValue.Data()[Index];
        }
        break;

      case NodeType::Align:
        for (Foundation::Size Index = 0; Index < Layout.Size; ++Index) {
          Result.Buffer_.Bytes()[Layout.Offset + Index] = NodeValue.Fill();
        }
        break;

      default:
        break;
      }

      ++NodeIndex;
    }
  }

  return Result;
}
} // namespace UEFIpp::Architecture::X64::Assembly

namespace UEFIpp::Architecture::X64::Assembly {
Assembler::Assembler(Memory::AllocatorStub Allocator) : Program_(Allocator) {}

auto Assembler::GetProgram() noexcept -> Program & { return Program_; }

auto Assembler::GetProgram() const noexcept -> const Program & {
  return Program_;
}

auto Assembler::CreateSection(SectionKind Kind, Library::StringView Name,
                              Foundation::Size Alignment)
    -> Library::Expected<SectionId, AssemblyError> {
  return Program_.CreateSection(Kind, Name, Alignment);
}

auto Assembler::SwitchSection(SectionId Section) noexcept -> Foundation::Bool {
  return Program_.SwitchSection(Section);
}

auto Assembler::CreateLabel() -> Library::Expected<Label, AssemblyError> {
  return Program_.CreateLabel();
}

auto Assembler::Bind(Label Value)
    -> Library::Expected<Foundation::Void, AssemblyError> {
  return Program_.Bind(Value);
}

auto Assembler::Emit(const InstructionSet::Instruction &Value)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  return Program_.AppendInstruction(Value);
}

auto Assembler::EmitData(Library::Span<const Foundation::Byte> Data)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  return Program_.AppendData(Data);
}

auto Assembler::Align(Foundation::Size Alignment, Foundation::Byte Fill)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  return Program_.Align(Alignment, Fill);
}
} // namespace UEFIpp::Architecture::X64::Assembly
