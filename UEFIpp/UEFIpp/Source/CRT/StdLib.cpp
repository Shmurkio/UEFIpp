#include <UEFIpp/CRT/StdLib.hpp>
#include <UEFIpp/CRT/Heap.hpp>

extern "C"
{
	__declspec(restrict) void* malloc(std::size_t Size)
	{
		return UEFIpp::CRT::Heap::Allocate(Size);
	}

	__declspec(restrict) void* calloc(std::size_t Count, std::size_t Size)
	{
		return UEFIpp::CRT::Heap::AllocateZeroed(Count, Size);
	}

	__declspec(restrict) void* realloc(void* Address, std::size_t Size)
	{
		return UEFIpp::CRT::Heap::Reallocate(Address, Size);
	}

	void free(void* Address)
	{
		UEFIpp::CRT::Heap::Free(Address);
	}

	void* aligned_alloc(std::size_t Alignment, std::size_t Size)
	{
		if (!Alignment || !UEFIpp::Foundation::Bit::IsPowerOfTwo(Alignment))
		{
			return nullptr;
		}

		if (!UEFIpp::Foundation::Bit::IsAligned(Size, Alignment))
		{
			return nullptr;
		}

		return UEFIpp::CRT::Heap::AllocateAligned(Size, Alignment);
	}
}