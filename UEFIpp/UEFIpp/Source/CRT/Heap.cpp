#include <UEFIpp/CRT/Heap.hpp>
#include <UEFIpp/Memory/Memory.hpp>
#include <UEFIpp/UEFI/Context.hpp>

namespace UEFIpp::CRT
{
	namespace
	{
		struct AllocationHeader
		{
			Foundation::Void* Base;
			Foundation::Size Size;
		};

		[[nodiscard]] auto HeaderFromAddress(const Foundation::Void* Address) -> AllocationHeader*
		{
			if (!Address)
			{
				return nullptr;
			}

			const auto* Bytes = Foundation::Cast::Auto<const Foundation::Uint8*>(Address);
			Bytes -= sizeof(AllocationHeader);

			const auto* Header = Foundation::Cast::Auto<const AllocationHeader*>(Bytes);

			return Foundation::Cast::RemoveConst(Header);
		}

		[[nodiscard]] auto AddressFromHeader(AllocationHeader* Header) -> Foundation::Void*
		{
			if (!Header)
			{
				return nullptr;
			}

			auto* Bytes = Foundation::Cast::Auto<Foundation::Uint8*>(Header);
			Bytes += sizeof(AllocationHeader);

			return Foundation::Cast::Auto<Foundation::Void*>(Bytes);
		}
	}

	auto Heap::Allocate(Foundation::Size NumberOfBytes) -> Foundation::Void*
	{
		return AllocateAligned(NumberOfBytes, alignof(AllocationHeader));
	}

	auto Heap::AllocateZeroed(Foundation::Size Count, Foundation::Size Size) -> Foundation::Void*
	{
		if (!Count || !Size)
		{
			return nullptr;
		}

		const auto Total = Count * Size;

		if (Total / Count != Size)
		{
			return nullptr;
		}

		auto* Address = Allocate(Total);

		if (!Address)
		{
			return nullptr;
		}

		Memory::Zero(Address, Total);

		return Address;
	}

	auto Heap::Reallocate(Foundation::Void* Address, Foundation::Size NumberOfBytes) -> Foundation::Void*
	{
		if (!Address)
		{
			return Allocate(NumberOfBytes);
		}

		if (!NumberOfBytes)
		{
			Free(Address);
			return nullptr;
		}

		const auto OldSize = AllocationSize(Address);
		auto* NewAddress = Allocate(NumberOfBytes);

		if (!NewAddress)
		{
			return nullptr;
		}

		Memory::Copy(NewAddress, Address, OldSize < NumberOfBytes ? OldSize : NumberOfBytes);
		Free(Address);

		return NewAddress;
	}

	auto Heap::Free(Foundation::Void* Address) -> void
	{
		if (!Address)
		{
			return;
		}

		auto* Header = HeaderFromAddress(Address);

		if (!Header || !Header->Base)
		{
			return;
		}

		Memory::Allocator::FreePool(Header->Base);
	}

	auto Heap::AllocationSize(const Foundation::Void* Address) -> Foundation::Size
	{
		const auto* Header = HeaderFromAddress(Address);

		if (!Header)
		{
			return 0;
		}

		return Header->Size;
	}

	auto Heap::AllocateAligned(Foundation::Size NumberOfBytes, Foundation::Size Alignment) -> Foundation::Void*
	{
		if (!NumberOfBytes || !Alignment)
		{
			return nullptr;
		}

		if (!Foundation::Bit::IsPowerOfTwo(Alignment))
		{
			return nullptr;
		}

		if (Alignment < alignof(AllocationHeader))
		{
			Alignment = alignof(AllocationHeader);
		}

		const auto TotalSize = NumberOfBytes + Alignment - 1 + sizeof(AllocationHeader);
		auto* Base = Memory::Allocator::AllocatePool(TotalSize, UEFI::Context::CrtMemoryType(), false);

		if (!Base)
		{
			return nullptr;
		}

		const auto BaseAddress = Foundation::Cast::PointerToAddress<Foundation::UintPtr>(Base);
		const auto UserAddress = Foundation::Bit::AlignUp(BaseAddress + sizeof(AllocationHeader), Foundation::Cast::Auto<Foundation::UintPtr>(Alignment));
		auto* Header = Foundation::Cast::Auto<AllocationHeader*>(UserAddress - sizeof(AllocationHeader));

		Header->Base = Base;
		Header->Size = NumberOfBytes;

		return Foundation::Cast::Auto<Foundation::Void*>(UserAddress);
	}
}