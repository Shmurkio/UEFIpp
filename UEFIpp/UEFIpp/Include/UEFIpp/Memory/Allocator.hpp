#pragma once

#include <UEFIpp/Architecture/Architecture.hpp>
#include <UEFIpp/UEFI/Memory.hpp>

namespace UEFIpp::Memory
{
	class Allocator
	{
	public:
		Allocator() = delete;

		[[nodiscard]] static auto AllocatePool(Foundation::Size NumberOfBytes, UEFI::MemoryType Type = UEFI::MemoryType::BootServicesData, Foundation::Bool ZeroMemory = true) -> Foundation::Void*;
		static auto FreePool(Foundation::Void* Address) -> Foundation::Bool;

		[[nodiscard]] static auto AllocatePages(Foundation::Size NumberOfPages, UEFI::MemoryType Type = UEFI::MemoryType::BootServicesData, Foundation::Bool ZeroMemory = true) -> Foundation::Void*;
		static auto FreePages(Foundation::Void* Address, Foundation::Size NumberOfPages) -> Foundation::Bool;

		[[nodiscard]] static auto AllocatePageTable() -> Architecture::X64::PageTable*;
		static auto FreePageTable(Architecture::X64::PageTable* Table) -> Foundation::Bool;
	};
}