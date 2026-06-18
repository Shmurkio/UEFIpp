#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Types.hpp>
#include <UEFIpp/UEFI/Memory.hpp>

namespace UEFIpp::Protocols
{
	class PciIo;

	enum class PciIoWidth : Foundation::Uint32
	{
		Uint8,
		Uint16,
		Uint32,
		Uint64,

		FifoUint8,
		FifoUint16,
		FifoUint32,
		FifoUint64,

		FillUint8,
		FillUint16,
		FillUint32,
		FillUint64,

		Maximum
	};

	enum class PciIoOperation : Foundation::Uint32
	{
		BusMasterRead,
		BusMasterWrite,
		BusMasterCommonBuffer,
		Maximum
	};

	enum class PciIoAttributeOperation : Foundation::Uint32
	{
		Get,
		Set,
		Enable,
		Disable,
		Supported,
		Maximum
	};

	enum class PciIoAttribute : Foundation::Uint64
	{
		Io = 0x0100,
		Memory = 0x0200,
		BusMaster = 0x0400,
		MemoryWriteCombine = 0x0080,
		MemoryCached = 0x0800,
		MemoryDisable = 0x1000,
		EmbeddedDevice = 0x2000,
		EmbeddedRom = 0x4000,
		DualAddressCycle = 0x8000
	};

	struct PciIoAccess
	{
		using Function = UEFI::StatusCode(*)(PciIo* This, PciIoWidth Width, Foundation::Uint8 BarIndex, Foundation::Uint64 Offset, Foundation::UintN Count, Foundation::Void* Buffer);

		Function Read;
		Function Write;
	};

	struct PciIoConfigAccess
	{
		using Function = UEFI::StatusCode(*)(PciIo* This, PciIoWidth Width, Foundation::Uint32 Offset, Foundation::UintN Count, Foundation::Void* Buffer);

		Function Read;
		Function Write;
	};

	using PciIoPollMemFn = UEFI::StatusCode(*)(PciIo* This, PciIoWidth Width, Foundation::Uint8 BarIndex, Foundation::Uint64 Offset, Foundation::Uint64 Mask, Foundation::Uint64 Value, Foundation::Uint64 Delay, Foundation::Uint64* Result);
	using PciIoPollIoFn = PciIoPollMemFn;

	using PciIoCopyMemFn = UEFI::StatusCode(*)(PciIo* This, PciIoWidth Width, Foundation::Uint8 DestBarIndex, Foundation::Uint64 DestOffset, Foundation::Uint8 SrcBarIndex, Foundation::Uint64 SrcOffset, Foundation::UintN Count);
	using PciIoMapFn = UEFI::StatusCode(*)(PciIo* This, PciIoOperation Operation, Foundation::Void* HostAddress, Foundation::UintN* NumberOfBytes, Foundation::Uint64* DeviceAddress, Foundation::Void** Mapping);
	using PciIoUnmapFn = UEFI::StatusCode(*)(PciIo* This, Foundation::Void* Mapping);
	using PciIoAllocateBufferFn = UEFI::StatusCode(*)(PciIo* This, UEFI::AllocateType Type, UEFI::MemoryType MemoryType, Foundation::UintN Pages, Foundation::Void** HostAddress, Foundation::Uint64 Attributes);
	using PciIoFreeBufferFn = UEFI::StatusCode(*)(PciIo* This, Foundation::UintN Pages, Foundation::Void* HostAddress);
	using PciIoFlushFn = UEFI::StatusCode(*)(PciIo* This);
	using PciIoGetLocationFn = UEFI::StatusCode(*)(PciIo* This, Foundation::UintN* Segment, Foundation::UintN* Bus, Foundation::UintN* Device, Foundation::UintN* Function);
	using PciIoAttributesFn = UEFI::StatusCode(*)(PciIo* This, PciIoAttributeOperation Operation, Foundation::Uint64 Attributes, Foundation::Uint64* Result);
	using PciIoGetBarAttributesFn = UEFI::StatusCode(*)(PciIo* This, Foundation::Uint8 BarIndex, Foundation::Uint64* Supports, Foundation::Void** Resources);
	using PciIoSetBarAttributesFn = UEFI::StatusCode(*)(PciIo* This, Foundation::Uint64 Attributes, Foundation::Uint8 BarIndex, Foundation::Uint64* Offset, Foundation::Uint64* Length);

	class PciIo
	{
	public:
		PciIoPollMemFn PollMem;
		PciIoPollIoFn PollIo;

		PciIoAccess Mem;
		PciIoAccess Io;
		PciIoConfigAccess Pci;

		PciIoCopyMemFn CopyMem;
		PciIoMapFn Map;
		PciIoUnmapFn Unmap;

		PciIoAllocateBufferFn AllocateBuffer;
		PciIoFreeBufferFn FreeBuffer;
		PciIoFlushFn Flush;

		PciIoGetLocationFn GetLocation;
		PciIoAttributesFn Attributes;
		PciIoGetBarAttributesFn GetBarAttributes;
		PciIoSetBarAttributesFn SetBarAttributes;

		Foundation::Uint64 RomSize;
		Foundation::Void* RomImage;

		[[nodiscard]] auto ReadConfig8(Foundation::Uint32 Offset, Foundation::Uint8& Value) -> UEFI::StatusCode
		{
			return Pci.Read(this, PciIoWidth::Uint8, Offset, 1, &Value);
		}

		[[nodiscard]] auto ReadConfig16(Foundation::Uint32 Offset, Foundation::Uint16& Value) -> UEFI::StatusCode
		{
			return Pci.Read(this, PciIoWidth::Uint16, Offset, 1, &Value);
		}

		[[nodiscard]] auto ReadConfig32(Foundation::Uint32 Offset, Foundation::Uint32& Value) -> UEFI::StatusCode
		{
			return Pci.Read(this, PciIoWidth::Uint32, Offset, 1, &Value);
		}

		[[nodiscard]] auto WriteConfig16(Foundation::Uint32 Offset, Foundation::Uint16 Value) -> UEFI::StatusCode
		{
			return Pci.Write(this, PciIoWidth::Uint16, Offset, 1, &Value);
		}

		[[nodiscard]] auto ReadBar32(Foundation::Uint8 BarIndex, Foundation::Uint64 Offset, Foundation::Uint32& Value) -> UEFI::StatusCode
		{
			return Mem.Read(this, PciIoWidth::Uint32, BarIndex, Offset, 1, &Value);
		}

		[[nodiscard]] auto WriteBar32(Foundation::Uint8 BarIndex, Foundation::Uint64 Offset, Foundation::Uint32 Value) -> UEFI::StatusCode
		{
			return Mem.Write(this, PciIoWidth::Uint32, BarIndex, Offset, 1, &Value);
		}
	};

	static_assert(sizeof(PciIoAccess) == 16);
	static_assert(sizeof(PciIoConfigAccess) == 16);
	static_assert(sizeof(PciIo) == 160);
	static_assert(Foundation::Traits::IsStandardLayout<PciIo>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<PciIo>::Value);
}