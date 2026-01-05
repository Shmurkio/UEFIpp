#pragma once

#include "../Uefi.hpp"

namespace Image
{
	CBOOLEAN
	IsRangeInBounds(
		IN CUINT64 Offset,
		IN CUINT64 Size,
		IN CUINT64 Total
	);

	EFI_STATUS
	GetNtHeaders64(
		IN PCVOID ImageBase,
		OUT EFI_IMAGE_NT_HEADERS64*& Nt
	);

	EFI_STATUS
	GetImageBase(
		IN PCVOID Address,
		OUT PVOID& ImageBase,
		IN CUINT64 MaxBackScanMb
	);

	EFI_STATUS
	GetImageSize(
		IN PCVOID ImageBase,
		OUT UINT64& ImageSize
	);
}