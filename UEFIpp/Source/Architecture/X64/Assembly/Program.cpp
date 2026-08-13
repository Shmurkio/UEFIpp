#include <UEFIpp/Architecture/X64/Assembly/Program.hpp>

namespace UEFIpp::Architecture::X64::Assembly {
Program::Program(Memory::AllocatorStub Allocator)
    : Allocator_(Allocator), Sections_(Allocator), Labels_(Allocator) {
  if (auto *Section = Sections_.EmplaceBack(SectionId{0}, SectionKind::Text, 16,
                                            Allocator_)) {
    if (Section->SetName(Library::StringView{".text"})) {
      CurrentSection_ = Section->Id();
    } else {
      (void)Sections_.PopBack();
    }
  }
}

auto Program::CreateSection(SectionKind Kind, Library::StringView Name,
                            Foundation::Size Alignment)
    -> Library::Expected<SectionId, AssemblyError> {
  if (!Alignment || !Foundation::Bit::IsPowerOfTwo(Alignment) ||
      Sections_.Size() >=
          static_cast<Foundation::Size>(SectionId::InvalidValue)) {
    return Library::MakeUnexpected(AssemblyError::InvalidArgument);
  }

  const SectionId Id{static_cast<Foundation::Uint32>(Sections_.Size())};

  auto *Created = Sections_.EmplaceBack(Id, Kind, Alignment, Allocator_);

  if (!Created) {
    return Library::MakeUnexpected(AssemblyError::AllocationFailure);
  }

  if (!Created->SetName(Name)) {
    (void)Sections_.PopBack();
    return Library::MakeUnexpected(AssemblyError::AllocationFailure);
  }

  return Id;
}

auto Program::SwitchSection(SectionId Section) noexcept -> Foundation::Bool {
  if (!SectionById(Section)) {
    return false;
  }

  CurrentSection_ = Section;
  return true;
}

auto Program::CreateLabel() -> Library::Expected<Label, AssemblyError> {
  if (Labels_.Size() >= static_cast<Foundation::Size>(Label::InvalidId)) {
    return Library::MakeUnexpected(AssemblyError::AllocationFailure);
  }

  const Label Value{static_cast<Foundation::Uint32>(Labels_.Size())};

  if (!Labels_.PushBack(LabelState{})) {
    return Library::MakeUnexpected(AssemblyError::AllocationFailure);
  }

  return Value;
}

auto Program::Bind(Label Value)
    -> Library::Expected<Foundation::Void, AssemblyError> {
  if (!IsLabelValid(Value)) {
    return Library::MakeUnexpected(AssemblyError::InvalidLabel);
  }

  auto &State = Labels_[Value.Id()];

  if (State.Bound) {
    return Library::MakeUnexpected(AssemblyError::LabelAlreadyBound);
  }

  auto *Current = SectionById(CurrentSection_);

  if (!Current) {
    return Library::MakeUnexpected(AssemblyError::InvalidSection);
  }

  if (!Current->Append(Node::LabelNode(Value, Allocator_))) {
    return Library::MakeUnexpected(AssemblyError::AllocationFailure);
  }

  State.Bound = true;
  State.Section = CurrentSection_;

  return {};
}

auto Program::AppendInstruction(const InstructionSet::Instruction &Value)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  auto *Current = SectionById(CurrentSection_);

  if (!Current) {
    return Library::MakeUnexpected(AssemblyError::InvalidSection);
  }

  const auto Index = Current->Nodes().Size();

  if (!Current->Append(Node::InstructionNode(Value, Allocator_))) {
    return Library::MakeUnexpected(AssemblyError::AllocationFailure);
  }

  return Index;
}

auto Program::AppendInstruction(const InstructionSet::Instruction &Value,
                                Label Target,
                                Foundation::Uint8 TargetOperandIndex)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  if (!IsLabelValid(Target)) {
    return Library::MakeUnexpected(AssemblyError::InvalidLabel);
  }

  auto *Current = SectionById(CurrentSection_);

  if (!Current) {
    return Library::MakeUnexpected(AssemblyError::InvalidSection);
  }

  if (TargetOperandIndex >= Value.OperandCount()) {
    return Library::MakeUnexpected(AssemblyError::InvalidArgument);
  }

  auto NodeValue = Node::InstructionNode(Value, Allocator_);

  NodeValue.SetLabelTarget(Target, TargetOperandIndex);

  const auto Index = Current->Nodes().Size();

  if (!Current->Append(Foundation::Utility::Move(NodeValue))) {
    return Library::MakeUnexpected(AssemblyError::AllocationFailure);
  }

  return Index;
}

auto Program::AppendData(Library::Span<const Foundation::Byte> Data)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  if (Data.IsNull()) {
    return Library::MakeUnexpected(AssemblyError::InvalidArgument);
  }

  auto *Current = SectionById(CurrentSection_);

  if (!Current) {
    return Library::MakeUnexpected(AssemblyError::InvalidSection);
  }

  auto NodeValue = Node::DataNode(Data, Allocator_);

  if (NodeValue.Data().Size() != Data.Size()) {
    return Library::MakeUnexpected(AssemblyError::AllocationFailure);
  }

  const auto Index = Current->Nodes().Size();

  if (!Current->Append(Foundation::Utility::Move(NodeValue))) {
    return Library::MakeUnexpected(AssemblyError::AllocationFailure);
  }

  return Index;
}

auto Program::Align(Foundation::Size Alignment, Foundation::Byte Fill)
    -> Library::Expected<Foundation::Size, AssemblyError> {
  if (!Alignment || !Foundation::Bit::IsPowerOfTwo(Alignment)) {
    return Library::MakeUnexpected(AssemblyError::InvalidArgument);
  }

  auto *Current = SectionById(CurrentSection_);

  if (!Current) {
    return Library::MakeUnexpected(AssemblyError::InvalidSection);
  }

  const auto Index = Current->Nodes().Size();

  if (!Current->Append(Node::AlignNode(Alignment, Fill, Allocator_))) {
    return Library::MakeUnexpected(AssemblyError::AllocationFailure);
  }

  return Index;
}

auto Program::SectionById(SectionId Id) noexcept -> Section * {
  if (!Id.IsValid() || Id.Value() >= Sections_.Size()) {
    return nullptr;
  }

  return &Sections_[Id.Value()];
}

auto Program::SectionById(SectionId Id) const noexcept -> const Section * {
  if (!Id.IsValid() || Id.Value() >= Sections_.Size()) {
    return nullptr;
  }

  return &Sections_[Id.Value()];
}

auto Program::IsLabelValid(Label Value) const noexcept -> Foundation::Bool {
  return Value.IsValid() && Value.Id() < Labels_.Size();
}

auto Program::IsLabelBound(Label Value) const noexcept -> Foundation::Bool {
  return IsLabelValid(Value) && Labels_[Value.Id()].Bound;
}
} // namespace UEFIpp::Architecture::X64::Assembly

namespace UEFIpp::Architecture::X64::Assembly {
auto Program::Sections() noexcept -> Library::Span<Section> {
  return Sections_.View();
}

auto Program::Sections() const noexcept -> Library::Span<const Section> {
  return Sections_.View();
}
} // namespace UEFIpp::Architecture::X64::Assembly
