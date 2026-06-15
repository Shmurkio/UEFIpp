#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Types.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Guid.hpp>
#include <UEFIpp/UEFI/TableHeader.hpp>
#include <UEFIpp/UEFI/Memory.hpp>
#include <UEFIpp/UEFI/DevicePath.hpp>

namespace UEFIpp::UEFI::Table
{
	enum class TimerDelay : Foundation::Uint32
	{
		Cancel,
		Periodic,
		Relative
	};

	enum class InterfaceType : Foundation::Uint32
	{
		Native
	};

	enum class LocateSearchType : Foundation::Uint32
	{
		AllHandles,
		ByRegisterNotify,
		ByProtocol
	};

	class OpenProtocolInformationEntry
	{
	public:
		Handle AgentHandle;
		Handle ControllerHandle;
		Foundation::Uint32 Attributes;
		Foundation::Uint32 OpenCount;

		[[nodiscard]] constexpr auto operator<=>(const OpenProtocolInformationEntry&) const = default;
	};

	using RaiseTplFn = Tpl(*)(Tpl NewTpl);
	using RestoreTplFn = Foundation::Void(*)(Tpl OldTpl);

	using AllocatePagesFn = StatusCode(*)(AllocateType Type, MemoryType MemoryType, Foundation::UintN Pages, PhysicalAddress* Memory);
	using FreePagesFn = StatusCode(*)(PhysicalAddress Memory, Foundation::UintN Pages);
	using GetMemoryMapFn = StatusCode(*)(Foundation::UintN* MemoryMapSize, MemoryDescriptor* MemoryMap, Foundation::UintN* MapKey, Foundation::UintN* DescriptorSize, Foundation::Uint32* DescriptorVersion);
	using AllocatePoolFn = StatusCode(*)(MemoryType PoolType, Foundation::UintN Size, Foundation::Void** Buffer);
	using FreePoolFn = StatusCode(*)(Foundation::Void* Buffer);

	using EventNotifyFn = Foundation::Void(*)(Event Event, Foundation::Void* Context);
	using CreateEventFn = StatusCode(*)(Foundation::Uint32 Type, Tpl NotifyTpl, EventNotifyFn NotifyFunction, Foundation::Void* NotifyContext, Event* Event);
	using SetTimerFn = StatusCode(*)(Event Event, TimerDelay Type, Foundation::Uint64 TriggerTime);
	using WaitForEventFn = StatusCode(*)(Foundation::UintN NumberOfEvents, Event* Event, Foundation::UintN* Index);
	using SignalEventFn = StatusCode(*)(Event Event);
	using CloseEventFn = StatusCode(*)(Event Event);
	using CheckEventFn = StatusCode(*)(Event Event);

	using InstallProtocolInterfaceFn = StatusCode(*)(Handle* Handle, Guid* Protocol, InterfaceType InterfaceType, Foundation::Void* Interface);
	using ReinstallProtocolInterfaceFn = StatusCode(*)(Handle Handle, Guid* Protocol, Foundation::Void* OldInterface, Foundation::Void* NewInterface);
	using UninstallProtocolInterfaceFn = StatusCode(*)(Handle Handle, Guid* Protocol, Foundation::Void* Interface);
	using HandleProtocolFn = StatusCode(*)(Handle Handle, Guid* Protocol, Foundation::Void** Interface);

	using RegisterProtocolNotifyFn = StatusCode(*)(Guid* Protocol, Event Event, Foundation::Void** Registration);
	using LocateHandleFn = StatusCode(*)(LocateSearchType SearchType, Guid* Protocol, Foundation::Void* SearchKey, Foundation::UintN* BufferSize, Handle* Buffer);
	using LocateDevicePathFn = StatusCode(*)(Guid* Protocol, DevicePathProtocol** DevicePath, Handle* Device);
	using InstallConfigurationTableFn = StatusCode(*)(Guid* Guid, Foundation::Void* Table);

	using LoadImageFn = StatusCode(*)(Foundation::Bool BootPolicy, Handle ParentImageHandle, DevicePathProtocol* DevicePath, Foundation::Void* SourceBuffer, Foundation::UintN SourceSize, Handle* ImageHandle);
	using StartImageFn = StatusCode(*)(Handle ImageHandle, Foundation::UintN* ExitDataSize, Foundation::WChar** ExitData);
	using ExitFn = StatusCode(*)(Handle ImageHandle, Status ExitStatus, Foundation::UintN ExitDataSize, Foundation::WChar* ExitData);
	using UnloadImageFn = StatusCode(*)(Handle ImageHandle);
	using ExitBootServicesFn = StatusCode(*)(Handle ImageHandle, Foundation::UintN MapKey);

