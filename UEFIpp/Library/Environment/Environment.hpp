#pragma once

#include <cstdint>
#include "../String/String.hpp"
#include "../../UEFIpp.hpp"

namespace Environment
{
	typedef struct _FILE
	{
		String FilePath{};
		void* FileBuffer = nullptr;
		uint64_t FileSize = 0;
	} FILE, *PFILE;

	using CFILE = const FILE;
	using PCFILE = const FILE*;

	typedef struct _IMAGE
	{
		String ImagePath{};
		PEFI_LOADED_IMAGE_PROTOCOL LoadedImage = nullptr;
		EFI_HANDLE ImageHandle = nullptr;
	} IMAGE, *PIMAGE;

	using CIMAGE = const IMAGE;
	using PCIMAGE = const IMAGE*;

	auto LoadFile(const String& FilePath, FILE& File) -> bool;
	auto UnloadFile(FILE& File) -> bool;
	auto WriteFile(const String& FilePath, const void* FileBuffer, uint64_t FileSize) -> bool;

	auto LoadImage(const String& ImagePath, IMAGE& Image) -> bool;
	auto UnloadImage(IMAGE& Image) -> bool;
	auto StartImage(CIMAGE& Image) -> bool;

	auto FindImageByGuid(CEFI_GUID& ImageGuid, IMAGE& Image) -> bool;
}