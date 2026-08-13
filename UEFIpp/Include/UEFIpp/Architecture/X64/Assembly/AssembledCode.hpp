#pragma once

#include <UEFIpp/Architecture/X64/Assembly/CodeBuffer.hpp>
#include <UEFIpp/Architecture/X64/Assembly/Relocation.hpp>
#include <UEFIpp/Architecture/X64/Assembly/Section.hpp>

namespace UEFIpp::Architecture::X64::Assembly {

struct AssembledSection {
  Foundation::Uint32 Id{};
  SectionKind Kind{};
  Foundation::Size Offset{};
  Foundation::Size Size{};
  Foundation::Size Alignment{1};
};

class AssembledCode {
public:
  constexpr explicit AssembledCode(
      Memory::AllocatorStub Allocator = {}) noexcept
      : Buffer_(Allocator), Sections_(Allocator), Relocations_(Allocator) {}

  [[nodiscard]] auto Bytes() noexcept -> Library::Span<Foundation::Byte>;

  [[nodiscard]] auto Bytes() const noexcept
      -> Library::Span<const Foundation::Byte>;

  [[nodiscard]] auto Sections() const noexcept
      -> Library::Span<const AssembledSection>;

  [[nodiscard]] auto Relocations() const noexcept
      -> Library::Span<const Relocation>;

  [[nodiscard]] constexpr auto BaseAddress() const noexcept
      -> Foundation::Uint64 {
    return BaseAddress_;
  }

  [[nodiscard]] constexpr auto Allocator() const noexcept
      -> Memory::AllocatorStub {
    return Buffer_.Allocator();
  }

private:
  friend class Assembler;

  CodeBuffer Buffer_;
  Library::Vector<AssembledSection> Sections_;
  Library::Vector<Relocation> Relocations_;
  Foundation::Uint64 BaseAddress_{};
};
} // namespace UEFIpp::Architecture::X64::Assembly
