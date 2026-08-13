#include <UEFIpp/Stream/File/FileOutputStream.hpp>

namespace UEFIpp::Stream {
FileOutputStream::FileOutputStream(Memory::AllocatorStub Allocator) noexcept
    : File_(Allocator) {}

FileOutputStream::FileOutputStream(const FileSystem::Path &Path,
                                   FileSystem::FileOpenMode Mode,
                                   Foundation::Bool AutoFlush,
                                   Memory::AllocatorStub Allocator)
    : File_(Allocator) {
  (void)Open(Path, Mode, AutoFlush);
}

FileOutputStream::FileOutputStream(const FileSystem::Path &Path,
                                   Memory::AllocatorStub Allocator)
    : FileOutputStream(Path, FileSystem::FileOpenMode::CreateReadWrite, false,
                       Allocator) {}

FileOutputStream::FileOutputStream(const FileSystem::Path &Path,
                                   FileSystem::FileOpenMode Mode,
                                   Memory::AllocatorStub Allocator)
    : FileOutputStream(Path, Mode, false, Allocator) {}

FileOutputStream::~FileOutputStream() {
  (void)Flush();
  (void)Close();
}

auto FileOutputStream::Open(const FileSystem::Path &Path,
                            FileSystem::FileOpenMode Mode,
                            Foundation::Bool AutoFlush) -> Foundation::Bool {
  if (!Close()) {
    return false;
  }

  if (!File_.Open(Path, Mode)) {
    Sink_.Reset(nullptr);
    return false;
  }

  Sink_.Reset(&File_, AutoFlush);
  return true;
}

auto FileOutputStream::Close() -> Foundation::Bool {
  Sink_.Reset(nullptr);
  return File_.Close();
}

auto FileOutputStream::Flush() -> Foundation::Bool { return File_.Flush(); }

auto FileOutputStream::Valid() const -> Foundation::Bool {
  return File_.Valid();
}

auto FileOutputStream::Allocator() const noexcept -> Memory::AllocatorStub {
  return File_.Allocator();
}

auto FileOutputStream::Write(const Foundation::Void *Data,
                             Foundation::Size Size) -> Foundation::Bool {
  if (!Data && Size != 0) {
    return false;
  }

  return File_.Write(Foundation::Cast::Auto<const Foundation::Uint8 *>(Data),
                     Foundation::Cast::Auto<Foundation::Uint64>(Size));
}

auto FileOutputStream::Write(Library::Span<const Foundation::Byte> Data)
    -> Foundation::Bool {
  return Write(Data.Data(), Data.SizeInBytes());
}

auto FileOutputStream::Write(const Library::Vector<Foundation::Byte> &Data)
    -> Foundation::Bool {
  return Write({Data.Data(), Data.Size()});
}
} // namespace UEFIpp::Stream
