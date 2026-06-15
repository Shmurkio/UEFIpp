#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Types.hpp>
#include <UEFIpp/UEFI/Guid.hpp>

namespace UEFIpp::UEFI
{
	class CapsuleBlockDescriptor
	{
	public:
		Foundation::Uint64 Length;

		union
		{
			PhysicalAddress DataBlock;
			PhysicalAddress ContinuationPointer;
		};

		[[nodiscard]] constexpr auto operator<=>(const CapsuleBlockDescriptor&) const = default;
	};

	class CapsuleHeader
	{
	public:
		Guid CapsuleGuid;
		Foundation::Uint32 HeaderSize;
		Foundation::Uint32 Flags;
		Foundation::Uint32 CapsuleImageSize;

		[[nodiscard]] constexpr auto operator<=>(const CapsuleHeader&) const = default;
	};

	class CapsuleTable
	{
	public:
		Foundation::Uint32 CapsuleArrayNumber;
		Foundation::Void* CapsulePtr[1];
	};

	inline constexpr auto CapsuleFlagsPersistAcrossReset = Foundation::Bit::Mask<Foundation::Uint32>(16);
	inline constexpr auto CapsuleFlagsPopulateSystemTable = Foundation::Bit::Mask<Foundation::Uint32>(17);
	inline constexpr auto CapsuleFlagsInitiateReset = Foundation::Bit::Mask<Foundation::Uint32>(18);

	static_assert(sizeof(CapsuleBlockDescriptor) == 16);
	static_assert(sizeof(CapsuleHeader) == 28);

	static_assert(Foundation::Traits::IsStandardLayout<CapsuleBlockDescriptor>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<CapsuleBlockDescriptor>::Value);

	static_assert(Foundation::Traits::IsStandardLayout<CapsuleHeader>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<CapsuleHeader>::Value);

	static_assert(Foundation::Traits::IsStandardLayout<CapsuleTable>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<CapsuleTable>::Value);
}