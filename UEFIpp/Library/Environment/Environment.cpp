#include "Environment.hpp"
#include "../Memory/Memory.hpp"
#include "../Util/Util.hpp"

static
inline
BOOLEAN
IsEndNode(
	IN	PCEFI_DEVICE_PATH_PROTOCOL	Node
)
{
	return Node->Type == END_DEVICE_PATH_TYPE;
}

static
inline
UINT16
DpLen(
	IN	PCEFI_DEVICE_PATH_PROTOCOL	Node
)
{
	return static_cast<UINT16>(Node->Length[0] | (Node->Length[1] << 8));
}

static
inline
PEFI_DEVICE_PATH_PROTOCOL
NextNode(
	IN	PCEFI_DEVICE_PATH_PROTOCOL	Node
)
{
	return Cast::To<PEFI_DEVICE_PATH_PROTOCOL>(Cast::To<PUINT8>(Node) + DpLen(Node));
}

static
inline
UINT64
DevicePathSize(
	IN	PCEFI_DEVICE_PATH_PROTOCOL	DevicePath
)
{
	UINT64 Size = 0;
	PCEFI_DEVICE_PATH_PROTOCOL Node = DevicePath;

	for (;;)
	{
		Size += DpLen(Node);

		if (IsEndNode(Node))
		{
			break;
		}

		Node = NextNode(Node);
	}

	return Size;
}

static
EFI_STATUS
BuildFileDevicePath(
	IN	PCEFI_DEVICE_PATH_PROTOCOL	BaseDevicePath,
	IN	PCWSTR						FilePath,
	OUT	PEFI_DEVICE_PATH_PROTOCOL& DevicePath
)
{
	DevicePath = nullptr;

	if (!BaseDevicePath || !FilePath || !*FilePath)
	{
		return EFI_INVALID_PARAMETER;
	}

	CUINT64 BaseSize = DevicePathSize(BaseDevicePath);

	if (BaseSize < sizeof(EFI_DEVICE_PATH_PROTOCOL))
	{
		return EFI_INVALID_PARAMETER;
	}

	CUINT64 BaseNoEndSize = BaseSize - sizeof(EFI_DEVICE_PATH_PROTOCOL);
	UINT64 PathChars = 0;

	for (PCWSTR p = FilePath; *p; ++p)
	{
		++PathChars;
	}

	CUINT64 FileNodeSize = (sizeof(FILEPATH_DEVICE_PATH) - sizeof(WCHAR)) + ((PathChars + 1) * sizeof(WCHAR));
	CUINT64 EndNodeSize = sizeof(EFI_DEVICE_PATH_PROTOCOL);
	CUINT64 TotalSize = BaseNoEndSize + FileNodeSize + EndNodeSize;
	PEFI_DEVICE_PATH_PROTOCOL NewPath = nullptr;
	EFI_STATUS Status = gBS->AllocatePool(EfiBootServicesData, TotalSize, reinterpret_cast<PVOID*>(&NewPath));

	if (EfiError(Status))
	{
		return Status;
	}

	{
		PCUINT8 Src = reinterpret_cast<PCUINT8>(BaseDevicePath);
		PUINT8 Dst = reinterpret_cast<PUINT8>(NewPath);

		for (UINT64 i = 0; i < BaseNoEndSize; ++i)
		{
			Dst[i] = Src[i];
		}
	}

	PFILEPATH_DEVICE_PATH FileNode = reinterpret_cast<PFILEPATH_DEVICE_PATH>(reinterpret_cast<PUINT8>(NewPath) + BaseNoEndSize);

	FileNode->Header.Type = MEDIA_DEVICE_PATH;
	FileNode->Header.SubType = MEDIA_FILEPATH_DP;

	FileNode->Header.Length[0] = static_cast<UINT8>(FileNodeSize & 0xFF);
	FileNode->Header.Length[1] = static_cast<UINT8>((FileNodeSize >> 8) & 0xFF);

	for (UINT64 i = 0; i < PathChars; ++i)
	{
		FileNode->PathName[i] = FilePath[i];
	}

	FileNode->PathName[PathChars] = L'\0';

	PEFI_DEVICE_PATH_PROTOCOL EndNode = reinterpret_cast<PEFI_DEVICE_PATH_PROTOCOL>(reinterpret_cast<PUINT8>(FileNode) + FileNodeSize);

	EndNode->Type = END_DEVICE_PATH_TYPE;
	EndNode->SubType = END_ENTIRE_DEVICE_PATH_SUBTYPE;
	EndNode->Length[0] = static_cast<UINT8>(EndNodeSize & 0xFF);
	EndNode->Length[1] = static_cast<UINT8>((EndNodeSize >> 8) & 0xFF);

	DevicePath = NewPath;

	return EFI_SUCCESS;
}

