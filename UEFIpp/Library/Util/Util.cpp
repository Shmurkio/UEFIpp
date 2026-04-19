#include "Util.hpp"
#include "../Memory/Memory.hpp"
#include "../../UEFIpp.hpp"

auto Util::IsGuidEqual(CGUID& Guid1, CGUID& Guid2) -> bool
{
	uint64_t MatchingBytes = 0;

	if (!Memory::CompareMemory(&Guid1, &Guid2, sizeof(GUID), MatchingBytes))
	{
		return false;
	}

	return MatchingBytes == sizeof(GUID);
}

auto Util::GetImageSize(void* ImageBase, uint64_t& ImageSize) -> bool
{
	ImageSize = 0;

	if (!ImageBase)
	{
		return false;
	}

	auto DosHeader = Cast::To<PIMAGE_DOS_HEADER>(ImageBase);

	if (DosHeader->e_magic != 0x5A4D)
	{
		return false;
	}

	auto NtHeaders = Cast::To<PIMAGE_NT_HEADERS64>(Cast::To<uint8_t*>(ImageBase) + DosHeader->e_lfanew);

	if (NtHeaders->Signature != 0x00004550)
	{
		return false;
	}

	ImageSize = NtHeaders->OptionalHeader.SizeOfImage;

	return true;
}

auto Util::GetNtHeaders64(void* ImageBase, PIMAGE_NT_HEADERS64& NtHeaders) -> bool
{
	NtHeaders = nullptr;

	if (!ImageBase)
	{
		return false;
	}

	auto DosHeader = Cast::To<PIMAGE_DOS_HEADER>(ImageBase);

	if (DosHeader->e_magic != 0x5A4D)
	{
		return false;
	}

	NtHeaders = Cast::To<PIMAGE_NT_HEADERS64>(Cast::To<uint8_t*>(ImageBase) + DosHeader->e_lfanew);

	if (NtHeaders->Signature != 0x00004550)
	{
		return false;
	}

	return true;
}

auto Util::GetSectionHeaders(void* ImageBase, PIMAGE_SECTION_HEADER& SectionHeaders, uint16_t& NumberOfSections) -> bool
{
	SectionHeaders = nullptr;
	NumberOfSections = 0;

	PIMAGE_NT_HEADERS64 NtHeaders = nullptr;

	if (!GetNtHeaders64(ImageBase, NtHeaders))
	{
		return false;
	}

	NumberOfSections = NtHeaders->FileHeader.NumberOfSections;
	SectionHeaders = Cast::To<PIMAGE_SECTION_HEADER>(Cast::To<uint8_t*>(&NtHeaders->OptionalHeader) + NtHeaders->FileHeader.SizeOfOptionalHeader);

	return true;
}