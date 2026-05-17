#include "Image.hpp"
#include "../Util/Util.hpp"

Image::~Image()
{
    Unload();
}

Image::Image(Image&& Other) noexcept
{
    *this = Memory::Move(Other);
}

auto Image::operator=(Image&& Other) noexcept -> Image&
{
    if (this == &Other)
    {
        return *this;
    }

    Unload();

    Path_ = Memory::Move(Other.Path_);
    Handle_ = Other.Handle_;
    ParentHandle_ = Other.ParentHandle_;
    Args_ = Memory::Move(Other.Args_);
    ImageBase_ = Other.ImageBase_;
    ImageSize_ = Other.ImageSize_;
    ExitData_ = Memory::Move(Other.ExitData_);
    UnloadFunction_ = Other.UnloadFunction_;

    Other.Handle_ = nullptr;
    Other.ParentHandle_ = nullptr;
    Other.ImageBase_ = nullptr;
    Other.ImageSize_ = 0;
    Other.UnloadFunction_ = nullptr;

    return *this;
}

auto Image::Unload() -> bool
{
    auto Success = true;

    if (Handle_)
    {
        auto Status = gBS->UnloadImage(Cast::To<EFI_HANDLE>(Handle_));

        if (EfiError(Status))
        {
            Success = false;
        }
    }

    Path_ = {};
    Handle_ = nullptr;
    ParentHandle_ = nullptr;
    Args_.Clear();
    ImageBase_ = nullptr;
    ImageSize_ = 0;
    ExitData_ = {};
    UnloadFunction_ = nullptr;

    return Success;
}

auto Image::Loaded() const -> bool
{
    return Handle_ != nullptr;
}

auto Image::Empty() const -> bool
{
    return !Loaded();
}

auto Image::Path() const -> const String&
{
    return Path_;
}

auto Image::Handle() const -> void*
{
    return Handle_;
}

auto Image::ParentHandle() const -> void*
{
    return ParentHandle_;
}

auto Image::Args() -> Vector<String>&
{
    return Args_;
}

auto Image::Args() const -> const Vector<String>&
{
    return Args_;
}

auto Image::ImageBase() const -> void*
{
    return ImageBase_;
}

auto Image::ImageSize() const -> uint64_t
{
    return ImageSize_;
}

auto Image::ExitData() const -> const String&
{
    return ExitData_;
}

auto Image::UnloadFunction() const -> void*
{
    return UnloadFunction_;
}

auto Image::Start() -> bool
{
    if (!Handle_)
    {
        return false;
    }

    uint64_t ExitDataSize = 0;
    wchar_t* ExitData = nullptr;

    auto Status = gBS->StartImage(Cast::To<EFI_HANDLE>(Handle_), &ExitDataSize, &ExitData);

    if (ExitData && ExitDataSize)
    {
        ExitData_ = ExitData;
        Memory::FreePool(ExitData, false);
    }

    return !EfiError(Status);
}

static auto DevicePathLength(PCEFI_DEVICE_PATH_PROTOCOL Node) -> uint16_t
{
    return Cast::To<uint16_t>(Node->Length[0] | (Node->Length[1] << 8));
}

static auto DevicePathNext(PCEFI_DEVICE_PATH_PROTOCOL Node) -> PEFI_DEVICE_PATH_PROTOCOL
{
    return Cast::To<PEFI_DEVICE_PATH_PROTOCOL>(Cast::To<uint8_t*>(Node) + DevicePathLength(Node));
}

static auto DevicePathIsEnd(PCEFI_DEVICE_PATH_PROTOCOL Node) -> bool
{
    return Node->Type == END_DEVICE_PATH_TYPE;
}

static auto DevicePathSize(PCEFI_DEVICE_PATH_PROTOCOL DevicePath) -> uint64_t
{
    if (!DevicePath)
    {
        return 0;
    }

    uint64_t Size = 0;

    for (auto Node = DevicePath;; Node = DevicePathNext(Node))
    {
        auto Length = DevicePathLength(Node);

        Size += Length;

        if (DevicePathIsEnd(Node))
        {
            break;
        }
    }

    return Size;
}

