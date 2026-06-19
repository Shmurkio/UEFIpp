#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::UEFI
{
	using Handle = Foundation::Void*;
	using Event = Foundation::Void*;
	using PhysicalAddress = Foundation::Uint64;
	using VirtualAddress = Foundation::Uint64;
	using Tpl = Foundation::UintN;

	struct ListEntry
	{
		ListEntry* ForwardLink;
		ListEntry* BackLink;
	};

    struct Time
    {
        Foundation::Uint16 Year;
        Foundation::Uint8 Month;
        Foundation::Uint8 Day;
        Foundation::Uint8 Hour;
        Foundation::Uint8 Minute;
        Foundation::Uint8 Second;
        Foundation::Uint8 Pad1;
        Foundation::Uint32 Nanosecond;
        Foundation::Int16 TimeZone;
        Foundation::Uint8 Daylight;
        Foundation::Uint8 Pad2;
    };
}