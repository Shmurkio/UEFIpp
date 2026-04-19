#include "Memory.hpp"
#include "../../UEFIpp.hpp"
#include "../Cpu/Cpu.hpp"
#include "../Vector/Vector.hpp"

typedef struct _MAP_ENTRY
{
	void* Virtual;
	void* Physical;
	uint64_t NumberOfBytes;
	uint64_t Flags;
	bool Present;
} MAP_ENTRY, *PMAP_ENTRY;

static constexpr uint64_t MAP_BASE = 0xFFFF900000000000ULL;
static constexpr uint64_t MAP_LIMIT = 0xFFFF9FFFFFFFF000ULL;

static Vector<MAP_ENTRY> gMappings;
static uint64_t gNextMapVirtual = MAP_BASE;

static constexpr uint64_t PAGE_SIZE = 0x1000;
static constexpr uint64_t PAGE_OFFSET_MASK = PAGE_SIZE - 1;
static constexpr uint64_t PAGE_FRAME_MASK = 0x000FFFFFFFFFF000ULL;

static constexpr uint64_t LARGE_PAGE_2MB_SIZE = 0x200000;
static constexpr uint64_t LARGE_PAGE_2MB_MASK = 0x000FFFFFFFE00000ULL;

static constexpr uint64_t LARGE_PAGE_1GB_SIZE = 0x40000000;
static constexpr uint64_t LARGE_PAGE_1GB_MASK = 0x000FFFFFC0000000ULL;
static constexpr uint64_t ENTRIES_PER_TABLE = 512;

static bool gRecursivePagingInitialized = false;
static uint16_t gRecursivePml4Index = 0;
static uint64_t gRecursiveCr3Physical = 0;
static uint64_t gRecursivePml4Physical = 0;
static Memory::PHARDWARE_PTE gRecursivePml4Virtual = nullptr;

auto Memory::ZeroMemory(void* Address, uint64_t NumberOfBytes) -> bool
{
	if (!Address || !NumberOfBytes)
	{
		return false;
	}

	auto Byte = Cast::To<uint8_t*>(Address);

	for (uint64_t i = 0; i < NumberOfBytes; ++i)
	{
		Byte[i] = 0;
	}

	return true;
}

auto Memory::AllocatePool(void*& Buffer, uint64_t NumberOfBytes, bool Runtime, bool ZeroBytes) -> bool
{
	Buffer = nullptr;

	if (!NumberOfBytes)
	{
		return false;
	}

	void* Allocation = nullptr;
	auto Type = Runtime ? EFI_MEMORY_TYPE::EfiRuntimeServicesData : EFI_MEMORY_TYPE::EfiBootServicesData;
	auto Status = gBS->AllocatePool(Type, NumberOfBytes, &Allocation);

	if (EfiError(Status) || !Allocation)
	{
		return false;
	}

	if (ZeroBytes)
	{
		ZeroMemory(Allocation, NumberOfBytes);
	}

	Buffer = Allocation;

	return true;
}

auto Memory::FreePool(void*& Buffer, bool ClearPointer) -> bool
{
	if (!Buffer)
	{
		return false;
	}

	auto Status = gBS->FreePool(Buffer);

	if (EfiError(Status))
	{
		return false;
	}

	if (ClearPointer)
	{
		Buffer = nullptr;
	}

	return true;
}

auto Memory::AllocatePages(void*& Buffer, uint64_t NumberOfPages, bool Runtime, bool ZeroBytes) -> bool
{
	Buffer = nullptr;

	if (!NumberOfPages)
	{
		return false;
	}

	void* Allocation = nullptr;
	auto Type = Runtime ? EFI_MEMORY_TYPE::EfiRuntimeServicesData : EFI_MEMORY_TYPE::EfiBootServicesData;

	auto Status = gBS->AllocatePages(EFI_ALLOCATE_TYPE::AllocateAnyPages, Type, NumberOfPages, &Allocation);

	if (EfiError(Status) || !Allocation)
	{
		return false;
	}

	if (ZeroBytes)
	{
		ZeroMemory(Allocation, NumberOfPages * PAGE_SIZE);
	}

	Buffer = Allocation;

	return true;
}

auto Memory::FreePages(void*& Buffer, uint64_t NumberOfPages, bool ClearPointer) -> bool
{
	if (!Buffer || !NumberOfPages)
	{
		return false;
	}

	auto Status = gBS->FreePages(Cast::To<uint64_t>(Buffer), NumberOfPages);

	if (EfiError(Status))
	{
		return false;
	}

	if (ClearPointer)
	{
		Buffer = nullptr;
	}

	return true;
}

auto Memory::AlignDown(uint64_t Value, uint64_t Alignment) -> uint64_t
{
	return Value & ~(Alignment - 1ULL);
}

auto Memory::AlignUp(uint64_t Value, uint64_t Alignment) -> uint64_t
{
	return AlignDown(Value + Alignment - 1ULL, Alignment);
}

