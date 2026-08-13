#include <UEFIpp/Protocols/File.hpp>

namespace UEFIpp::Protocols {
auto File::OpenFile(File *&NewHandle, const Foundation::WChar *FileName,
                    Foundation::Uint64 OpenMode, Foundation::Uint64 Attributes)
    -> UEFI::StatusCode {
  return Open(this, &NewHandle, FileName, OpenMode, Attributes);
}

auto File::CloseFile() -> UEFI::StatusCode { return Close(this); }

auto File::DeleteFile() -> UEFI::StatusCode { return Delete(this); }

auto File::ReadFile(Foundation::Void *Buffer, Foundation::UintN &BufferSize)
    -> UEFI::StatusCode {
  return Read(this, &BufferSize, Buffer);
}

auto File::WriteFile(const Foundation::Void *Buffer,
                     Foundation::UintN &BufferSize) -> UEFI::StatusCode {
  return Write(this, &BufferSize, Buffer);
}

auto File::CurrentPosition(Foundation::Uint64 &Position) -> UEFI::StatusCode {
  return GetPosition(this, &Position);
}

auto File::SetCurrentPosition(Foundation::Uint64 Position) -> UEFI::StatusCode {
  return SetPosition(this, Position);
}

auto File::QueryInfo(UEFI::Guid &InformationType, Foundation::Void *Buffer,
                     Foundation::UintN &BufferSize) -> UEFI::StatusCode {
  return GetInfo(this, &InformationType, &BufferSize, Buffer);
}

auto File::UpdateInfo(UEFI::Guid &InformationType, Foundation::Void *Buffer,
                      Foundation::UintN BufferSize) -> UEFI::StatusCode {
  return SetInfo(this, &InformationType, BufferSize, Buffer);
}

auto File::FlushFile() -> UEFI::StatusCode { return Flush(this); }
} // namespace UEFIpp::Protocols
