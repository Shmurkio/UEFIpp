#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Types.hpp>
#include <UEFIpp/UEFI/Memory.hpp>
#include <UEFIpp/UEFI/SystemTable.hpp>

namespace UEFIpp::Protocols
{
	class LoadedImage;

	using LoadedImageUnloadFn = UEFI::StatusCode(*)(UEFI::Handle ImageHandle);

	class LoadedImage
	{
	public:
		Foundation::Uint32 Revision;
		UEFI::Handle ParentHandle;
		UEFI::Table::System* SystemTable;

		UEFI::Handle DeviceHandle;
		Foundation::Void* FilePath;
		Foundation::Void* Reserved;

		Foundation::Uint32 LoadOptionsSize;
		Foundation::Void* LoadOptions;

		Foundation::Void* ImageBase;
		Foundation::Uint64 ImageSize;
		UEFI::MemoryType ImageCodeType;
		UEFI::MemoryType ImageDataType;

		LoadedImageUnloadFn Unload;

		[[nodiscard]] auto HasLoadOptions() const -> Foundation::Bool
		{
			return LoadOptions && LoadOptionsSize;
		}

		[[nodiscard]] auto LoadOptionsText() const -> const Foundation::Char16*
		{
			return Foundation::Cast::Auto<const Foundation::Char16*>(LoadOptions);
		}
	};

	static_assert(sizeof(LoadedImage) == 96);
	static_assert(Foundation::Traits::IsStandardLayout<LoadedImage>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<LoadedImage>::Value);
}