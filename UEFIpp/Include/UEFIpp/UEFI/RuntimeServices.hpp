#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Types.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Guid.hpp>
#include <UEFIpp/UEFI/TableHeader.hpp>
#include <UEFIpp/UEFI/Memory.hpp>
#include <UEFIpp/UEFI/Capsule.hpp>

namespace UEFIpp::UEFI::Table
{
	enum class ResetType : Foundation::Uint32
	{
		Cold,
		Warm,
		Shutdown,
		PlatformSpecific
	};

	class Time
	{
	public:
		Foundation::Uint16 Year;
		Foundation::Uint8 Month;
		Foundation::Uint8 Day;
		Foundation::Uint8 Hour;
		Foundation::Uint8 Minute;
		Foundation::Uint8 Second;
		Foundation::Uint8 Pad1;
		Foundation::Uint32 Nanosecond;
		Foundation::Int16 TimeZone;
		Foundation::Uint8 Daylight;
		Foundation::Uint8 Pad2;

		[[nodiscard]] constexpr auto operator<=>(const Time&) const = default;
	};

	class TimeCapabilities
	{
	public:
		Foundation::Uint32 Resolution;
		Foundation::Uint32 Accuracy;
		Foundation::Bool SetsToZero;

		[[nodiscard]] constexpr auto operator<=>(const TimeCapabilities&) const = default;
	};

	using GetTimeFn = StatusCode(*)(Time* Time, TimeCapabilities* Capabilities);
	using SetTimeFn = StatusCode(*)(Time* Time);
	using GetWakeupTimeFn = StatusCode(*)(Foundation::Bool* Enabled, Foundation::Bool* Pending, Time* Time);
	using SetWakeupTimeFn = StatusCode(*)(Foundation::Bool Enable, Time* Time);

	using SetVirtualAddressMapFn = StatusCode(*)(Foundation::UintN MemoryMapSize, Foundation::UintN DescriptorSize, Foundation::Uint32 DescriptorVersion, MemoryDescriptor* VirtualMap);
	using ConvertPointerFn = StatusCode(*)(Foundation::UintN DebugDisposition, Foundation::Void** Address);

	using GetVariableFn = StatusCode(*)(Foundation::WChar* VariableName, Guid* VendorGuid, Foundation::Uint32* Attributes, Foundation::UintN* DataSize, Foundation::Void* Data);
	using GetNextVariableNameFn = StatusCode(*)(Foundation::UintN* VariableNameSize, Foundation::WChar* VariableName, Guid* VendorGuid);
	using SetVariableFn = StatusCode(*)(Foundation::WChar* VariableName, Guid* VendorGuid, Foundation::Uint32 Attributes, Foundation::UintN DataSize, Foundation::Void* Data);

	using GetNextHighMonotonicCountFn = StatusCode(*)(Foundation::Uint32* HighCount);
	using ResetSystemFn = Foundation::Void(*)(ResetType ResetType, Status ResetStatus, Foundation::UintN DataSize, Foundation::Void* ResetData);

	using UpdateCapsuleFn = StatusCode(*)(CapsuleHeader** CapsuleHeaderArray, Foundation::UintN CapsuleCount, PhysicalAddress ScatterGatherList);
	using QueryCapsuleCapabilitiesFn = StatusCode(*)(CapsuleHeader** CapsuleHeaderArray, Foundation::UintN CapsuleCount, Foundation::Uint64* MaximumCapsuleSize, ResetType* ResetType);
	using QueryVariableInfoFn = StatusCode(*)(Foundation::Uint32 Attributes, Foundation::Uint64* MaximumVariableStorageSize, Foundation::Uint64* RemainingVariableStorageSize, Foundation::Uint64* MaximumVariableSize);

	class RuntimeServices
	{
	public:
		Header Header;

		GetTimeFn GetTime;
		SetTimeFn SetTime;
		GetWakeupTimeFn GetWakeupTime;
		SetWakeupTimeFn SetWakeupTime;

		SetVirtualAddressMapFn SetVirtualAddressMap;
		ConvertPointerFn ConvertPointer;

		GetVariableFn GetVariable;
		GetNextVariableNameFn GetNextVariableName;
		SetVariableFn SetVariable;

		GetNextHighMonotonicCountFn GetNextHighMonotonicCount;
		ResetSystemFn ResetSystem;

		UpdateCapsuleFn UpdateCapsule;
		QueryCapsuleCapabilitiesFn QueryCapsuleCapabilities;
		QueryVariableInfoFn QueryVariableInfo;
	};
}