#include <UEFIpp/Architecture/X64/Assembly/Section.hpp>

namespace UEFIpp::Architecture::X64::Assembly {
Section::Section(SectionId Id, SectionKind Kind, Foundation::Size Alignment,
                 Memory::AllocatorStub Allocator)
    : Id_(Id), Kind_(Kind), Name_(Allocator),
      Alignment_(Alignment ? Alignment : 1), Nodes_(Allocator) {}

auto Section::SetName(Library::StringView Name) -> Foundation::Bool {
  return Name_.Assign(Name);
}

auto Section::Name() const noexcept -> Library::StringView {
  return Name_.View();
}

auto Section::Nodes() noexcept -> Library::Span<Node> { return Nodes_.View(); }

auto Section::Nodes() const noexcept -> Library::Span<const Node> {
  return Nodes_.View();
}

auto Section::Append(Node &&Value) -> Foundation::Bool {
  return Nodes_.PushBack(Foundation::Utility::Move(Value));
}
} // namespace UEFIpp::Architecture::X64::Assembly