auto OpenFileFromFs(EFI_HANDLE FsHandle, String FilePath, void*& FileBuffer, uint64_t& FileSize) -> bool
{
	if (!FsHandle || !FilePath.Size())
	{
		return false;
	}

	PEFI_SIMPLE_FILE_SYSTEM_PROTOCOL Fs = nullptr;
	auto Status = gBS->HandleProtocol(FsHandle, &gEfiSimpleFileSystemProtocolGuid, Cast::To<void**>(&Fs));

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
	Status = Root->Open(Root, &File, FilePath.WcharStr(), EFI_FILE_MODE_READ, 0);

	Root->Close(Root);

	if (EfiError(Status) || !File)
	{
		return false;
	}

	uint64_t InfoSize = 0;
	Status = File->GetInfo(File, &gEfiFileInfoGuid, &InfoSize, nullptr);

	if (Status != EFI_BUFFER_TOO_SMALL || !InfoSize)
	{
		File->Close(File);
		return false;
	}

	PEFI_FILE_INFO FileInfo = nullptr;
	
	if (!Memory::AllocatePool(FileInfo, InfoSize, false, true))
	{
		File->Close(File);
		return false;
	}

	Status = File->GetInfo(File, &gEfiFileInfoGuid, &InfoSize, FileInfo);

	if (EfiError(Status) || !FileInfo || !FileInfo->FileSize)
	{
		Memory::FreePool(FileInfo, false);
		File->Close(File);
		return false;
	}

	auto Size = FileInfo->FileSize;
	Memory::FreePool(FileInfo, false);

	void* Buffer = nullptr;

	if (!Memory::AllocatePool(Buffer, Size, false, true))
	{
		File->Close(File);
		return false;
	}

	auto ReadSize = Size;
	Status = File->Read(File, &ReadSize, Buffer);
	File->Close(File);

	if (EfiError(Status) || ReadSize != Size)
	{
		Memory::FreePool(Buffer, false);
		return false;
	}

	FileBuffer = Buffer;
	FileSize = Size;

	return true;
}

auto LoadImageFromFs(EFI_HANDLE FsHandle, String ImagePath, PEFI_LOADED_IMAGE_PROTOCOL& LoadedImage, EFI_HANDLE& ImageHandle) -> bool
{
	LoadedImage = nullptr;
	ImageHandle = nullptr;

	if (!FsHandle || !ImagePath.Size())
	{
		return false;
	}
	
	PEFI_DEVICE_PATH_PROTOCOL BaseDp = nullptr;
	auto Status = gBS->HandleProtocol(FsHandle, &gEfiDevicePathProtocolGuid, Cast::To<void**>(&BaseDp));

	if (EfiError(Status) || !BaseDp)
	{
		return false;
	}

	PEFI_DEVICE_PATH_PROTOCOL FullDp = nullptr;
	Status = BuildFileDevicePath(BaseDp, ImagePath.WcharStr(), FullDp);

	if (EfiError(Status) || !FullDp)
	{
		return false;
	}

	EFI_HANDLE Handle = nullptr;
	Status = gBS->LoadImage(false, gImageHandle, FullDp, nullptr, 0, &Handle);

	Memory::FreePool(FullDp, false);

	if (EfiError(Status) || !Handle)
	{
		return false;
	}

	PEFI_LOADED_IMAGE_PROTOCOL Image = nullptr;
	Status = gBS->HandleProtocol(Handle, &gEfiLoadedImageProtocolGuid, Cast::To<void**>(&Image));

	if (EfiError(Status) || !Image)
	{
		gBS->UnloadImage(Handle);
		return false;
	}

	LoadedImage = Image;
	ImageHandle = Handle;

	return true;
}

