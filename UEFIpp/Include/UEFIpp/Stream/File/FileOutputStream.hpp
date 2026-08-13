#pragma once

#include <UEFIpp/Memory/AllocatorStub.hpp>

#include <UEFIpp/FileSystem/File.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Stream/Output/FileSink.hpp>
#include <UEFIpp/Stream/Output/Output.hpp>

namespace UEFIpp::Stream {
class FileOutputStream {
public:
  FileOutputStream() = default;
  explicit FileOutputStream(Memory::AllocatorStub Allocator) noexcept;

  explicit FileOutputStream(
      const FileSystem::Path &Path,
      FileSystem::FileOpenMode Mode = FileSystem::FileOpenMode::CreateReadWrite,
      Foundation::Bool AutoFlush = false, Memory::AllocatorStub Allocator = {});

  explicit FileOutputStream(const FileSystem::Path &Path,
                            Memory::AllocatorStub Allocator);

  FileOutputStream(const FileSystem::Path &Path, FileSystem::FileOpenMode Mode,
                   Memory::AllocatorStub Allocator);

  ~FileOutputStream();

  [[nodiscard]] auto Open(
      const FileSystem::Path &Path,
      FileSystem::FileOpenMode Mode = FileSystem::FileOpenMode::CreateReadWrite,
      Foundation::Bool AutoFlush = false) -> Foundation::Bool;

  [[nodiscard]] auto Close() -> Foundation::Bool;
  [[nodiscard]] auto Flush() -> Foundation::Bool;
  [[nodiscard]] auto Valid() const -> Foundation::Bool;
  [[nodiscard]] auto Allocator() const noexcept -> Memory::AllocatorStub;
  [[nodiscard]] auto Write(const Foundation::Void *Data, Foundation::Size Size)
      -> Foundation::Bool;
  [[nodiscard]] auto Write(Library::Span<const Foundation::Byte> Data)
      -> Foundation::Bool;
  [[nodiscard]] auto Write(const Library::Vector<Foundation::Byte> &Data)
      -> Foundation::Bool;

  template <typename TValue>
  auto operator<<(const TValue &Value) -> FileOutputStream & {
    Output_ << Value;
    return *this;
  }

private:
  FileSystem::File File_{};
  FileOutputSink Sink_{&File_};
  Output<FileOutputSink> Output_{Sink_};
};
} // namespace UEFIpp::Stream
