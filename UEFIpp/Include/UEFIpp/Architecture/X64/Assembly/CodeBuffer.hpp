#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Memory/AllocatorStub.hpp>

namespace UEFIpp::Architecture::X64::Assembly {
class CodeBuffer {
public:
  constexpr explicit CodeBuffer(Memory::AllocatorStub Allocator = {}) noexcept
      : Bytes_(Allocator) {}

  [[nodiscard]] auto Reserve(Foundation::Size Capacity) -> Foundation::Bool;

  [[nodiscard]] auto Resize(Foundation::Size Size, Foundation::Byte Fill = 0)
      -> Foundation::Bool;

  [[nodiscard]] auto Append(Library::Span<const Foundation::Byte> Bytes)
      -> Foundation::Bool;

  [[nodiscard]] auto AppendByte(Foundation::Byte Value) -> Foundation::Bool;

  [[nodiscard]] auto Align(Foundation::Size Alignment,
                           Foundation::Byte Fill = 0) -> Foundation::Bool;

  [[nodiscard]] auto Bytes() noexcept -> Library::Span<Foundation::Byte>;

  [[nodiscard]] auto Bytes() const noexcept
      -> Library::Span<const Foundation::Byte>;

  [[nodiscard]] auto Data() noexcept -> Foundation::Byte *;

  [[nodiscard]] auto Data() const noexcept -> const Foundation::Byte *;

  [[nodiscard]] auto Size() const noexcept -> Foundation::Size;

  [[nodiscard]] constexpr auto Allocator() const noexcept
      -> Memory::AllocatorStub {
    return Bytes_.Allocator();
  }

private:
  Library::Vector<Foundation::Byte> Bytes_;
};
} // namespace UEFIpp::Architecture::X64::Assembly
