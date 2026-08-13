#pragma once

#include <UEFIpp/UEFI/Memory.hpp>

namespace UEFIpp::Memory
{
	// UEFI-bound pool allocation facade retained for existing CRT code.
	class Allocator
	{
	public:
		Allocator() = delete;

		[[nodiscard]] static auto AllocatePool(Foundation::Size NumberOfBytes, UEFI::MemoryType Type = UEFI::MemoryType::BootServicesData, Foundation::Bool ZeroMemory = true) -> Foundation::Void*;
		[[nodiscard]] static auto FreePool(Foundation::Void* Address) -> Foundation::Bool;
	};
}