	using GetNextMonotonicCountFn = StatusCode(*)(Foundation::Uint64* Count);
	using StallFn = StatusCode(*)(Foundation::UintN Microseconds);
	using SetWatchdogTimerFn = StatusCode(*)(Foundation::UintN Timeout, Foundation::Uint64 WatchdogCode, Foundation::UintN DataSize, Foundation::WChar* WatchdogData);

	using ConnectControllerFn = StatusCode(*)(Handle ControllerHandle, Handle* DriverImageHandle, DevicePathProtocol* RemainingDevicePath, Foundation::Bool Recursive);
	using DisconnectControllerFn = StatusCode(*)(Handle ControllerHandle, Handle DriverImageHandle, Handle ChildHandle);

	using OpenProtocolFn = StatusCode(*)(Handle ProtocolHandle, Guid* Protocol, Foundation::Void** Interface, Handle AgentHandle, Handle ControllerHandle, Foundation::Uint32 Attributes);
	using CloseProtocolFn = StatusCode(*)(Handle ProtocolHandle, Guid* Protocol, Handle AgentHandle, Handle ControllerHandle);
	using OpenProtocolInformationFn = StatusCode(*)(Handle ProtocolHandle, Guid* Protocol, OpenProtocolInformationEntry** EntryBuffer, Foundation::UintN* EntryCount);
	using ProtocolsPerHandleFn = StatusCode(*)(Handle ProtocolHandle, Guid*** ProtocolBuffer, Foundation::UintN* ProtocolBufferCount);

	using LocateHandleBufferFn = StatusCode(*)(LocateSearchType SearchType, Guid* Protocol, Foundation::Void* SearchKey, Foundation::UintN* NoHandles, Handle** Buffer);
	using LocateProtocolFn = StatusCode(*)(Guid* Protocol, Foundation::Void* Registration, Foundation::Void** Interface);

	using InstallMultipleProtocolInterfacesFn = StatusCode(*)(Handle* Handle, ...);
	using UninstallMultipleProtocolInterfacesFn = StatusCode(*)(Handle Handle, ...);

	using CalculateCrc32Fn = StatusCode(*)(Foundation::Void* Data, Foundation::UintN DataSize, Foundation::Uint32* Crc32);
	using CopyMemFn = Foundation::Void(*)(Foundation::Void* Destination, Foundation::Void* Source, Foundation::UintN Length);
	using SetMemFn = Foundation::Void(*)(Foundation::Void* Buffer, Foundation::UintN Size, Foundation::Uint8 Value);

	using CreateEventExFn = StatusCode(*)(Foundation::Uint32 Type, Tpl NotifyTpl, EventNotifyFn NotifyFunction, const Foundation::Void* NotifyContext, const Guid* EventGroup, Event* Event);

	class BootServices
	{
	public:
		Header Header;

		RaiseTplFn RaiseTpl;
		RestoreTplFn RestoreTpl;

		AllocatePagesFn AllocatePages;
		FreePagesFn FreePages;
		GetMemoryMapFn GetMemoryMap;
		AllocatePoolFn AllocatePool;
		FreePoolFn FreePool;

		CreateEventFn CreateEvent;
		SetTimerFn SetTimer;
		WaitForEventFn WaitForEvent;
		SignalEventFn SignalEvent;
		CloseEventFn CloseEvent;
		CheckEventFn CheckEvent;

		InstallProtocolInterfaceFn InstallProtocolInterface;
		ReinstallProtocolInterfaceFn ReinstallProtocolInterface;
		UninstallProtocolInterfaceFn UninstallProtocolInterface;
		HandleProtocolFn HandleProtocol;

		Foundation::Void* Reserved;

		RegisterProtocolNotifyFn RegisterProtocolNotify;
		LocateHandleFn LocateHandle;
		LocateDevicePathFn LocateDevicePath;
		InstallConfigurationTableFn InstallConfigurationTable;

		LoadImageFn LoadImage;
		StartImageFn StartImage;
		ExitFn Exit;
		UnloadImageFn UnloadImage;
		ExitBootServicesFn ExitBootServices;

		GetNextMonotonicCountFn GetNextMonotonicCount;
		StallFn Stall;
		SetWatchdogTimerFn SetWatchdogTimer;

		ConnectControllerFn ConnectController;
		DisconnectControllerFn DisconnectController;

		OpenProtocolFn OpenProtocol;
		CloseProtocolFn CloseProtocol;
		OpenProtocolInformationFn OpenProtocolInformation;
		ProtocolsPerHandleFn ProtocolsPerHandle;

		LocateHandleBufferFn LocateHandleBuffer;
		LocateProtocolFn LocateProtocol;

		InstallMultipleProtocolInterfacesFn InstallMultipleProtocolInterfaces;
		UninstallMultipleProtocolInterfacesFn UninstallMultipleProtocolInterfaces;

		CalculateCrc32Fn CalculateCrc32;
		CopyMemFn CopyMem;
		SetMemFn SetMem;

		CreateEventExFn CreateEventEx;
	};
}