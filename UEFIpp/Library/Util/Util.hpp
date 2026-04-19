#pragma once

#include <cstdint>
#include "../../UEFIpp.hpp"

namespace Util
{
	auto IsGuidEqual(CGUID& Guid1, CGUID& Guid2) -> bool;
	auto GetImageSize(void* ImageBase, uint64_t& ImageSize) -> bool;
	auto GetNtHeaders64(void* ImageBase, PIMAGE_NT_HEADERS64& NtHeaders) -> bool;
	auto GetSectionHeaders(void* ImageBase, PIMAGE_SECTION_HEADER& SectionHeaders, uint16_t& NumberOfSections) -> bool;
}