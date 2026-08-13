#pragma once

#include <UEFIpp/Architecture/X64/Assembly/Error.hpp>
#include <UEFIpp/Architecture/X64/Assembly/Section.hpp>
#include <UEFIpp/Library/Containers/Expected.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>

namespace UEFIpp::Architecture::X64::Assembly {
class Program {
private:
  struct LabelState {
    Foundation::Bool Bound{};
    SectionId Section{};
  };

public:
  explicit Program(Memory::AllocatorStub Allocator = {});

  [[nodiscard]] auto CreateSection(SectionKind Kind, Library::StringView Name,
                                   Foundation::Size Alignment = 1)
      -> Library::Expected<SectionId, AssemblyError>;

  [[nodiscard]] auto SwitchSection(SectionId Section) noexcept
      -> Foundation::Bool;

  [[nodiscard]] auto CreateLabel() -> Library::Expected<Label, AssemblyError>;

  [[nodiscard]] auto Bind(Label Value)
      -> Library::Expected<Foundation::Void, AssemblyError>;

  [[nodiscard]] auto AppendInstruction(const InstructionSet::Instruction &Value)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto AppendInstruction(const InstructionSet::Instruction &Value,
                                       Label Target,
                                       Foundation::Uint8 TargetOperandIndex)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto AppendData(Library::Span<const Foundation::Byte> Data)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Align(Foundation::Size Alignment,
                           Foundation::Byte Fill = 0x90)
      -> Library::Expected<Foundation::Size, AssemblyError>;

  [[nodiscard]] auto Sections() noexcept -> Library::Span<Section>;

  [[nodiscard]] auto Sections() const noexcept -> Library::Span<const Section>;

  [[nodiscard]] auto SectionById(SectionId Id) noexcept -> Section *;

  [[nodiscard]] auto SectionById(SectionId Id) const noexcept
      -> const Section *;

  [[nodiscard]] constexpr auto CurrentSection() const noexcept -> SectionId {
    return CurrentSection_;
  }

  [[nodiscard]] auto IsLabelValid(Label Value) const noexcept
      -> Foundation::Bool;

  [[nodiscard]] auto IsLabelBound(Label Value) const noexcept
      -> Foundation::Bool;

  [[nodiscard]] constexpr auto Allocator() const noexcept
      -> Memory::AllocatorStub {
    return Allocator_;
  }

private:
  Memory::AllocatorStub Allocator_{};
  Library::Vector<Section> Sections_;
  Library::Vector<LabelState> Labels_;
  SectionId CurrentSection_{};
};
} // namespace UEFIpp::Architecture::X64::Assembly