static auto IsCanonicalAddress(uint64_t Address) -> bool
{
	auto Upper = Address >> 48;
	return Upper == 0x0000ULL || Upper == 0xFFFFULL;
}

auto Memory::IsCanonicalAddress(uint64_t Address) -> bool
{
	auto Upper = Address >> 48;
	return (Upper == 0x0000ULL) || (Upper == 0xFFFFULL);
}

static auto InvalidatePage(void* Address) -> void
{
	if (!Address)
	{
		return;
	}

	__invlpg(Address);
}

static auto MakeCanonicalAddress(uint16_t Pml4Index, uint16_t PdptIndex, uint16_t PdIndex, uint16_t PtIndex, uint16_t Offset) -> void*
{
	uint64_t Value = (Cast::To<uint64_t>(Pml4Index & 0x1FF) << 39) | (Cast::To<uint64_t>(PdptIndex & 0x1FF) << 30) | (Cast::To<uint64_t>(PdIndex & 0x1FF) << 21) | (Cast::To<uint64_t>(PtIndex & 0x1FF) << 12) | (Cast::To<uint64_t>(Offset & 0xFFF));

	if (Value & (1ULL << 47))
	{
		Value |= 0xFFFF000000000000ULL;
	}

	return Cast::To<void*>(Value);
}

static auto IsEntryPresent(Memory::HARDWARE_PTE& Entry) -> bool
{
	return Entry.Present;
}

static auto IsEntryLargePage(Memory::HARDWARE_PTE& Entry) -> bool
{
	return Entry.LargePage;
}

static auto IsPageAligned(uint64_t Address) -> bool
{
	return (Address & PAGE_OFFSET_MASK) == 0;
}

static auto EntryToPhysical(Memory::HARDWARE_PTE& Entry) -> uint64_t
{
	if (!IsEntryPresent(Entry))
	{
		return 0;
	}

	return Entry.Value & PAGE_FRAME_MASK;
}

static auto FindFreePml4Index(Memory::PHARDWARE_PTE& Pml4, uint16_t& Index) -> bool
{
	Index = 0;

	if (!Pml4)
	{
		return false;
	}

	for (uint16_t i = 0; i < ENTRIES_PER_TABLE; ++i)
	{
		if (!IsEntryPresent(Pml4[i]))
		{
			Index = i;
			return true;
		}
	}

	return false;
}

static auto GetRecursivePml4Va() -> Memory::PHARDWARE_PTE
{
	if (!gRecursivePagingInitialized)
	{
		return nullptr;
	}

	return Cast::To<Memory::PHARDWARE_PTE>(MakeCanonicalAddress(gRecursivePml4Index, gRecursivePml4Index, gRecursivePml4Index, gRecursivePml4Index, 0));
}

static auto GetRecursivePdptVa(Memory::VIRTUAL_ADDRESS& Va) -> Memory::PHARDWARE_PTE
{
	if (!gRecursivePagingInitialized)
	{
		return nullptr;
	}

	return Cast::To<Memory::PHARDWARE_PTE>(MakeCanonicalAddress(gRecursivePml4Index, gRecursivePml4Index, gRecursivePml4Index, Va.Pml4Index, 0));
}

static auto GetRecursivePdVa(Memory::VIRTUAL_ADDRESS& Va) -> Memory::PHARDWARE_PTE
{
	if (!gRecursivePagingInitialized)
	{
		return nullptr;
	}

	return Cast::To<Memory::PHARDWARE_PTE>(MakeCanonicalAddress(gRecursivePml4Index, gRecursivePml4Index, Va.Pml4Index, Va.PdptIndex, 0));
}

static auto GetRecursivePtVa(Memory::VIRTUAL_ADDRESS& Va) -> Memory::PHARDWARE_PTE
{
	if (!gRecursivePagingInitialized)
	{
		return nullptr;
	}

	return Cast::To<Memory::PHARDWARE_PTE>(MakeCanonicalAddress(gRecursivePml4Index, Va.Pml4Index, Va.PdptIndex, Va.PdIndex, 0));
}

auto InitializeRecursivePaging() -> bool
{
	if (gRecursivePagingInitialized)
	{
		return true;
	}

	auto CurrentCr3Physical = Cpu::ReadCr3() & PAGE_FRAME_MASK; // Might break...
	auto CurrentPml4 = Cast::To<Memory::PHARDWARE_PTE>(CurrentCr3Physical);

	if (!CurrentCr3Physical || !CurrentPml4)
	{
		return false;
	}

	uint16_t RecursiveIndex = 0;

	if (!FindFreePml4Index(CurrentPml4, RecursiveIndex))
	{
		return false;
	}

	Memory::PHARDWARE_PTE NewPml4 = nullptr;

	if (!Memory::AllocatePages(NewPml4, 1, false, true) || !NewPml4)
	{
		return false;
	}

	if (!Memory::CopyMemory(NewPml4, CurrentPml4, PAGE_SIZE))
	{
		FreePages(NewPml4, 1, true);
		return false;
	}

	auto NewPml4Physical = Memory::AlignDown(Cast::To<uint64_t>(NewPml4), PAGE_SIZE);

	NewPml4[RecursiveIndex].Value = 0;
	NewPml4[RecursiveIndex].Value = (NewPml4Physical & PAGE_FRAME_MASK) | Memory::PAGE_PRESENT | Memory::PAGE_READ_WRITE;

	Cpu::WriteCr3(NewPml4Physical);

	gRecursivePml4Index = RecursiveIndex;
	gRecursiveCr3Physical = NewPml4Physical;
	gRecursivePml4Physical = NewPml4Physical;

	gRecursivePagingInitialized = true;
	gRecursivePml4Virtual = GetRecursivePml4Va();

	if (!gRecursivePml4Virtual)
	{
		gRecursivePagingInitialized = false;
		return false;
	}

	return true;
}

