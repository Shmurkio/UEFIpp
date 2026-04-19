#pragma once

#include <cstdint>
#include "../Cast/Cast.hpp"

namespace Memory
{
	typedef union _VIRTUAL_ADDRESS
	{
		uint64_t Value;

		struct
		{
			uint64_t Offset : 12;
			uint64_t PtIndex : 9;
			uint64_t PdIndex : 9;
			uint64_t PdptIndex : 9;
			uint64_t Pml4Index : 9;
			uint64_t SignExtend : 16;
		};
	} VIRTUAL_ADDRESS, *PVIRTUAL_ADDRESS;

	typedef union _HARDWARE_PTE
	{
		uint64_t Value;

		struct
		{
			uint64_t Present : 1;
			uint64_t ReadWrite : 1;
			uint64_t UserSupervisor : 1;
			uint64_t PageWriteThrough : 1;
			uint64_t PageCacheDisable : 1;
			uint64_t Accessed : 1;
			uint64_t Dirty : 1;
			uint64_t LargePage : 1;
			uint64_t Global : 1;
			uint64_t Ignored2 : 3;
			uint64_t PageFrameNumber : 40;
			uint64_t Ignored3 : 11;
			uint64_t NoExecute : 1;
		};
	} HARDWARE_PTE, *PHARDWARE_PTE;

	typedef struct _PAGE_WALK
	{
		VIRTUAL_ADDRESS Va;

		uint64_t Cr3Physical;
		uint64_t Pml4Physical;
		uint64_t PdptPhysical;
		uint64_t PdPhysical;
		uint64_t PtPhysical;

		HARDWARE_PTE Pml4Entry;
		HARDWARE_PTE PdptEntry;
		HARDWARE_PTE PdEntry;
		HARDWARE_PTE PtEntry;

		enum class LEVEL : uint32_t
		{
			None = 0,
			Pdpt,
			Pd,
			Pt
		};

		LEVEL LeafLevel;
	} PAGE_WALK, *PPAGE_WALK;

	static constexpr uint64_t PAGE_PRESENT = 1ULL << 0;
	static constexpr uint64_t PAGE_READ_WRITE = 1ULL << 1;
	static constexpr uint64_t PAGE_USER = 1ULL << 2;
	static constexpr uint64_t PAGE_WRITE_THROUGH = 1ULL << 3;
	static constexpr uint64_t PAGE_CACHE_DISABLE = 1ULL << 4;
	static constexpr uint64_t PAGE_ACCESSED = 1ULL << 5;
	static constexpr uint64_t PAGE_DIRTY = 1ULL << 6;
	static constexpr uint64_t PAGE_LARGE_PAGE = 1ULL << 7;
	static constexpr uint64_t PAGE_GLOBAL = 1ULL << 8;
	static constexpr uint64_t PAGE_NO_EXECUTE = 1ULL << 63;

	auto ZeroMemory(void* Address, uint64_t NumberOfBytes) -> bool;

	auto AllocatePool(void*& Buffer, uint64_t NumberOfBytes, bool Runtime, bool ZeroBytes) -> bool;

	template<typename T>
	auto AllocatePool(T*& Buffer, uint64_t NumberOfBytes, bool Runtime, bool ZeroBytes) -> bool
	{
		Buffer = { 0 };

		if (!NumberOfBytes)
		{
			return false;
		}

		void* Allocation = nullptr;

		if (!AllocatePool(Allocation, NumberOfBytes, Runtime, ZeroBytes))
		{
			return false;
		}

		Buffer = Cast::To<T*>(Allocation);

		return true;
	}

	auto FreePool(void*& Buffer, bool ClearPointer) -> bool;

	template<typename T>
	auto FreePool(T*& Buffer, bool ClearPointer) -> bool
	{
		if (!Buffer)
		{
			return false;
		}

		auto Allocation = Cast::To<void*>(Buffer);

		if (!FreePool(Allocation, ClearPointer))
		{
			return false;
		}

		Buffer = Cast::To<T*>(Allocation);

		return true;
	}

	auto AllocatePages(void*& Buffer, uint64_t NumberOfPages, bool Runtime, bool ZeroBytes) -> bool;