auto Environment::LoadImage(const String& ImagePath, IMAGE& Image) -> bool
{
	Image = {};

	if (!ImagePath.Size())
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
		PEFI_LOADED_IMAGE_PROTOCOL LoadedImage = nullptr;
		EFI_HANDLE ImageHandle = nullptr;

		if (!LoadImageFromFs(FsHandles[i], ImagePath, LoadedImage, ImageHandle))
		{
			continue;
		}

		Image.ImagePath = ImagePath;
		Image.LoadedImage = LoadedImage;
		Image.ImageHandle = ImageHandle;

		Memory::FreePool(FsHandles, false);
		return true;
	}

	Memory::FreePool(FsHandles, false);
	return false;
}

auto Environment::UnloadImage(IMAGE& Image) -> bool
{
	auto Success = true;

	if (Image.ImageHandle)
	{
		auto Status = gBS->UnloadImage(Image.ImageHandle);

		if (EfiError(Status))
		{
			Success = false;
		}
	}

	Image.ImagePath = {};
	Image.LoadedImage = nullptr;
	Image.ImageHandle = nullptr;

	return Success;
}

auto Environment::StartImage(CIMAGE& Image) -> bool
{
	if (!Image.ImageHandle)
	{
		return false;
	}

	auto Status = gBS->StartImage(Image.ImageHandle, nullptr, nullptr);

	if (EfiError(Status))
	{
		return false;
	}

	return true;
}

static auto TryGetFilePathFromDevicePath(PCEFI_DEVICE_PATH_PROTOCOL DevicePath, String& FilePath) -> bool
{
	FilePath = {};

	if (!DevicePath)
	{
		return false;
	}

	for (auto Node = DevicePath; Node && !IsEndNode(Node); Node = NextNode(Node))
	{
		if (Node->Type != MEDIA_DEVICE_PATH || Node->SubType != MEDIA_FILEPATH_DP)
		{
			continue;
		}

		auto PathNode = Cast::To<const FILEPATH_DEVICE_PATH*>(Node);
		FilePath = PathNode->PathName;
		return FilePath.Size() != 0;
	}

	return false;
}

auto Environment::FindImageByGuid(CEFI_GUID& ImageGuid, IMAGE& Image) -> bool
{
	Image = {};

	uint64_t HandleCount = 0;
	PEFI_HANDLE Handles = nullptr;

	auto Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiLoadedImageProtocolGuid, nullptr, &HandleCount, &Handles);

	if (EfiError(Status) || !HandleCount || !Handles)
	{
		return false;
	}

	for (uint64_t i = 0; i < HandleCount; ++i)
	{
		PEFI_LOADED_IMAGE_PROTOCOL LoadedImage = nullptr;
		Status = gBS->HandleProtocol(Handles[i], &gEfiLoadedImageProtocolGuid, Cast::To<void**>(&LoadedImage));

		if (EfiError(Status) || !LoadedImage || !LoadedImage->FilePath)
		{
			continue;
		}

		for (auto Node = LoadedImage->FilePath; Node && !IsEndNode(Node); Node = NextNode(Node))
		{
			if (Node->Type != MEDIA_DEVICE_PATH || Node->SubType != MEDIA_FW_VOL_FILEPATH_DP_LOCAL)
			{
				continue;
			}

			if (DpLen(Node) < sizeof(FV_FILEPATH_DEVICE_PATH_LOCAL))
			{
				continue;
			}

			auto FvNode = Cast::To<PFV_FILEPATH_DEVICE_PATH_LOCAL>(Node);

			if (!Util::IsGuidEqual(FvNode->NameGuid, ImageGuid))
			{
				continue;
			}

			String ResolvedPath = {};

			if (!TryGetFilePathFromDevicePath(LoadedImage->FilePath, ResolvedPath))
			{
				ResolvedPath = ImageGuid;
			}

			Image.ImagePath = ResolvedPath;
			Image.LoadedImage = LoadedImage;
			Image.ImageHandle = Handles[i];
			Memory::FreePool(Handles, false);

			return true;
		}
	}

	Memory::FreePool(Handles, false);

	return false;
}