auto Memory::WalkPageTables(void* VirtualAddress, PAGE_WALK& Walk) -> bool
{
	ZeroMemory(&Walk, sizeof(Walk));

	if (!VirtualAddress)
	{
		return false;
	}

	auto VaValue = Cast::To<uint64_t>(VirtualAddress);

	if (!IsCanonicalAddress(VaValue))
	{
		return false;
	}

	if (!InitializeRecursivePaging())
	{
		return false;
	}

	Walk.Va.Value = VaValue;
	Walk.LeafLevel = PAGE_WALK::LEVEL::None;
	Walk.Cr3Physical = Cpu::ReadCr3() & PAGE_FRAME_MASK;
	Walk.Pml4Physical = Walk.Cr3Physical;

	auto Pml4 = GetRecursivePml4Va();

	if (!Pml4)
	{
		return false;
	}

	Walk.Pml4Entry = Pml4[Walk.Va.Pml4Index];

	if (!IsEntryPresent(Walk.Pml4Entry))
	{
		return false;
	}

	Walk.PdptPhysical = EntryToPhysical(Walk.Pml4Entry);

	auto Pdpt = GetRecursivePdptVa(Walk.Va);

	if (!Pdpt)
	{
		return false;
	}

	Walk.PdptEntry = Pdpt[Walk.Va.PdptIndex];

	if (!IsEntryPresent(Walk.PdptEntry))
	{
		return false;
	}

	if (IsEntryLargePage(Walk.PdptEntry))
	{
		Walk.LeafLevel = PAGE_WALK::LEVEL::Pdpt;
		return true;
	}

	Walk.PdPhysical = EntryToPhysical(Walk.PdptEntry);

	auto Pd = GetRecursivePdVa(Walk.Va);

	if (!Pd)
	{
		return false;
	}

	Walk.PdEntry = Pd[Walk.Va.PdIndex];

	if (!IsEntryPresent(Walk.PdEntry))
	{
		return false;
	}

	if (IsEntryLargePage(Walk.PdEntry))
	{
		Walk.LeafLevel = PAGE_WALK::LEVEL::Pd;
		return true;
	}

	Walk.PtPhysical = EntryToPhysical(Walk.PdEntry);

	auto Pt = GetRecursivePtVa(Walk.Va);

	if (!Pt)
	{
		return false;
	}

	Walk.PtEntry = Pt[Walk.Va.PtIndex];

	if (!IsEntryPresent(Walk.PtEntry))
	{
		return false;
	}

	Walk.LeafLevel = PAGE_WALK::LEVEL::Pt;

	return true;
}

auto Memory::GetLeafEntry(void* VirtualAddress, HARDWARE_PTE& Entry, PAGE_WALK::LEVEL& Level) -> bool
{
	ZeroMemory(&Entry, sizeof(Entry));
	Level = PAGE_WALK::LEVEL::None;

	PAGE_WALK Walk = {};

	if (!WalkPageTables(VirtualAddress, Walk))
	{
		return false;
	}

	switch (Walk.LeafLevel)
	{
	case PAGE_WALK::LEVEL::Pdpt:
	{
		Entry = Walk.PdptEntry;
		Level = Walk.LeafLevel;
		return true;
	}
	case PAGE_WALK::LEVEL::Pd:
	{
		Entry = Walk.PdEntry;
		Level = Walk.LeafLevel;
		return true;
	}
	case PAGE_WALK::LEVEL::Pt:
	{
		Entry = Walk.PtEntry;
		Level = Walk.LeafLevel;
		return true;
	}
	default:
	{
		break;
	}
	}

	return false;
}

