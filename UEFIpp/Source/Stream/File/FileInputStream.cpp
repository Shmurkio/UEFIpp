#include <UEFIpp/Stream/File/FileInputStream.hpp>

#include <UEFIpp/Memory/Memory.hpp>

namespace UEFIpp::Stream {
FileInputStream::FileInputStream(Memory::AllocatorStub Allocator) noexcept
    : File_(Allocator), Buffer_(Allocator) {}

FileInputStream::FileInputStream(const FileSystem::Path &Path,
                                 FileSystem::FileOpenMode Mode,
                                 Memory::AllocatorStub Allocator)
    : File_(Allocator), Buffer_(Allocator) {
  (void)Open(Path, Mode);
}

FileInputStream::FileInputStream(const FileSystem::Path &Path,
                                 Memory::AllocatorStub Allocator)
    : FileInputStream(Path, FileSystem::FileOpenMode::Read, Allocator) {}

FileInputStream::FileInputStream(FileInputStream &&Other) noexcept {
  *this = Foundation::Utility::Move(Other);
}

auto FileInputStream::operator=(FileInputStream &&Other) noexcept
    -> FileInputStream & {
  if (this == &Other) {
    return *this;
  }

  (void)Close();
  File_ = Foundation::Utility::Move(Other.File_);
  Buffer_ = Foundation::Utility::Move(Other.Buffer_);
  Position_ = Other.Position_;
  Open_ = Other.Open_;
  Other.Position_ = 0;
  Other.Open_ = false;
  return *this;
}

FileInputStream::~FileInputStream() { (void)Close(); }

auto FileInputStream::Open(const FileSystem::Path &Path,
                           FileSystem::FileOpenMode Mode) -> Foundation::Bool {
  if (!Close()) {
    return false;
  }

  if (!File_.Open(Path, Mode)) {
    return false;
  }

  if (!File_.Read(Buffer_)) {
    (void)File_.Close();
    Buffer_.Clear();
    return false;
  }

  Position_ = 0;
  Open_ = true;
  return true;
}

auto FileInputStream::Close() -> Foundation::Bool {
  Buffer_.Clear();
  Position_ = 0;
  Open_ = false;
  return File_.Close();
}

auto FileInputStream::Valid() const -> Foundation::Bool {
  return Open_ && File_.Valid();
}

FileInputStream::operator Foundation::Bool() const { return Valid(); }

auto FileInputStream::Empty() const -> Foundation::Bool {
  return Buffer_.Empty();
}

auto FileInputStream::Eof() const -> Foundation::Bool {
  return Position_ >= Buffer_.Size();
}

auto FileInputStream::Size() const -> Foundation::Uint64 {
  return Foundation::Cast::Auto<Foundation::Uint64>(Buffer_.Size());
}

auto FileInputStream::Tell() const -> Foundation::Uint64 {
  return Foundation::Cast::Auto<Foundation::Uint64>(Position_);
}

auto FileInputStream::Seek(Foundation::Uint64 Position) -> Foundation::Bool {
  if (!Valid() || Position > Size()) {
    return false;
  }

  Position_ = Foundation::Cast::Auto<Foundation::Size>(Position);
  return true;
}

auto FileInputStream::Rewind() -> Foundation::Void { Position_ = 0; }

auto FileInputStream::Peek(Foundation::Char &Character) const
    -> Foundation::Bool {
  if (!Valid() || Eof()) {
    return false;
  }

  Character =
      Foundation::Cast::Auto<Foundation::Char>(Buffer_.Data()[Position_]);
  return true;
}

auto FileInputStream::Get(Foundation::Char &Character) -> Foundation::Bool {
  if (!Peek(Character)) {
    return false;
  }

  ++Position_;
  return true;
}

auto FileInputStream::Read(Foundation::Uint8 *Data, Foundation::Uint64 Length,
                           Foundation::Uint64 &BytesRead) -> Foundation::Bool {
  BytesRead = 0;

  if (!Valid() || (!Data && Length != 0)) {
    return false;
  }

  if (Length == 0 || Eof()) {
    return true;
  }

  const auto Remaining = Buffer_.Size() - Position_;
  const auto Wanted = Foundation::Cast::Auto<Foundation::Size>(Length);
  const auto Count = Wanted < Remaining ? Wanted : Remaining;
  Memory::Copy(Data, Buffer_.Data() + Position_, Count);
  Position_ += Count;
  BytesRead = Foundation::Cast::Auto<Foundation::Uint64>(Count);
  return true;
}

auto FileInputStream::ReadLine(Library::String &Line) -> Foundation::Bool {
  Line.Clear();

  if (!Valid() || Eof()) {
    return false;
  }

  for (;;) {
    Foundation::Char Character{};

    if (!Get(Character) || Character == '\n') {
      break;
    }

    if (Character == '\r') {
      Foundation::Char Next{};

      if (Peek(Next) && Next == '\n') {
        ++Position_;
      }

      break;
    }

    if (!Line.PushBack(Character)) {
      Line.Clear();
      return false;
    }
  }

  return true;
}

auto FileInputStream::ReadToken(Library::String &Token) -> Foundation::Bool {
  Token.Clear();

  if (!Valid()) {
    return false;
  }

  SkipWhitespace();

  if (Eof()) {
    return false;
  }

  for (;;) {
    Foundation::Char Character{};

    if (!Peek(Character) || IsWhitespace(Character)) {
      break;
    }

    ++Position_;
    if (!Token.PushBack(Character)) {
      Token.Clear();
      return false;
    }
  }

  return !Token.Empty();
}

auto FileInputStream::File() -> FileSystem::File & { return File_; }

auto FileInputStream::File() const -> const FileSystem::File & { return File_; }

auto FileInputStream::Buffer() const
    -> const Library::Vector<Foundation::Uint8> & {
  return Buffer_;
}

auto FileInputStream::Allocator() const noexcept -> Memory::AllocatorStub {
  return Buffer_.Allocator();
}

auto FileInputStream::operator>>(Library::String &String) -> FileInputStream & {
  (void)ReadToken(String);
  return *this;
}

auto FileInputStream::operator>>(Foundation::Char &Character)
    -> FileInputStream & {
  (void)Get(Character);
  return *this;
}

auto FileInputStream::IsWhitespace(Foundation::Char Character)
    -> Foundation::Bool {
  return Character == ' ' || Character == '\t' || Character == '\r' ||
         Character == '\n';
}

auto FileInputStream::SkipWhitespace() -> Foundation::Void {
  while (!Eof()) {
    const auto Character =
        Foundation::Cast::Auto<Foundation::Char>(Buffer_.Data()[Position_]);

    if (!IsWhitespace(Character)) {
      break;
    }

    ++Position_;
  }
}

auto GetLine(FileInputStream &Stream, Library::String &Line)
    -> Foundation::Bool {
  return Stream.ReadLine(Line);
}
} // namespace UEFIpp::Stream
