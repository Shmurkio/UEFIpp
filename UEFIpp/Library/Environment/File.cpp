#include "File.hpp"
#include "../../UEFIpp.hpp"
#include "../Memory/Memory.hpp"

static auto ConvertTime(const EFI_TIME& Time) -> File::FILE_TIME
{
    return
    {
        Time.Year,
        Time.Month,
        Time.Day,
        Time.Hour,
        Time.Minute,
        Time.Second,
        Time.Nanosecond,
        Time.TimeZone,
        Time.Daylight
    };
}

static auto GetFileNameFromPath(const String& Path) -> String
{
    auto LastSlash = Path.Find(L'\\');

    for (uint64_t i = 0; i < Path.Size(); ++i)
    {
        auto Index = Path.Find(L'\\', i);

        if (Index < 0)
        {
            break;
        }

        LastSlash = Index;
        i = Cast::To<uint64_t>(Index);
    }

    if (LastSlash < 0)
    {
        return Path;
    }

    return Path.Substring(Cast::To<uint64_t>(LastSlash) + 1, Path.Size());
}

File::~File()
{
    Unload();
}

File::File(File&& Other) noexcept
{
    *this = Memory::Move(Other);
}

auto File::operator=(File&& Other) noexcept -> File&
{
    if (this == &Other)
    {
        return *this;
    }

    Unload();

    Path_ = Memory::Move(Other.Path_);
    Name_ = Memory::Move(Other.Name_);
    Buffer_ = Memory::Move(Other.Buffer_);
    PhysicalSize_ = Other.PhysicalSize_;
    Attribute_ = Other.Attribute_;
    CreateTime_ = Other.CreateTime_;
    LastAccessTime_ = Other.LastAccessTime_;
    ModificationTime_ = Other.ModificationTime_;
    FsHandle_ = Other.FsHandle_;
    Loaded_ = Other.Loaded_;

    Other.PhysicalSize_ = 0;
    Other.Attribute_ = 0;
    Other.FsHandle_ = nullptr;
    Other.Loaded_ = false;

    return *this;
}

auto File::Load(const String& FilePath) -> bool
{
    Unload();

    if (FilePath.Empty())
    {
        return false;
    }

    uint64_t FsCount = 0;
    PEFI_HANDLE FsHandles = nullptr;

    auto Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, nullptr, &FsCount, &FsHandles);

    if (EfiError(Status) || !FsCount || !FsHandles)
    {
        return false;
    }

    for (uint64_t i = 0; i < FsCount; ++i)
    {
        if (LoadFromFileSystem(FsHandles[i], FilePath))
        {
            Memory::FreePool(FsHandles, false);
            return true;
        }
    }

    Memory::FreePool(FsHandles, false);
    return false;
}

