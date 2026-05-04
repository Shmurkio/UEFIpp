#pragma once

#include "File.hpp"
#include "FrameBuffer.hpp"
#include "Keyboard.hpp"

namespace Environment
{

	typedef struct _IMAGE
	{
		String ImagePath{};
		PEFI_LOADED_IMAGE_PROTOCOL LoadedImage = nullptr;
		EFI_HANDLE ImageHandle = nullptr;
	} IMAGE, *PIMAGE;

	using CIMAGE = const IMAGE;
	using PCIMAGE = const IMAGE*;

	auto LoadImage(const String& ImagePath, IMAGE& Image) -> bool;
	auto UnloadImage(IMAGE& Image) -> bool;
	auto StartImage(CIMAGE& Image) -> bool;

	auto FindImageByGuid(CEFI_GUID& ImageGuid, IMAGE& Image) -> bool;
}