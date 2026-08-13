#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Types.hpp>

namespace UEFIpp::UEFI
{
	enum class AllocateType : Foundation::Uint32
	{
		AllocateAnyPages,
		AllocateMaxAddress,
		AllocateAddress,
		MaxAllocateType
	};

	enum class MemoryType : Foundation::Uint32
	{
		ReservedMemoryType,
		LoaderCode,
		LoaderData,
		BootServicesCode,
		BootServicesData,
		RuntimeServicesCode,
		RuntimeServicesData,
		ConventionalMemory,
		UnusableMemory,
		ACPIReclaimMemory,
		ACPIMemoryNVS,
		MemoryMappedIO,
		MemoryMappedIOPortSpace,
		PalCode,
		PersistentMemory,
		UnacceptedMemoryType,
		MaxMemoryType
	};

	using MemoryAttribute = Foundation::Uint64;

	inline constexpr auto MemoryUc = Foundation::Bit::Mask<MemoryAttribute>(0);
	inline constexpr auto MemoryWc = Foundation::Bit::Mask<MemoryAttribute>(1);
	inline constexpr auto MemoryWt = Foundation::Bit::Mask<MemoryAttribute>(2);
	inline constexpr auto MemoryWb = Foundation::Bit::Mask<MemoryAttribute>(3);
	inline constexpr auto MemoryUce = Foundation::Bit::Mask<MemoryAttribute>(4);

	inline constexpr auto MemoryWp = Foundation::Bit::Mask<MemoryAttribute>(12);
	inline constexpr auto MemoryRp = Foundation::Bit::Mask<MemoryAttribute>(13);
	inline constexpr auto MemoryXp = Foundation::Bit::Mask<MemoryAttribute>(14);
	inline constexpr auto MemoryNv = Foundation::Bit::Mask<MemoryAttribute>(15);
	inline constexpr auto MemoryMoreReliable = Foundation::Bit::Mask<MemoryAttribute>(16);
	inline constexpr auto MemoryRo = Foundation::Bit::Mask<MemoryAttribute>(17);
	inline constexpr auto MemorySp = Foundation::Bit::Mask<MemoryAttribute>(18);
	inline constexpr auto MemoryCpuCrypto = Foundation::Bit::Mask<MemoryAttribute>(19);
	inline constexpr auto MemoryHotPluggable = Foundation::Bit::Mask<MemoryAttribute>(20);

	inline constexpr auto MemoryIsaValid = Foundation::Bit::Mask<MemoryAttribute>(62);
	inline constexpr auto MemoryRuntime = Foundation::Bit::Mask<MemoryAttribute>(63);

	inline constexpr auto MemoryIsaMask = Foundation::Bit::RangeMask<MemoryAttribute>(44, 16);

	inline constexpr auto CacheAttributeMask = MemoryUc | MemoryWc | MemoryWt | MemoryWb | MemoryUce | MemoryWp;

	inline constexpr auto MemoryAccessMask = MemoryRp | MemoryXp | MemoryRo;

	inline constexpr auto MemoryAttributeMask = MemoryAccessMask | MemorySp | MemoryCpuCrypto;

	inline constexpr auto MemoryDescriptorVersion = Foundation::Uint32{ 1 };

	class MemoryDescriptor
	{
	public:
		Foundation::Uint32 Type;
		PhysicalAddress PhysicalStart;
		VirtualAddress VirtualStart;
		Foundation::Uint64 NumberOfPages;
		MemoryAttribute Attribute;

		[[nodiscard]] constexpr auto operator<=>(const MemoryDescriptor&) const = default;
	};

	static_assert(sizeof(MemoryDescriptor) == 40);
	static_assert(Foundation::Traits::IsStandardLayout<MemoryDescriptor>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<MemoryDescriptor>::Value);
}