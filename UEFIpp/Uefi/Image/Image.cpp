#include "Image.hpp"

#define PAGE_SIZE 0x1000

CBOOLEAN
Image::IsRangeInBounds(
	IN CUINT64 Offset,
	IN CUINT64 Size,
	IN CUINT64 Total
)
{
	if (Offset > Total)
	{
		return false;
	}

	if (Size > Total)
	{
		return false;
	}

	if (Offset + Size < Offset)
	{
		return false;
	}

	return (Offset + Size) <= Total;
}

EFI_STATUS
Image::GetNtHeaders64(
	IN PCVOID ImageBase,
	OUT EFI_IMAGE_NT_HEADERS64*& Nt
)
{
	Nt = nullptr;

	if (!ImageBase)
	{
		return EFI_INVALID_PARAMETER;
	}

	if (!ImageBase)
	{
		return EFI_INVALID_PARAMETER;
	}

	const EFI_IMAGE_DOS_HEADER* DosHeader = reinterpret_cast<const EFI_IMAGE_DOS_HEADER*>(ImageBase);

	if (DosHeader->e_magic != EFI_IMAGE_DOS_SIGNATURE)
	{
		return EFI_UNSUPPORTED;
	}

	const EFI_IMAGE_NT_HEADERS64* NtHeaders = reinterpret_cast<const EFI_IMAGE_NT_HEADERS64*>(reinterpret_cast<PCUINT8>(ImageBase) + DosHeader->e_lfanew);

	if (NtHeaders->Signature != EFI_IMAGE_NT_SIGNATURE)
	{
		return EFI_UNSUPPORTED;
	}

	if (NtHeaders->OptionalHeader.Magic != EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
		return EFI_UNSUPPORTED;
	}

	CUINT64 SectionOffset = DosHeader->e_lfanew + sizeof(UINT32) + sizeof(EFI_IMAGE_FILE_HEADER) + NtHeaders->FileHeader.SizeOfOptionalHeader;
	CUINT64 SectionBytes = NtHeaders->FileHeader.NumberOfSections * sizeof(EFI_IMAGE_SECTION_HEADER);

	Nt = const_cast<EFI_IMAGE_NT_HEADERS64*>(NtHeaders);

	return EFI_SUCCESS;
}

EFI_STATUS
Image::GetImageSize(
	IN PCVOID ImageBase,
	OUT UINT64& ImageSize
)
{
	ImageSize = 0;

	if (!ImageBase)
	{
		return EFI_INVALID_PARAMETER;
	}

	EFI_IMAGE_NT_HEADERS64* NtHeaders = nullptr;
	EFI_STATUS Status = GetNtHeaders64(ImageBase, NtHeaders);

	if (EfiError(Status))
	{
		return Status;
	}

	ImageSize = NtHeaders->OptionalHeader.SizeOfImage;

	return EFI_SUCCESS;
}

static
inline
UINT64
AlignDown(
	IN CUINT64 Address,
	IN CUINT64 PageSize
)
{
	return Address & ~(PageSize - 1);
}

EFI_STATUS
Image::GetImageBase(
	IN PCVOID Address,
	OUT PVOID& ImageBase,
	IN CUINT64 MaxBackScanMb
)
{
	ImageBase = nullptr;

	if (!Address || !MaxBackScanMb)
	{
		return EFI_INVALID_PARAMETER;
	}

	UINT64 Start = AlignDown(reinterpret_cast<UINT64>(Address), PAGE_SIZE);
	CUINT64 MaxBackScan = MaxBackScanMb * 1024ULL * 1024ULL;
	UINT64 Min = (Start > MaxBackScan) ? (Start - MaxBackScan) : 0;
	CUINT64 A = reinterpret_cast<UINT64>(Address);

	for (UINT64 i = Start; i >= Min; i -= PAGE_SIZE)
	{
		PCVOID Base = reinterpret_cast<PCVOID>(i);

		if (*reinterpret_cast<PCUINT16>(Base) != EFI_IMAGE_DOS_SIGNATURE)
		{
			if (i < PAGE_SIZE)
			{
				break;
			}

			continue;
		}

		EFI_IMAGE_NT_HEADERS64* NtHeaders = nullptr;
		EFI_STATUS Status = GetNtHeaders64(Base, NtHeaders);

		if (EfiError(Status))
		{
			if (i < PAGE_SIZE)
			{
				break;
			}

			continue;
		}

		UINT64 Size = NtHeaders->OptionalHeader.SizeOfImage;

		if (!Size)
		{
			if (i < PAGE_SIZE)
			{
				break;
			}

			continue;
		}

		UINT64 B = reinterpret_cast<UINT64>(Base);

		if (A >= B && (A - B) < Size)
		{
			ImageBase = const_cast<PVOID>(Base);
			return EFI_SUCCESS;
		}

		if (i < PAGE_SIZE)
		{
			break;
		}
	}

	return EFI_NOT_FOUND;
}