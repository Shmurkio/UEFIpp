#pragma once

#include <UEFIpp/FileSystem/File.hpp>
#include <UEFIpp/IO/Core/Concepts.hpp>

namespace UEFIpp::IO {
enum class FileWriteMode : Foundation::Uint8 { Current, Append };

class FileSource final {
public:
  constexpr explicit FileSource(Memory::AllocatorStub Allocator = {}) noexcept
      : File_(Allocator) {}

  FileSource(const FileSource &) = delete;
  auto operator=(const FileSource &) -> FileSource & = delete;
  FileSource(FileSource &&) noexcept = default;
  auto operator=(FileSource &&) noexcept -> FileSource & = default;

  [[nodiscard]] auto Open(
      const FileSystem::Path &Path,
      FileSystem::FileOpenMode Mode = FileSystem::FileOpenMode::Read)
      -> Result<>;
  [[nodiscard]] auto ReadSome(Library::Span<Foundation::Byte> Bytes)
      -> Result<ReadCount>;
  [[nodiscard]] auto Seek(Foundation::Uint64 Position)
      -> Result<Foundation::Uint64>;
  [[nodiscard]] auto Tell() -> Result<Foundation::Uint64>;
  [[nodiscard]] auto Close() -> Result<>;
  [[nodiscard]] constexpr auto File() noexcept -> FileSystem::File & {
    return File_;
  }

private:
  FileSystem::File File_{};
  Foundation::Bool Open_{};
};

class FileSink final {
public:
  constexpr explicit FileSink(Memory::AllocatorStub Allocator = {}) noexcept
      : File_(Allocator) {}

  FileSink(const FileSink &) = delete;
  auto operator=(const FileSink &) -> FileSink & = delete;
  FileSink(FileSink &&) noexcept = default;
  auto operator=(FileSink &&) noexcept -> FileSink & = default;

  [[nodiscard]] auto Open(
      const FileSystem::Path &Path,
      FileSystem::FileOpenMode Mode = FileSystem::FileOpenMode::CreateReadWrite,
      FileWriteMode WriteMode = FileWriteMode::Current) -> Result<>;
  [[nodiscard]] auto WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size>;
  [[nodiscard]] auto WriteAt(Foundation::Uint64 Position,
                             Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size>;
  [[nodiscard]] auto Seek(Foundation::Uint64 Position)
      -> Result<Foundation::Uint64>;
  [[nodiscard]] auto Tell() -> Result<Foundation::Uint64>;
  [[nodiscard]] auto Truncate(Foundation::Uint64 Size) -> Result<>;
  [[nodiscard]] auto Flush() -> Result<>;
  [[nodiscard]] auto Close() -> Result<>;
  [[nodiscard]] constexpr auto File() noexcept -> FileSystem::File & {
    return File_;
  }

private:
  FileSystem::File File_{};
  FileWriteMode WriteMode_{FileWriteMode::Current};
  Foundation::Bool Open_{};
};
} // namespace UEFIpp::IO