auto Memory::GetLeafEntryPointer(void* VirtualAddress, PHARDWARE_PTE& Entry, PAGE_WALK::LEVEL& Level) -> bool
{
	Entry = nullptr;
	Level = PAGE_WALK::LEVEL::None;

	PAGE_WALK Walk = {};

	if (!WalkPageTables(VirtualAddress, Walk))
	{
		return false;
	}

	switch (Walk.LeafLevel)
	{
	case PAGE_WALK::LEVEL::Pdpt:
	{
		auto Pdpt = GetRecursivePdptVa(Walk.Va);

		if (!Pdpt)
		{
			return false;
		}

		Entry = &Pdpt[Walk.Va.PdptIndex];
		Level = Walk.LeafLevel;
		return true;
	}
	case PAGE_WALK::LEVEL::Pd:
	{
		auto Pd = GetRecursivePdVa(Walk.Va);

		if (!Pd)
		{
			return false;
		}

		Entry = &Pd[Walk.Va.PdIndex];
		Level = Walk.LeafLevel;
		return true;
	}
	case PAGE_WALK::LEVEL::Pt:
	{
		auto Pt = GetRecursivePtVa(Walk.Va);

		if (!Pt)
		{
			return false;
		}

		Entry = &Pt[Walk.Va.PtIndex];
		Level = Walk.LeafLevel;
		return true;
	}
	default:
	{
		break;
	}
	}

	return false;
}

auto Memory::VirtualToPhysical(void* VirtualAddress, void*& PhysicalAddress) -> bool
{
	PhysicalAddress = nullptr;

	if (!VirtualAddress)
	{
		return false;
	}

	PAGE_WALK Walk = {};

	if (!WalkPageTables(VirtualAddress, Walk))
	{
		return false;
	}

	auto Va = Cast::To<uint64_t>(VirtualAddress);

	uint64_t PhysicalBase = 0;
	uint64_t Offset = 0;

	switch (Walk.LeafLevel)
	{
	case PAGE_WALK::LEVEL::Pt:
	{
		if (!IsEntryPresent(Walk.PtEntry))
		{
			return false;
		}

		PhysicalBase = Walk.PtEntry.Value & PAGE_FRAME_MASK;
		Offset = Va & PAGE_OFFSET_MASK;
		break;
	}
	case PAGE_WALK::LEVEL::Pd:
	{
		if (!IsEntryPresent(Walk.PdEntry) || !IsEntryLargePage(Walk.PdEntry))
		{
			return false;
		}

		PhysicalBase = Walk.PdEntry.Value & LARGE_PAGE_2MB_MASK;
		Offset = Va & (LARGE_PAGE_2MB_SIZE - 1);
		break;
	}
	case PAGE_WALK::LEVEL::Pdpt:
	{
		if (!IsEntryPresent(Walk.PdptEntry) || !IsEntryLargePage(Walk.PdptEntry))
		{
			return false;
		}

		PhysicalBase = Walk.PdptEntry.Value & LARGE_PAGE_1GB_MASK;
		Offset = Va & (LARGE_PAGE_1GB_SIZE - 1);
		break;
	}
	default:
	{
		return false;
	}
	}

	PhysicalAddress = Cast::To<void*>(PhysicalBase + Offset);

	return true;
}

static auto EnsurePageTable(void* VirtualAddress, Memory::PHARDWARE_PTE& Pt) -> bool
{
	Pt = nullptr;

	if (!VirtualAddress)
	{
		return false;
	}

	Memory::VIRTUAL_ADDRESS Va = {};
	Va.Value = Cast::To<uint64_t>(VirtualAddress);

	auto Pml4 = GetRecursivePml4Va();

	if (!Pml4)
	{
		return false;
	}

	auto Pml4Entry = &Pml4[Va.Pml4Index];

	if (!IsEntryPresent(*Pml4Entry))
	{
		void* NewPage = nullptr;

		if (!Memory::AllocatePages(NewPage, 1, false, true) || !NewPage)
		{
			return false;
		}

		void* NewPhysicalVa = nullptr;

		if (!Memory::VirtualToPhysical(NewPage, NewPhysicalVa) || !NewPhysicalVa)
		{
			Memory::FreePages(NewPage, 1, true);
			return false;
		}

		auto NewPhysical = Memory::AlignDown(Cast::To<uint64_t>(NewPhysicalVa), PAGE_SIZE);
		Pml4Entry->Value = NewPhysical | Memory::PAGE_PRESENT | Memory::PAGE_READ_WRITE;
		InvalidatePage(GetRecursivePdptVa(Va));
	}

	auto Pdpt = GetRecursivePdptVa(Va);

	if (!Pdpt)
	{
		return false;
	}

	auto PdptEntry = &Pdpt[Va.PdptIndex];

	if (!IsEntryPresent(*PdptEntry))
	{
		void* NewPage = nullptr;

		if (!Memory::AllocatePages(NewPage, 1, false, true) || !NewPage)
		{
			return false;
		}

		void* NewPhysicalVa = nullptr;

		if (!Memory::VirtualToPhysical(NewPage, NewPhysicalVa) || !NewPhysicalVa)
		{
			Memory::FreePages(NewPage, 1, true);
			return false;
		}

		auto NewPhysical = Memory::AlignDown(Cast::To<uint64_t>(NewPhysicalVa), PAGE_SIZE);
		PdptEntry->Value = NewPhysical | Memory::PAGE_PRESENT | Memory::PAGE_READ_WRITE;
		InvalidatePage(GetRecursivePdVa(Va));
	}
	else if (IsEntryLargePage(*PdptEntry))
	{
		return false;
	}

	auto Pd = GetRecursivePdVa(Va);

	if (!Pd)
	{
		return false;
	}

	auto PdEntry = &Pd[Va.PdIndex];

	if (!IsEntryPresent(*PdEntry))
	{
		void* NewPage = nullptr;

		if (!Memory::AllocatePages(NewPage, 1, false, true) || !NewPage)
		{
			return false;
		}

		void* NewPhysicalVa = nullptr;

		if (!Memory::VirtualToPhysical(NewPage, NewPhysicalVa) || !NewPhysicalVa)
		{
			Memory::FreePages(NewPage, 1, true);
			return false;
		}

		auto NewPhysical = Memory::AlignDown(Cast::To<uint64_t>(NewPhysicalVa), PAGE_SIZE);
		PdEntry->Value = NewPhysical | Memory::PAGE_PRESENT | Memory::PAGE_READ_WRITE;
		InvalidatePage(GetRecursivePtVa(Va));
	}
	else if (IsEntryLargePage(*PdEntry))
	{
		return false;
	}

	Pt = GetRecursivePtVa(Va);

	return Pt != nullptr;
}