	template<typename T>
	auto AllocatePages(T*& Buffer, uint64_t NumberOfPages, bool Runtime, bool ZeroBytes) -> bool
	{
		Buffer = { 0 };

		if (!NumberOfPages)
		{
			return false;
		}

		void* Allocation = nullptr;

		if (!AllocatePages(Allocation, NumberOfPages, Runtime, ZeroBytes))
		{
			return false;
		}

		Buffer = Cast::To<T*>(Allocation);

		return true;
	}

	auto FreePages(void*& Buffer, uint64_t NumberOfPages, bool ClearPointer) -> bool;

	template<typename T>
	auto FreePages(T*& Buffer, uint64_t NumberOfPages, bool ClearPointer) -> bool
	{
		if (!Buffer)
		{
			return false;
		}

		auto Allocation = Cast::To<void*>(Buffer);

		if (!FreePages(Allocation, NumberOfPages, ClearPointer))
		{
			return false;
		}

		Buffer = Cast::To<T*>(Allocation);

		return true;
	}

	auto AlignDown(uint64_t Value, uint64_t Alignment) -> uint64_t;
	auto AlignUp(uint64_t Value, uint64_t Alignment) -> uint64_t;

	template<typename T>
	constexpr auto Move(T& Value) -> typename Cast::Detail::RemoveReference<T>::Type&&
	{
		return static_cast<typename Cast::Detail::RemoveReference<T>::Type&&>(Value);
	}

	template<typename T>
	constexpr auto Forward(typename Cast::Detail::RemoveReference<T>::Type& Value) -> T&&
	{
		return static_cast<T&&>(Value);
	}

	template<typename T>
	constexpr auto Forward(typename Cast::Detail::RemoveReference<T>::Type&& Value) -> T&&
	{
		return static_cast<T&&>(Value);
	}

	auto CopyMemory(void* Destination, void* Source, uint64_t NumberOfBytes, bool BypassWriteProtect = false) -> bool;

	template<typename T, typename U>
	auto CopyMemory(T* Destination, U* Source, uint64_t NumberOfBytes, bool BypassWriteProtect = false) -> bool
	{
		return CopyMemory(Cast::To<void*>(Destination), Cast::To<void*>(Source), NumberOfBytes, BypassWriteProtect);
	}

	auto IsCanonicalAddress(uint64_t Address) -> bool;

	template<typename T>
	auto IsCanonicalAddress(T* Address) -> bool
	{
		return IsCanonicalAddress(Cast::To<uint64_t>(Address));
	}

	auto WalkPageTables(void* VirtualAddress, PAGE_WALK& Walk) -> bool;

	auto GetLeafEntry(void* VirtualAddress, HARDWARE_PTE& Entry, PAGE_WALK::LEVEL& Level) -> bool;
	auto GetLeafEntryPointer(void* VirtualAddress, PHARDWARE_PTE& Entry, PAGE_WALK::LEVEL& Level) -> bool;

	auto VirtualToPhysical(void* VirtualAddress, void*& PhysicalAddress) -> bool;

	auto MapPage(void* PhysicalAddress, uint64_t Flags, void*& VirtualAddress) -> bool;

	auto MapPhysicalRange(void* PhysicalAddress, uint64_t NumberOfBytes, uint64_t Flags, void*& VirtualAddress) -> bool;

	auto UnmapPage(void* VirtualAddress) -> bool;

	auto UnmapRange(void* VirtualAddress) -> bool;

	auto GetMappedPhysical(void* VirtualAddress, void*& PhysicalAddress) -> bool;

	auto IsMappedByMemoryLibrary(void* VirtualAddress) -> bool;

	auto CompareMemory(const void* Buffer1, const void* Buffer2, uint64_t NumberOfBytes, uint64_t& MatchingBytes) -> bool;

	auto ReadU16(void* Address, uint16_t& Value) -> bool;
	auto ReadU32(void* Address, uint32_t& Value) -> bool;
}

void* __cdecl operator new(size_t Size);
void* __cdecl operator new[](size_t Size);

void* __cdecl operator new(size_t Size, void* Address) noexcept;
void* __cdecl operator new[](size_t Size, void* Address) noexcept;

void __cdecl operator delete(void* Memory) noexcept;
void __cdecl operator delete[](void* Memory) noexcept;

void __cdecl operator delete(void* Memory, size_t Size) noexcept;
void __cdecl operator delete[](void* Memory, size_t Size) noexcept;