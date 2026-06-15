#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Optional.hpp>
#include <UEFIpp/Protocols/Access.hpp>
#include <UEFIpp/Protocols/LoadedImage.hpp>
#include <UEFIpp/UEFI/DevicePath.hpp>
#include <UEFIpp/UEFI/Guid.hpp>
#include <UEFIpp/UEFI/Types.hpp>

namespace UEFIpp::Loader
{
	inline constexpr auto DevicePathSubTypeFirmwareVolumeFile = Foundation::Uint8{ 0x06 };

	class LoadedImageInfo
	{
	public:
		UEFI::Handle Handle{};
		Protocols::LoadedImage* Image{};
		Foundation::Void* Base{};
		Foundation::Uint64 Size{};
	};

	class FirmwareVolumeFilePathDevicePath
	{
	public:
		UEFI::DevicePathProtocol Header;
		UEFI::Guid FileGuid;
	};

	static_assert(sizeof(FirmwareVolumeFilePathDevicePath) == 20);

	[[nodiscard]] inline auto NextDevicePathNode(const UEFI::DevicePathProtocol* Node) -> const UEFI::DevicePathProtocol*
	{
		const auto* Bytes = Foundation::Cast::Auto<const Foundation::Uint8*>(Node);
		return Foundation::Cast::Auto<const UEFI::DevicePathProtocol*>(Bytes + Node->Size());
	}

	[[nodiscard]] inline auto DevicePathContainsFileGuid(const UEFI::DevicePathProtocol* Path, const UEFI::Guid& FileGuid) -> Foundation::Bool
	{
		if (!Path)
		{
			return false;
		}

		for (const auto* Node = Path; !UEFI::IsEndEntireDevicePath(*Node); Node = NextDevicePathNode(Node))
		{
			const auto Size = Node->Size();

			if (Size < sizeof(UEFI::DevicePathProtocol))
			{
				return false;
			}

			if (Node->Type == UEFI::DevicePathTypeMedia && Node->SubType == DevicePathSubTypeFirmwareVolumeFile && Size >= sizeof(FirmwareVolumeFilePathDevicePath))
			{
				const auto* FileNode = Foundation::Cast::Auto<const FirmwareVolumeFilePathDevicePath*>(Node);

				if (FileNode->FileGuid == FileGuid)
				{
					return true;
				}
			}

			if (UEFI::IsEndInstanceDevicePath(*Node))
			{
				continue;
			}
		}

		return false;
	}

	[[nodiscard]] inline auto FindLoadedImageByFileGuid(const Protocols::Access& Protocols, const UEFI::Guid& FileGuid) -> Library::Optional<LoadedImageInfo>
	{
		const auto Handles = Protocols.LocateHandles<Protocols::LoadedImage>();

		if (!Handles)
		{
			return Library::NullOpt;
		}

		for (const auto Handle : *Handles)
		{
			const auto Image = Protocols.Handle<Protocols::LoadedImage>(Handle);

			if (!Image)
			{
				continue;
			}

			const auto* FilePath = Foundation::Cast::Auto<const UEFI::DevicePathProtocol*>((*Image)->FilePath);

			if (!DevicePathContainsFileGuid(FilePath, FileGuid))
			{
				continue;
			}

			LoadedImageInfo Info{};
			Info.Handle = Handle;
			Info.Image = *Image;
			Info.Base = (*Image)->ImageBase;
			Info.Size = (*Image)->ImageSize;

			return Info;
		}

		return Library::NullOpt;
	}
}