auto MapPageToAddress(void* PhysicalAddress, void* VirtualAddress, uint64_t Flags) -> bool
{
	if (!PhysicalAddress || !VirtualAddress)
	{
		return false;
	}

	auto Physical = Cast::To<uint64_t>(PhysicalAddress);
	auto Virtual = Cast::To<uint64_t>(VirtualAddress);

	if (!IsPageAligned(Physical) || !IsPageAligned(Virtual))
	{
		return false;
	}

	if (!IsCanonicalAddress(Virtual))
	{
		return false;
	}

	if (!InitializeRecursivePaging())
	{
		return false;
	}

	Memory::PHARDWARE_PTE Pt = nullptr;

	if (!EnsurePageTable(VirtualAddress, Pt) || !Pt)
	{
		return false;
	}

	Memory::VIRTUAL_ADDRESS Va = {};
	Va.Value = Virtual;

	auto PtEntry = &Pt[Va.PtIndex];

	if (IsEntryPresent(*PtEntry))
	{
		return false;
	}

	PtEntry->Value = 0;
	PtEntry->Value = (Physical & PAGE_FRAME_MASK) | Flags | 1ULL;

	InvalidatePage(VirtualAddress);

	return true;
}

auto MapPhysicalRangeToAddress(void* PhysicalAddress, uint64_t NumberOfBytes, void* VirtualAddress, uint64_t Flags) -> bool
{
	if (!PhysicalAddress || !NumberOfBytes || !VirtualAddress)
	{
		return false;
	}

	auto Physical = Cast::To<uint64_t>(PhysicalAddress);
	auto Virtual = Cast::To<uint64_t>(VirtualAddress);

	if (!IsCanonicalAddress(Virtual))
	{
		return false;
	}

	auto PhysicalBase = Memory::AlignDown(Physical, PAGE_SIZE);
	auto PhysicalOffset = Physical - PhysicalBase;

	auto VirtualBase = Memory::AlignDown(Virtual, PAGE_SIZE);
	auto VirtualOffset = Virtual - VirtualBase;

	if (PhysicalOffset != VirtualOffset)
	{
		return false;
	}

	auto TotalSize = Memory::AlignUp(PhysicalOffset + NumberOfBytes, PAGE_SIZE);
	auto PageCount = TotalSize / PAGE_SIZE;

	for (uint64_t i = 0; i < PageCount; ++i)
	{
		auto CurrentPhysical = PhysicalBase + (i * PAGE_SIZE);
		auto CurrentVirtual = VirtualBase + (i * PAGE_SIZE);

		if (!MapPageToAddress(Cast::To<void*>(CurrentPhysical), Cast::To<void*>(CurrentVirtual), Flags))
		{
			return false;
		}
	}

	return true;
}

auto UnmapPageFromAddress(void* PhysicalAddress, void* VirtualAddress) -> bool
{
	if (!PhysicalAddress || !VirtualAddress)
	{
		return false;
	}

	auto Physical = Cast::To<uint64_t>(PhysicalAddress);
	auto Virtual = Cast::To<uint64_t>(VirtualAddress);

	if (!IsPageAligned(Physical) || !IsPageAligned(Virtual))
	{
		return false;
	}

	if (!IsCanonicalAddress(Virtual))
	{
		return false;
	}

	if (!InitializeRecursivePaging())
	{
		return false;
	}

	Memory::PHARDWARE_PTE Entry = nullptr;
	Memory::PAGE_WALK::LEVEL Level = Memory::PAGE_WALK::LEVEL::None;

	if (!GetLeafEntryPointer(VirtualAddress, Entry, Level) || !Entry)
	{
		return false;
	}

	if (Level != Memory::PAGE_WALK::LEVEL::Pt)
	{
		return false;
	}

	if (!IsEntryPresent(*Entry))
	{
		return false;
	}

	auto MappedPhysical = Entry->Value & PAGE_FRAME_MASK;

	if (MappedPhysical != (Physical & PAGE_FRAME_MASK))
	{
		return false;
	}

	Entry->Value = 0;

	InvalidatePage(VirtualAddress);

	return true;
}

