#include "Util.hpp"
#include "../Memory/Memory.hpp"
#include "../../UEFIpp.hpp"

extern "C" uint8_t AsmRdRand64(uint64_t* Value);
extern "C" uint8_t AsmRdSeed64(uint64_t* Value);

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

static constexpr uint32_t MAX_ATTEMPTS = 16;

static auto GetRandom64(uint64_t& Value) -> bool
{
	for (uint32_t i = 0; i < MAX_ATTEMPTS; ++i)
	{
		if (AsmRdSeed64(&Value))
		{
			return true;
		}
	}

	for (uint32_t i = 0; i < MAX_ATTEMPTS; ++i)
	{
		if (AsmRdRand64(&Value))
		{
			return true;
		}
	}

	return false;
}

auto Util::FillRandomBytes(void* Buffer, uint64_t Size) -> bool
{
	if (!Buffer || !Size)
	{
		return false;
	}

	auto Bytes = Cast::To<uint8_t*>(Buffer);
	uint64_t Offset = 0;

	while ((Offset + sizeof(uint64_t)) <= Size)
	{
		uint64_t Value = 0;

		if (!GetRandom64(Value))
		{
			return false;
		}

		for (uint32_t i = 0; i < sizeof(uint64_t); ++i)
		{
			Bytes[Offset + i] = Cast::To<uint8_t>((Value >> (i * 8)) & 0xFF);
		}

		Offset += sizeof(uint64_t);
	}

	if (Offset < Size)
	{
		uint64_t Value = 0;

		if (!GetRandom64(Value))
		{
			return false;
		}

		auto Remaining = Size - Offset;

		for (uint64_t i = 0; i < Remaining; ++i)
		{
			Bytes[Offset + i] = Cast::To<uint8_t>((Value >> (i * 8)) & 0xFF);
		}
	}

	return true;
}

auto Util::MakeRandomBuffer(Vector<uint8_t>& Buffer, uint64_t Size) -> bool
{
	if (!Buffer.Resize(Size))
	{
		return false;
	}

	return FillRandomBytes(Buffer.Data(), Size);
}

auto Util::WriteBigEndian16(void* Buffer, uint16_t Value) -> void
{
	if (!Buffer)
	{
		return;
	}

	auto Bytes = Cast::To<uint8_t*>(Buffer);
	Bytes[0] = Cast::To<uint8_t>((Value >> 8) & 0xFF);
	Bytes[1] = Cast::To<uint8_t>(Value & 0xFF);
}

auto Util::WriteBigEndian24(void* Buffer, uint32_t Value) -> void
{
	if (!Buffer)
	{
		return;
	}

	auto Bytes = Cast::To<uint8_t*>(Buffer);
	Bytes[0] = Cast::To<uint8_t>((Value >> 16) & 0xFF);
	Bytes[1] = Cast::To<uint8_t>((Value >> 8) & 0xFF);
	Bytes[2] = Cast::To<uint8_t>(Value & 0xFF);
}

auto Util::WriteBigEndian32(void* Buffer, uint32_t Value) -> void
{
	if (!Buffer)
	{
		return;
	}

	auto Bytes = Cast::To<uint8_t*>(Buffer);
	Bytes[0] = Cast::To<uint8_t>((Value >> 24) & 0xFF);
	Bytes[1] = Cast::To<uint8_t>((Value >> 16) & 0xFF);
	Bytes[2] = Cast::To<uint8_t>((Value >> 8) & 0xFF);
	Bytes[3] = Cast::To<uint8_t>(Value & 0xFF);
}

auto Util::ReadBigEndian16(const void* Buffer, uint16_t& Value) -> bool
{
	Value = 0;

	if (!Buffer)
	{
		return false;
	}

	auto Bytes = Cast::To<const uint8_t*>(Buffer);

	Value = (Cast::To<uint16_t>(Bytes[0]) << 8) | Cast::To<uint16_t>(Bytes[1]);

	return true;
}

auto Util::ReadBigEndian24(const void* Buffer, uint32_t& Value) -> bool
{
	Value = 0;

	if (!Buffer)
	{
		return false;
	}

	auto Bytes = Cast::To<const uint8_t*>(Buffer);

	Value = (Cast::To<uint32_t>(Bytes[0]) << 16) | (Cast::To<uint32_t>(Bytes[1]) << 8) | Cast::To<uint32_t>(Bytes[2]);

	return true;
}

auto Util::ReadBigEndian32(const void* Buffer, uint32_t& Value) -> bool
{
	Value = 0;

	if (!Buffer)
	{
		return false;
	}

	auto Bytes = Cast::To<const uint8_t*>(Buffer);

	Value = (Cast::To<uint32_t>(Bytes[0]) << 24) | (Cast::To<uint32_t>(Bytes[1]) << 16) | (Cast::To<uint32_t>(Bytes[2]) << 8) | Cast::To<uint32_t>(Bytes[3]);

	return true;
}