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
}