auto File::LoadFromFileSystem(void* FsHandle, const String& FilePath) -> bool
{
    Unload();

    if (!FsHandle || FilePath.Empty())
    {
        return false;
    }

    auto EfiFsHandle = Cast::To<EFI_HANDLE>(FsHandle);

    PEFI_SIMPLE_FILE_SYSTEM_PROTOCOL Fs = nullptr;
    auto Status = gBS->HandleProtocol(EfiFsHandle, &gEfiSimpleFileSystemProtocolGuid, Cast::To<void**>(&Fs));

    if (EfiError(Status) || !Fs)
    {
        return false;
    }

    PEFI_FILE_PROTOCOL Root = nullptr;
    Status = Fs->OpenVolume(Fs, &Root);

    if (EfiError(Status) || !Root)
    {
        return false;
    }

    PEFI_FILE_PROTOCOL EfiFile = nullptr;
    Status = Root->Open(Root, &EfiFile, FilePath.WcharStr(), EFI_FILE_MODE_READ, 0);

    Root->Close(Root);

    if (EfiError(Status) || !EfiFile)
    {
        return false;
    }

    uint64_t InfoSize = 0;
    Status = EfiFile->GetInfo(EfiFile, &gEfiFileInfoGuid, &InfoSize, nullptr);

    if (Status != EFI_BUFFER_TOO_SMALL || !InfoSize)
    {
        EfiFile->Close(EfiFile);
        return false;
    }

    PEFI_FILE_INFO Info = nullptr;

    if (!Memory::AllocatePool(Info, InfoSize, false, true))
    {
        EfiFile->Close(EfiFile);
        return false;
    }

    Status = EfiFile->GetInfo(EfiFile, &gEfiFileInfoGuid, &InfoSize, Info);

    if (EfiError(Status) || !Info)
    {
        Memory::FreePool(Info, false);
        EfiFile->Close(EfiFile);
        return false;
    }

    if ((Info->Attribute & EFI_FILE_DIRECTORY) != 0)
    {
        Memory::FreePool(Info, false);
        EfiFile->Close(EfiFile);
        return false;
    }

    auto FileSize = Info->FileSize;

    if (!Buffer_.Resize(FileSize))
    {
        Memory::FreePool(Info, false);
        EfiFile->Close(EfiFile);
        return false;
    }

    auto ReadSize = FileSize;
    Status = EfiFile->Read(EfiFile, &ReadSize, Buffer_.Data());

    if (EfiError(Status) || ReadSize != FileSize)
    {
        Buffer_.Clear();
        Memory::FreePool(Info, false);
        EfiFile->Close(EfiFile);
        return false;
    }

    Path_ = FilePath;
    Name_ = GetFileNameFromPath(FilePath);
    PhysicalSize_ = Info->PhysicalSize;
    Attribute_ = Info->Attribute;
    CreateTime_ = ConvertTime(Info->CreateTime);
    LastAccessTime_ = ConvertTime(Info->LastAccessTime);
    ModificationTime_ = ConvertTime(Info->ModificationTime);
    FsHandle_ = FsHandle;
    Loaded_ = true;

    Memory::FreePool(Info, false);
    EfiFile->Close(EfiFile);

    return true;
}

auto File::Unload() -> bool
{
    Path_ = {};
    Name_ = {};
    Buffer_.Clear();

    PhysicalSize_ = 0;
    Attribute_ = 0;
    CreateTime_ = {};
    LastAccessTime_ = {};
    ModificationTime_ = {};
    FsHandle_ = nullptr;
    Loaded_ = false;

    return true;
}

auto File::Reload() -> bool
{
    if (Path_.Empty())
    {
        return false;
    }

    auto OldPath = Path_;
    auto OldFsHandle = FsHandle_;

    if (OldFsHandle)
    {
        return LoadFromFileSystem(OldFsHandle, OldPath);
    }

    return Load(OldPath);
}

auto File::Save() const -> bool
{
    if (Path_.Empty())
    {
        return false;
    }

    return Write(Path_, Buffer_);
}

auto File::SaveAs(const String& FilePath) const -> bool
{
    return Write(FilePath, Buffer_);
}

auto File::Write(const String& FilePath, const Vector<uint8_t>& FileBuffer) -> bool
{
    return Write(FilePath, FileBuffer.Data(), FileBuffer.Size());
}

auto File::Write(const String& FilePath, const void* FileBuffer, uint64_t FileSize) -> bool
{
    if (FilePath.Empty() || !FileBuffer || !FileSize)
    {
        return false;
    }

    PEFI_LOADED_IMAGE_PROTOCOL LoadedImage = nullptr;
    auto Status = gBS->HandleProtocol(
        gImageHandle,
        &gEfiLoadedImageProtocolGuid,
        Cast::To<void**>(&LoadedImage)
    );

    if (EfiError(Status) || !LoadedImage || !LoadedImage->DeviceHandle)
    {
        return false;
    }

    PEFI_SIMPLE_FILE_SYSTEM_PROTOCOL Fs = nullptr;
    Status = gBS->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, Cast::To<void**>(&Fs));

    if (EfiError(Status) || !Fs)
    {
        return false;
    }

    PEFI_FILE_PROTOCOL Root = nullptr;
    Status = Fs->OpenVolume(Fs, &Root);

    if (EfiError(Status) || !Root)
    {
        return false;
    }

    PEFI_FILE_PROTOCOL EfiFile = nullptr;
    Status = Root->Open(Root, &EfiFile, FilePath.WcharStr(), EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 0);

    Root->Close(Root);

    if (EfiError(Status) || !EfiFile)
    {
        return false;
    }

    Status = EfiFile->SetPosition(EfiFile, 0);

    if (EfiError(Status))
    {
        EfiFile->Close(EfiFile);
        return false;
    }

    auto WriteSize = FileSize;
    Status = EfiFile->Write(EfiFile, &WriteSize, Cast::To<void*>(FileBuffer));

    EfiFile->Close(EfiFile);

    return !EfiError(Status) && WriteSize == FileSize;
}