static auto BuildFileDevicePath(PCEFI_DEVICE_PATH_PROTOCOL BaseDevicePath, const String& FilePath, PEFI_DEVICE_PATH_PROTOCOL& DevicePath) -> bool
{
    DevicePath = nullptr;

    if (!BaseDevicePath || FilePath.Empty())
    {
        return false;
    }

    auto BaseSize = DevicePathSize(BaseDevicePath);

    if (BaseSize < sizeof(EFI_DEVICE_PATH_PROTOCOL))
    {
        return false;
    }

    auto BaseNoEndSize = BaseSize - sizeof(EFI_DEVICE_PATH_PROTOCOL);
    auto PathChars = FilePath.Size();

    auto FileNodeSize = sizeof(FILEPATH_DEVICE_PATH) - sizeof(wchar_t) + ((PathChars + 1) * sizeof(wchar_t));

    auto EndNodeSize = sizeof(EFI_DEVICE_PATH_PROTOCOL);
    auto TotalSize = BaseNoEndSize + FileNodeSize + EndNodeSize;

    PEFI_DEVICE_PATH_PROTOCOL NewPath = nullptr;

    if (!Memory::AllocatePool(NewPath, TotalSize, false, true))
    {
        return false;
    }

    auto Src = Cast::To<const uint8_t*>(BaseDevicePath);
    auto Dst = Cast::To<uint8_t*>(NewPath);

    for (uint64_t i = 0; i < BaseNoEndSize; ++i)
    {
        Dst[i] = Src[i];
    }

    auto FileNode = Cast::To<PFILEPATH_DEVICE_PATH>(Dst + BaseNoEndSize);

    FileNode->Header.Type = MEDIA_DEVICE_PATH;
    FileNode->Header.SubType = MEDIA_FILEPATH_DP;
    FileNode->Header.Length[0] = Cast::To<uint8_t>(FileNodeSize & 0xFF);
    FileNode->Header.Length[1] = Cast::To<uint8_t>((FileNodeSize >> 8) & 0xFF);

    auto Path = FilePath.WcharStr();

    for (uint64_t i = 0; i < PathChars; ++i)
    {
        FileNode->PathName[i] = Path[i];
    }

    FileNode->PathName[PathChars] = L'\0';

    auto EndNode = Cast::To<PEFI_DEVICE_PATH_PROTOCOL>(Dst + BaseNoEndSize + FileNodeSize);

    EndNode->Type = END_DEVICE_PATH_TYPE;
    EndNode->SubType = END_ENTIRE_DEVICE_PATH_SUBTYPE;
    EndNode->Length[0] = Cast::To<uint8_t>(EndNodeSize & 0xFF);
    EndNode->Length[1] = Cast::To<uint8_t>((EndNodeSize >> 8) & 0xFF);

    DevicePath = NewPath;

    return true;
}

