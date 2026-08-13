#include <UEFIpp/IO/Transport/File.hpp>

namespace UEFIpp::IO {
auto FileSource::Open(const FileSystem::Path &Path,
                      FileSystem::FileOpenMode Mode) -> Result<> {
  auto Closed = Close();
  if (!Closed) {
    return Closed;
  }
  if (!File_.Open(Path, Mode)) {
    return Failure(Error::FromStatus(Operation::Open,
                                     File_.LastStatus().Code()));
  }
  Open_ = true;
  auto Position = Seek(0);
  if (!Position) {
    return Failure(Position.Error());
  }
  return {};
}

auto FileSource::ReadSome(Library::Span<Foundation::Byte> Bytes)
    -> Result<ReadCount> {
  if (!Open_ || !File_.Valid()) {
    return Failure(Error::Semantic(ErrorCode::Closed, Operation::Read));
  }
  if (Bytes.Empty()) {
    return ReadCount{};
  }

  auto &Handle = File_.Handle();
  auto Count = Foundation::Cast::Auto<Foundation::UintN>(Bytes.Size());
  const auto Status = Handle.ReadFile(Bytes.Data(), Count);
  if (UEFI::IsError(Status)) {
    return Failure(Error::FromStatus(Operation::Read, Status, 0, Count));
  }
  return ReadCount{Count, Count == 0};
}

auto FileSource::Seek(Foundation::Uint64 Position)
    -> Result<Foundation::Uint64> {
  if (!Open_ || !File_.Valid()) {
    return Failure(Error::Semantic(ErrorCode::Closed, Operation::Seek));
  }
  const auto Status = File_.Handle().SetCurrentPosition(Position);
  if (UEFI::IsError(Status)) {
    return Failure(Error::FromStatus(Operation::Seek, Status, Position));
  }
  return Position;
}

auto FileSource::Tell() -> Result<Foundation::Uint64> {
  if (!Open_ || !File_.Valid()) {
    return Failure(Error::Semantic(ErrorCode::Closed, Operation::Seek));
  }
  Foundation::Uint64 Position{};
  const auto Status = File_.Handle().CurrentPosition(Position);
  if (UEFI::IsError(Status)) {
    return Failure(Error::FromStatus(Operation::Seek, Status));
  }
  return Position;
}

auto FileSource::Close() -> Result<> {
  if (!Open_ && !File_.Valid()) {
    return {};
  }
  Open_ = false;
  if (!File_.Close()) {
    return Failure(Error::FromStatus(Operation::Close,
                                     File_.LastStatus().Code()));
  }
  return {};
}

auto FileSink::Open(const FileSystem::Path &Path,
                    FileSystem::FileOpenMode Mode, FileWriteMode WriteMode)
    -> Result<> {
  auto Closed = Close();
  if (!Closed) {
    return Closed;
  }
  if (!File_.Open(Path, Mode)) {
    return Failure(Error::FromStatus(Operation::Open,
                                     File_.LastStatus().Code()));
  }
  Open_ = true;
  WriteMode_ = WriteMode;
  return {};
}

auto FileSink::WriteSome(Library::Span<const Foundation::Byte> Bytes)
    -> Result<Foundation::Size> {
  if (!Open_ || !File_.Valid()) {
    return Failure(Error::Semantic(ErrorCode::Closed, Operation::Write));
  }
  if (Bytes.Empty()) {
    return Foundation::Size{};
  }
  if (WriteMode_ == FileWriteMode::Append) {
    const auto PositionStatus =
        File_.Handle().SetCurrentPosition(Foundation::Uint64(-1));
    if (UEFI::IsError(PositionStatus)) {
      return Failure(Error::FromStatus(Operation::Seek, PositionStatus));
    }
  }
  auto Count = Foundation::Cast::Auto<Foundation::UintN>(Bytes.Size());
  const auto Status = File_.Handle().WriteFile(Bytes.Data(), Count);
  if (UEFI::IsError(Status)) {
    return Failure(Error::FromStatus(Operation::Write, Status, 0, Count));
  }
  return Count;
}

auto FileSink::WriteAt(Foundation::Uint64 Position,
                       Library::Span<const Foundation::Byte> Bytes)
    -> Result<Foundation::Size> {
  auto PositionResult = Seek(Position);
  if (!PositionResult) {
    return Failure(PositionResult.Error());
  }
  const auto OldMode = WriteMode_;
  WriteMode_ = FileWriteMode::Current;
  auto Result = WriteSome(Bytes);
  WriteMode_ = OldMode;
  return Result;
}

auto FileSink::Seek(Foundation::Uint64 Position)
    -> Result<Foundation::Uint64> {
  if (!Open_ || !File_.Valid()) {
    return Failure(Error::Semantic(ErrorCode::Closed, Operation::Seek));
  }
  const auto Status = File_.Handle().SetCurrentPosition(Position);
  if (UEFI::IsError(Status)) {
    return Failure(Error::FromStatus(Operation::Seek, Status, Position));
  }
  return Position;
}

auto FileSink::Tell() -> Result<Foundation::Uint64> {
  if (!Open_ || !File_.Valid()) {
    return Failure(Error::Semantic(ErrorCode::Closed, Operation::Seek));
  }
  Foundation::Uint64 Position{};
  const auto Status = File_.Handle().CurrentPosition(Position);
  if (UEFI::IsError(Status)) {
    return Failure(Error::FromStatus(Operation::Seek, Status));
  }
  return Position;
}

auto FileSink::Truncate(Foundation::Uint64 Size) -> Result<> {
  if (!Open_ || !File_.Valid()) {
    return Failure(Error::Semantic(ErrorCode::Closed, Operation::Truncate));
  }
  if (!File_.Resize(Size)) {
    return Failure(Error::FromStatus(Operation::Truncate,
                                     File_.LastStatus().Code()));
  }
  return {};
}

auto FileSink::Flush() -> Result<> {
  if (!Open_ || !File_.Valid()) {
    return Failure(Error::Semantic(ErrorCode::Closed, Operation::Flush));
  }
  const auto Status = File_.Handle().FlushFile();
  if (UEFI::IsError(Status)) {
    return Failure(Error::FromStatus(Operation::Flush, Status));
  }
  return {};
}

auto FileSink::Close() -> Result<> {
  if (!Open_ && !File_.Valid()) {
    return {};
  }
  Open_ = false;
  if (!File_.Close()) {
    return Failure(Error::FromStatus(Operation::Close,
                                     File_.LastStatus().Code()));
  }
  return {};
}
} // namespace UEFIpp::IO
