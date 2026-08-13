#include <UEFIpp/Architecture/X64/Assembly/AssembledCode.hpp>

namespace UEFIpp::Architecture::X64::Assembly {
auto AssembledCode::Bytes() noexcept -> Library::Span<Foundation::Byte> {
  return Buffer_.Bytes();
}

auto AssembledCode::Bytes() const noexcept
    -> Library::Span<const Foundation::Byte> {
  return Buffer_.Bytes();
}

auto AssembledCode::Sections() const noexcept
    -> Library::Span<const AssembledSection> {
  return Sections_.View();
}

auto AssembledCode::Relocations() const noexcept
    -> Library::Span<const Relocation> {
  return Relocations_.View();
}
} // namespace UEFIpp::Architecture::X64::Assembly
