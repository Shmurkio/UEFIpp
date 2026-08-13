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

}
