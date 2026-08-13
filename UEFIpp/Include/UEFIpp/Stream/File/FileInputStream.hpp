#pragma once

#include <UEFIpp/Memory/AllocatorStub.hpp>

#include <UEFIpp/FileSystem/File.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/String/String.hpp>

namespace UEFIpp::Stream {
class FileInputStream {
public:
  FileInputStream() = default;
  explicit FileInputStream(Memory::AllocatorStub Allocator) noexcept;

  explicit FileInputStream(
      const FileSystem::Path &Path,
      FileSystem::FileOpenMode Mode = FileSystem::FileOpenMode::Read,
      Memory::AllocatorStub Allocator = {});

  explicit FileInputStream(const FileSystem::Path &Path,
                           Memory::AllocatorStub Allocator);

  FileInputStream(const FileInputStream &) = delete;
  auto operator=(const FileInputStream &) -> FileInputStream & = delete;

  FileInputStream(FileInputStream &&Other) noexcept;
  auto operator=(FileInputStream &&Other) noexcept -> FileInputStream &;
  ~FileInputStream();

  [[nodiscard]] auto
  Open(const FileSystem::Path &Path,
       FileSystem::FileOpenMode Mode = FileSystem::FileOpenMode::Read)
      -> Foundation::Bool;

  [[nodiscard]] auto Close() -> Foundation::Bool;
  [[nodiscard]] auto Valid() const -> Foundation::Bool;
  [[nodiscard]] explicit operator Foundation::Bool() const;
  [[nodiscard]] auto Empty() const -> Foundation::Bool;
  [[nodiscard]] auto Eof() const -> Foundation::Bool;
  [[nodiscard]] auto Size() const -> Foundation::Uint64;
  [[nodiscard]] auto Tell() const -> Foundation::Uint64;
  [[nodiscard]] auto Seek(Foundation::Uint64 Position) -> Foundation::Bool;
  auto Rewind() -> Foundation::Void;
  [[nodiscard]] auto Peek(Foundation::Char &Character) const
      -> Foundation::Bool;
  [[nodiscard]] auto Get(Foundation::Char &Character) -> Foundation::Bool;
  [[nodiscard]] auto Read(Foundation::Uint8 *Data, Foundation::Uint64 Length,
                          Foundation::Uint64 &BytesRead) -> Foundation::Bool;
  [[nodiscard]] auto ReadLine(Library::String &Line) -> Foundation::Bool;
  [[nodiscard]] auto ReadToken(Library::String &Token) -> Foundation::Bool;
  [[nodiscard]] auto File() -> FileSystem::File &;
  [[nodiscard]] auto File() const -> const FileSystem::File &;
  [[nodiscard]] auto Buffer() const
      -> const Library::Vector<Foundation::Uint8> &;
  [[nodiscard]] auto Allocator() const noexcept -> Memory::AllocatorStub;
  auto operator>>(Library::String &String) -> FileInputStream &;
  auto operator>>(Foundation::Char &Character) -> FileInputStream &;

private:
  [[nodiscard]] static auto IsWhitespace(Foundation::Char Character)
      -> Foundation::Bool;
  auto SkipWhitespace() -> Foundation::Void;

  FileSystem::File File_{};
  Library::Vector<Foundation::Uint8> Buffer_{};
  Foundation::Size Position_{};
  Foundation::Bool Open_{};
};

[[nodiscard]] auto GetLine(FileInputStream &Stream, Library::String &Line)
    -> Foundation::Bool;
} // namespace UEFIpp::Stream
