#pragma once

#include <UEFIpp/Executable/Pe/Directory.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Memory/AllocatorStub.hpp>

namespace UEFIpp::Executable::Pe {
struct Export {
  constexpr Export() = default;

  constexpr explicit Export(Memory::AllocatorStub Allocator) noexcept
      : Name(Allocator), Forwarder(Allocator) {}

  Library::String Name{};
  Foundation::Uint16 Ordinal{};
  Foundation::Uint32 Rva{};
  Foundation::Uint64 Address{};
  Foundation::Bool Forwarded{};
  Library::String Forwarder{};
};

class Image {
public:
  using ExportVector = Library::Vector<Export>;

  constexpr Image() = default;

  explicit Image(const Foundation::Void *Base,
                 Memory::AllocatorStub Allocator = {});

  [[nodiscard]] auto IsValid() const -> Foundation::Bool;
  [[nodiscard]] auto Is32() const -> Foundation::Bool;
  [[nodiscard]] auto Is64() const -> Foundation::Bool;
  [[nodiscard]] auto Base() const -> const Foundation::Void *;
  [[nodiscard]] auto BaseAddress() const -> Foundation::Uint64;
  [[nodiscard]] auto DosHeader() const -> const Pe::DosHeader *;
  [[nodiscard]] auto NtHeaders32() const -> const Pe::NtHeaders32 *;
  [[nodiscard]] auto NtHeaders64() const -> const Pe::NtHeaders64 *;
  [[nodiscard]] auto FileHeader() const -> const Pe::FileHeader *;
  [[nodiscard]] auto ImageSize() const -> Foundation::Uint32;
  [[nodiscard]] auto EntryPointRva() const -> Foundation::Uint32;
  [[nodiscard]] auto GetDirectory(DirectoryIndex Index) const -> Directory;
  [[nodiscard]] auto SectionHeaders() const -> const Pe::SectionHeader *;
  [[nodiscard]] auto SectionCount() const -> Foundation::Uint16;
  [[nodiscard]] auto Exports() const -> ExportVector;

  [[nodiscard]] auto RvaToPointer(Foundation::Uint32 Rva) const
      -> const Foundation::Void *;

  template <typename T>
  [[nodiscard]] auto RvaToPointer(Foundation::Uint32 Rva) const -> const T * {
    return Foundation::Cast::Auto<const T *>(RvaToPointer(Rva));
  }

private:
  const Foundation::Uint8 *Base_{};
  Memory::AllocatorStub Allocator_{};

  [[nodiscard]] auto NtSignaturePointer() const -> const Foundation::Uint32 *;
  [[nodiscard]] auto OptionalHeaderMagicPointer() const
      -> const Foundation::Uint16 *;
  [[nodiscard]] auto ReadAsciiString(const Foundation::Char *Text) const
      -> Library::String;
};
} // namespace UEFIpp::Executable::Pe
