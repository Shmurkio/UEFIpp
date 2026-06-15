#include <UEFIpp/Memory/Memory.hpp>
#include <UEFIpp/UEFI/Context.hpp>
#include <UEFIpp/UEFI/BootServices.hpp>

namespace
{
	[[nodiscard]] auto& BootServices()
	{
		return UEFIpp::UEFI::Context::BootServices();
	}
}

namespace UEFIpp::Memory
{
	auto Allocator::AllocatePool(Foundation::Size NumberOfBytes, UEFI::MemoryType Type, Foundation::Bool ZeroMemory) -> Foundation::Void*
	{
		if (!NumberOfBytes)
		{
			return nullptr;
		}

		Foundation::Void* Address = nullptr;
		const auto Status = BootServices().AllocatePool(Type, NumberOfBytes, &Address);

		if (UEFI::IsError(Status) || !Address)
		{
			return nullptr;
		}

		if (ZeroMemory)
		{
			Zero(Address, NumberOfBytes);
		}

		return Address;
	}

	auto Allocator::FreePool(Foundation::Void* Address) -> Foundation::Bool
	{
		if (!Address)
		{
			return false;
		}

		const auto Status = BootServices().FreePool(Address);

		return UEFI::IsSuccess(Status);
	}

	auto Allocator::AllocatePages(Foundation::Size NumberOfPages, UEFI::MemoryType Type, Foundation::Bool ZeroMemory) -> Foundation::Void*
	{
		if (!NumberOfPages)
		{
			return nullptr;
		}

		UEFI::PhysicalAddress Physical = 0;
		const auto Status = BootServices().AllocatePages(UEFI::AllocateType::AllocateAnyPages, Type, NumberOfPages, &Physical);

		if (UEFI::IsError(Status) || !Physical)
		{
			return nullptr;
		}

		auto Address = Foundation::Cast::AddressToPointer<Foundation::Void*>(Physical);

		if (ZeroMemory)
		{
			Zero(Address, NumberOfPages * Architecture::X64::Page::Size);
		}

		return Address;
	}

	auto Allocator::FreePages(Foundation::Void* Address, Foundation::Size NumberOfPages) -> Foundation::Bool
	{
		if (!Address || !NumberOfPages)
		{
			return false;
		}

		auto Physical = Foundation::Cast::PointerToAddress<UEFI::PhysicalAddress>(Address);
		const auto Status = BootServices().FreePages(Physical, NumberOfPages);

		return UEFI::IsSuccess(Status);
	}

	auto Allocator::AllocatePageTable() -> Architecture::X64::PageTable*
	{
		return Foundation::Cast::To<Architecture::X64::PageTable*>(AllocatePages(1, UEFI::MemoryType::BootServicesData, true));
	}

	auto Allocator::FreePageTable(Architecture::X64::PageTable* Table) -> Foundation::Bool
	{
		return FreePages(Table, 1);
	}
}