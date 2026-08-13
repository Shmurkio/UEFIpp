#include <UEFIpp/Architecture/X64/Assembly/CodeBuffer.hpp>

namespace UEFIpp::Architecture::X64::Assembly {
auto CodeBuffer::Reserve(Foundation::Size Capacity) -> Foundation::Bool {
  return Bytes_.Reserve(Capacity);
}

auto CodeBuffer::Resize(Foundation::Size Size, Foundation::Byte Fill)
    -> Foundation::Bool {
  return Bytes_.Resize(Size, Fill);
}

auto CodeBuffer::Append(Library::Span<const Foundation::Byte> Bytes)
    -> Foundation::Bool {
  return Bytes_.Append(Bytes);
}

auto CodeBuffer::AppendByte(Foundation::Byte Value) -> Foundation::Bool {
  return Bytes_.PushBack(Value);
}

auto CodeBuffer::Align(Foundation::Size Alignment, Foundation::Byte Fill)
    -> Foundation::Bool {
  if (!Alignment || !Foundation::Bit::IsPowerOfTwo(Alignment)) {
    return false;
  }

  const auto Aligned = Foundation::Bit::AlignUp(Bytes_.Size(), Alignment);

  return Bytes_.Resize(Aligned, Fill);
}

auto CodeBuffer::Bytes() noexcept -> Library::Span<Foundation::Byte> {
  return Bytes_.View();
}

auto CodeBuffer::Bytes() const noexcept
    -> Library::Span<const Foundation::Byte> {
  return Bytes_.View();
}

auto CodeBuffer::Data() noexcept -> Foundation::Byte * { return Bytes_.Data(); }

auto CodeBuffer::Data() const noexcept -> const Foundation::Byte * {
  return Bytes_.Data();
}

auto CodeBuffer::Size() const noexcept -> Foundation::Size {
  return Bytes_.Size();
}
} // namespace UEFIpp::Architecture::X64::Assembly