auto UnmapPhysicalRangeFromAddress(void* PhysicalAddress, uint64_t NumberOfBytes, void* VirtualAddress) -> bool
{
	if (!PhysicalAddress || !NumberOfBytes || !VirtualAddress)
	{
		return false;
	}

	auto Physical = Cast::To<uint64_t>(PhysicalAddress);
	auto Virtual = Cast::To<uint64_t>(VirtualAddress);

	if (!IsCanonicalAddress(Virtual))
	{
		return false;
	}

	auto PhysicalBase = Memory::AlignDown(Physical, PAGE_SIZE);
	auto PhysicalOffset = Physical - PhysicalBase;

	auto VirtualBase = Memory::AlignDown(Virtual, PAGE_SIZE);
	auto VirtualOffset = Virtual - VirtualBase;

	if (PhysicalOffset != VirtualOffset)
	{
		return false;
	}

	auto TotalSize = Memory::AlignUp(PhysicalOffset + NumberOfBytes, PAGE_SIZE);
	auto PageCount = TotalSize / PAGE_SIZE;

	for (uint64_t i = 0; i < PageCount; ++i)
	{
		auto CurrentPhysical = PhysicalBase + (i * PAGE_SIZE);
		auto CurrentVirtual = VirtualBase + (i * PAGE_SIZE);

		if (!UnmapPageFromAddress(Cast::To<void*>(CurrentPhysical), Cast::To<void*>(CurrentVirtual)))
		{
			return false;
		}
	}

	return true;
}

auto Memory::CopyMemory(void* Destination, void* Source, uint64_t NumberOfBytes, bool BypassWriteProtect) -> bool
{
	if (!Destination || !Source || !NumberOfBytes)
	{
		return false;
	}

	if (!BypassWriteProtect)
	{
		auto Dest = Cast::To<uint8_t*>(Destination);
		auto Src = Cast::To<uint8_t*>(Source);

		for (uint64_t i = 0; i < NumberOfBytes; ++i)
		{
			Dest[i] = Src[i];
		}

		return true;
	}

	void* DestinationPhysical = nullptr;

	if (!VirtualToPhysical(Destination, DestinationPhysical) || !DestinationPhysical)
	{
		return false;
	}

	void* WritableMapping = nullptr;

	if (!MapPhysicalRange(DestinationPhysical, NumberOfBytes, PAGE_READ_WRITE, WritableMapping) || !WritableMapping)
	{
		return false;
	}

	auto Success = false;
	auto OriginalCr0 = Cpu::ReadCr0();
	auto Cr0WithoutWp = OriginalCr0 & ~(1ULL << 16);

	Cpu::WriteCr0(Cr0WithoutWp);

	auto Dest = Cast::To<uint8_t*>(WritableMapping);
	auto Src = Cast::To<uint8_t*>(Source);

	for (uint64_t i = 0; i < NumberOfBytes; ++i)
	{
		Dest[i] = Src[i];
	}

	Success = true;

	Cpu::WriteCr0(OriginalCr0);

	if (!UnmapRange(WritableMapping))
	{
		return false;
	}

	return Success;
}

static auto RangesOverlap(uint64_t StartA, uint64_t SizeA, uint64_t StartB, uint64_t SizeB) -> bool
{
	if (!SizeA || !SizeB)
	{
		return false;
	}

	auto EndA = StartA + SizeA;
	auto EndB = StartB + SizeB;

	return (StartA < EndB) && (StartB < EndA);
}

static auto FindMappingByVirtual(void* VirtualAddress) -> PMAP_ENTRY
{
	if (!VirtualAddress)
	{
		return nullptr;
	}

	auto Virtual = Cast::To<uint64_t>(VirtualAddress);

	for (auto& Entry : gMappings)
	{
		if (!Entry.Present)
		{
			continue;
		}

		auto Base = Cast::To<uint64_t>(Entry.Virtual);
		auto Size = Entry.NumberOfBytes;

		if (!Size)
		{
			continue;
		}

		if (Virtual >= Base && Virtual < (Base + Size))
		{
			return &Entry;
		}
	}

	return nullptr;
}

static auto IsRangeFree(uint64_t VirtualBase, uint64_t NumberOfBytes) -> bool
{
	if (!VirtualBase || !NumberOfBytes)
	{
		return false;
	}

	if (!Memory::IsCanonicalAddress(VirtualBase))
	{
		return false;
	}

	if ((VirtualBase + NumberOfBytes) < VirtualBase)
	{
		return false;
	}

	if ((VirtualBase + NumberOfBytes - 1ULL) > MAP_LIMIT)
	{
		return false;
	}

	for (auto& Entry : gMappings)
	{
		if (!Entry.Present)
		{
			continue;
		}

		auto ExistingBase = Cast::To<uint64_t>(Entry.Virtual);
		auto ExistingSize = Entry.NumberOfBytes;

		if (RangesOverlap(VirtualBase, NumberOfBytes, ExistingBase, ExistingSize))
		{
			return false;
		}
	}

	return true;
}

