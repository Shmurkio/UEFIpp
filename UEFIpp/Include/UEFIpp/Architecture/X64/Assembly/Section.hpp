#pragma once

#include <UEFIpp/Architecture/X64/Assembly/Node.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Library/String/StringView.hpp>
#include <UEFIpp/Memory/AllocatorStub.hpp>

namespace UEFIpp::Architecture::X64::Assembly {
enum class SectionKind : Foundation::Uint8 { Text, ReadOnlyData, Data, Custom };

class SectionId {
public:
  static constexpr Foundation::Uint32 InvalidValue =
      static_cast<Foundation::Uint32>(-1);

  constexpr SectionId() noexcept = default;

  constexpr explicit SectionId(Foundation::Uint32 Value) noexcept
      : Value_(Value) {}

  [[nodiscard]] constexpr auto Value() const noexcept -> Foundation::Uint32 {
    return Value_;
  }

  [[nodiscard]] constexpr auto IsValid() const noexcept -> Foundation::Bool {
    return Value_ != InvalidValue;
  }

  [[nodiscard]] constexpr auto operator==(const SectionId &) const noexcept
      -> Foundation::Bool = default;

private:
  Foundation::Uint32 Value_{InvalidValue};
};

class Section {
public:
  Section(SectionId Id, SectionKind Kind, Foundation::Size Alignment,
          Memory::AllocatorStub Allocator = {});

  [[nodiscard]] auto SetName(Library::StringView Name) -> Foundation::Bool;

  [[nodiscard]] constexpr auto Id() const noexcept -> SectionId { return Id_; }

  [[nodiscard]] constexpr auto Kind() const noexcept -> SectionKind {
    return Kind_;
  }

  [[nodiscard]] auto Name() const noexcept -> Library::StringView;

  [[nodiscard]] constexpr auto Alignment() const noexcept -> Foundation::Size {
    return Alignment_;
  }

  [[nodiscard]] auto Nodes() noexcept -> Library::Span<Node>;

  [[nodiscard]] auto Nodes() const noexcept -> Library::Span<const Node>;

  [[nodiscard]] auto Append(Node &&Value) -> Foundation::Bool;

  [[nodiscard]] constexpr auto Allocator() const noexcept
      -> Memory::AllocatorStub {
    return Nodes_.Allocator();
  }

private:
  SectionId Id_{};
  SectionKind Kind_{SectionKind::Custom};
  Library::String Name_;
  Foundation::Size Alignment_{1};
  Library::Vector<Node> Nodes_;
};
} // namespace UEFIpp::Architecture::X64::Assembly
