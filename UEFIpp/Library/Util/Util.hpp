#pragma once

#include <cstdint>
#include "../../UEFIpp.hpp"
#include "../Vector/Vector.hpp"

namespace Util
{
	auto IsGuidEqual(CGUID& Guid1, CGUID& Guid2) -> bool;
	auto GetImageSize(void* ImageBase, uint64_t& ImageSize) -> bool;
	auto GetNtHeaders64(void* ImageBase, PIMAGE_NT_HEADERS64& NtHeaders) -> bool;
	auto GetSectionHeaders(void* ImageBase, PIMAGE_SECTION_HEADER& SectionHeaders, uint16_t& NumberOfSections) -> bool;
	auto FillRandomBytes(void* Buffer, uint64_t Size) -> bool;
	auto MakeRandomBuffer(Vector<uint8_t>& Buffer, uint64_t Size) -> bool;
	auto WriteBigEndian16(void* Buffer, uint16_t Value) -> void;
	auto WriteBigEndian24(void* Buffer, uint32_t Value) -> void;
	auto WriteBigEndian32(void* Buffer, uint32_t Value) -> void;
	auto ReadBigEndian16(const void* Buffer, uint16_t& Value) -> bool;
	auto ReadBigEndian24(const void* Buffer, uint32_t& Value) -> bool;
	auto ReadBigEndian32(const void* Buffer, uint32_t& Value) -> bool;
}