static auto FindFreeVirtualRange(uint64_t NumberOfBytes, void*& VirtualAddress) -> bool
{
	VirtualAddress = nullptr;

	if (!NumberOfBytes)
	{
		return false;
	}

	auto Size = Memory::AlignUp(NumberOfBytes, PAGE_SIZE);
	auto Candidate = Memory::AlignUp(gNextMapVirtual, PAGE_SIZE);

	if (Candidate < MAP_BASE)
	{
		Candidate = MAP_BASE;
	}

	for (;;)
	{
		if (Candidate < MAP_BASE || Candidate > MAP_LIMIT)
		{
			break;
		}

		if ((Candidate + Size) < Candidate)
		{
			break;
		}

		if ((Candidate + Size - 1ULL) > MAP_LIMIT)
		{
			break;
		}

		if (IsRangeFree(Candidate, Size))
		{
			VirtualAddress = Cast::To<void*>(Candidate);
			gNextMapVirtual = Candidate + Size;
			return true;
		}

		Candidate += PAGE_SIZE;
	}

	Candidate = MAP_BASE;

	for (;;)
	{
		if (Candidate < MAP_BASE || Candidate > MAP_LIMIT)
		{
			break;
		}

		if ((Candidate + Size) < Candidate)
		{
			break;
		}

		if ((Candidate + Size - 1ULL) > MAP_LIMIT)
		{
			break;
		}

		if (IsRangeFree(Candidate, Size))
		{
			VirtualAddress = Cast::To<void*>(Candidate);
			gNextMapVirtual = Candidate + Size;
			return true;
		}

		Candidate += PAGE_SIZE;
	}

	return false;
}

static auto AddMapping(void* PhysicalAddress, void* VirtualAddress, uint64_t NumberOfBytes, uint64_t Flags) -> bool
{
	if (!PhysicalAddress || !VirtualAddress || !NumberOfBytes)
	{
		return false;
	}

	MAP_ENTRY Entry = {};
	Entry.Physical = PhysicalAddress;
	Entry.Virtual = VirtualAddress;
	Entry.NumberOfBytes = NumberOfBytes;
	Entry.Flags = Flags;
	Entry.Present = true;

	return gMappings.PushBack(Entry);
}

auto Memory::MapPage(void* PhysicalAddress, uint64_t Flags, void*& VirtualAddress) -> bool
{
	VirtualAddress = nullptr;

	if (!PhysicalAddress)
	{
		return false;
	}

	auto Physical = Cast::To<uint64_t>(PhysicalAddress);

	if (!IsPageAligned(Physical))
	{
		return false;
	}

	void* ChosenVirtual = nullptr;

	if (!FindFreeVirtualRange(PAGE_SIZE, ChosenVirtual) || !ChosenVirtual)
	{
		return false;
	}

	if (!MapPageToAddress(PhysicalAddress, ChosenVirtual, Flags))
	{
		return false;
	}

	if (!AddMapping(PhysicalAddress, ChosenVirtual, PAGE_SIZE, Flags))
	{
		UnmapPageFromAddress(PhysicalAddress, ChosenVirtual);
		return false;
	}

	VirtualAddress = ChosenVirtual;

	return true;
}

auto Memory::MapPhysicalRange(void* PhysicalAddress, uint64_t NumberOfBytes, uint64_t Flags, void*& VirtualAddress) -> bool
{
	VirtualAddress = nullptr;

	if (!PhysicalAddress || !NumberOfBytes)
	{
		return false;
	}

	auto Physical = Cast::To<uint64_t>(PhysicalAddress);

	auto PhysicalBase = AlignDown(Physical, PAGE_SIZE);
	auto PhysicalOffset = Physical - PhysicalBase;
	auto TotalSize = AlignUp(PhysicalOffset + NumberOfBytes, PAGE_SIZE);

	void* ChosenVirtualBase = nullptr;

	if (!FindFreeVirtualRange(TotalSize, ChosenVirtualBase) || !ChosenVirtualBase)
	{
		return false;
	}

	auto ReturnedVirtual = Cast::To<void*>(Cast::To<uint64_t>(ChosenVirtualBase) + PhysicalOffset);

	if (!MapPhysicalRangeToAddress(PhysicalAddress, NumberOfBytes, ReturnedVirtual, Flags))
	{
		return false;
	}

	if (!AddMapping(PhysicalAddress, ReturnedVirtual, NumberOfBytes, Flags))
	{
		UnmapPhysicalRangeFromAddress(PhysicalAddress, NumberOfBytes, ReturnedVirtual);
		return false;
	}

	VirtualAddress = ReturnedVirtual;

	return true;
}

