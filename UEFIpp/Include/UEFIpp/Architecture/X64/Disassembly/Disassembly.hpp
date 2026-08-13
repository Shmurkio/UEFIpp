#pragma once

#include <UEFIpp/Architecture/X64/Instruction/DecodedInstruction.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Memory/AllocatorStub.hpp>

namespace UEFIpp::Architecture::X64::Disassembly {
class Disassembly {
public:
  constexpr explicit Disassembly(Memory::AllocatorStub Allocator = {}) noexcept
      : Instructions_(Allocator) {}

  [[nodiscard]] auto Instructions() noexcept
      -> Library::Span<InstructionSet::DecodedInstruction>;

  [[nodiscard]] auto Instructions() const noexcept
      -> Library::Span<const InstructionSet::DecodedInstruction>;

  [[nodiscard]] constexpr auto BaseAddress() const noexcept
      -> Foundation::Uint64 {
    return BaseAddress_;
  }

  [[nodiscard]] constexpr auto SizeInBytes() const noexcept
      -> Foundation::Size {
    return SizeInBytes_;
  }

  [[nodiscard]] constexpr auto Allocator() const noexcept
      -> Memory::AllocatorStub {
    return Instructions_.Allocator();
  }

private:
  friend class Disassembler;

  Library::Vector<InstructionSet::DecodedInstruction> Instructions_;
  Foundation::Uint64 BaseAddress_{};
  Foundation::Size SizeInBytes_{};
};
} // namespace UEFIpp::Architecture::X64::Disassembly