auto File::Loaded() const -> bool
{
    return Loaded_;
}

auto File::Empty() const -> bool
{
    return Buffer_.Empty();
}

auto File::Path() const -> const String&
{
    return Path_;
}

auto File::Name() const -> const String&
{
    return Name_;
}

auto File::Buffer() -> Vector<uint8_t>&
{
    return Buffer_;
}

auto File::Buffer() const -> const Vector<uint8_t>&
{
    return Buffer_;
}

auto File::Data() -> void*
{
    return Buffer_.Data();
}

auto File::Data() const -> const void*
{
    return Buffer_.Data();
}

auto File::Bytes() -> uint8_t*
{
    return Buffer_.Data();
}

auto File::Bytes() const -> const uint8_t*
{
    return Buffer_.Data();
}

auto File::Size() const -> uint64_t
{
    return Buffer_.Size();
}

auto File::PhysicalSize() const -> uint64_t
{
    return PhysicalSize_;
}

auto File::Attribute() const -> uint64_t
{
    return Attribute_;
}

auto File::FileSystemHandle() const -> void*
{
    return FsHandle_;
}

auto File::CreateTime() const -> const FILE_TIME&
{
    return CreateTime_;
}

auto File::LastAccessTime() const -> const FILE_TIME&
{
    return LastAccessTime_;
}

auto File::ModificationTime() const -> const FILE_TIME&
{
    return ModificationTime_;
}

auto File::IsDirectory() const -> bool
{
    return (Attribute_ & EFI_FILE_DIRECTORY) != 0;
}

auto File::IsReadOnly() const -> bool
{
    return (Attribute_ & EFI_FILE_READ_ONLY) != 0;
}

auto File::IsHidden() const -> bool
{
    return (Attribute_ & EFI_FILE_HIDDEN) != 0;
}

auto File::IsSystem() const -> bool
{
    return (Attribute_ & EFI_FILE_SYSTEM) != 0;
}

auto File::IsReserved() const -> bool
{
    return (Attribute_ & EFI_FILE_RESERVED) != 0;
}

auto File::IsArchive() const -> bool
{
    return (Attribute_ & EFI_FILE_ARCHIVE) != 0;
}

auto File::Delete() -> bool
{
    if (Path_.Empty())
    {
        return false;
    }

    auto Result = Delete(Path_);

    if (Result)
    {
        Unload();
    }

    return Result;
}

auto File::Delete(const String& FilePath) -> bool
{
    if (FilePath.Empty())
    {
        return false;
    }

    PEFI_LOADED_IMAGE_PROTOCOL LoadedImage = nullptr;
    auto Status = gBS->HandleProtocol(gImageHandle, &gEfiLoadedImageProtocolGuid, Cast::To<void**>(&LoadedImage));

    if (EfiError(Status) || !LoadedImage || !LoadedImage->DeviceHandle)
    {
        return false;
    }

    PEFI_SIMPLE_FILE_SYSTEM_PROTOCOL Fs = nullptr;
    Status = gBS->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, Cast::To<void**>(&Fs));

    if (EfiError(Status) || !Fs)
    {
        return false;
    }

    PEFI_FILE_PROTOCOL Root = nullptr;
    Status = Fs->OpenVolume(Fs, &Root);

    if (EfiError(Status) || !Root)
    {
        return false;
    }

    PEFI_FILE_PROTOCOL File = nullptr;
    Status = Root->Open(Root, &File, FilePath.WcharStr(), EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0);

    Root->Close(Root);

    if (EfiError(Status) || !File)
    {
        return false;
    }

    Status = File->Delete(File);

    return !EfiError(Status);
}