#include <UEFIpp/Architecture/X64/Disassembly/Disassembly.hpp>

namespace UEFIpp::Architecture::X64::Disassembly {
auto Disassembly::Instructions() noexcept
    -> Library::Span<InstructionSet::DecodedInstruction> {
  return Instructions_.View();
}

auto Disassembly::Instructions() const noexcept
    -> Library::Span<const InstructionSet::DecodedInstruction> {
  return Instructions_.View();
}
} // namespace UEFIpp::Architecture::X64::Disassembly
