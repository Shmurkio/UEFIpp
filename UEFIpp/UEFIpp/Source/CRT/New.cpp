#include <UEFIpp/CRT/Heap.hpp>
#include <UEFIpp/CRT/New.hpp>
#include <UEFIpp/CRT/StdLib.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>

namespace
{
	[[nodiscard]] auto AlignmentValue(std::align_val_t Alignment) -> std::size_t
	{
		return UEFIpp::Foundation::Cast::Auto<std::size_t>(Alignment);
	}

	[[nodiscard]] auto AllocateOrAbort(std::size_t Size) -> void*
	{
		auto* Address = malloc(Size);

		if (!Address)
		{
			// Add exceptions later.
			for (;;)
			{
			}
		}

		return Address;
	}

	[[nodiscard]] auto AllocateAlignedOrAbort(std::size_t Size, std::align_val_t Alignment) -> void*
	{
		auto* Address = UEFIpp::CRT::Heap::AllocateAligned(Size, AlignmentValue(Alignment));

		if (!Address)
		{
			// Add exceptions later.
			for (;;)
			{
			}
		}

		return Address;
	}
}

void* operator new(std::size_t Size)
{
	return AllocateOrAbort(Size);
}

void* operator new[](std::size_t Size)
{
	return AllocateOrAbort(Size);
}

void* operator new(std::size_t Size, std::align_val_t Alignment)
{
	return AllocateAlignedOrAbort(Size, Alignment);
}

void* operator new[](std::size_t Size, std::align_val_t Alignment)
{
	return AllocateAlignedOrAbort(Size, Alignment);
}

void* operator new(std::size_t Size, const std::nothrow_t&) noexcept
{
	return malloc(Size);
}

void* operator new[](std::size_t Size, const std::nothrow_t&) noexcept
{
	return malloc(Size);
}

void* operator new(std::size_t Size, std::align_val_t Alignment, const std::nothrow_t&) noexcept
{
	return UEFIpp::CRT::Heap::AllocateAligned(Size, AlignmentValue(Alignment));
}

void* operator new[](std::size_t Size, std::align_val_t Alignment, const std::nothrow_t&) noexcept
{
	return UEFIpp::CRT::Heap::AllocateAligned(Size, AlignmentValue(Alignment));
}

void operator delete(void* Address) noexcept
{
	free(Address);
}

void operator delete[](void* Address) noexcept
{
	free(Address);
}

void operator delete(void* Address, std::size_t) noexcept
{
	free(Address);
}

void operator delete[](void* Address, std::size_t) noexcept
{
	free(Address);
}

void operator delete(void* Address, std::align_val_t) noexcept
{
	free(Address);
}

void operator delete[](void* Address, std::align_val_t) noexcept
{
	free(Address);
}

void operator delete(void* Address, std::size_t, std::align_val_t) noexcept
{
	free(Address);
}

void operator delete[](void* Address, std::size_t, std::align_val_t) noexcept
{
	free(Address);
}

void operator delete(void* Address, const std::nothrow_t&) noexcept
{
	free(Address);
}

void operator delete[](void* Address, const std::nothrow_t&) noexcept
{
	free(Address);
}

void operator delete(void* Address, std::align_val_t, const std::nothrow_t&) noexcept
{
	free(Address);
}

void operator delete[](void* Address, std::align_val_t, const std::nothrow_t&) noexcept
{
	free(Address);
}