auto Memory::UnmapPage(void* VirtualAddress) -> bool
{
	if (!VirtualAddress)
	{
		return false;
	}

	auto Entry = FindMappingByVirtual(VirtualAddress);

	if (!Entry || !Entry->Present)
	{
		return false;
	}

	if (Entry->NumberOfBytes > PAGE_SIZE)
	{
		return false;
	}

	if (!UnmapPageFromAddress(Entry->Physical, Entry->Virtual))
	{
		return false;
	}

	Entry->Present = false;

	return true;
}

auto Memory::UnmapRange(void* VirtualAddress) -> bool
{
	if (!VirtualAddress)
	{
		return false;
	}

	auto Entry = FindMappingByVirtual(VirtualAddress);

	if (!Entry || !Entry->Present)
	{
		return false;
	}

	if (!UnmapPhysicalRangeFromAddress(Entry->Physical, Entry->NumberOfBytes, Entry->Virtual))
	{
		return false;
	}

	Entry->Present = false;

	return true;
}

auto Memory::GetMappedPhysical(void* VirtualAddress, void*& PhysicalAddress) -> bool
{
	PhysicalAddress = nullptr;

	if (!VirtualAddress)
	{
		return false;
	}

	auto Entry = FindMappingByVirtual(VirtualAddress);

	if (!Entry || !Entry->Present)
	{
		return false;
	}

	auto Virtual = Cast::To<uint64_t>(VirtualAddress);
	auto EntryVirtual = Cast::To<uint64_t>(Entry->Virtual);
	auto EntryPhysical = Cast::To<uint64_t>(Entry->Physical);

	auto Offset = Virtual - EntryVirtual;

	if (Offset >= Entry->NumberOfBytes)
	{
		return false;
	}

	PhysicalAddress = Cast::To<void*>(EntryPhysical + Offset);

	return true;
}

auto Memory::IsMappedByMemoryLibrary(void* VirtualAddress) -> bool
{
	return FindMappingByVirtual(VirtualAddress) != nullptr;
}

void* __cdecl operator new(size_t Size)
{
	void* Allocation = nullptr;

	if (!Memory::AllocatePool(Allocation, static_cast<uint64_t>(Size), false, true))
	{
		return nullptr;
	}

	return Allocation;
}

void* __cdecl operator new[](size_t Size)
{
	void* Allocation = nullptr;

	if (!Memory::AllocatePool(Allocation, static_cast<uint64_t>(Size), false, true))
	{
		return nullptr;
	}

	return Allocation;
}

void* __cdecl operator new(size_t, void* Address) noexcept
{
	return Address;
}

void* __cdecl operator new[](size_t, void* Address) noexcept
{
	return Address;
}

void __cdecl operator delete(void* Memory) noexcept
{
	if (!Memory)
	{
		return;
	}

	void* Allocation = Memory;
	Memory::FreePool(Allocation, false);
}

void __cdecl operator delete[](void* Memory) noexcept
{
	if (!Memory)
	{
		return;
	}

	void* Allocation = Memory;
	Memory::FreePool(Allocation, false);
}

void __cdecl operator delete(void* Memory, size_t) noexcept
{
	if (!Memory)
	{
		return;
	}

	void* Allocation = Memory;
	Memory::FreePool(Allocation, false);
}

void __cdecl operator delete[](void* Memory, size_t) noexcept
{
	if (!Memory)
	{
		return;
	}

	void* Allocation = Memory;
	Memory::FreePool(Allocation, false);
}

auto Memory::CompareMemory(const void* Buffer1, const void* Buffer2, uint64_t NumberOfBytes, uint64_t& MatchingBytes) -> bool
{
	MatchingBytes = 0;

	if (!Buffer1 || !Buffer2 || !NumberOfBytes)
	{
		return false;
	}

	if (NumberOfBytes > (UINT64_MAX - Cast::To<uint64_t>(Buffer1)) || NumberOfBytes > (UINT64_MAX - Cast::To<uint64_t>(Buffer2)))
	{
		return false;
	}

	if (Buffer1 == Buffer2)
	{
		MatchingBytes = NumberOfBytes;
		return true;
	}

	auto Ptr1 = Cast::To<uint8_t*>(Buffer1);
	auto Ptr2 = Cast::To<uint8_t*>(Buffer2);

	for (uint64_t i = 0; i < NumberOfBytes; ++i)
	{
		if (Ptr1[i] != Ptr2[i])
		{
			return false;
		}

		++MatchingBytes;
	}
	
	return true;
}

auto Memory::ReadU16(void* Address, uint16_t& Value) -> bool
{
	Value = 0;

	if (!Address)
	{
		return false;
	}

	Value = *Cast::To<volatile uint16_t*>(Address);

	return true;
}

auto Memory::ReadU32(void* Address, uint32_t& Value) -> bool
{
	Value = 0;

	if (!Address)
	{
		return false;
	}

	Value = *Cast::To<volatile uint32_t*>(Address);

	return true;
}