auto Image::Load(const String& Path) -> bool
{
    Unload();

    if (Path.Empty())
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
        PEFI_DEVICE_PATH_PROTOCOL BaseDevicePath = nullptr;

        Status = gBS->HandleProtocol(FsHandles[i], &gEfiDevicePathProtocolGuid, Cast::To<void**>(&BaseDevicePath));

        if (EfiError(Status) || !BaseDevicePath)
        {
            continue;
        }

        PEFI_DEVICE_PATH_PROTOCOL FullDevicePath = nullptr;

        if (!BuildFileDevicePath(BaseDevicePath, Path, FullDevicePath))
        {
            continue;
        }

        EFI_HANDLE ImageHandle = nullptr;

        Status = gBS->LoadImage(false, gImageHandle, FullDevicePath, nullptr, 0, &ImageHandle);

        Memory::FreePool(FullDevicePath, false);

        if (EfiError(Status) || !ImageHandle)
        {
            continue;
        }

        PEFI_LOADED_IMAGE_PROTOCOL LoadedImage = nullptr;

        Status = gBS->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, Cast::To<void**>(&LoadedImage));

        if (EfiError(Status) || !LoadedImage)
        {
            gBS->UnloadImage(ImageHandle);
            continue;
        }

        Path_ = Path;
        Handle_ = ImageHandle;
        ParentHandle_ = LoadedImage->ParentHandle;
        ImageBase_ = LoadedImage->ImageBase;
        ImageSize_ = LoadedImage->ImageSize;
        UnloadFunction_ = Cast::To<void*>(LoadedImage->Unload);

        Memory::FreePool(FsHandles, false);
        return true;
    }

    Memory::FreePool(FsHandles, false);

    return false;
}

static auto TryGetFilePathFromDevicePath(PCEFI_DEVICE_PATH_PROTOCOL DevicePath, String& FilePath) -> bool
{
    FilePath = {};

    if (!DevicePath)
    {
        return false;
    }

    for (auto Node = DevicePath; Node && !DevicePathIsEnd(Node); Node = DevicePathNext(Node))
    {
        if (Node->Type != MEDIA_DEVICE_PATH || Node->SubType != MEDIA_FILEPATH_DP)
        {
            continue;
        }

        if (DevicePathLength(Node) < sizeof(FILEPATH_DEVICE_PATH))
        {
            continue;
        }

        auto PathNode = Cast::To<const FILEPATH_DEVICE_PATH*>(Node);

        FilePath = PathNode->PathName;

        return !FilePath.Empty();
    }

    return false;
}

auto Image::FindByGuid(CGUID& Guid) -> Image
{
    Image Result{};

    uint64_t HandleCount = 0;
    PEFI_HANDLE Handles = nullptr;

    auto Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiLoadedImageProtocolGuid, nullptr, &HandleCount, &Handles);

    if (EfiError(Status) || !HandleCount || !Handles)
    {
        return {};
    }

    for (uint64_t i = 0; i < HandleCount; ++i)
    {
        PEFI_LOADED_IMAGE_PROTOCOL LoadedImage = nullptr;

        Status = gBS->HandleProtocol(Handles[i], &gEfiLoadedImageProtocolGuid, Cast::To<void**>(&LoadedImage));

        if (EfiError(Status) || !LoadedImage || !LoadedImage->FilePath)
        {
            continue;
        }

        for (auto Node = LoadedImage->FilePath; Node && !DevicePathIsEnd(Node); Node = DevicePathNext(Node))
        {
            if (Node->Type != MEDIA_DEVICE_PATH || Node->SubType != MEDIA_FW_VOL_FILEPATH_DP_LOCAL)
            {
                continue;
            }

            if (DevicePathLength(Node) < sizeof(FV_FILEPATH_DEVICE_PATH_LOCAL))
            {
                continue;
            }

            auto FvNode = Cast::To<PFV_FILEPATH_DEVICE_PATH_LOCAL>(Node);

            if (!Util::IsGuidEqual(FvNode->NameGuid, Guid))
            {
                continue;
            }

            String ResolvedPath{};

            if (!TryGetFilePathFromDevicePath(LoadedImage->FilePath, ResolvedPath))
            {
                ResolvedPath = Guid;
            }

            Result.Path_ = ResolvedPath;
            Result.Handle_ = Handles[i];
            Result.ParentHandle_ = LoadedImage->ParentHandle;
            Result.ImageBase_ = LoadedImage->ImageBase;
            Result.ImageSize_ = LoadedImage->ImageSize;
            Result.UnloadFunction_ = Cast::To<void*>(LoadedImage->Unload);

            Memory::FreePool(Handles, false);

            return Result;
        }
    }

    Memory::FreePool(Handles, false);

    return {};
}