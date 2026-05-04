#pragma once

#include <cstdint>
#include <type_traits>
#include <intrin.h>

#define IN
#define OUT
#define OPTIONAL
#define MAYBE_UNUSED [[maybe_unused]]

using UINT8 = uint8_t;
using CUINT8 = const UINT8;
using PUINT8 = UINT8*;
using PCUINT8 = const UINT8*;

using UINT16 = uint16_t;
using CUINT16 = const UINT16;
using PUINT16 = UINT16*;
using PCUINT16 = const UINT16*;

using UINT32 = uint32_t;
using CUINT32 = const UINT32;
using PUINT32 = UINT32*;
using PCUINT32 = const UINT32*;

using UINT64 = uint64_t;
using CUINT64 = const UINT64;
using PUINT64 = UINT64*;
using PCUINT64 = const UINT64*;

using UINTN = uintptr_t;
using CUINTN = const UINTN;
using PUINTN = UINTN*;
using PCUINTN = const UINTN*;

using INT8 = int8_t;
using CINT8 = const INT8;
using PINT8 = INT8*;
using PCINT8 = const INT8*;

using INT16 = int16_t;
using CINT16 = const INT16;
using PINT16 = INT16*;
using PCINT16 = const INT16;

using INT32 = int32_t;
using CINT32 = const INT32;
using PINT32 = INT32*;
using PCINT32 = const INT32*;

using INT64 = int64_t;
using CINT64 = const INT64;
using PINT64 = INT64*;
using PCINT64 = const INT64*;

using INTN = intptr_t;
using CINTN = const INTN;
using PINTN = INTN*;
using PCINTN = const INTN*;

using SIZE_T = UINTN;
using CSIZE_T = const SIZE_T;
using PSIZE_T = SIZE_T*;
using PCSIZE_T = const SIZE_T*;

using CHAR8 = char8_t;
using CCHAR8 = const CHAR8;
using PCHAR8 = CHAR8*;
using PCCHAR8 = const CHAR8*;

using CHAR16 = char16_t;
using CCHAR16 = const CHAR16;
using PCHAR16 = CHAR16*;
using PCCHAR16 = const CHAR16*;

using CHAR32 = char32_t;
using CCHAR32 = const CHAR32;
using PCHAR32 = CHAR32*;
using PCCHAR32 = const CHAR32*;

using CHAR = char;
using CCHAR = const CHAR;
using PCHAR = CHAR*;
using PSTR = CHAR*;
using PCSTR = const CHAR*;

using WCHAR = wchar_t;
using CWCHAR = const WCHAR;
using PWSTR = WCHAR*;
using PCWSTR = const WCHAR*;

using UCHAR = unsigned char;
using CUCHAR = const UCHAR;
using PUCHAR = UCHAR*;
using PCUCHAR = const UCHAR*;

using BOOLEAN = bool;
using CBOOLEAN = const BOOLEAN;
using PBOOLEAN = BOOLEAN*;
using CPBOOLEAN = const BOOLEAN*;

using VOID = void;
using PVOID = VOID*;
using PCVOID = const VOID*;

using EFI_HANDLE = PVOID;
using PEFI_HANDLE = EFI_HANDLE*;

using EFI_EVENT = PVOID;
using PEFI_EVENT = EFI_EVENT*;

typedef struct _GUID
{
	uint32_t Data1;
	uint16_t Data2;
	uint16_t Data3;
	uint8_t Data4[8];
} GUID, *PGUID;

using CGUID = const GUID;
using PCGUID = const GUID*;

using EFI_GUID = GUID;
using PEFI_GUID = GUID*;
using CEFI_GUID = const GUID;
using PCEFI_GUID = const GUID*;

using EFI_TPL = UINT64;
using EFI_LBA = UINT64;

using EFI_STATUS = UINT64;

inline constexpr EFI_STATUS EFI_ERROR_BIT = (EFI_STATUS{ 1 } << 63);

[[nodiscard]] inline constexpr BOOLEAN EfiError(EFI_STATUS Status) { return (Status & EFI_ERROR_BIT) != 0; }
[[nodiscard]] inline constexpr BOOLEAN EfiWarn(EFI_STATUS Status) { return !EfiError(Status) && (Status != 0); }

inline constexpr EFI_STATUS EFI_SUCCESS = 0;
inline constexpr EFI_STATUS EFI_LOAD_ERROR = EFI_ERROR_BIT | 1;
inline constexpr EFI_STATUS EFI_INVALID_PARAMETER = EFI_ERROR_BIT | 2;
inline constexpr EFI_STATUS EFI_UNSUPPORTED = EFI_ERROR_BIT | 3;
inline constexpr EFI_STATUS EFI_BAD_BUFFER_SIZE = EFI_ERROR_BIT | 4;
inline constexpr EFI_STATUS EFI_BUFFER_TOO_SMALL = EFI_ERROR_BIT | 5;
inline constexpr EFI_STATUS EFI_NOT_READY = EFI_ERROR_BIT | 6;
inline constexpr EFI_STATUS EFI_DEVICE_ERROR = EFI_ERROR_BIT | 7;
inline constexpr EFI_STATUS EFI_WRITE_PROTECTED = EFI_ERROR_BIT | 8;
inline constexpr EFI_STATUS EFI_OUT_OF_RESOURCES = EFI_ERROR_BIT | 9;
inline constexpr EFI_STATUS EFI_VOLUME_CORRUPTED = EFI_ERROR_BIT | 10;
inline constexpr EFI_STATUS EFI_VOLUME_FULL = EFI_ERROR_BIT | 11;
inline constexpr EFI_STATUS EFI_NO_MEDIA = EFI_ERROR_BIT | 12;
inline constexpr EFI_STATUS EFI_MEDIA_CHANGED = EFI_ERROR_BIT | 13;
inline constexpr EFI_STATUS EFI_NOT_FOUND = EFI_ERROR_BIT | 14;
inline constexpr EFI_STATUS EFI_ACCESS_DENIED = EFI_ERROR_BIT | 15;
inline constexpr EFI_STATUS EFI_NO_RESPONSE = EFI_ERROR_BIT | 16;
inline constexpr EFI_STATUS EFI_NO_MAPPING = EFI_ERROR_BIT | 17;
inline constexpr EFI_STATUS EFI_TIMEOUT = EFI_ERROR_BIT | 18;
inline constexpr EFI_STATUS EFI_NOT_STARTED = EFI_ERROR_BIT | 19;
inline constexpr EFI_STATUS EFI_ALREADY_STARTED = EFI_ERROR_BIT | 20;
inline constexpr EFI_STATUS EFI_ABORTED = EFI_ERROR_BIT | 21;
inline constexpr EFI_STATUS EFI_ICMP_ERROR = EFI_ERROR_BIT | 22;
inline constexpr EFI_STATUS EFI_TFTP_ERROR = EFI_ERROR_BIT | 23;
inline constexpr EFI_STATUS EFI_PROTOCOL_ERROR = EFI_ERROR_BIT | 24;
inline constexpr EFI_STATUS EFI_INCOMPATIBLE_VERSION = EFI_ERROR_BIT | 25;
inline constexpr EFI_STATUS EFI_SECURITY_VIOLATION = EFI_ERROR_BIT | 26;
inline constexpr EFI_STATUS EFI_CRC_ERROR = EFI_ERROR_BIT | 27;
inline constexpr EFI_STATUS EFI_END_OF_MEDIA = EFI_ERROR_BIT | 28;
inline constexpr EFI_STATUS EFI_END_OF_FILE = EFI_ERROR_BIT | 31;
inline constexpr EFI_STATUS EFI_INVALID_LANGUAGE = EFI_ERROR_BIT | 32;
inline constexpr EFI_STATUS EFI_COMPROMISED_DATA = EFI_ERROR_BIT | 33;

inline constexpr EFI_STATUS EFI_WARN_UNKNOWN_GLYPH = 1;
inline constexpr EFI_STATUS EFI_WARN_DELETE_FAILURE = 2;
inline constexpr EFI_STATUS EFI_WARN_WRITE_FAILURE = 3;
inline constexpr EFI_STATUS EFI_WARN_BUFFER_TOO_SMALL = 4;

typedef enum _EFI_MEMORY_TYPE : UINT32
{
	EfiReservedMemoryType,
	EfiLoaderCode,
	EfiLoaderData,
	EfiBootServicesCode,
	EfiBootServicesData,
	EfiRuntimeServicesCode,
	EfiRuntimeServicesData,
	EfiConventionalMemory,
	EfiUnusableMemory,
	EfiACPIReclaimMemory,
	EfiACPIMemoryNVS,
	EfiMemoryMappedIO,
	EfiMemoryMappedIOPortSpace,
	EfiPalCode,
	EfiPersistentMemory,
	EfiMaxMemoryType
} EFI_MEMORY_TYPE, *PEFI_MEMORY_TYPE;

typedef enum _EFI_TIMER_DELAY : UINT32
{
	TimerCancel,
	TimerPeriodic,
	TimerRelative
} EFI_TIMER_DELAY, *PEFI_TIMER_DELAY;

typedef enum _EFI_ALLOCATE_TYPE : UINT32
{
	AllocateAnyPages,
	AllocateMaxAddress,
	AllocateAddress,
	MaxAllocateType
} EFI_ALLOCATE_TYPE, *PEFI_ALLOCATE_TYPE;

typedef struct _EFI_TABLE_HEADER
{
	UINT64 Signature;
	UINT32 Revision;
	UINT32 HeaderSize;
	UINT32 CRC32;
	UINT32 Reserved;
} EFI_TABLE_HEADER, *PEFI_TABLE_HEADER;

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL, *PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

using EfiTextResetFn = EFI_STATUS(__cdecl)(PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, BOOLEAN ExtendedVerification);
using EfiTextOutputStringFn = EFI_STATUS(__cdecl)(PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, PCWSTR String);
using EfiTextTestStringFn = EFI_STATUS(__cdecl)(PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, PCWSTR String);
using EfiTextQueryModeFn = EFI_STATUS(__cdecl)(PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, UINT64 ModeNumber, PUINT64 Columns, PUINT64 Rows);
using EfiTextSetModeFn = EFI_STATUS(__cdecl)(PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, UINT64 ModeNumber);
using EfiTextSetAttributesFn = EFI_STATUS(__cdecl)(PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, UINT64 Attribute);
using EfiTextClearScreenFn = EFI_STATUS(__cdecl)(PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This);
using EfiTextSetCursorPositionFn = EFI_STATUS(__cdecl)(PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, UINT64 Column, UINT64 Row);
using EfiTextEnableCursor = EFI_STATUS(__cdecl)(PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, BOOLEAN Visible);

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_MODE
{
	INT32 MaxMode;
	INT32 Mode;
	INT32 Attribute;
	INT32 CursorColumn;
	INT32 CursorRow;
	BOOLEAN CursorVisible;
} EFI_SIMPLE_TEXT_OUTPUT_MODE, *PEFI_SIMPLE_TEXT_OUTPUT_MODE;

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
{
	EfiTextResetFn* Reset;
	EfiTextOutputStringFn* OutputString;
	EfiTextTestStringFn* TestString;
	EfiTextQueryModeFn* QueryMode;
	EfiTextSetModeFn* SetMode;
	EfiTextSetAttributesFn* SetAttributes;
	EfiTextClearScreenFn* ClearScreen;
	EfiTextSetCursorPositionFn* SetCursorPosition;
	EfiTextEnableCursor EnableCursor;
	PEFI_SIMPLE_TEXT_OUTPUT_MODE Mode;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL, *PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL, *PEFI_SIMPLE_TEXT_INPUT_PROTOCOL;

typedef struct _EFI_INPUT_KEY
{
	UINT16 ScanCode;
	UINT16 UnicodeChar;
} EFI_INPUT_KEY, *PEFI_INPUT_KEY;

using EfiTextInputResetFn = EFI_STATUS(__cdecl)(PEFI_SIMPLE_TEXT_INPUT_PROTOCOL This, BOOLEAN ExtendedVerification);
using EfiTextInputReadKeyStrokeFn = EFI_STATUS(__cdecl)(PEFI_SIMPLE_TEXT_INPUT_PROTOCOL This, PEFI_INPUT_KEY Key);

typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL
{
	EfiTextInputResetFn* Reset;
	EfiTextInputReadKeyStrokeFn* ReadKeyStroke;
	EFI_EVENT WaitForKey;
} EFI_SIMPLE_TEXT_INPUT_PROTOCOL, *PEFI_SIMPLE_TEXT_INPUT_PROTOCOL;

typedef struct _EFI_MEMORY_DESCRIPTOR
{
	UINT32 Type;
	UINT64 PhysicalStart;
	UINT64 VirtualStart;
	UINT64 NumberOfPages;
	UINT64 Attribute;
} EFI_MEMORY_DESCRIPTOR, *PEFI_MEMORY_DESCRIPTOR;

typedef enum _EFI_INTERFACE_TYPE : UINT32
{
	EFI_NATIVE_INTERFACE
} EFI_INTERFACE_TYPE, *PEFI_INTERFACE_TYPE;

typedef enum _EFI_LOCATE_SEARCH_TYPE : UINT32
{
	AllHandles,
	ByRegisterNotify,
	ByProtocol
} EFI_LOCATE_SEARCH_TYPE, *PEFI_LOCATE_SEARCH_TYPE;

typedef struct _EFI_OPEN_PROTOCOL_INFORMATION_ENTRY
{
	EFI_HANDLE AgentHandle;
	EFI_HANDLE ControllerHandle;
	UINT32 Attributes;
	UINT32 OpenCount;
} EFI_OPEN_PROTOCOL_INFORMATION_ENTRY, *PEFI_OPEN_PROTOCOL_INFORMATION_ENTRY;

typedef struct _EFI_DEVICE_PATH_PROTOCOL EFI_DEVICE_PATH_PROTOCOL, *PEFI_DEVICE_PATH_PROTOCOL;

using EfiRaiseTplFn = EFI_TPL(__cdecl)(EFI_TPL NewTpl);
using EfiRestoreTplFn = VOID(__cdecl)(EFI_TPL OldTpl);

using EfiAllocatePagesFn = EFI_STATUS(__cdecl)(EFI_ALLOCATE_TYPE Type, EFI_MEMORY_TYPE MemoryType, UINT64 Pages, PVOID* Memory);
using EfiFreePagesFn = EFI_STATUS(__cdecl)(UINT64 Memory, UINT64 Pages);
using EfiGetMemoryMapFn = EFI_STATUS(__cdecl)(PUINT64 MemoryMapSize, PEFI_MEMORY_DESCRIPTOR MemoryMap, PUINT64 MapKey, PUINT64 DescriptorSize, PUINT32 DescriptorVersion);
using EfiAllocatePoolFn = EFI_STATUS(__cdecl)(EFI_MEMORY_TYPE PoolType, UINT64 Size, PVOID* Buffer);
using EfiFreePoolFn = EFI_STATUS(__cdecl)(PVOID Buffer);

using EfiEventNotifyFn = VOID(__cdecl)(EFI_EVENT Event, PVOID Context);
using EfiCreateEventFn = EFI_STATUS(__cdecl)(UINT32 Type, EFI_TPL NotifyTpl, EfiEventNotifyFn* NotifyFunction, PVOID NotifyContext, PEFI_EVENT Event);
using EfiSetTimerFn = EFI_STATUS(__cdecl)(EFI_EVENT Event, EFI_TIMER_DELAY Type, UINT64 TriggerTime);
using EfiWaitForEventFn = EFI_STATUS(__cdecl)(UINT64 NumberOfEvents, PEFI_EVENT Event, PUINT64 Index);
using EfiSignalEventFn = EFI_STATUS(__cdecl)(EFI_EVENT Event);
using EfiCloseEventFn = EFI_STATUS(__cdecl)(EFI_EVENT Event);
using EfiCheckEventFn = EFI_STATUS(__cdecl)(EFI_EVENT Event);
using EfiCreateEventExFn = EFI_STATUS(__cdecl)(UINT32 Type, EFI_TPL NotifyTpl, EfiEventNotifyFn* NotifyFunction, PVOID NotifyContext, PCEFI_GUID EventGroup, PEFI_EVENT Event);

using EfiInstallProtocolInterfaceFn = EFI_STATUS(__cdecl)(PEFI_HANDLE Handle, PCEFI_GUID Protocol, EFI_INTERFACE_TYPE InterfaceType, PVOID Interface);
using EfiReinstallProtocolInterfaceFn = EFI_STATUS(__cdecl)(EFI_HANDLE Handle, PEFI_GUID Protocol, PVOID OldInterface, PVOID NewInterface);
using EfiUninstallProtocolInterfaceFn = EFI_STATUS(__cdecl)(EFI_HANDLE Handle, PEFI_GUID Protocol, PVOID Interface);
using EfiHandleProtocolFn = EFI_STATUS(__cdecl)(EFI_HANDLE Handle, PCEFI_GUID Protocol, PVOID* Interface);
using EfiRegisterProtocolNotifyFn = EFI_STATUS(__cdecl)(PCEFI_GUID Protocol, EFI_EVENT Event, PVOID* Registration);
using EfiLocateHandleFn = EFI_STATUS(__cdecl)(EFI_LOCATE_SEARCH_TYPE SearchType, PCEFI_GUID Protocol, PVOID SearchKey, PUINT64 BufferSize, PEFI_HANDLE Buffer);
using EfiLocateDevicePathFn = EFI_STATUS(__cdecl)(PCEFI_GUID Protocol, PEFI_DEVICE_PATH_PROTOCOL* DevicePath, PEFI_HANDLE Device);
using EfiInstallConfigurationTableFn = EFI_STATUS(__cdecl)(PEFI_GUID Guid, PVOID Table);
using EfiOpenProtocolFn = EFI_STATUS(__cdecl)(EFI_HANDLE Handle, PCEFI_GUID Protocol, PVOID* Interface, EFI_HANDLE AgentHandle, EFI_HANDLE ControllerHandle, UINT32 Attributes);
using EfiCloseProtocolFn = EFI_STATUS(__cdecl)(EFI_HANDLE Handle, PCEFI_GUID Protocol, EFI_HANDLE AgentHandle, EFI_HANDLE ControllerHandle);
using EfiOpenProtocolInformationFn = EFI_STATUS(__cdecl)(EFI_HANDLE Handle, PCEFI_GUID Protocol, PEFI_OPEN_PROTOCOL_INFORMATION_ENTRY* EntryBuffer, PUINT64 EntryCount);
using EfiProtocolsPerHandleFn = EFI_STATUS(__cdecl)(EFI_HANDLE Handle, PEFI_GUID** ProtocolBuffer, PUINT64 ProtocolBufferCount);
using EfiLocateHandleBufferFn = EFI_STATUS(__cdecl)(EFI_LOCATE_SEARCH_TYPE SearchType, PCEFI_GUID Protocol, PVOID SearchKey, PUINT64 NoHandles, PEFI_HANDLE* Buffer);
using EfiLocateProtocolFn = EFI_STATUS(__cdecl)(PCEFI_GUID Protocol, PVOID Registration, PVOID* Interface);
using EfiInstallMultipleProtocolInterfacesFn = EFI_STATUS(__cdecl)(PEFI_HANDLE Handle, ...);
using EfiUninstallMultipleProtocolInterfacesFn = EFI_STATUS(__cdecl)(EFI_HANDLE Handle, ...);

using EfiLoadImageFn = EFI_STATUS(__cdecl)(BOOLEAN BootPolicy, EFI_HANDLE ParentImageHandle, PEFI_DEVICE_PATH_PROTOCOL DevicePath, PVOID SourceBuffer, UINT64 SourceSize, PEFI_HANDLE ImageHandle);
using EfiStartImageFn = EFI_STATUS(__cdecl)(EFI_HANDLE ImageHandle, PUINT64 ExitDataSize, PWSTR* ExitData);
using EfiExitFn = EFI_STATUS(__cdecl)(EFI_HANDLE ImageHandle, EFI_STATUS ExitStatus, UINT64 ExitDataSize, PWSTR ExitData);
using EfiUnloadImageFn = EFI_STATUS(__cdecl)(EFI_HANDLE ImageHandle);
using EfiExitBootServicesFn = EFI_STATUS(__cdecl)(EFI_HANDLE ImageHandle, UINT64 MapKey);

using EfiGetNextMonotonicCountFn = EFI_STATUS(__cdecl)(PUINT64* Count);
using EfiStallFn = EFI_STATUS(__cdecl)(UINT64 Microseconds);
using EfiSetWatchdogTimerFn = EFI_STATUS(__cdecl)(UINT64 Timeout, UINT64 WatchdogCode, UINT64 DataSize, PWSTR WatchdogData);

using EfiConnectControllerFn = EFI_STATUS(__cdecl)(EFI_HANDLE ControllerHandle, PEFI_HANDLE DriverImageHandle, PEFI_DEVICE_PATH_PROTOCOL RemainingDevicePath, BOOLEAN Recursive);
using EfiDisconnectControllerFn = EFI_STATUS(__cdecl)(EFI_HANDLE ControllerHandle, EFI_HANDLE DriverImageHandle, EFI_HANDLE ChildHandle);

using EfiCalculateCrc32Fn = EFI_STATUS(__cdecl)(PCVOID Data, UINT64 DataSize, PUINT32 Crc32);

using EfiCopyMemFn = VOID(__cdecl)(PVOID Destination, PCVOID Source, UINT64 Length);
using EfiSetMemFn = VOID(__cdecl)(PVOID Buffer, UINT64 Size, UINT8 Value);

typedef struct _EFI_BOOT_SERVICES
{
	// The table header for the EFI Boot Services Table.
	EFI_TABLE_HEADER Hdr;

	// Task Priority Services.
	EfiRaiseTplFn* RaiseTPL;
	EfiRestoreTplFn* RestoreTPL;

	// Memory Services.
	EfiAllocatePagesFn* AllocatePages;
	EfiFreePagesFn* FreePages;
	EfiGetMemoryMapFn* GetMemoryMap;
	EfiAllocatePoolFn* AllocatePool;
	EfiFreePoolFn* FreePool;

	// Event & Timer Services.
	EfiCreateEventFn* CreateEvent;
	EfiSetTimerFn* SetTimer;
	EfiWaitForEventFn* WaitForEvent;
	EfiSignalEventFn* SignalEvent;
	EfiCloseEventFn* CloseEvent;
	EfiCheckEventFn* CheckEvent;

	// Protocol Handler Services.
	EfiInstallProtocolInterfaceFn* InstallProtocolInterface;
	EfiReinstallProtocolInterfaceFn* ReinstallProtocolInterface;
	EfiUninstallProtocolInterfaceFn* UninstallProtocolInterface;
	EfiHandleProtocolFn* HandleProtocol;
	PVOID Reserved;
	EfiRegisterProtocolNotifyFn* RegisterProtocolNotify;
	EfiLocateHandleFn* LocateHandle;
	EfiLocateDevicePathFn* LocateDevicePath;
	EfiInstallConfigurationTableFn* InstallConfigurationTable;

	// Image Services.
	EfiLoadImageFn* LoadImage;
	EfiStartImageFn* StartImage;
	EfiExitFn* Exit;
	EfiUnloadImageFn* UnloadImage;
	EfiExitBootServicesFn* ExitBootServices;

	// Miscellaneous Services.
	EfiGetNextMonotonicCountFn* GetNextMonotonicCount;
	EfiStallFn* Stall;
	EfiSetWatchdogTimerFn* SetWatchdogTimer;

	// Driver Support Services.
	EfiConnectControllerFn* ConnectController;
	EfiDisconnectControllerFn* DisconnectController;

	// Open and Close Protocol Services.
	EfiOpenProtocolFn* OpenProtocol;
	EfiCloseProtocolFn* CloseProtocol;
	EfiOpenProtocolInformationFn* OpenProtocolInformation;

	// Library Services.
	EfiProtocolsPerHandleFn* ProtocolsPerHandle;
	EfiLocateHandleBufferFn* LocateHandleBuffer;
	EfiLocateProtocolFn* LocateProtocol;
	EfiInstallMultipleProtocolInterfacesFn* InstallMultipleProtocolInterfaces;
	EfiUninstallMultipleProtocolInterfacesFn* UninstallMultipleProtocolInterfaces;

	// 32-bit CRC Services.
	EfiCalculateCrc32Fn* CalculateCrc32;

	// Miscellaneous Services.
	EfiCopyMemFn* CopyMem;
	EfiSetMemFn* SetMem;
	EfiCreateEventExFn* CreateEventEx;
} EFI_BOOT_SERVICES, *PEFI_BOOT_SERVICES;

typedef struct _EFI_TIME
{
	UINT16 Year;
	UINT8 Month;
	UINT8 Day;
	UINT8 Hour;
	UINT8 Minute;
	UINT8 Second;
	UINT8 Pad1;
	UINT32 Nanosecond;
	INT16 TimeZone;
	UINT8 Daylight;
	UINT8 Pad2;
} EFI_TIME, *PEFI_TIME;

typedef struct _EFI_TIME_CAPABILITIES
{
	UINT32 Resolution;
	UINT32 Accuracy;
	BOOLEAN SetsToZero;
} EFI_TIME_CAPABILITIES, *PEFI_TIME_CAPABILITIES;

typedef enum _EFI_RESET_TYPE : UINT32
{
	EfiResetCold,
	EfiResetWarm,
	EfiResetShutdown,
	EfiResetPlatformSpecific
} EFI_RESET_TYPE, *PEFI_RESET_TYPE;

typedef struct _EFI_CAPSULE_HEADER
{
	EFI_GUID CapsuleGuid;
	UINT32 HeaderSize;
	UINT32 Flags;
	UINT32 CapsuleImageSize;
} EFI_CAPSULE_HEADER, *PEFI_CAPSULE_HEADER;

using EfiGetTimeFn = EFI_STATUS(__cdecl)(PEFI_TIME Time, PEFI_TIME_CAPABILITIES Capabilities);
using EfiSetTimeFn = EFI_STATUS(__cdecl)(PEFI_TIME Time);
using EfiGetWakeupTimeFn = EFI_STATUS(__cdecl)(PBOOLEAN Enabled, PBOOLEAN Pending, PEFI_TIME Time);
using EfiSetWakeupTimeFn = EFI_STATUS(__cdecl)(BOOLEAN Enable, PEFI_TIME Time);
using EfiSetVirtualAddressMapFn = EFI_STATUS(__cdecl)(UINT64 MemoryMapSize, UINT64 DescriptorSize, UINT32 DescriptorVersion, PEFI_MEMORY_DESCRIPTOR VirtualMap);
using EfiConvertPointerFn = EFI_STATUS(__cdecl)(UINT64 DebugPosition, PVOID* Address);
using EfiGetVariableFn = EFI_STATUS(__cdecl)(PCWSTR VariableName, PEFI_GUID VendorGuid, PUINT32 Attributes, PUINT64 DataSize, PVOID Data);
using EfiGetNextVariableNameFn = EFI_STATUS(__cdecl)(PUINT64 VariableNameSize, PCWSTR VariableName, PEFI_GUID VendorGuid);
using EfiSetVariableFn = EFI_STATUS(__cdecl)(PCWSTR VariableName, PEFI_GUID VendorGuid, UINT32 Attributes, UINT64 DataSize, PVOID Data);
using EfiGetNextHighMonoCountFn = EFI_STATUS(__cdecl)(PUINT32 HighCount);
using EfiResetSystemFn = VOID(__cdecl)(EFI_RESET_TYPE ResetType, EFI_STATUS ResetStatus, UINT64 DataSize, PVOID ResetData);
using EfiUpdateCapsuleFn = EFI_STATUS(__cdecl)(PEFI_CAPSULE_HEADER* CapsuleHeaderArray, UINT64 CapsuleCount, UINT64 ScatterGatherList);
using EfiQueryCapsuleCapabilitiesFn = EFI_STATUS(__cdecl)(PEFI_CAPSULE_HEADER* CapsuleHeaderArray, UINT64 CapsuleCount, PUINT64 MaximumCapsuleSize, PEFI_RESET_TYPE ResetType);
using EfiQueryVariableInfoFn = EFI_STATUS(__cdecl)(UINT32 Attributes, PUINT64 MaximumVariableStorageSize, PUINT64 RemainingVariableStorageSize, PUINT64 MaximumVariableSize);

typedef struct _EFI_RUNTIME_SERVICES
{
	// The table header for the EFI Runtime Services Table.
	EFI_TABLE_HEADER Hdr;

	// Time Services.
	EfiGetTimeFn* GetTime;
	EfiSetTimeFn* SetTime;
	EfiGetWakeupTimeFn* GetWakeupTime;
	EfiSetWakeupTimeFn* SetWakeupTime;

	// Virtual Memory Services.
	EfiSetVirtualAddressMapFn* SetVirtualAddressMap;
	EfiConvertPointerFn* ConvertPointer;

	// Variable Services.
	EfiGetVariableFn* GetVariable;
	EfiGetNextVariableNameFn* GetNextVariableName;
	EfiSetVariableFn* SetVariable;

	// Miscellaneous Services.
	EfiGetNextHighMonoCountFn* GetNextHighMonotonicCount;
	EfiResetSystemFn* ResetSystem;

	// UEFI 2.0 Capsule Services.
	EfiUpdateCapsuleFn* UpdateCapsule;
	EfiQueryCapsuleCapabilitiesFn* QueryCapsuleCapabilities;

	// Miscellaneous UEFI 2.0 Service.
	EfiQueryVariableInfoFn* QueryVariableInfo;
} EFI_RUNTIME_SERVICES, *PEFI_RUNTIME_SERVICES;

typedef struct _EFI_CONFIGURATION_TABLE
{
	EFI_GUID VendorGuid;
	PVOID VendorTable;
} EFI_CONFIGURATION_TABLE, *PEFI_CONFIGURATION_TABLE;

typedef struct _EFI_SYSTEM_TABLE
{
	EFI_TABLE_HEADER Hdr;
	PCWSTR FirmwareVendor;
	UINT32 FirmwareRevision;
	EFI_HANDLE ConsoleInHandle;
	PEFI_SIMPLE_TEXT_INPUT_PROTOCOL ConIn;
	EFI_HANDLE ConsoleOutHandle;
	PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL ConOut;
	EFI_HANDLE StandardErrorHandle;
	PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL StdErr;
	PEFI_RUNTIME_SERVICES RuntimeServices;
	PEFI_BOOT_SERVICES BootServices;
	UINT64 NumberOfTableEntries;
	PEFI_CONFIGURATION_TABLE ConfigurationTable;
} EFI_SYSTEM_TABLE, *PEFI_SYSTEM_TABLE;

constexpr CEFI_GUID gEfiSimpleFileSystemProtocolGuid = { 0x0964e5b22, 0x6459, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } };

typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL EFI_SIMPLE_FILE_SYSTEM_PROTOCOL, * PEFI_SIMPLE_FILE_SYSTEM_PROTOCOL;
typedef struct _EFI_FILE_PROTOCOL EFI_FILE_PROTOCOL, * PEFI_FILE_PROTOCOL;

using EfiSimpleFileSystemProtocolOpenVolumeFn = EFI_STATUS(__cdecl)(
	IN	PEFI_SIMPLE_FILE_SYSTEM_PROTOCOL	This,
	OUT	PEFI_FILE_PROTOCOL* Root
	);

constexpr CUINT64 EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_REVISION = 0x00010000;
constexpr CUINT64 EFI_FILE_IO_INTERFACE_REVISION = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_REVISION;

typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
{
	UINT64										Revision;
	EfiSimpleFileSystemProtocolOpenVolumeFn* OpenVolume;
} EFI_SIMPLE_FILE_SYSTEM_PROTOCOL, * PEFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

using CEFI_SIMPLE_FILE_SYSTEM_PROTOCOL = const EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;
using PCEFI_SIMPLE_FILE_SYSTEM_PROTOCOL = const EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*;

using EfiFileOpenFn = EFI_STATUS(__cdecl)(
	IN	PEFI_FILE_PROTOCOL	This,
	OUT PEFI_FILE_PROTOCOL* NewHandle,
	IN	PCWSTR				FileName,
	IN	UINT64				OpenMode,
	IN	UINT64				Attributes
	);

constexpr CUINT64 EFI_FILE_MODE_READ = 0x0000000000000001;
constexpr CUINT64 EFI_FILE_MODE_WRITE = 0x0000000000000002;
constexpr CUINT64 EFI_FILE_MODE_CREATE = 0x8000000000000000;

constexpr CUINT64 EFI_FILE_READ_ONLY = 0x0000000000000001;
constexpr CUINT64 EFI_FILE_HIDDEN = 0x0000000000000002;
constexpr CUINT64 EFI_FILE_SYSTEM = 0x0000000000000004;
constexpr CUINT64 EFI_FILE_RESERVED = 0x0000000000000008;
constexpr CUINT64 EFI_FILE_DIRECTORY = 0x0000000000000010;
constexpr CUINT64 EFI_FILE_ARCHIVE = 0x0000000000000020;
constexpr CUINT64 EFI_FILE_VALID_ATTR = 0x0000000000000037;

using EfiFileCloseFn = EFI_STATUS(__cdecl)(
	IN	PEFI_FILE_PROTOCOL	This
	);

using EfiFileDeleteFn = EFI_STATUS(__cdecl)(
	IN	PEFI_FILE_PROTOCOL	This
	);

using EfiFileReadFn = EFI_STATUS(__cdecl)(
	IN		PEFI_FILE_PROTOCOL	This,
	IN OUT	PUINT64				BufferSize,
	OUT		PVOID				Buffer
	);

using EfiFileWriteFn = EFI_STATUS(__cdecl)(
	IN		PEFI_FILE_PROTOCOL	This,
	IN OUT	PUINT64				BufferSize,
	IN		PVOID				Buffer
	);

using EfiFileSetPositionFn = EFI_STATUS(__cdecl)(
	IN	PEFI_FILE_PROTOCOL	This,
	IN	UINT64				Position
	);

using EfiFileGetPositionFn = EFI_STATUS(__cdecl)(
	IN	PEFI_FILE_PROTOCOL	This,
	OUT PUINT64				Position
	);

using EfiFileGetInfoFn = EFI_STATUS(__cdecl)(
	IN		PEFI_FILE_PROTOCOL	This,
	IN		PCEFI_GUID			InformationType,
	IN OUT	PUINT64				BufferSize,
	OUT		PVOID				Buffer
	);

using EfiFileSetInfoFn = EFI_STATUS(__cdecl)(
	IN	PEFI_FILE_PROTOCOL	This,
	IN	PCEFI_GUID			InformationType,
	IN	UINT64				BufferSize,
	IN	PVOID				Buffer
	);

using EfiFileFlushFn = EFI_STATUS(__cdecl)(
	IN	PEFI_FILE_PROTOCOL	This
	);

typedef struct _EFI_FILE_IO_TOKEN
{
	EFI_EVENT	Event;
	EFI_STATUS	Status;
	UINT64		BufferSize;
	PVOID		Buffer;
} EFI_FILE_IO_TOKEN, * PEFI_FILE_IO_TOKEN;

using CEFI_FILE_IO_TOKEN = const EFI_FILE_IO_TOKEN;
using PCEFI_FILE_IO_TOKEN = const EFI_FILE_IO_TOKEN*;

using EfiFileOpenExFn = EFI_STATUS(__cdecl)(
	IN		PEFI_FILE_PROTOCOL	This,
	OUT		PEFI_FILE_PROTOCOL* NewHandle,
	IN		PCWSTR				FileName,
	IN		UINT64				OpenMode,
	IN		UINT64				Attributes,
	IN OUT	PEFI_FILE_IO_TOKEN	Token
	);

using EfiFileReadExFn = EFI_STATUS(__cdecl)(
	IN		PEFI_FILE_PROTOCOL	This,
	IN OUT	PEFI_FILE_IO_TOKEN	Token
	);

using EfiFileWriteExFn = EFI_STATUS(__cdecl)(
	IN		PEFI_FILE_PROTOCOL	This,
	IN OUT	PEFI_FILE_IO_TOKEN	Token
	);

using EfiFileFlushExFn = EFI_STATUS(__cdecl)(
	IN		PEFI_FILE_PROTOCOL	This,
	IN OUT	PEFI_FILE_IO_TOKEN	Token
	);

constexpr CUINT64 EFI_FILE_PROTOCOL_REVISION = 0x00010000;
constexpr CUINT64 EFI_FILE_PROTOCOL_REVISION2 = 0x00020000;
constexpr CUINT64 EFI_FILE_PROTOCOL_LATEST_REVISION = EFI_FILE_PROTOCOL_REVISION2;

constexpr CUINT64 EFI_FILE_REVISION = EFI_FILE_PROTOCOL_REVISION;

typedef struct _EFI_FILE_PROTOCOL
{
	UINT64					Revision;
	EfiFileOpenFn* Open;
	EfiFileCloseFn* Close;
	EfiFileDeleteFn* Delete;
	EfiFileReadFn* Read;
	EfiFileWriteFn* Write;
	EfiFileGetPositionFn* GetPosition;
	EfiFileSetPositionFn* SetPosition;
	EfiFileGetInfoFn* GetInfo;
	EfiFileSetInfoFn* SetInfo;
	EfiFileFlushFn* Flush;
	EfiFileOpenExFn* OpenEx;
	EfiFileReadExFn* ReadEx;
	EfiFileWriteExFn* WriteEx;
	EfiFileFlushExFn* FlushEx;
} EFI_FILE_PROTOCOL, * PEFI_FILE_PROTOCOL;

constexpr CEFI_GUID gEfiDevicePathProtocolGuid = { 0x09576e91, 0x6d3f, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } };

#pragma pack(push, 1)
typedef struct _EFI_DEVICE_PATH_PROTOCOL
{
	UINT8	Type;
	UINT8	SubType;
	UINT8	Length[2];
} EFI_DEVICE_PATH_PROTOCOL, * PEFI_DEVICE_PATH_PROTOCOL;

using CEFI_DEVICE_PATH_PROTOCOL = const EFI_DEVICE_PATH_PROTOCOL;
using PCEFI_DEVICE_PATH_PROTOCOL = const EFI_DEVICE_PATH_PROTOCOL*;
using EFI_DEVICE_PATH = EFI_DEVICE_PATH_PROTOCOL;

constexpr CUINT8 HARDWARE_DEVICEPATH = 0x01;
constexpr CUINT8 HW_PCI_DP = 0x01;

typedef struct _PCI_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT8						Function;
	UINT8						Device;
} PCI_DEVICE_PATH, * PPCI_DEVICE_PATH;

using CPCI_DEVICE_PATH = const PCI_DEVICE_PATH;
using PCPCI_DEVICE_PATH = const PCI_DEVICE_PATH*;

constexpr CUINT8 HW_PCCARD_DP = 0x02;

typedef struct _PCCARD_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT8						Function;
	UINT8						Device;
} PCCARD_DEVICE_PATH, * PPCCARD_DEVICE_PATH;

using CPCCARD_DEVICE_PATH = const PCCARD_DEVICE_PATH;
using PCPCCARD_DEVICE_PATH = const PCCARD_DEVICE_PATH*;

constexpr CUINT8 HW_MEMMAP_DP = 0x03;

typedef struct _MEMMAP_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT32						StartAddress;
	UINT32						EndAddress;
} MEMMAP_DEVICE_PATH, * PMEMMAP_DEVICE_PATH;

using CMEMMAP_DEVICE_PATH = const MEMMAP_DEVICE_PATH;
using PCMEMMAP_DEVICE_PATH = const MEMMAP_DEVICE_PATH*;

constexpr CUINT8 HW_VENDOR_DP = 0x04;

typedef struct _VENDOR_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	EFI_GUID					Guid;
	UINT8						Data[1];
} VENDOR_DEVICE_PATH, * PVENDOR_DEVICE_PATH;

using CVENDOR_DEVICE_PATH = const VENDOR_DEVICE_PATH;
using PCVENDOR_DEVICE_PATH = const VENDOR_DEVICE_PATH*;

constexpr CUINT8 HW_CONTROLLER_DP = 0x05;

typedef struct _CONTROLLER_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT32						ControllerNumber;
} CONTROLLER_DEVICE_PATH, * PCONTROLLER_DEVICE_PATH;

using CCONTROLLER_DEVICE_PATH = const CONTROLLER_DEVICE_PATH;
using PCCONTROLLER_DEVICE_PATH = const CONTROLLER_DEVICE_PATH*;

constexpr CUINT8 HW_BMC_DP = 0x06;

typedef struct _BMC_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT8						InterfaceType;
	UINT8						ChannelNumber;
	UINT8						Reserved[2];
	UINT32						LUN;
} BMC_DEVICE_PATH, * PBMC_DEVICE_PATH;

using CBMC_DEVICE_PATH = const BMC_DEVICE_PATH;
using PCBMC_DEVICE_PATH = const BMC_DEVICE_PATH*;

constexpr CUINT8 ACPI_DEVICE_PATH = 0x02;
constexpr CUINT8 ACPI_DP = 0x01;

typedef struct _ACPI_HID_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT32						HID;
	UINT32						UID;
} ACPI_HID_DEVICE_PATH, * PACPI_HID_DEVICE_PATH;

using CACPI_HID_DEVICE_PATH = const ACPI_HID_DEVICE_PATH;
using PCACPI_HID_DEVICE_PATH = const ACPI_HID_DEVICE_PATH*;

constexpr CUINT8 ACPI_EXTENDED_DP = 0x02;

typedef struct _ACPI_EXTENDED_HID_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT32						HID;
	UINT32						UID;
	UINT32						CID;
} ACPI_EXTENDED_HID_DEVICE_PATH, * PACPI_EXTENDED_HID_DEVICE_PATH;

using CACPI_EXTENDED_HID_DEVICE_PATH = const ACPI_EXTENDED_HID_DEVICE_PATH;
using PCACPI_EXTENDED_HID_DEVICE_PATH = const ACPI_EXTENDED_HID_DEVICE_PATH*;

constexpr CUINT16 PNP_EISA_ID_CONST = 0x41D0;

constexpr
UINT32
EISA_ID(
	IN	UINT32	Name,
	IN	UINT32	Num
)
{
	return (Name & 0xFFFFU) | ((Num & 0xFFFFU) << 16);
}

constexpr
UINT32
EISA_PNP_ID(
	IN	UINT32	PNPId
)
{
	return EISA_ID(PNP_EISA_ID_CONST, PNPId);
}

constexpr
UINT32
EFI_PNP_ID(
	IN	UINT32	PNPId
)
{
	return EISA_ID(PNP_EISA_ID_CONST, PNPId);
}

constexpr CUINT16 PNP_EISA_ID_MASK = 0xFFFF;

constexpr
UINT16
EISA_ID_TO_NUM(
	IN	UINT32	EisaId
)
{
	return static_cast<UINT16>((EisaId >> 16) & 0xFFFFU);
}

constexpr
UINT16
EISA_ID_TO_NAME(
	IN	UINT32	EisaId
)
{
	return static_cast<UINT16>(EisaId & 0xFFFFU);
}

constexpr CUINT8 ACPI_ADR_DP = 0x03;

typedef struct _ACPI_ADR_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT64						ADR;
} ACPI_ADR_DEVICE_PATH, * PACPI_ADR_DEVICE_PATH;

using CACPI_ADR_DEVICE_PATH = const ACPI_ADR_DEVICE_PATH;
using PCACPI_ADR_DEVICE_PATH = const ACPI_ADR_DEVICE_PATH*;

constexpr CUINT8 ACPI_NVDIMM_DP = 0x04;

typedef struct _ACPI_NVDIMM_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT32                      NFITDeviceHandle;
} ACPI_NVDIMM_DEVICE_PATH, * PACPI_NVDIMM_DEVICE_PATH;

using CACPI_NVDIMM_DEVICE_PATH = const ACPI_NVDIMM_DEVICE_PATH;
using PCACPI_NVDIMM_DEVICE_PATH = const ACPI_NVDIMM_DEVICE_PATH*;

constexpr
UINT32
ACPI_DISPLAY_ADR(
	IN	UINT32	DeviceIdScheme,
	IN	UINT32	HeadId,
	IN	UINT32	NonVgaOutput,
	IN	UINT32	BiosCanDetect,
	IN	UINT32	VendorInfo,
	IN	UINT32	Type,
	IN	UINT32	Port,
	IN	UINT32	Index
)
{
	return ((DeviceIdScheme & 0x1U) << 31) | ((HeadId & 0x7U) << 18) | ((NonVgaOutput & 0x1U) << 17) | ((BiosCanDetect & 0x1U) << 16) | ((VendorInfo & 0xFU) << 12) | ((Type & 0xFU) << 8) | ((Port & 0xFU) << 4) | ((Index & 0xFU));
}

constexpr CUINT8 MESSAGING_DEVICE_PATH = 0x03;
constexpr CUINT8 MSG_ATAPI_DP = 0x01;

typedef struct _ATAPI_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT8						PrimarySecondary;
	UINT8						SlaveMaster;
	UINT16						Lun;
} ATAPI_DEVICE_PATH, * PATAPI_DEVICE_PATH;

using CATAPI_DEVICE_PATH = const ATAPI_DEVICE_PATH;
using PCATAPI_DEVICE_PATH = const ATAPI_DEVICE_PATH*;

constexpr CUINT8 MSG_SCSI_DP = 0x02;

typedef struct _SCSI_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT16						Pun;
	UINT16						Lun;
} SCSI_DEVICE_PATH, * PSCSI_DEVICE_PATH;

using CSCSI_DEVICE_PATH = const SCSI_DEVICE_PATH;
using PCSCSI_DEVICE_PATH = const SCSI_DEVICE_PATH*;

constexpr CUINT8 MSG_FIBRECHANNEL_DP = 0x03;

typedef struct _FIBRECHANNEL_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT32						Reserved;
	UINT64						WWN;
	UINT64						Lun;
} FIBRECHANNEL_DEVICE_PATH, * PFIBRECHANNEL_DEVICE_PATH;

using CFIBRECHANNEL_DEVICE_PATH = const FIBRECHANNEL_DEVICE_PATH;
using PCFIBRECHANNEL_DEVICE_PATH = const FIBRECHANNEL_DEVICE_PATH*;

constexpr CUINT8 MSG_FIBRECHANNELEX_DP = 0x15;

typedef struct _FIBRECHANNELEX_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT32						Reserved;
	UINT8						WWN[8];
	UINT8						Lun[8];
} FIBRECHANNELEX_DEVICE_PATH, * PFIBRECHANNELEX_DEVICE_PATH;

using CFIBRECHANNELEX_DEVICE_PATH = const FIBRECHANNELEX_DEVICE_PATH;
using PCFIBRECHANNELEX_DEVICE_PATH = const FIBRECHANNELEX_DEVICE_PATH*;

constexpr CUINT8 MSG_1394_DP = 0x04;

typedef struct _F1394_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT32						Reserved;
	UINT64						Guid;
} F1394_DEVICE_PATH, * PF1394_DEVICE_PATH;

using CF1394_DEVICE_PATH = const F1394_DEVICE_PATH;
using PCF1394_DEVICE_PATH = const F1394_DEVICE_PATH*;

constexpr CUINT8 MSG_USB_DP = 0x05;

typedef struct _USB_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT8						ParentPortNumber;
	UINT8						InterfaceNumber;
} USB_DEVICE_PATH, * PUSB_DEVICE_PATH;

using CUSB_DEVICE_PATH = const USB_DEVICE_PATH;
using PCUSB_DEVICE_PATH = const USB_DEVICE_PATH*;

constexpr CUINT8 MSG_USB_CLASS_DP = 0x0F;

typedef struct _USB_CLASS_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT16						VendorId;
	UINT16						ProductId;
	UINT8						DeviceClass;
	UINT8 						DeviceSubClass;
	UINT8						DeviceProtocol;
} USB_CLASS_DEVICE_PATH, * PUSB_CLASS_DEVICE_PATH;

using CUSB_CLASS_DEVICE_PATH = const USB_CLASS_DEVICE_PATH;
using PCUSB_CLASS_DEVICE_PATH = const USB_CLASS_DEVICE_PATH*;

constexpr CUINT8 MSG_USB_WWID_DP = 0x10;

typedef struct _USB_WWID_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT16						InterfaceNumber;
	UINT16						VendorId;
	UINT16						ProductId;
} USB_WWID_DEVICE_PATH, * PUSB_WWID_DEVICE_PATH;

using CUSB_WWID_DEVICE_PATH = const USB_WWID_DEVICE_PATH;
using PCUSB_WWID_DEVICE_PATH = const USB_WWID_DEVICE_PATH*;

constexpr CUINT8 MSG_DEVICE_LOGICAL_UNIT_DP = 0x11;

typedef struct _DEVICE_LOGICAL_UNIT_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT8						Lun;
} DEVICE_LOGICAL_UNIT_DEVICE_PATH, * PDEVICE_LOGICAL_UNIT_DEVICE_PATH;

using CDEVICE_LOGICAL_UNIT_DEVICE_PATH = const DEVICE_LOGICAL_UNIT_DEVICE_PATH;
using PCDEVICE_LOGICAL_UNIT_DEVICE_PATH = const DEVICE_LOGICAL_UNIT_DEVICE_PATH*;

constexpr CUINT8 MSG_SATA_DP = 0x12;

typedef struct _SATA_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT16						HBAPortNumber;
	UINT16						PortMultiplierPortNumber;
	UINT16						Lun;
} SATA_DEVICE_PATH, * PSATA_DEVICE_PATH;

using CSATA_DEVICE_PATH = const SATA_DEVICE_PATH;
using PCSATA_DEVICE_PATH = const SATA_DEVICE_PATH*;

constexpr CUINT8 MSG_I2O_DP = 0x06;

typedef struct _I2O_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT32						Tid;
} I2O_DEVICE_PATH, * PI2O_DEVICE_PATH;

using CI2O_DEVICE_PATH = const I2O_DEVICE_PATH;
using PCI2O_DEVICE_PATH = const I2O_DEVICE_PATH*;

constexpr CUINT8 MSG_MAC_ADDR_DP = 0x0B;

typedef struct _EFI_MAC_ADDRESS
{
	UINT8	Addr[32];
} EFI_MAC_ADDRESS, * PEFI_MAC_ADDRESS;

using CEFI_MAC_ADDRESS = const EFI_MAC_ADDRESS;
using PCEFI_MAC_ADDRESS = const EFI_MAC_ADDRESS*;

typedef struct _MAC_ADDR_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	EFI_MAC_ADDRESS				MacAddress;
	UINT8						IfType;
} MAC_ADDR_DEVICE_PATH, * PMAC_ADDR_DEVICE_PATH;

using CMAC_ADDR_DEVICE_PATH = const MAC_ADDR_DEVICE_PATH;
using PCMAC_ADDR_DEVICE_PATH = const MAC_ADDR_DEVICE_PATH*;

constexpr CUINT8 MSG_IPv4_DP = 0x0C;

typedef struct _IPv4_ADDRESS
{
	UINT8	Addr[4];
} IPv4_ADDRESS, * PIPv4_ADDRESS;

using CIPv4_ADDRESS = const IPv4_ADDRESS;
using PCIPv4_ADDRESS = const IPv4_ADDRESS*;
using EFI_IPv4_ADDRESS = IPv4_ADDRESS;
using PEFI_IPv4_ADDRESS = EFI_IPv4_ADDRESS*;
using CEFI_IPv4_ADDRESS = const EFI_IPv4_ADDRESS;
using PCEFI_IPv4_ADDRESS = const EFI_IPv4_ADDRESS*;

typedef struct _IPv4_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	EFI_IPv4_ADDRESS			LocalIpAddress;
	EFI_IPv4_ADDRESS			RemoteIpAddress;
	UINT16						LocalPort;
	UINT16						RemotePort;
	UINT16						Protocol;
	BOOLEAN 					StaticIpAddress;
	EFI_IPv4_ADDRESS			GatewayIpAddress;
	EFI_IPv4_ADDRESS			SubnetMask;
} IPv4_DEVICE_PATH, * PIPv4_DEVICE_PATH;

using CIPv4_DEVICE_PATH = const IPv4_DEVICE_PATH;
using PCIPv4_DEVICE_PATH = const IPv4_DEVICE_PATH*;

constexpr CUINT8 MSG_IPv6_DP = 0x0D;

typedef struct _IPv6_ADDRESS
{
	UINT8	Addr[16];
} IPv6_ADDRESS, * PIPv6_ADDRESS;

using CIPv6_ADDRESS = const IPv6_ADDRESS;
using PCIPv6_ADDRESS = const IPv6_ADDRESS*;
using EFI_IPv6_ADDRESS = IPv6_ADDRESS;
using PEFI_IPv6_ADDRESS = EFI_IPv6_ADDRESS*;
using CEFI_IPv6_ADDRESS = const EFI_IPv6_ADDRESS;
using PCEFI_IPv6_ADDRESS = const EFI_IPv6_ADDRESS*;

typedef struct _IPv6_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	EFI_IPv6_ADDRESS 			LocalIpAddress;
	EFI_IPv6_ADDRESS 			RemoteIpAddress;
	UINT16						LocalPort;
	UINT16						RemotePort;
	UINT16						Protocol;
	UINT8						IpAddressOrigin;
	UINT8						PrefixLength;
	EFI_IPv6_ADDRESS			GatewayIpAddress;
} IPv6_DEVICE_PATH, * PIPv6_DEVICE_PATH;

using CIPv6_DEVICE_PATH = const IPv6_DEVICE_PATH;
using PCIPv6_DEVICE_PATH = const IPv6_DEVICE_PATH*;

constexpr CUINT8 MSG_INFINIBAND_DP = 0x09;

typedef struct _INFINIBAND_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT32						ResourceFlags;
	UINT8						PortGid[16];
	UINT64						ServiceId;
	UINT64						TargetPortId;
	UINT64						DeviceId;
} INFINIBAND_DEVICE_PATH, * PINFINIBAND_DEVICE_PATH;

using CINFINIBAND_DEVICE_PATH = const INFINIBAND_DEVICE_PATH;
using PCINFINIBAND_DEVICE_PATH = const INFINIBAND_DEVICE_PATH*;

constexpr CUINT8 INFINIBAND_RESOURCE_FLAG_IOC_SERVICE = 0x01;
constexpr CUINT8 INFINIBAND_RESOURCE_FLAG_EXTENDED_BOOT_ENVIRONMENT = 0x02;
constexpr CUINT8 INFINIBAND_RESOURCE_FLAG_CONSOLE_PROTOCOL = 0x04;
constexpr CUINT8 INFINIBAND_RESOURCE_FLAG_STORAGE_PROTOCOL = 0x08;
constexpr CUINT8 INFINIBAND_RESOURCE_FLAG_NETWORK_PROTOCOL = 0x10;

constexpr CUINT8 MSG_UART_DP = 0x0E;

typedef struct _UART_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT32						Reserved;
	UINT64						BaudRate;
	UINT8						DataBits;
	UINT8						Parity;
	UINT8						StopBits;
} UART_DEVICE_PATH, * PUART_DEVICE_PATH;

using CUART_DEVICE_PATH = const UART_DEVICE_PATH;
using PCUART_DEVICE_PATH = const UART_DEVICE_PATH*;

constexpr CUINT8 NVDIMM_NAMESPACE_DP = 0x20;

typedef struct _NVDIMM_NAMESPACE_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	EFI_GUID					Uuid;
} NVDIMM_NAMESPACE_DEVICE_PATH, * PNVDIMM_NAMESPACE_DEVICE_PATH;

using CNVDIMM_NAMESPACE_DEVICE_PATH = const NVDIMM_NAMESPACE_DEVICE_PATH;
using PCNVDIMM_NAMESPACE_DEVICE_PATH = const NVDIMM_NAMESPACE_DEVICE_PATH*;

constexpr CUINT8 MSG_VENDOR_DP2 = 0x0A;

using VENDOR_DEFINED_DEVICE_PATH = VENDOR_DEVICE_PATH;
using PVENDOR_DEFINED_DEVICE_PATH = PVENDOR_DEVICE_PATH;
using CVENDOR_DEFINED_DEVICE_PATH = const VENDOR_DEFINED_DEVICE_PATH;
using PCVENDOR_DEFINED_DEVICE_PATH = const VENDOR_DEFINED_DEVICE_PATH*;

constexpr CEFI_GUID EFI_PC_ANSI_GUID = { 0xe0c14753, 0xf9be, 0x11d2, {0x9a, 0x0c, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d } };
constexpr CEFI_GUID EFI_VT_100_GUID = { 0xdfa66065, 0xb419, 0x11d3, {0x9a, 0x2d, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d } };
constexpr CEFI_GUID EFI_VT_100_PLUS_GUID = { 0x7baec70b, 0x57e0, 0x4c76, {0x8e, 0x87, 0x2f, 0x9e, 0x28, 0x08, 0x83, 0x43 } };
constexpr CEFI_GUID EFI_VT_UTF8_GUID = { 0xad15a0d6, 0x8bec, 0x4acf, {0xa0, 0x73, 0xd0, 0x1d, 0xe7, 0x7e, 0x2d, 0x88 } };
constexpr CEFI_GUID DEVICE_PATH_MESSAGING_PC_ANSI = EFI_PC_ANSI_GUID;
constexpr CEFI_GUID DEVICE_PATH_MESSAGING_VT_100 = EFI_VT_100_GUID;
constexpr CEFI_GUID DEVICE_PATH_MESSAGING_VT_100_PLUS = EFI_VT_100_PLUS_GUID;
constexpr CEFI_GUID DEVICE_PATH_MESSAGING_VT_UTF8 = EFI_VT_UTF8_GUID;

typedef struct _UART_FLOW_CONTROL_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	EFI_GUID					Guid;
	UINT32						FlowControlMap;
} UART_FLOW_CONTROL_DEVICE_PATH, * PUART_FLOW_CONTROL_DEVICE_PATH;

using CUART_FLOW_CONTROL_DEVICE_PATH = const UART_FLOW_CONTROL_DEVICE_PATH;
using PCUART_FLOW_CONTROL_DEVICE_PATH = const UART_FLOW_CONTROL_DEVICE_PATH*;

constexpr CUINT32 UART_FLOW_CONTROL_HARDWARE = 0x00000001;
constexpr CUINT32 UART_FLOW_CONTROL_XON_XOFF = 0x00000010;

constexpr CEFI_GUID EFI_SAS_DEVICE_PATH_GUID = { 0xd487ddb4, 0x008b, 0x11d9, {0xaf, 0xdc, 0x00, 0x10, 0x83, 0xff, 0xca, 0x4d } };
constexpr CEFI_GUID DEVICE_PATH_MESSAGING_SAS = EFI_SAS_DEVICE_PATH_GUID;

typedef struct _SAS_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	EFI_GUID					Guid;
	UINT32						Reserved;
	UINT64						SasAddress;
	UINT64						Lun;
	UINT16						DeviceTopology;
	UINT16						RelativeTargetPort;
} SAS_DEVICE_PATH, * PSAS_DEVICE_PATH;

using CSAS_DEVICE_PATH = const SAS_DEVICE_PATH;
using PCSAS_DEVICE_PATH = const SAS_DEVICE_PATH*;

constexpr CUINT8 MSG_SASEX_DP = 0x16;

typedef struct _SASEX_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT8						SasAddress[8];
	UINT8						Lun[8];
	UINT16						DeviceTopology;
	UINT16						RelativeTargetPort;
} SASEX_DEVICE_PATH, * PSASEX_DEVICE_PATH;

using CSASEX_DEVICE_PATH = const SASEX_DEVICE_PATH;
using PCSASEX_DEVICE_PATH = const SASEX_DEVICE_PATH*;

constexpr CUINT8 MSG_NVME_NAMESPACE_DP = 0x17;

typedef struct _NVME_NAMESPACE_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT32						NamespaceId;
	UINT64						NamespaceUuid;
} NVME_NAMESPACE_DEVICE_PATH, * PNVME_NAMESPACE_DEVICE_PATH;

using CNVME_NAMESPACE_DEVICE_PATH = const NVME_NAMESPACE_DEVICE_PATH;
using PCNVME_NAMESPACE_DEVICE_PATH = const NVME_NAMESPACE_DEVICE_PATH*;

constexpr CUINT8 MSG_NVME_OF_NAMESPACE_DP = 0x22;

typedef struct _NVME_OF_NAMESPACE_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT8						NamespaceIdType;
	UINT8						NamespaceId[16];
	CHAR						SubsystemNqn[];
} NVME_OF_NAMESPACE_DEVICE_PATH, * PNVME_OF_NAMESPACE_DEVICE_PATH;

using CNVME_OF_NAMESPACE_DEVICE_PATH = const NVME_OF_NAMESPACE_DEVICE_PATH;
using PCNVME_OF_NAMESPACE_DEVICE_PATH = const NVME_OF_NAMESPACE_DEVICE_PATH*;

constexpr CUINT8 MSG_DNS_DP = 0x1F;

typedef union _EFI_IP_ADDRESS
{
	UINT32				Addr[4];
	EFI_IPv4_ADDRESS	v4;
	EFI_IPv6_ADDRESS	v6;
} EFI_IP_ADDRESS, * PEFI_IP_ADDRESS;

using CEFI_IP_ADDRESS = const EFI_IP_ADDRESS;
using PCEFI_IP_ADDRESS = const EFI_IP_ADDRESS*;

typedef struct _DNS_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT8						IsIPv6;
	EFI_IP_ADDRESS				DnsServerIp[];
} DNS_DEVICE_PATH, * PDNS_DEVICE_PATH;

using CDNS_DEVICE_PATH = const DNS_DEVICE_PATH;
using PCDNS_DEVICE_PATH = const DNS_DEVICE_PATH*;

constexpr CUINT8 MSG_URI_DP = 0x18;

typedef struct _URI_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	CHAR						Uri[];
} URI_DEVICE_PATH, * PURI_DEVICE_PATH;

using CURI_DEVICE_PATH = const URI_DEVICE_PATH;
using PCURI_DEVICE_PATH = const URI_DEVICE_PATH*;

constexpr CUINT8 MSG_UFS_DP = 0x19;

typedef struct _UFS_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT8						Pun;
	UINT8						Lun;
} UFS_DEVICE_PATH, * PUFS_DEVICE_PATH;

using CUFS_DEVICE_PATH = const UFS_DEVICE_PATH;
using PCUFS_DEVICE_PATH = const UFS_DEVICE_PATH*;

constexpr CUINT8 MSG_SD_DP = 0x1A;

typedef struct _SD_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT8						SlotNumber;
} SD_DEVICE_PATH, * PSD_DEVICE_PATH;

using CSD_DEVICE_PATH = const SD_DEVICE_PATH;
using PCSD_DEVICE_PATH = const SD_DEVICE_PATH*;

constexpr CUINT8 MSG_EMMC_DP = 0x1D;

typedef struct _EMMC_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT8						SlotNumber;
} EMMC_DEVICE_PATH, * PEMMC_DEVICE_PATH;

using CEMMC_DEVICE_PATH = const EMMC_DEVICE_PATH;
using PCEMMC_DEVICE_PATH = const EMMC_DEVICE_PATH*;

constexpr CUINT8 MSG_ISCSI_DP = 0x13;

typedef struct _ISCSI_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT16						NetworkProtocol;
	UINT16						LoginOption;
	UINT64						Lun;
	UINT16						TargetPortalGroupTag;
} ISCSI_DEVICE_PATH, * PISCSI_DEVICE_PATH;

using CISCSI_DEVICE_PATH = const ISCSI_DEVICE_PATH;
using PCISCSI_DEVICE_PATH = const ISCSI_DEVICE_PATH*;

constexpr CUINT16 ISCSI_LOGIN_OPTION_NO_HEADER_DIGEST = 0x0000;
constexpr CUINT16 ISCSI_LOGIN_OPTION_HEADER_DIGEST_USING_CRC32C = 0x0002;
constexpr CUINT16 ISCSI_LOGIN_OPTION_NO_DATA_DIGEST = 0x0000;
constexpr CUINT16 ISCSI_LOGIN_OPTION_DATA_DIGEST_USING_CRC32C = 0x0008;
constexpr CUINT16 ISCSI_LOGIN_OPTION_AUTHMETHOD_CHAP = 0x0000;
constexpr CUINT16 ISCSI_LOGIN_OPTION_AUTHMETHOD_NON = 0x1000;
constexpr CUINT16 ISCSI_LOGIN_OPTION_CHAP_BI = 0x0000;
constexpr CUINT16 ISCSI_LOGIN_OPTION_CHAP_UNI = 0x2000;

constexpr CUINT8 MSG_VLAN_DP = 0x14;

typedef struct _VLAN_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT16						VlanId;
} VLAN_DEVICE_PATH, * PVLAN_DEVICE_PATH;

using CVLAN_DEVICE_PATH = const VLAN_DEVICE_PATH;
using PCVLAN_DEVICE_PATH = const VLAN_DEVICE_PATH*;

constexpr CUINT8 MSG_BLUETOOTH_DP = 0x1B;

typedef struct _BLUETOOTH_ADDRESS
{
	UINT8	Address[6];
} BLUETOOTH_ADDRESS, * PBLUETOOTH_ADDRESS;

using CBLUETOOTH_ADDRESS = const BLUETOOTH_ADDRESS;
using PCBLUETOOTH_ADDRESS = const BLUETOOTH_ADDRESS*;

typedef struct _BLUETOOTH_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	BLUETOOTH_ADDRESS			BD_ADDR;
} BLUETOOTH_DEVICE_PATH, * PBLUETOOTH_DEVICE_PATH;

using CBLUETOOTH_DEVICE_PATH = const BLUETOOTH_DEVICE_PATH;
using PCBLUETOOTH_DEVICE_PATH = const BLUETOOTH_DEVICE_PATH*;

constexpr CUINT8 MSG_WIFI_DP = 0x1C;

typedef struct _WIFI_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT8						SSId[32];
} WIFI_DEVICE_PATH, * PWIFI_DEVICE_PATH;

using CWIFI_DEVICE_PATH = const WIFI_DEVICE_PATH;
using PCWIFI_DEVICE_PATH = const WIFI_DEVICE_PATH*;

constexpr CUINT8 MSG_BLUETOOTH_LE_DP = 0x1E;

typedef struct _BLUETOOTH_LE_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	BLUETOOTH_ADDRESS			Address;
} BLUETOOTH_LE_DEVICE_PATH, * PBLUETOOTH_LE_DEVICE_PATH;

using CBLUETOOTH_LE_DEVICE_PATH = const BLUETOOTH_LE_DEVICE_PATH;
using PCBLUETOOTH_LE_DEVICE_PATH = const BLUETOOTH_LE_DEVICE_PATH*;

constexpr CUINT8 MEDIA_DEVICE_PATH = 0x04;
constexpr CUINT8 MEDIA_HARDDRIVE_DP = 0x01;

typedef struct _HARDDRIVE_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT32						PartitionNumber;
	UINT64						PartitionStart;
	UINT64						PartitionSize;
	UINT8						Signature[16];
	UINT8						MBRType;
	UINT8						SignatureType;
} HARDDRIVE_DEVICE_PATH, * PHARDDRIVE_DEVICE_PATH;

using CHARDDRIVE_DEVICE_PATH = const HARDDRIVE_DEVICE_PATH;
using PCHARDDRIVE_DEVICE_PATH = const HARDDRIVE_DEVICE_PATH*;

constexpr CUINT8 MBR_TYPE_PCAT = 0x01;
constexpr CUINT8 MBR_TYPE_EFI_PARTITION_TABLE_HEADER = 0x02;

constexpr CUINT8 NO_DISK_SIGNATURE = 0x00;
constexpr CUINT8 SIGNATURE_TYPE_MBR = 0x01;
constexpr CUINT8 SIGNATURE_TYPE_GUID = 0x02;

constexpr CUINT8 MEDIA_CDROM_DP = 0x02;

typedef struct _CDROM_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT32						BootEntry;
	UINT64 						PartitionStart;
	UINT64 						PartitionSize;
} CDROM_DEVICE_PATH, * PCDROM_DEVICE_PATH;

using CCDROM_DEVICE_PATH = const CDROM_DEVICE_PATH;
using PCCDROM_DEVICE_PATH = const CDROM_DEVICE_PATH*;

constexpr CUINT8 MEDIA_VENDOR_DP = 0x03;
constexpr CUINT8 MEDIA_FILEPATH_DP = 0x04;

typedef struct _FILEPATH_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	WCHAR						PathName[1];
} FILEPATH_DEVICE_PATH, * PFILEPATH_DEVICE_PATH;

using CFILEPATH_DEVICE_PATH = const FILEPATH_DEVICE_PATH;
using PCFILEPATH_DEVICE_PATH = const FILEPATH_DEVICE_PATH*;

constexpr CUINT64 SIZE_OF_FILEPATH_DEVICE_PATH = offsetof(FILEPATH_DEVICE_PATH, PathName);

constexpr CUINT8 MEDIA_PROTOCOL_DP = 0x05;

typedef struct _MEDIA_PROTOCOL_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	EFI_GUID					Protocol;
} MEDIA_PROTOCOL_DEVICE_PATH, * PMEDIA_PROTOCOL_DEVICE_PATH;

using CMEDIA_PROTOCOL_DEVICE_PATH = const MEDIA_PROTOCOL_DEVICE_PATH;
using PCMEDIA_PROTOCOL_DEVICE_PATH = const MEDIA_PROTOCOL_DEVICE_PATH*;

constexpr CUINT8 MEDIA_PIWG_FW_FILE_DP = 0x06;

typedef struct _MEDIA_FW_VOL_FILEPATH_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	EFI_GUID 					FvFileName;
} MEDIA_FW_VOL_FILEPATH_DEVICE_PATH, * PMEDIA_FW_VOL_FILEPATH_DEVICE_PATH;

using CMEDIA_FW_VOL_FILEPATH_DEVICE_PATH = const MEDIA_FW_VOL_FILEPATH_DEVICE_PATH;
using PCMEDIA_FW_VOL_FILEPATH_DEVICE_PATH = const MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*;

constexpr CUINT8 MEDIA_PIWG_FW_VOL_DP = 0x07;

typedef struct _MEDIA_FW_VOL_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	EFI_GUID 					FvName;
} MEDIA_FW_VOL_DEVICE_PATH, * PMEDIA_FW_VOL_DEVICE_PATH;

using CMEDIA_FW_VOL_DEVICE_PATH = const MEDIA_FW_VOL_DEVICE_PATH;
using PCMEDIA_FW_VOL_DEVICE_PATH = const MEDIA_FW_VOL_DEVICE_PATH*;

constexpr CUINT8 MEDIA_RELATIVE_OFFSET_RANGE_DP = 0x08;

typedef struct _MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT32						Reserved;
	UINT64						StartingOffset;
	UINT64						EndingOffset;
} MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH, * PMEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH;

using CMEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH = const MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH;
using PCMEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH = const MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH*;

constexpr CEFI_GUID EFI_ACPI_6_0_NFIT_GUID_RAM_DISK_SUPPORTING_VIRTUAL_DISK_REGION_VOLATILE = { 0x77AB535A, 0x45FC, 0x624B, { 0x55, 0x60, 0xF7, 0xB2, 0x81, 0xD1, 0xF9, 0x6E } };
constexpr CEFI_GUID EFI_ACPI_6_0_NFIT_GUID_RAM_DISK_SUPPORTING_VIRTUAL_CD_REGION_VOLATILE = { 0x3D5ABD30, 0x4175, 0x87CE, { 0x6D, 0x64, 0xD2, 0xAD, 0xE5, 0x23, 0xC4, 0xBB } };
constexpr CEFI_GUID EFI_ACPI_6_0_NFIT_GUID_RAM_DISK_SUPPORTING_VIRTUAL_DISK_REGION_PERSISTENT = { 0x5CEA02C9, 0x4D07, 0x69D3, { 0x26, 0x9F ,0x44, 0x96, 0xFB, 0xE0, 0x96, 0xF9 } };
constexpr CEFI_GUID EFI_ACPI_6_0_NFIT_GUID_RAM_DISK_SUPPORTING_VIRTUAL_CD_REGION_PERSISTENT = { 0x08018188, 0x42CD, 0xBB48, { 0x10, 0x0F, 0x53, 0x87, 0xD5, 0x3D, 0xED, 0x3D } };
constexpr CEFI_GUID gEfiVirtualDiskGuid = EFI_ACPI_6_0_NFIT_GUID_RAM_DISK_SUPPORTING_VIRTUAL_DISK_REGION_VOLATILE;
constexpr CEFI_GUID gEfiVirtualCdGuid = EFI_ACPI_6_0_NFIT_GUID_RAM_DISK_SUPPORTING_VIRTUAL_CD_REGION_VOLATILE;
constexpr CEFI_GUID gEfiPersistentVirtualDiskGuid = EFI_ACPI_6_0_NFIT_GUID_RAM_DISK_SUPPORTING_VIRTUAL_DISK_REGION_PERSISTENT;
constexpr CEFI_GUID gEfiPersistentVirtualCdGuid = EFI_ACPI_6_0_NFIT_GUID_RAM_DISK_SUPPORTING_VIRTUAL_CD_REGION_PERSISTENT;

constexpr CUINT8 MEDIA_RAM_DISK_DP = 0x09;

typedef struct _MEDIA_RAM_DISK_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT32						StartingAddr[2];
	UINT32 						EndingAddr[2];
	EFI_GUID					TypeGuid;
	UINT16						Instance;
} MEDIA_RAM_DISK_DEVICE_PATH, * PMEDIA_RAM_DISK_DEVICE_PATH;

using CMEDIA_RAM_DISK_DEVICE_PATH = const MEDIA_RAM_DISK_DEVICE_PATH;
using PCMEDIA_RAM_DISK_DEVICE_PATH = const MEDIA_RAM_DISK_DEVICE_PATH*;

constexpr CUINT8 BBS_DEVICE_PATH = 0x05;
constexpr CUINT8 BBS_BBS_DP = 0x01;

typedef struct _BBS_BBS_DEVICE_PATH
{
	EFI_DEVICE_PATH_PROTOCOL	Header;
	UINT16						DeviceType;
	UINT16						StatusFlag;
	CHAR						String[1];
} BBS_BBS_DEVICE_PATH, * PBBS_BBS_DEVICE_PATH;

using CBBS_BBS_DEVICE_PATH = const BBS_BBS_DEVICE_PATH;
using PCBBS_BBS_DEVICE_PATH = const BBS_BBS_DEVICE_PATH*;

constexpr CUINT8 BBS_TYPE_FLOPPY = 0x01;
constexpr CUINT8 BBS_TYPE_HARDDRIVE = 0x02;
constexpr CUINT8 BBS_TYPE_CDROM = 0x03;
constexpr CUINT8 BBS_TYPE_PCMCIA = 0x04;
constexpr CUINT8 BBS_TYPE_USB = 0x05;
constexpr CUINT8 BBS_TYPE_EMBEDDED_NETWORK = 0x06;
constexpr CUINT8 BBS_TYPE_BEV = 0x80;
constexpr CUINT8 BBS_TYPE_UNKNOWN = 0xFF;

typedef union _EFI_DEV_PATH
{
	EFI_DEVICE_PATH_PROTOCOL                   DevPath;
	PCI_DEVICE_PATH                            Pci;
	PCCARD_DEVICE_PATH                         PcCard;
	MEMMAP_DEVICE_PATH                         MemMap;
	VENDOR_DEVICE_PATH                         Vendor;

	CONTROLLER_DEVICE_PATH                     Controller;
	BMC_DEVICE_PATH                            Bmc;
	ACPI_HID_DEVICE_PATH                       Acpi;
	ACPI_EXTENDED_HID_DEVICE_PATH              ExtendedAcpi;
	ACPI_ADR_DEVICE_PATH                       AcpiAdr;

	ATAPI_DEVICE_PATH                          Atapi;
	SCSI_DEVICE_PATH                           Scsi;
	ISCSI_DEVICE_PATH                          Iscsi;
	FIBRECHANNEL_DEVICE_PATH                   FibreChannel;
	FIBRECHANNELEX_DEVICE_PATH                 FibreChannelEx;

	F1394_DEVICE_PATH                          F1394;
	USB_DEVICE_PATH                            Usb;
	SATA_DEVICE_PATH                           Sata;
	USB_CLASS_DEVICE_PATH                      UsbClass;
	USB_WWID_DEVICE_PATH                       UsbWwid;
	DEVICE_LOGICAL_UNIT_DEVICE_PATH            LogicUnit;
	I2O_DEVICE_PATH                            I2O;
	MAC_ADDR_DEVICE_PATH                       MacAddr;
	IPv4_DEVICE_PATH                           Ipv4;
	IPv6_DEVICE_PATH                           Ipv6;
	VLAN_DEVICE_PATH                           Vlan;
	INFINIBAND_DEVICE_PATH                     InfiniBand;
	UART_DEVICE_PATH                           Uart;
	UART_FLOW_CONTROL_DEVICE_PATH              UartFlowControl;
	SAS_DEVICE_PATH                            Sas;
	SASEX_DEVICE_PATH                          SasEx;
	NVME_NAMESPACE_DEVICE_PATH                 NvmeNamespace;
	NVME_OF_NAMESPACE_DEVICE_PATH              NvmeOfNamespace;
	DNS_DEVICE_PATH                            Dns;
	URI_DEVICE_PATH                            Uri;
	BLUETOOTH_DEVICE_PATH                      Bluetooth;
	WIFI_DEVICE_PATH                           WiFi;
	UFS_DEVICE_PATH                            Ufs;
	SD_DEVICE_PATH                             Sd;
	EMMC_DEVICE_PATH                           Emmc;
	HARDDRIVE_DEVICE_PATH                      HardDrive;
	CDROM_DEVICE_PATH                          CD;

	FILEPATH_DEVICE_PATH                       FilePath;
	MEDIA_PROTOCOL_DEVICE_PATH                 MediaProtocol;

	MEDIA_FW_VOL_DEVICE_PATH                   FirmwareVolume;
	MEDIA_FW_VOL_FILEPATH_DEVICE_PATH          FirmwareFile;
	MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH    Offset;
	MEDIA_RAM_DISK_DEVICE_PATH                 RamDisk;
	BBS_BBS_DEVICE_PATH                        Bbs;
} EFI_DEV_PATH, * PEFI_DEV_PATH;

using CEFI_DEV_PATH = const EFI_DEV_PATH;
using PCEFI_DEV_PATH = const EFI_DEV_PATH*;

typedef union _EFI_DEV_PATH_PTR
{
	PEFI_DEVICE_PATH_PROTOCOL					DevPath;
	PPCI_DEVICE_PATH							Pci;
	PPCCARD_DEVICE_PATH							PcCard;
	PMEMMAP_DEVICE_PATH							MemMap;
	PVENDOR_DEVICE_PATH							Vendor;

	PCONTROLLER_DEVICE_PATH						Controller;
	PBMC_DEVICE_PATH							Bmc;
	PACPI_HID_DEVICE_PATH						Acpi;
	PACPI_EXTENDED_HID_DEVICE_PATH				ExtendedAcpi;
	PACPI_ADR_DEVICE_PATH						AcpiAdr;

	PATAPI_DEVICE_PATH							Atapi;
	PSCSI_DEVICE_PATH							Scsi;
	PISCSI_DEVICE_PATH							Iscsi;
	PFIBRECHANNEL_DEVICE_PATH					FibreChannel;
	PFIBRECHANNELEX_DEVICE_PATH					FibreChannelEx;

	PF1394_DEVICE_PATH							F1394;
	PUSB_DEVICE_PATH							Usb;
	PSATA_DEVICE_PATH							Sata;
	PUSB_CLASS_DEVICE_PATH						UsbClass;
	PUSB_WWID_DEVICE_PATH						UsbWwid;
	PDEVICE_LOGICAL_UNIT_DEVICE_PATH			LogicUnit;
	PI2O_DEVICE_PATH							I2O;
	PMAC_ADDR_DEVICE_PATH						MacAddr;
	PIPv4_DEVICE_PATH							Ipv4;
	PIPv6_DEVICE_PATH							Ipv6;
	PVLAN_DEVICE_PATH							Vlan;
	PINFINIBAND_DEVICE_PATH						InfiniBand;
	PUART_DEVICE_PATH							Uart;
	PUART_FLOW_CONTROL_DEVICE_PATH				UartFlowControl;
	PSAS_DEVICE_PATH							Sas;
	PSASEX_DEVICE_PATH							SasEx;
	PNVME_NAMESPACE_DEVICE_PATH					NvmeNamespace;
	PNVME_OF_NAMESPACE_DEVICE_PATH				NvmeOfNamespace;
	PDNS_DEVICE_PATH							Dns;
	PURI_DEVICE_PATH							Uri;
	PBLUETOOTH_DEVICE_PATH						Bluetooth;
	PWIFI_DEVICE_PATH							WiFi;
	PUFS_DEVICE_PATH							Ufs;
	PSD_DEVICE_PATH								Sd;
	PEMMC_DEVICE_PATH							Emmc;
	PHARDDRIVE_DEVICE_PATH						HardDrive;
	PCDROM_DEVICE_PATH							CD;

	PFILEPATH_DEVICE_PATH						FilePath;
	PMEDIA_PROTOCOL_DEVICE_PATH					MediaProtocol;

	PMEDIA_FW_VOL_DEVICE_PATH					FirmwareVolume;
	PMEDIA_FW_VOL_FILEPATH_DEVICE_PATH			FirmwareFile;
	PMEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH	Offset;
	PMEDIA_RAM_DISK_DEVICE_PATH					RamDisk;
	PBBS_BBS_DEVICE_PATH						Bbs;
	PUINT8										Raw;
} EFI_DEV_PATH_PTR, * PEFI_DEV_PATH_PTR;

using CEFI_DEV_PATH_PTR = const EFI_DEV_PATH_PTR;
using PCEFI_DEV_PATH_PTR = const EFI_DEV_PATH_PTR*;
#pragma pack(pop)

constexpr CUINT8 END_DEVICE_PATH_TYPE = 0x7F;
constexpr CUINT8 END_ENTIRE_DEVICE_PATH_SUBTYPE = 0xFF;
constexpr CUINT8 END_INSTANCE_DEVICE_PATH_SUBTYPE = 0x01;

constexpr CEFI_GUID gEfiFileInfoGuid = { 0x09576E92, 0x6D3F, 0x11D2, { 0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B } };

typedef struct _EFI_FILE_INFO
{
	UINT64		Size;
	UINT64		FileSize;
	UINT64		PhysicalSize;
	EFI_TIME	CreateTime;
	EFI_TIME	LastAccessTime;
	EFI_TIME	ModificationTime;
	UINT64		Attribute;
	WCHAR		FileName[1];
} EFI_FILE_INFO, * PEFI_FILE_INFO;

using CEFI_FILE_INFO = const EFI_FILE_INFO;
using PCEFI_FILE_INFO = const EFI_FILE_INFO*;

constexpr
UINT64
SIZE_OF_EFI_FILE_INFO(
	VOID
)
{
	return offsetof(EFI_FILE_INFO, FileName);
}

constexpr
auto
DevicePathType(
	IN	PCEFI_DEVICE_PATH_PROTOCOL	Node
) -> UINT8
{
	return Node->Type;
}

constexpr
auto
DevicePathSubType(
	IN	PCEFI_DEVICE_PATH_PROTOCOL	Node
) -> UINT8
{
	return Node->SubType;
}

using EfiDevicePathToTextPathFn = PCWSTR(__cdecl)(
	IN PCEFI_DEVICE_PATH_PROTOCOL DevicePath,
	IN CBOOLEAN DisplayOnly,
	IN CBOOLEAN AllowShortcuts
);

using EfiDevicePathToTextNodeFn = PCWSTR(__cdecl)(
	IN PCEFI_DEVICE_PATH_PROTOCOL DeviceNode,
	IN CBOOLEAN DisplayOnly,
	IN CBOOLEAN AllowShortcuts
);

typedef struct _EFI_DEVICE_PATH_TO_TEXT_PROTOCOL
{
	EfiDevicePathToTextNodeFn* ConvertDeviceNodeToText;
	EfiDevicePathToTextPathFn* ConvertDevicePathToPath;
} EFI_DEVICE_PATH_TO_TEXT_PROTOCOL, *PEFI_DEVICE_PATH_TO_TEXT_PROTOCOL;

using CEFI_DEVICE_PATH_TO_TEXT_PROTOCOL = const EFI_DEVICE_PATH_TO_TEXT_PROTOCOL;
using PCEFI_DEVICE_PATH_TO_TEXT_PROTOCOL = const EFI_DEVICE_PATH_TO_TEXT_PROTOCOL*;

constexpr EFI_GUID gEfiDevicePathToTextProtocolGuid = { 0x8B843E20, 0x8132, 0x4852, { 0x90, 0xCC, 0x55, 0x1A, 0x4E, 0x4A, 0x7F, 0x1C } };
constexpr CEFI_GUID gEfiGlobalVariableGuid = { 0x8BE4DF61, 0x93CA, 0x11D2, { 0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C } };

using EfiImageUnloadFn = EFI_STATUS(__cdecl)(EFI_HANDLE ImageHandle);

typedef struct _EFI_LOADED_IMAGE_PROTOCOL
{
	UINT32 Revision;
	EFI_HANDLE ParentHandle;
	PEFI_SYSTEM_TABLE SystemTable;
	EFI_HANDLE DeviceHandle;
	PEFI_DEVICE_PATH_PROTOCOL FilePath;
	PVOID Reserved;
	UINT32 LoadOptionsSize;
	PWSTR LoadOptions;
	PVOID ImageBase;
	UINT64 ImageSize;
	EFI_MEMORY_TYPE ImageCodeType;
	EFI_MEMORY_TYPE ImageDataType;
	EfiImageUnloadFn* Unload;
} EFI_LOADED_IMAGE_PROTOCOL, *PEFI_LOADED_IMAGE_PROTOCOL;

using CEFI_LOADED_IMAGE_PROTOCOL = const EFI_LOADED_IMAGE_PROTOCOL;
using PCEFI_LOADED_IMAGE_PROTOCOL = const EFI_LOADED_IMAGE_PROTOCOL*;

constexpr EFI_GUID gEfiLoadedImageProtocolGuid = { 0x5B1B31A1, 0x9562, 0x11d2, { 0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B } };

//
// PE32+ Subsystem type for EFI images
//
#define EFI_IMAGE_SUBSYSTEM_EFI_APPLICATION          10
#define EFI_IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER  11
#define EFI_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER       12
#define EFI_IMAGE_SUBSYSTEM_SAL_RUNTIME_DRIVER       13///< defined PI Specification, 1.0

//
// PE32+ Machine type for EFI images
//
#define IMAGE_FILE_MACHINE_I386            0x014c
#define IMAGE_FILE_MACHINE_IA64            0x0200
#define IMAGE_FILE_MACHINE_EBC             0x0EBC
#define IMAGE_FILE_MACHINE_X64             0x8664
#define IMAGE_FILE_MACHINE_ARMTHUMB_MIXED  0x01c2
#define IMAGE_FILE_MACHINE_ARM64           0xAA64
#define IMAGE_FILE_MACHINE_RISCV32         0x5032
#define IMAGE_FILE_MACHINE_RISCV64         0x5064
#define IMAGE_FILE_MACHINE_RISCV128        0x5128
#define IMAGE_FILE_MACHINE_LOONGARCH32     0x6232
#define IMAGE_FILE_MACHINE_LOONGARCH64     0x6264

//
// EXE file formats
//
#define SIGNATURE_16(A, B)        ((A) | (B << 8))
#define SIGNATURE_32(A, B, C, D)  (SIGNATURE_16 (A, B) | (SIGNATURE_16 (C, D) << 16))
#define SIGNATURE_64(A, B, C, D, E, F, G, H) \
    (SIGNATURE_32 (A, B, C, D) | ((UINT64) (SIGNATURE_32 (E, F, G, H)) << 32))
#define EFI_IMAGE_DOS_SIGNATURE     SIGNATURE_16('M', 'Z')
#define EFI_IMAGE_OS2_SIGNATURE     SIGNATURE_16('N', 'E')
#define EFI_IMAGE_OS2_SIGNATURE_LE  SIGNATURE_16('L', 'E')
#define EFI_IMAGE_NT_SIGNATURE      SIGNATURE_32('P', 'E', '\0', '\0')

///
/// PE images can start with an optional DOS header, so if an image is run
/// under DOS it can print an error message.
///
typedef struct {
	UINT16    e_magic;    ///< Magic number.
	UINT16    e_cblp;     ///< Bytes on last page of file.
	UINT16    e_cp;       ///< Pages in file.
	UINT16    e_crlc;     ///< Relocations.
	UINT16    e_cparhdr;  ///< Size of header in paragraphs.
	UINT16    e_minalloc; ///< Minimum extra paragraphs needed.
	UINT16    e_maxalloc; ///< Maximum extra paragraphs needed.
	UINT16    e_ss;       ///< Initial (relative) SS value.
	UINT16    e_sp;       ///< Initial SP value.
	UINT16    e_csum;     ///< Checksum.
	UINT16    e_ip;       ///< Initial IP value.
	UINT16    e_cs;       ///< Initial (relative) CS value.
	UINT16    e_lfarlc;   ///< File address of relocation table.
	UINT16    e_ovno;     ///< Overlay number.
	UINT16    e_res[4];   ///< Reserved words.
	UINT16    e_oemid;    ///< OEM identifier (for e_oeminfo).
	UINT16    e_oeminfo;  ///< OEM information; e_oemid specific.
	UINT16    e_res2[10]; ///< Reserved words.
	UINT32    e_lfanew;   ///< File address of new exe header.
} EFI_IMAGE_DOS_HEADER;

///
/// COFF File Header (Object and Image).
///
typedef struct {
	UINT16    Machine;
	UINT16    NumberOfSections;
	UINT32    TimeDateStamp;
	UINT32    PointerToSymbolTable;
	UINT32    NumberOfSymbols;
	UINT16    SizeOfOptionalHeader;
	UINT16    Characteristics;
} EFI_IMAGE_FILE_HEADER;

///
/// Size of EFI_IMAGE_FILE_HEADER.
///
#define EFI_IMAGE_SIZEOF_FILE_HEADER  20

//
// Characteristics
//
#define EFI_IMAGE_FILE_RELOCS_STRIPPED      BIT0     ///< 0x0001  Relocation info stripped from file.
#define EFI_IMAGE_FILE_EXECUTABLE_IMAGE     BIT1     ///< 0x0002  File is executable  (i.e. no unresolved externel references).
#define EFI_IMAGE_FILE_LINE_NUMS_STRIPPED   BIT2     ///< 0x0004  Line numbers stripped from file.
#define EFI_IMAGE_FILE_LOCAL_SYMS_STRIPPED  BIT3     ///< 0x0008  Local symbols stripped from file.
#define EFI_IMAGE_FILE_LARGE_ADDRESS_AWARE  BIT5     ///< 0x0020  Supports addresses > 2-GB
#define EFI_IMAGE_FILE_BYTES_REVERSED_LO    BIT7     ///< 0x0080  Bytes of machine word are reversed.
#define EFI_IMAGE_FILE_32BIT_MACHINE        BIT8     ///< 0x0100  32 bit word machine.
#define EFI_IMAGE_FILE_DEBUG_STRIPPED       BIT9     ///< 0x0200  Debugging info stripped from file in .DBG file.
#define EFI_IMAGE_FILE_SYSTEM               BIT12    ///< 0x1000  System File.
#define EFI_IMAGE_FILE_DLL                  BIT13    ///< 0x2000  File is a DLL.
#define EFI_IMAGE_FILE_BYTES_REVERSED_HI    BIT15    ///< 0x8000  Bytes of machine word are reversed.

///
/// Header Data Directories.
///
typedef struct {
	UINT32    VirtualAddress;
	UINT32    Size;
} EFI_IMAGE_DATA_DIRECTORY;

//
// Directory Entries
//
#define EFI_IMAGE_DIRECTORY_ENTRY_EXPORT       0
#define EFI_IMAGE_DIRECTORY_ENTRY_IMPORT       1
#define EFI_IMAGE_DIRECTORY_ENTRY_RESOURCE     2
#define EFI_IMAGE_DIRECTORY_ENTRY_EXCEPTION    3
#define EFI_IMAGE_DIRECTORY_ENTRY_SECURITY     4
#define EFI_IMAGE_DIRECTORY_ENTRY_BASERELOC    5
#define EFI_IMAGE_DIRECTORY_ENTRY_DEBUG        6
#define EFI_IMAGE_DIRECTORY_ENTRY_COPYRIGHT    7
#define EFI_IMAGE_DIRECTORY_ENTRY_GLOBALPTR    8
#define EFI_IMAGE_DIRECTORY_ENTRY_TLS          9
#define EFI_IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG  10

#define EFI_IMAGE_NUMBER_OF_DIRECTORY_ENTRIES  16

///
/// @attention
/// EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC means PE32 and
/// EFI_IMAGE_OPTIONAL_HEADER32 must be used. The data structures only vary
/// after NT additional fields.
///
#define EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC  0x10b

///
/// Optional Header Standard Fields for PE32.
///
typedef struct {
	///
	/// Standard fields.
	///
	UINT16                      Magic;
	UINT8                       MajorLinkerVersion;
	UINT8                       MinorLinkerVersion;
	UINT32                      SizeOfCode;
	UINT32                      SizeOfInitializedData;
	UINT32                      SizeOfUninitializedData;
	UINT32                      AddressOfEntryPoint;
	UINT32                      BaseOfCode;
	UINT32                      BaseOfData; ///< PE32 contains this additional field, which is absent in PE32+.
	///
	/// Optional Header Windows-Specific Fields.
	///
	UINT32                      ImageBase;
	UINT32                      SectionAlignment;
	UINT32                      FileAlignment;
	UINT16                      MajorOperatingSystemVersion;
	UINT16                      MinorOperatingSystemVersion;
	UINT16                      MajorImageVersion;
	UINT16                      MinorImageVersion;
	UINT16                      MajorSubsystemVersion;
	UINT16                      MinorSubsystemVersion;
	UINT32                      Win32VersionValue;
	UINT32                      SizeOfImage;
	UINT32                      SizeOfHeaders;
	UINT32                      CheckSum;
	UINT16                      Subsystem;
	UINT16                      DllCharacteristics;
	UINT32                      SizeOfStackReserve;
	UINT32                      SizeOfStackCommit;
	UINT32                      SizeOfHeapReserve;
	UINT32                      SizeOfHeapCommit;
	UINT32                      LoaderFlags;
	UINT32                      NumberOfRvaAndSizes;
	EFI_IMAGE_DATA_DIRECTORY    DataDirectory[EFI_IMAGE_NUMBER_OF_DIRECTORY_ENTRIES];
} EFI_IMAGE_OPTIONAL_HEADER32;

///
/// @attention
/// EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC means PE32+ and
/// EFI_IMAGE_OPTIONAL_HEADER64 must be used. The data structures only vary
/// after NT additional fields.
///
#define EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC  0x20b

///
/// Optional Header Standard Fields for PE32+.
///
typedef struct {
	///
	/// Standard fields.
	///
	UINT16                      Magic;
	UINT8                       MajorLinkerVersion;
	UINT8                       MinorLinkerVersion;
	UINT32                      SizeOfCode;
	UINT32                      SizeOfInitializedData;
	UINT32                      SizeOfUninitializedData;
	UINT32                      AddressOfEntryPoint;
	UINT32                      BaseOfCode;
	///
	/// Optional Header Windows-Specific Fields.
	///
	UINT64                      ImageBase;
	UINT32                      SectionAlignment;
	UINT32                      FileAlignment;
	UINT16                      MajorOperatingSystemVersion;
	UINT16                      MinorOperatingSystemVersion;
	UINT16                      MajorImageVersion;
	UINT16                      MinorImageVersion;
	UINT16                      MajorSubsystemVersion;
	UINT16                      MinorSubsystemVersion;
	UINT32                      Win32VersionValue;
	UINT32                      SizeOfImage;
	UINT32                      SizeOfHeaders;
	UINT32                      CheckSum;
	UINT16                      Subsystem;
	UINT16                      DllCharacteristics;
	UINT64                      SizeOfStackReserve;
	UINT64                      SizeOfStackCommit;
	UINT64                      SizeOfHeapReserve;
	UINT64                      SizeOfHeapCommit;
	UINT32                      LoaderFlags;
	UINT32                      NumberOfRvaAndSizes;
	EFI_IMAGE_DATA_DIRECTORY    DataDirectory[EFI_IMAGE_NUMBER_OF_DIRECTORY_ENTRIES];
} EFI_IMAGE_OPTIONAL_HEADER64;

///
/// @attention
/// EFI_IMAGE_NT_HEADERS32 is for use ONLY by tools.
///
typedef struct {
	UINT32                         Signature;
	EFI_IMAGE_FILE_HEADER          FileHeader;
	EFI_IMAGE_OPTIONAL_HEADER32    OptionalHeader;
} EFI_IMAGE_NT_HEADERS32;

#define EFI_IMAGE_SIZEOF_NT_OPTIONAL32_HEADER  sizeof (EFI_IMAGE_NT_HEADERS32)

///
/// @attention
/// EFI_IMAGE_HEADERS64 is for use ONLY by tools.
///
typedef struct {
	UINT32                         Signature;
	EFI_IMAGE_FILE_HEADER          FileHeader;
	EFI_IMAGE_OPTIONAL_HEADER64    OptionalHeader;
} EFI_IMAGE_NT_HEADERS64;

#define EFI_IMAGE_SIZEOF_NT_OPTIONAL64_HEADER  sizeof (EFI_IMAGE_NT_HEADERS64)

//
// Other Windows Subsystem Values
//
#define EFI_IMAGE_SUBSYSTEM_UNKNOWN      0
#define EFI_IMAGE_SUBSYSTEM_NATIVE       1
#define EFI_IMAGE_SUBSYSTEM_WINDOWS_GUI  2
#define EFI_IMAGE_SUBSYSTEM_WINDOWS_CUI  3
#define EFI_IMAGE_SUBSYSTEM_OS2_CUI      5
#define EFI_IMAGE_SUBSYSTEM_POSIX_CUI    7

//
// DLL Characteristics
//
#define IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA        0x0020
#define IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE           0x0040
#define IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY        0x0080
#define IMAGE_DLLCHARACTERISTICS_NX_COMPAT              0x0100
#define IMAGE_DLLCHARACTERISTICS_NO_ISOLATION           0x0200
#define IMAGE_DLLCHARACTERISTICS_NO_SEH                 0x0400
#define IMAGE_DLLCHARACTERISTICS_NO_BIND                0x0800
#define IMAGE_DLLCHARACTERISTICS_APPCONTAINER           0x1000
#define IMAGE_DLLCHARACTERISTICS_WDM_DRIVER             0x2000
#define IMAGE_DLLCHARACTERISTICS_GUARD_CF               0x4000
#define IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE  0x8000

///
/// Length of ShortName.
///
#define EFI_IMAGE_SIZEOF_SHORT_NAME  8

///
/// Section Table. This table immediately follows the optional header.
///
typedef struct {
	UINT8     Name[EFI_IMAGE_SIZEOF_SHORT_NAME];
	union {
		UINT32    PhysicalAddress;
		UINT32    VirtualSize;
	} Misc;
	UINT32    VirtualAddress;
	UINT32    SizeOfRawData;
	UINT32    PointerToRawData;
	UINT32    PointerToRelocations;
	UINT32    PointerToLinenumbers;
	UINT16    NumberOfRelocations;
	UINT16    NumberOfLinenumbers;
	UINT32    Characteristics;
} EFI_IMAGE_SECTION_HEADER;

///
/// Size of EFI_IMAGE_SECTION_HEADER.
///
#define EFI_IMAGE_SIZEOF_SECTION_HEADER  40

//
// Section Flags Values
//
#define EFI_IMAGE_SCN_TYPE_NO_PAD             BIT3        ///< 0x00000008  ///< Reserved.
#define EFI_IMAGE_SCN_CNT_CODE                BIT5        ///< 0x00000020
#define EFI_IMAGE_SCN_CNT_INITIALIZED_DATA    BIT6        ///< 0x00000040
#define EFI_IMAGE_SCN_CNT_UNINITIALIZED_DATA  BIT7        ///< 0x00000080

#define EFI_IMAGE_SCN_LNK_OTHER   BIT8                    ///< 0x00000100  ///< Reserved.
#define EFI_IMAGE_SCN_LNK_INFO    BIT9                    ///< 0x00000200  ///< Section contains comments or some other type of information.
#define EFI_IMAGE_SCN_LNK_REMOVE  BIT11                   ///< 0x00000800  ///< Section contents will not become part of image.
#define EFI_IMAGE_SCN_LNK_COMDAT  BIT12                   ///< 0x00001000

#define EFI_IMAGE_SCN_ALIGN_1BYTES   BIT20                ///< 0x00100000
#define EFI_IMAGE_SCN_ALIGN_2BYTES   BIT21                ///< 0x00200000
#define EFI_IMAGE_SCN_ALIGN_4BYTES   (BIT20|BIT21)        ///< 0x00300000
#define EFI_IMAGE_SCN_ALIGN_8BYTES   BIT22                ///< 0x00400000
#define EFI_IMAGE_SCN_ALIGN_16BYTES  (BIT20|BIT22)        ///< 0x00500000
#define EFI_IMAGE_SCN_ALIGN_32BYTES  (BIT21|BIT22)        ///< 0x00600000
#define EFI_IMAGE_SCN_ALIGN_64BYTES  (BIT20|BIT21|BIT22)  ///< 0x00700000

#define EFI_IMAGE_SCN_MEM_DISCARDABLE  BIT25              ///< 0x02000000
#define EFI_IMAGE_SCN_MEM_NOT_CACHED   BIT26              ///< 0x04000000
#define EFI_IMAGE_SCN_MEM_NOT_PAGED    BIT27              ///< 0x08000000
#define EFI_IMAGE_SCN_MEM_SHARED       BIT28              ///< 0x10000000
#define EFI_IMAGE_SCN_MEM_EXECUTE      BIT29              ///< 0x20000000
#define EFI_IMAGE_SCN_MEM_READ         BIT30              ///< 0x40000000
#define EFI_IMAGE_SCN_MEM_WRITE        BIT31              ///< 0x80000000

///
/// Size of a Symbol Table Record.
///
#define EFI_IMAGE_SIZEOF_SYMBOL  18

//
// Symbols have a section number of the section in which they are
// defined. Otherwise, section numbers have the following meanings:
//
#define EFI_IMAGE_SYM_UNDEFINED  (UINT16) 0  ///< Symbol is undefined or is common.
#define EFI_IMAGE_SYM_ABSOLUTE   (UINT16) -1 ///< Symbol is an absolute value.
#define EFI_IMAGE_SYM_DEBUG      (UINT16) -2 ///< Symbol is a special debug item.

//
// Symbol Type (fundamental) values.
//
#define EFI_IMAGE_SYM_TYPE_NULL    0  ///< no type.
#define EFI_IMAGE_SYM_TYPE_VOID    1  ///< no valid type.
#define EFI_IMAGE_SYM_TYPE_CHAR    2  ///< type character.
#define EFI_IMAGE_SYM_TYPE_SHORT   3  ///< type short integer.
#define EFI_IMAGE_SYM_TYPE_INT     4
#define EFI_IMAGE_SYM_TYPE_LONG    5
#define EFI_IMAGE_SYM_TYPE_FLOAT   6
#define EFI_IMAGE_SYM_TYPE_DOUBLE  7
#define EFI_IMAGE_SYM_TYPE_STRUCT  8
#define EFI_IMAGE_SYM_TYPE_UNION   9
#define EFI_IMAGE_SYM_TYPE_ENUM    10 ///< enumeration.
#define EFI_IMAGE_SYM_TYPE_MOE     11 ///< member of enumeration.
#define EFI_IMAGE_SYM_TYPE_BYTE    12
#define EFI_IMAGE_SYM_TYPE_WORD    13
#define EFI_IMAGE_SYM_TYPE_UINT    14
#define EFI_IMAGE_SYM_TYPE_DWORD   15

//
// Symbol Type (derived) values.
//
#define EFI_IMAGE_SYM_DTYPE_NULL      0 ///< no derived type.
#define EFI_IMAGE_SYM_DTYPE_POINTER   1
#define EFI_IMAGE_SYM_DTYPE_FUNCTION  2
#define EFI_IMAGE_SYM_DTYPE_ARRAY     3

//
// Storage classes.
//
#define EFI_IMAGE_SYM_CLASS_END_OF_FUNCTION   ((UINT8) -1)
#define EFI_IMAGE_SYM_CLASS_NULL              0
#define EFI_IMAGE_SYM_CLASS_AUTOMATIC         1
#define EFI_IMAGE_SYM_CLASS_EXTERNAL          2
#define EFI_IMAGE_SYM_CLASS_STATIC            3
#define EFI_IMAGE_SYM_CLASS_REGISTER          4
#define EFI_IMAGE_SYM_CLASS_EXTERNAL_DEF      5
#define EFI_IMAGE_SYM_CLASS_LABEL             6
#define EFI_IMAGE_SYM_CLASS_UNDEFINED_LABEL   7
#define EFI_IMAGE_SYM_CLASS_MEMBER_OF_STRUCT  8
#define EFI_IMAGE_SYM_CLASS_ARGUMENT          9
#define EFI_IMAGE_SYM_CLASS_STRUCT_TAG        10
#define EFI_IMAGE_SYM_CLASS_MEMBER_OF_UNION   11
#define EFI_IMAGE_SYM_CLASS_UNION_TAG         12
#define EFI_IMAGE_SYM_CLASS_TYPE_DEFINITION   13
#define EFI_IMAGE_SYM_CLASS_UNDEFINED_STATIC  14
#define EFI_IMAGE_SYM_CLASS_ENUM_TAG          15
#define EFI_IMAGE_SYM_CLASS_MEMBER_OF_ENUM    16
#define EFI_IMAGE_SYM_CLASS_REGISTER_PARAM    17
#define EFI_IMAGE_SYM_CLASS_BIT_FIELD         18
#define EFI_IMAGE_SYM_CLASS_BLOCK             100
#define EFI_IMAGE_SYM_CLASS_FUNCTION          101
#define EFI_IMAGE_SYM_CLASS_END_OF_STRUCT     102
#define EFI_IMAGE_SYM_CLASS_FILE              103
#define EFI_IMAGE_SYM_CLASS_SECTION           104
#define EFI_IMAGE_SYM_CLASS_WEAK_EXTERNAL     105

//
// type packing constants
//
#define EFI_IMAGE_N_BTMASK  017
#define EFI_IMAGE_N_TMASK   060
#define EFI_IMAGE_N_TMASK1  0300
#define EFI_IMAGE_N_TMASK2  0360
#define EFI_IMAGE_N_BTSHFT  4
#define EFI_IMAGE_N_TSHIFT  2

//
// Communal selection types.
//
#define EFI_IMAGE_COMDAT_SELECT_NODUPLICATES  1
#define EFI_IMAGE_COMDAT_SELECT_ANY           2
#define EFI_IMAGE_COMDAT_SELECT_SAME_SIZE     3
#define EFI_IMAGE_COMDAT_SELECT_EXACT_MATCH   4
#define EFI_IMAGE_COMDAT_SELECT_ASSOCIATIVE   5

//
// the following values only be referred in PeCoff, not defined in PECOFF.
//
#define EFI_IMAGE_WEAK_EXTERN_SEARCH_NOLIBRARY  1
#define EFI_IMAGE_WEAK_EXTERN_SEARCH_LIBRARY    2
#define EFI_IMAGE_WEAK_EXTERN_SEARCH_ALIAS      3

///
/// Relocation format.
///
typedef struct {
	UINT32    VirtualAddress;
	UINT32    SymbolTableIndex;
	UINT16    Type;
} EFI_IMAGE_RELOCATION;

///
/// Size of EFI_IMAGE_RELOCATION
///
#define EFI_IMAGE_SIZEOF_RELOCATION  10

//
// I386 relocation types.
//
#define EFI_IMAGE_REL_I386_ABSOLUTE  0x0000 ///< Reference is absolute, no relocation is necessary.
#define EFI_IMAGE_REL_I386_DIR16     0x0001 ///< Direct 16-bit reference to the symbols virtual address.
#define EFI_IMAGE_REL_I386_REL16     0x0002 ///< PC-relative 16-bit reference to the symbols virtual address.
#define EFI_IMAGE_REL_I386_DIR32     0x0006 ///< Direct 32-bit reference to the symbols virtual address.
#define EFI_IMAGE_REL_I386_DIR32NB   0x0007 ///< Direct 32-bit reference to the symbols virtual address, base not included.
#define EFI_IMAGE_REL_I386_SEG12     0x0009 ///< Direct 16-bit reference to the segment-selector bits of a 32-bit virtual address.
#define EFI_IMAGE_REL_I386_SECTION   0x000A
#define EFI_IMAGE_REL_I386_SECREL    0x000B
#define EFI_IMAGE_REL_I386_REL32     0x0014 ///< PC-relative 32-bit reference to the symbols virtual address.

//
// x64 processor relocation types.
//
#define IMAGE_REL_AMD64_ABSOLUTE  0x0000
#define IMAGE_REL_AMD64_ADDR64    0x0001
#define IMAGE_REL_AMD64_ADDR32    0x0002
#define IMAGE_REL_AMD64_ADDR32NB  0x0003
#define IMAGE_REL_AMD64_REL32     0x0004
#define IMAGE_REL_AMD64_REL32_1   0x0005
#define IMAGE_REL_AMD64_REL32_2   0x0006
#define IMAGE_REL_AMD64_REL32_3   0x0007
#define IMAGE_REL_AMD64_REL32_4   0x0008
#define IMAGE_REL_AMD64_REL32_5   0x0009
#define IMAGE_REL_AMD64_SECTION   0x000A
#define IMAGE_REL_AMD64_SECREL    0x000B
#define IMAGE_REL_AMD64_SECREL7   0x000C
#define IMAGE_REL_AMD64_TOKEN     0x000D
#define IMAGE_REL_AMD64_SREL32    0x000E
#define IMAGE_REL_AMD64_PAIR      0x000F
#define IMAGE_REL_AMD64_SSPAN32   0x0010

///
/// Based relocation format.
///
typedef struct {
	UINT32    VirtualAddress;
	UINT32    SizeOfBlock;
} EFI_IMAGE_BASE_RELOCATION;

///
/// Size of EFI_IMAGE_BASE_RELOCATION.
///
#define EFI_IMAGE_SIZEOF_BASE_RELOCATION  8

//
// Based relocation types.
//
#define EFI_IMAGE_REL_BASED_ABSOLUTE        0
#define EFI_IMAGE_REL_BASED_HIGH            1
#define EFI_IMAGE_REL_BASED_LOW             2
#define EFI_IMAGE_REL_BASED_HIGHLOW         3
#define EFI_IMAGE_REL_BASED_HIGHADJ         4
#define EFI_IMAGE_REL_BASED_MIPS_JMPADDR    5
#define EFI_IMAGE_REL_BASED_ARM_MOV32A      5
#define EFI_IMAGE_REL_BASED_ARM_MOV32T      7
#define EFI_IMAGE_REL_BASED_IA64_IMM64      9
#define EFI_IMAGE_REL_BASED_MIPS_JMPADDR16  9
#define EFI_IMAGE_REL_BASED_DIR64           10

///
/// Relocation types of RISC-V processor.
///
#define EFI_IMAGE_REL_BASED_RISCV_HI20    5
#define EFI_IMAGE_REL_BASED_RISCV_LOW12I  7
#define EFI_IMAGE_REL_BASED_RISCV_LOW12S  8

//
// Relocation types of LoongArch processor.
//
#define EFI_IMAGE_REL_BASED_LOONGARCH32_MARK_LA  8
#define EFI_IMAGE_REL_BASED_LOONGARCH64_MARK_LA  8

///
/// Line number format.
///
typedef struct {
	union {
		UINT32    SymbolTableIndex; ///< Symbol table index of function name if Linenumber is 0.
		UINT32    VirtualAddress;   ///< Virtual address of line number.
	} Type;
	UINT16    Linenumber;       ///< Line number.
} EFI_IMAGE_LINENUMBER;

///
/// Size of EFI_IMAGE_LINENUMBER.
///
#define EFI_IMAGE_SIZEOF_LINENUMBER  6

//
// Archive format.
//
#define EFI_IMAGE_ARCHIVE_START_SIZE        8
#define EFI_IMAGE_ARCHIVE_START             "!<arch>\n"
#define EFI_IMAGE_ARCHIVE_END               "`\n"
#define EFI_IMAGE_ARCHIVE_PAD               "\n"
#define EFI_IMAGE_ARCHIVE_LINKER_MEMBER     "/               "
#define EFI_IMAGE_ARCHIVE_LONGNAMES_MEMBER  "//              "

///
/// Archive Member Headers
///
typedef struct {
	UINT8    Name[16];     ///< File member name - `/' terminated.
	UINT8    Date[12];     ///< File member date - decimal.
	UINT8    UserID[6];    ///< File member user id - decimal.
	UINT8    GroupID[6];   ///< File member group id - decimal.
	UINT8    Mode[8];      ///< File member mode - octal.
	UINT8    Size[10];     ///< File member size - decimal.
	UINT8    EndHeader[2]; ///< String to end header. (0x60 0x0A).
} EFI_IMAGE_ARCHIVE_MEMBER_HEADER;

///
/// Size of EFI_IMAGE_ARCHIVE_MEMBER_HEADER.
///
#define EFI_IMAGE_SIZEOF_ARCHIVE_MEMBER_HDR  60

//
// DLL Support
//

///
/// Export Directory Table.
///
typedef struct {
	UINT32    Characteristics;
	UINT32    TimeDateStamp;
	UINT16    MajorVersion;
	UINT16    MinorVersion;
	UINT32    Name;
	UINT32    Base;
	UINT32    NumberOfFunctions;
	UINT32    NumberOfNames;
	UINT32    AddressOfFunctions;
	UINT32    AddressOfNames;
	UINT32    AddressOfNameOrdinals;
} EFI_IMAGE_EXPORT_DIRECTORY;

//
// Based export types.
//
#define EFI_IMAGE_EXPORT_ORDINAL_BASE  1
#define EFI_IMAGE_EXPORT_ADDR_SIZE     4
#define EFI_IMAGE_EXPORT_ORDINAL_SIZE  2

///
/// Hint/Name Table.
///
typedef struct {
	UINT16    Hint;
	UINT8     Name[1];
} EFI_IMAGE_IMPORT_BY_NAME;

///
/// Import Address Table RVA (Thunk Table).
///
typedef struct {
	union {
		UINT32                      Function;
		UINT32                      Ordinal;
		EFI_IMAGE_IMPORT_BY_NAME* AddressOfData;
	} u1;
} EFI_IMAGE_THUNK_DATA;

#define EFI_IMAGE_ORDINAL_FLAG  BIT31                ///< Flag for PE32.
#define EFI_IMAGE_SNAP_BY_ORDINAL(Ordinal)  ((Ordinal & EFI_IMAGE_ORDINAL_FLAG) != 0)
#define EFI_IMAGE_ORDINAL(Ordinal)          (Ordinal & 0xffff)

///
/// Import Directory Table
///
typedef struct {
	UINT32                  Characteristics;
	UINT32                  TimeDateStamp;
	UINT32                  ForwarderChain;
	UINT32                  Name;
	EFI_IMAGE_THUNK_DATA* FirstThunk;
} EFI_IMAGE_IMPORT_DESCRIPTOR;

///
/// Debug Directory Format.
///
typedef struct {
	UINT32    Characteristics;
	UINT32    TimeDateStamp;
	UINT16    MajorVersion;
	UINT16    MinorVersion;
	UINT32    Type;
	UINT32    SizeOfData;
	UINT32    RVA;         ///< The address of the debug data when loaded, relative to the image base.
	UINT32    FileOffset;  ///< The file pointer to the debug data.
} EFI_IMAGE_DEBUG_DIRECTORY_ENTRY;

#define EFI_IMAGE_DEBUG_TYPE_CODEVIEW               2    ///< The Visual C++ debug information.
#define EFI_IMAGE_DEBUG_TYPE_EX_DLLCHARACTERISTICS  20

///
/// Debug Data Structure defined in Microsoft C++.
///
#define CODEVIEW_SIGNATURE_NB10  SIGNATURE_32('N', 'B', '1', '0')
typedef struct {
	UINT32    Signature;                      ///< "NB10"
	UINT32    Unknown;
	UINT32    Unknown2;
	UINT32    Unknown3;
	//
	// Filename of .PDB goes here
	//
} EFI_IMAGE_DEBUG_CODEVIEW_NB10_ENTRY;

///
/// Debug Data Structure defined in Microsoft C++.
///
#define CODEVIEW_SIGNATURE_RSDS  SIGNATURE_32('R', 'S', 'D', 'S')
typedef struct {
	UINT32    Signature;                      ///< "RSDS".
	UINT32    Unknown;
	UINT32    Unknown2;
	UINT32    Unknown3;
	UINT32    Unknown4;
	UINT32    Unknown5;
	//
	// Filename of .PDB goes here
	//
} EFI_IMAGE_DEBUG_CODEVIEW_RSDS_ENTRY;

///
/// Debug Data Structure defined by Apple Mach-O to Coff utility.
///
#define CODEVIEW_SIGNATURE_MTOC  SIGNATURE_32('M', 'T', 'O', 'C')
typedef struct {
	UINT32    Signature;                       ///< "MTOC".
	EFI_GUID      MachOUuid;
	//
	//  Filename of .DLL (Mach-O with debug info) goes here
	//
} EFI_IMAGE_DEBUG_CODEVIEW_MTOC_ENTRY;

//
// .pdata entries for X64
//
typedef struct {
	UINT32    FunctionStartAddress;
	UINT32    FunctionEndAddress;
	UINT32    UnwindInfoAddress;
} RUNTIME_FUNCTION;

typedef struct {
	UINT8    Version : 3;
	UINT8    Flags : 5;
	UINT8    SizeOfProlog;
	UINT8    CountOfUnwindCodes;
	UINT8    FrameRegister : 4;
	UINT8    FrameRegisterOffset : 4;
} UNWIND_INFO;

///
/// Extended DLL Characteristics
///
#define EFI_IMAGE_DLLCHARACTERISTICS_EX_CET_COMPAT          0x0001
#define EFI_IMAGE_DLLCHARACTERISTICS_EX_FORWARD_CFI_COMPAT  0x0040

typedef struct {
	UINT32    DllCharacteristicsEx;
} EFI_IMAGE_DEBUG_EX_DLLCHARACTERISTICS_ENTRY;

///
/// Resource format.
///
typedef struct {
	UINT32    Characteristics;
	UINT32    TimeDateStamp;
	UINT16    MajorVersion;
	UINT16    MinorVersion;
	UINT16    NumberOfNamedEntries;
	UINT16    NumberOfIdEntries;
	//
	// Array of EFI_IMAGE_RESOURCE_DIRECTORY_ENTRY entries goes here.
	//
} EFI_IMAGE_RESOURCE_DIRECTORY;

///
/// Resource directory entry format.
///
typedef struct {
	union {
		struct {
			UINT32    NameOffset : 31;
			UINT32    NameIsString : 1;
		} s;
		UINT32    Id;
	} u1;
	union {
		UINT32    OffsetToData;
		struct {
			UINT32    OffsetToDirectory : 31;
			UINT32    DataIsDirectory : 1;
		} s;
	} u2;
} EFI_IMAGE_RESOURCE_DIRECTORY_ENTRY;

///
/// Resource directory entry for string.
///
typedef struct {
	UINT16    Length;
	CHAR16    String[1];
} EFI_IMAGE_RESOURCE_DIRECTORY_STRING;

///
/// Resource directory entry for data array.
///
typedef struct {
	UINT32    OffsetToData;
	UINT32    Size;
	UINT32    CodePage;
	UINT32    Reserved;
} EFI_IMAGE_RESOURCE_DATA_ENTRY;

///
/// Header format for TE images, defined in the PI Specification, 1.0.
///
typedef struct {
	UINT16                      Signature;           ///< The signature for TE format = "VZ".
	UINT16                      Machine;             ///< From the original file header.
	UINT8                       NumberOfSections;    ///< From the original file header.
	UINT8                       Subsystem;           ///< From original optional header.
	UINT16                      StrippedSize;        ///< Number of bytes we removed from the header.
	UINT32                      AddressOfEntryPoint; ///< Offset to entry point -- from original optional header.
	UINT32                      BaseOfCode;          ///< From original image -- required for ITP debug.
	UINT64                      ImageBase;           ///< From original file header.
	EFI_IMAGE_DATA_DIRECTORY    DataDirectory[2];    ///< Only base relocation and debug directory.
} EFI_TE_IMAGE_HEADER;

#define EFI_TE_IMAGE_HEADER_SIGNATURE  SIGNATURE_16('V', 'Z')

//
// Data directory indexes in our TE image header
//
#define EFI_TE_IMAGE_DIRECTORY_ENTRY_BASERELOC  0
#define EFI_TE_IMAGE_DIRECTORY_ENTRY_DEBUG      1

///
/// Union of PE32, PE32+, and TE headers.
///
typedef union {
	EFI_IMAGE_NT_HEADERS32    Pe32;
	EFI_IMAGE_NT_HEADERS64    Pe32Plus;
	EFI_TE_IMAGE_HEADER       Te;
} EFI_IMAGE_OPTIONAL_HEADER_UNION;

typedef union {
	EFI_IMAGE_NT_HEADERS32* Pe32;
	EFI_IMAGE_NT_HEADERS64* Pe32Plus;
	EFI_TE_IMAGE_HEADER* Te;
	EFI_IMAGE_OPTIONAL_HEADER_UNION* Union;
} EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION;

extern PEFI_SYSTEM_TABLE gST;
extern EFI_HANDLE gImageHandle;
extern PEFI_BOOT_SERVICES gBS;
extern PEFI_SYSTEM_TABLE gST;
extern PEFI_RUNTIME_SERVICES gRT;

typedef struct _LIST_ENTRY {
	struct _LIST_ENTRY *Flink;
	struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;

typedef struct _FV_FILEPATH_DEVICE_PATH_LOCAL
{
	EFI_DEVICE_PATH_PROTOCOL Header;
	EFI_GUID NameGuid;
} FV_FILEPATH_DEVICE_PATH_LOCAL, * PFV_FILEPATH_DEVICE_PATH_LOCAL;

static constexpr UINT8 MEDIA_FW_VOL_FILEPATH_DP_LOCAL = 0x06;


typedef struct _IMAGE_SECTION_HEADER {
	uint8_t  Name[8]; // IMAGE_SIZEOF_SHORT_NAME = 8

	union {
		uint32_t PhysicalAddress;
		uint32_t VirtualSize;
	} Misc;

	uint32_t VirtualAddress;
	uint32_t SizeOfRawData;
	uint32_t PointerToRawData;
	uint32_t PointerToRelocations;
	uint32_t PointerToLinenumbers;
	uint16_t NumberOfRelocations;
	uint16_t NumberOfLinenumbers;
	uint32_t Characteristics;

} IMAGE_SECTION_HEADER, * PIMAGE_SECTION_HEADER;

typedef struct _IMAGE_DOS_HEADER {
    uint16_t e_magic;      // Magic number ("MZ" = 0x5A4D)
    uint16_t e_cblp;       // Bytes on last page of file
    uint16_t e_cp;         // Pages in file
    uint16_t e_crlc;       // Relocations
    uint16_t e_cparhdr;    // Size of header in paragraphs
    uint16_t e_minalloc;   // Minimum extra paragraphs needed
    uint16_t e_maxalloc;   // Maximum extra paragraphs needed
    uint16_t e_ss;         // Initial (relative) SS value
    uint16_t e_sp;         // Initial SP value
    uint16_t e_csum;       // Checksum
    uint16_t e_ip;         // Initial IP value
    uint16_t e_cs;         // Initial (relative) CS value
    uint16_t e_lfarlc;     // File address of relocation table
    uint16_t e_ovno;       // Overlay number
    uint16_t e_res[4];     // Reserved words
    uint16_t e_oemid;      // OEM identifier (for e_oeminfo)
    uint16_t e_oeminfo;    // OEM information
    uint16_t e_res2[10];   // Reserved words
    uint32_t e_lfanew;     // File offset to NT headers

} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _IMAGE_FILE_HEADER {
	uint16_t Machine;
	uint16_t NumberOfSections;
	uint32_t TimeDateStamp;
	uint32_t PointerToSymbolTable;
	uint32_t NumberOfSymbols;
	uint16_t SizeOfOptionalHeader;
	uint16_t Characteristics;
} IMAGE_FILE_HEADER, * PIMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY {
	uint32_t VirtualAddress;
	uint32_t Size;
} IMAGE_DATA_DIRECTORY, * PIMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_OPTIONAL_HEADER64 {
	uint16_t Magic;
	uint8_t  MajorLinkerVersion;
	uint8_t  MinorLinkerVersion;
	uint32_t SizeOfCode;
	uint32_t SizeOfInitializedData;
	uint32_t SizeOfUninitializedData;
	uint32_t AddressOfEntryPoint;
	uint32_t BaseOfCode;
	uint64_t ImageBase;
	uint32_t SectionAlignment;
	uint32_t FileAlignment;
	uint16_t MajorOperatingSystemVersion;
	uint16_t MinorOperatingSystemVersion;
	uint16_t MajorImageVersion;
	uint16_t MinorImageVersion;
	uint16_t MajorSubsystemVersion;
	uint16_t MinorSubsystemVersion;
	uint32_t Win32VersionValue;
	uint32_t SizeOfImage;
	uint32_t SizeOfHeaders;
	uint32_t CheckSum;
	uint16_t Subsystem;
	uint16_t DllCharacteristics;
	uint64_t SizeOfStackReserve;
	uint64_t SizeOfStackCommit;
	uint64_t SizeOfHeapReserve;
	uint64_t SizeOfHeapCommit;
	uint32_t LoaderFlags;
	uint32_t NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[16];
} IMAGE_OPTIONAL_HEADER64, * PIMAGE_OPTIONAL_HEADER64;

typedef struct _IMAGE_NT_HEADERS64 {
	uint32_t Signature;
	IMAGE_FILE_HEADER FileHeader;
	IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS64, * PIMAGE_NT_HEADERS64;

constexpr CEFI_GUID gEfiHttpServiceBindingProtocolGuid = { 0xbdc8e6af, 0xd9bc, 0x4379, {0xa7, 0x2a, 0xe0, 0xc4, 0xe7, 0x5d, 0xae, 0x1c } };
constexpr CEFI_GUID gEfiHttpProtocolGuid = { 0x7a59b29b, 0x910b, 0x4171, {0x82, 0x42, 0xa8, 0x5a, 0x0d, 0xf2, 0x5b, 0x5b } };

typedef struct _EFI_HTTP_PROTOCOL EFI_HTTP_PROTOCOL, *PEFI_HTTP_PROTOCOL;

typedef enum _EFI_HTTP_VERSION
{
	HttpVersion10,
	HttpVersion11,
	HttpVersionUnsupported
} EFI_HTTP_VERSION, *PEFI_HTTP_VERSION;

typedef enum _EFI_HTTP_METHOD
{
	HttpMethodGet,
	HttpMethodPost,
	HttpMethodPatch,
	HttpMethodOptions,
	HttpMethodConnect,
	HttpMethodHead,
	HttpMethodPut,
	HttpMethodDelete,
	HttpMethodTrace,
	HttpMethodMax
} EFI_HTTP_METHOD, *PEFI_HTTP_METHOD;

typedef enum _EFI_HTTP_STATUS_CODE 
{
	HTTP_STATUS_UNSUPPORTED_STATUS = 0,
	HTTP_STATUS_100_CONTINUE,
	HTTP_STATUS_101_SWITCHING_PROTOCOLS,
	HTTP_STATUS_200_OK,
	HTTP_STATUS_201_CREATED,
	HTTP_STATUS_202_ACCEPTED,
	HTTP_STATUS_203_NON_AUTHORITATIVE_INFORMATION,
	HTTP_STATUS_204_NO_CONTENT,
	HTTP_STATUS_205_RESET_CONTENT,
	HTTP_STATUS_206_PARTIAL_CONTENT,
	HTTP_STATUS_300_MULTIPLE_CHOICES,
	HTTP_STATUS_301_MOVED_PERMANENTLY,
	HTTP_STATUS_302_FOUND,
	HTTP_STATUS_303_SEE_OTHER,
	HTTP_STATUS_304_NOT_MODIFIED,
	HTTP_STATUS_305_USE_PROXY,
	HTTP_STATUS_307_TEMPORARY_REDIRECT,
	HTTP_STATUS_400_BAD_REQUEST,
	HTTP_STATUS_401_UNAUTHORIZED,
	HTTP_STATUS_402_PAYMENT_REQUIRED,
	HTTP_STATUS_403_FORBIDDEN,
	HTTP_STATUS_404_NOT_FOUND,
	HTTP_STATUS_405_METHOD_NOT_ALLOWED,
	HTTP_STATUS_406_NOT_ACCEPTABLE,
	HTTP_STATUS_407_PROXY_AUTHENTICATION_REQUIRED,
	HTTP_STATUS_408_REQUEST_TIME_OUT,
	HTTP_STATUS_409_CONFLICT,
	HTTP_STATUS_410_GONE,
	HTTP_STATUS_411_LENGTH_REQUIRED,
	HTTP_STATUS_412_PRECONDITION_FAILED,
	HTTP_STATUS_413_REQUEST_ENTITY_TOO_LARGE,
	HTTP_STATUS_414_REQUEST_URI_TOO_LARGE,
	HTTP_STATUS_415_UNSUPPORTED_MEDIA_TYPE,
	HTTP_STATUS_416_REQUESTED_RANGE_NOT_SATISFIED,
	HTTP_STATUS_417_EXPECTATION_FAILED,
	HTTP_STATUS_500_INTERNAL_SERVER_ERROR,
	HTTP_STATUS_501_NOT_IMPLEMENTED,
	HTTP_STATUS_502_BAD_GATEWAY,
	HTTP_STATUS_503_SERVICE_UNAVAILABLE,
	HTTP_STATUS_504_GATEWAY_TIME_OUT,
	HTTP_STATUS_505_HTTP_VERSION_NOT_SUPPORTED,
	HTTP_STATUS_308_PERMANENT_REDIRECT
} EFI_HTTP_STATUS_CODE, *PEFI_HTTP_STATUS_CODE;

typedef struct _EFI_HTTPv4_ACCESS_POINT 
{
	bool UseDefaultAddress;
	EFI_IPv4_ADDRESS LocalAddress;
	EFI_IPv4_ADDRESS LocalSubnet;
	uint16_t LocalPort;
} EFI_HTTPv4_ACCESS_POINT, *PEFI_HTTPv4_ACCESS_POINT;

typedef struct _EFI_HTTPv6_ACCESS_POINT 
{
	EFI_IPv6_ADDRESS LocalAddress;
	uint16_t LocalPort;
} EFI_HTTPv6_ACCESS_POINT, *PEFI_HTTPv6_ACCESS_POINT;

typedef struct _EFI_HTTP_CONFIG_DATA
{
	EFI_HTTP_VERSION HttpVersion;
	uint32_t TimeOutMillisec;
	bool LocalAddressIsIPv6;
	union
	{
		PEFI_HTTPv4_ACCESS_POINT IPv4Node;
		PEFI_HTTPv6_ACCESS_POINT IPv6Node;
	} AccessPoint;
} EFI_HTTP_CONFIG_DATA, *PEFI_HTTP_CONFIG_DATA;

typedef struct _EFI_HTTP_REQUEST_DATA
{
	EFI_HTTP_METHOD Method;
	wchar_t* Url;
} EFI_HTTP_REQUEST_DATA, *PEFI_HTTP_REQUEST_DATA;

typedef struct _EFI_HTTP_RESPONSE_DATA
{
	EFI_HTTP_STATUS_CODE StatusCode;
} EFI_HTTP_RESPONSE_DATA, *PEFI_HTTP_RESPONSE_DATA;

typedef struct _EFI_HTTP_HEADER
{
	char* FieldName;
	char* FieldValue;
} EFI_HTTP_HEADER ,*PEFI_HTTP_HEADER;

typedef struct _EFI_HTTP_MESSAGE
{
	union
	{
		PEFI_HTTP_REQUEST_DATA Request;
		PEFI_HTTP_RESPONSE_DATA Response;
	} Data;
	uint64_t HeaderCount;
	PEFI_HTTP_HEADER Headers;
	uint64_t BodyLength;
	void* Body;
} EFI_HTTP_MESSAGE, *PEFI_HTTP_MESSAGE;

typedef struct _EFI_HTTP_TOKEN
{
	EFI_EVENT Event;
	EFI_STATUS Status;
	PEFI_HTTP_MESSAGE Message;
} EFI_HTTP_TOKEN, *PEFI_HTTP_TOKEN;

using EfiHttpGetModeDataFn = EFI_STATUS(__cdecl)(PEFI_HTTP_PROTOCOL This, PEFI_HTTP_CONFIG_DATA HttpConfigData);
using EfiHttpConfigureFn = EFI_STATUS(__cdecl)(PEFI_HTTP_PROTOCOL This, PEFI_HTTP_CONFIG_DATA HttpConfigData);
using EfiHttpRequestFn = EFI_STATUS(__cdecl)(PEFI_HTTP_PROTOCOL This, PEFI_HTTP_TOKEN Token);
using EfiHttpCancelFn = EFI_STATUS(__cdecl)(PEFI_HTTP_PROTOCOL This, PEFI_HTTP_TOKEN Token);
using EfiHttpResponseFn = EFI_STATUS(__cdecl)(PEFI_HTTP_PROTOCOL This, PEFI_HTTP_TOKEN Token);
using EfiHttpPollFn = EFI_STATUS(__cdecl)(PEFI_HTTP_PROTOCOL This);

struct _EFI_HTTP_PROTOCOL
{
	EfiHttpGetModeDataFn* GetModeData;
	EfiHttpConfigureFn* Configure;
	EfiHttpRequestFn* Request;
	EfiHttpCancelFn* Cancel;
	EfiHttpResponseFn* Response;
	EfiHttpPollFn* Poll;
};

typedef struct _EFI_SERVICE_BINDING_PROTOCOL EFI_SERVICE_BINDING_PROTOCOL, *PEFI_SERVICE_BINDING_PROTOCOL;

using EfiServiceBindingCreateChildFn = EFI_STATUS(__cdecl)(PEFI_SERVICE_BINDING_PROTOCOL This, PEFI_HANDLE ChildHandle);
using EfiServiceBindingDestroyChildFn = EFI_STATUS(__cdecl)(PEFI_SERVICE_BINDING_PROTOCOL This, EFI_HANDLE ChildHandle);

struct _EFI_SERVICE_BINDING_PROTOCOL
{
	EfiServiceBindingCreateChildFn* CreateChild;
	EfiServiceBindingDestroyChildFn* DestroyChild;
};

#define EVT_TIMER          0x80000000
#define EVT_RUNTIME        0x40000000
#define EVT_NOTIFY_WAIT    0x00000100
#define EVT_NOTIFY_SIGNAL  0x00000200

#define EVT_SIGNAL_EXIT_BOOT_SERVICES      0x00000201
#define EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE  0x60000202

#define TPL_APPLICATION  4
#define TPL_CALLBACK     8
#define TPL_NOTIFY       16
#define TPL_HIGH_LEVEL   31

#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL   0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL         0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL        0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER  0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER            0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE            0x00000020

typedef struct _EFI_TCP4_PROTOCOL EFI_TCP4_PROTOCOL, *PEFI_TCP4_PROTOCOL;

typedef struct _EFI_TCP4_SERVICE_POINT {
	EFI_HANDLE          InstanceHandle;
	EFI_IPv4_ADDRESS    LocalAddress;
	UINT16              LocalPort;
	EFI_IPv4_ADDRESS    RemoteAddress;
	UINT16              RemotePort;
} EFI_TCP4_SERVICE_POINT, *PEFI_TCP4_SERVICE_POINT;

typedef struct _EFI_TCP4_VARIABLE_DATA {
	EFI_HANDLE                DriverHandle;
	UINT32                    ServiceCount;
	EFI_TCP4_SERVICE_POINT    Services[1];
} EFI_TCP4_VARIABLE_DATA, *PEFI_TCP4_VARIABLE_DATA;

typedef struct _EFI_TCP4_ACCESS_POINT  {
	BOOLEAN             UseDefaultAddress;
	EFI_IPv4_ADDRESS    StationAddress;
	EFI_IPv4_ADDRESS    SubnetMask;
	UINT16              StationPort;
	EFI_IPv4_ADDRESS    RemoteAddress;
	UINT16              RemotePort;
	BOOLEAN             ActiveFlag;
} EFI_TCP4_ACCESS_POINT, *PEFI_TCP4_ACCESS_POINT;

typedef struct _EFI_TCP4_OPTION {
	UINT32     ReceiveBufferSize;
	UINT32     SendBufferSize;
	UINT32     MaxSynBackLog;
	UINT32     ConnectionTimeout;
	UINT32     DataRetries;
	UINT32     FinTimeout;
	UINT32     TimeWaitTimeout;
	UINT32     KeepAliveProbes;
	UINT32     KeepAliveTime;
	UINT32     KeepAliveInterval;
	BOOLEAN    EnableNagle;
	BOOLEAN    EnableTimeStamp;
	BOOLEAN    EnableWindowScaling;
	BOOLEAN    EnableSelectiveAck;
	BOOLEAN    EnablePathMtuDiscovery;
} EFI_TCP4_OPTION, *PEFI_TCP4_OPTION;

typedef struct _EFI_TCP4_CONFIG_DATA {
	//
	// I/O parameters
	//
	UINT8                    TypeOfService;
	UINT8                    TimeToLive;

	//
	// Access Point
	//
	EFI_TCP4_ACCESS_POINT    AccessPoint;

	//
	// TCP Control Options
	//
	EFI_TCP4_OPTION* ControlOption;
} EFI_TCP4_CONFIG_DATA, *PEFI_TCP4_CONFIG_DATA;

typedef enum _EFI_TCP4_CONNECTION_STATE {
	Tcp4StateClosed = 0,
	Tcp4StateListen = 1,
	Tcp4StateSynSent = 2,
	Tcp4StateSynReceived = 3,
	Tcp4StateEstablished = 4,
	Tcp4StateFinWait1 = 5,
	Tcp4StateFinWait2 = 6,
	Tcp4StateClosing = 7,
	Tcp4StateTimeWait = 8,
	Tcp4StateCloseWait = 9,
	Tcp4StateLastAck = 10
} EFI_TCP4_CONNECTION_STATE, *PEFI_TCP4_CONNECTION_STATE;

typedef struct _EFI_TCP4_COMPLETION_TOKEN {
	EFI_EVENT     Event;
	EFI_STATUS    Status;
} EFI_TCP4_COMPLETION_TOKEN, *PEFI_TCP4_COMPLETION_TOKEN;

typedef struct _EFI_TCP4_CONNECTION_TOKEN {
	///
	/// The Status in the CompletionToken will be set to one of
	/// the following values if the active open succeeds or an unexpected
	/// error happens:
	/// EFI_SUCCESS:              The active open succeeds and the instance's
	///                           state is Tcp4StateEstablished.
	/// EFI_CONNECTION_RESET:     The connect fails because the connection is reset
	///                           either by instance itself or the communication peer.
	/// EFI_CONNECTION_REFUSED:   The connect fails because this connection is initiated with
	///                           an active open and the connection is refused.
	/// EFI_ABORTED:              The active open is aborted.
	/// EFI_TIMEOUT:              The connection establishment timer expires and
	///                           no more specific information is available.
	/// EFI_NETWORK_UNREACHABLE:  The active open fails because
	///                           an ICMP network unreachable error is received.
	/// EFI_HOST_UNREACHABLE:     The active open fails because an
	///                           ICMP host unreachable error is received.
	/// EFI_PROTOCOL_UNREACHABLE: The active open fails
	///                           because an ICMP protocol unreachable error is received.
	/// EFI_PORT_UNREACHABLE:     The connection establishment
	///                           timer times out and an ICMP port unreachable error is received.
	/// EFI_ICMP_ERROR:           The connection establishment timer timeout and some other ICMP
	///                           error is received.
	/// EFI_DEVICE_ERROR:         An unexpected system or network error occurred.
	/// EFI_NO_MEDIA:             There was a media error.
	///
	EFI_TCP4_COMPLETION_TOKEN    CompletionToken;
} EFI_TCP4_CONNECTION_TOKEN, *PEFI_TCP4_CONNECTION_TOKEN;

typedef struct _EFI_TCP4_LISTEN_TOKEN {
	EFI_TCP4_COMPLETION_TOKEN    CompletionToken;
	EFI_HANDLE                   NewChildHandle;
} EFI_TCP4_LISTEN_TOKEN, *PEFI_TCP4_LISTEN_TOKEN;

typedef struct _EFI_TCP4_FRAGMENT_DATA {
	UINT32    FragmentLength;
	VOID* FragmentBuffer;
} EFI_TCP4_FRAGMENT_DATA, *PEFI_TCP4_FRAGMENT_DATA;

typedef struct _EFI_TCP4_RECEIVE_DATA {
	BOOLEAN                   UrgentFlag;
	UINT32                    DataLength;
	UINT32                    FragmentCount;
	EFI_TCP4_FRAGMENT_DATA    FragmentTable[1];
} EFI_TCP4_RECEIVE_DATA, *PEFI_TCP4_RECEIVE_DATA;

typedef struct _EFI_TCP4_TRANSMIT_DATA {
	BOOLEAN                   Push;
	BOOLEAN                   Urgent;
	UINT32                    DataLength;
	UINT32                    FragmentCount;
	EFI_TCP4_FRAGMENT_DATA    FragmentTable[1];
} EFI_TCP4_TRANSMIT_DATA, *PEFI_TCP4_TRANSMIT_DATA;

typedef struct _EFI_TCP4_IO_TOKEN {
	///
	/// When transmission finishes or meets any unexpected error it will
	/// be set to one of the following values:
	/// EFI_SUCCESS:              The receiving or transmission operation
	///                           completes successfully.
	/// EFI_CONNECTION_FIN:       The receiving operation fails because the communication peer
	///                           has closed the connection and there is no more data in the
	///                           receive buffer of the instance.
	/// EFI_CONNECTION_RESET:     The receiving or transmission operation fails
	///                           because this connection is reset either by instance
	///                           itself or the communication peer.
	/// EFI_ABORTED:              The receiving or transmission is aborted.
	/// EFI_TIMEOUT:              The transmission timer expires and no more
	///                           specific information is available.
	/// EFI_NETWORK_UNREACHABLE:  The transmission fails
	///                           because an ICMP network unreachable error is received.
	/// EFI_HOST_UNREACHABLE:     The transmission fails because an
	///                           ICMP host unreachable error is received.
	/// EFI_PROTOCOL_UNREACHABLE: The transmission fails
	///                           because an ICMP protocol unreachable error is received.
	/// EFI_PORT_UNREACHABLE:     The transmission fails and an
	///                           ICMP port unreachable error is received.
	/// EFI_ICMP_ERROR:           The transmission fails and some other
	///                           ICMP error is received.
	/// EFI_DEVICE_ERROR:         An unexpected system or network error occurs.
	/// EFI_NO_MEDIA:             There was a media error.
	///
	EFI_TCP4_COMPLETION_TOKEN    CompletionToken;
	union {
		///
		/// When this token is used for receiving, RxData is a pointer to EFI_TCP4_RECEIVE_DATA.
		///
		EFI_TCP4_RECEIVE_DATA* RxData;
		///
		/// When this token is used for transmitting, TxData is a pointer to EFI_TCP4_TRANSMIT_DATA.
		///
		EFI_TCP4_TRANSMIT_DATA* TxData;
	} Packet;
} EFI_TCP4_IO_TOKEN, *PEFI_TCP4_IO_TOKEN;

typedef struct _EFI_TCP4_CLOSE_TOKEN {
	EFI_TCP4_COMPLETION_TOKEN    CompletionToken;
	BOOLEAN                      AbortOnClose;
} EFI_TCP4_CLOSE_TOKEN, *PEFI_TCP4_CLOSE_TOKEN;

typedef struct _EFI_IP4_CONFIG_DATA  {
	///
	/// The default IPv4 protocol packets to send and receive. Ignored
	/// when AcceptPromiscuous is TRUE.
	///
	UINT8               DefaultProtocol;
	///
	/// Set to TRUE to receive all IPv4 packets that get through the receive filters.
	/// Set to FALSE to receive only the DefaultProtocol IPv4
	/// packets that get through the receive filters.
	///
	BOOLEAN             AcceptAnyProtocol;
	///
	/// Set to TRUE to receive ICMP error report packets. Ignored when
	/// AcceptPromiscuous or AcceptAnyProtocol is TRUE.
	///
	BOOLEAN             AcceptIcmpErrors;
	///
	/// Set to TRUE to receive broadcast IPv4 packets. Ignored when
	/// AcceptPromiscuous is TRUE.
	/// Set to FALSE to stop receiving broadcast IPv4 packets.
	///
	BOOLEAN             AcceptBroadcast;
	///
	/// Set to TRUE to receive all IPv4 packets that are sent to any
	/// hardware address or any protocol address.
	/// Set to FALSE to stop receiving all promiscuous IPv4 packets
	///
	BOOLEAN             AcceptPromiscuous;
	///
	/// Set to TRUE to use the default IPv4 address and default routing table.
	///
	BOOLEAN             UseDefaultAddress;
	///
	/// The station IPv4 address that will be assigned to this EFI IPv4Protocol instance.
	///
	EFI_IPv4_ADDRESS    StationAddress;
	///
	/// The subnet address mask that is associated with the station address.
	///
	EFI_IPv4_ADDRESS    SubnetMask;
	///
	/// TypeOfService field in transmitted IPv4 packets.
	///
	UINT8               TypeOfService;
	///
	/// TimeToLive field in transmitted IPv4 packets.
	///
	UINT8               TimeToLive;
	///
	/// State of the DoNotFragment bit in transmitted IPv4 packets.
	///
	BOOLEAN             DoNotFragment;
	///
	/// Set to TRUE to send and receive unformatted packets. The other
	/// IPv4 receive filters are still applied. Fragmentation is disabled for RawData mode.
	///
	BOOLEAN             RawData;
	///
	/// The timer timeout value (number of microseconds) for the
	/// receive timeout event to be associated with each assembled
	/// packet. Zero means do not drop assembled packets.
	///
	UINT32              ReceiveTimeout;
	///
	/// The timer timeout value (number of microseconds) for the
	/// transmit timeout event to be associated with each outgoing
	/// packet. Zero means do not drop outgoing packets.
	///
	UINT32              TransmitTimeout;
} EFI_IP4_CONFIG_DATA, *PEFI_IP4_CONFIG_DATA;

typedef struct _EFI_IP4_ROUTE_TABLE {
	EFI_IPv4_ADDRESS    SubnetAddress;
	EFI_IPv4_ADDRESS    SubnetMask;
	EFI_IPv4_ADDRESS    GatewayAddress;
} EFI_IP4_ROUTE_TABLE, *PEFI_IP4_ROUTE_TABLE;

typedef struct _EFI_IP4_ICMP_TYPE {
	UINT8    Type;
	UINT8    Code;
} EFI_IP4_ICMP_TYPE, *PEFI_IP4_ICMP_TYPE;

typedef struct _EFI_IP4_MODE_DATA {
	///
	/// Set to TRUE after this EFI IPv4 Protocol instance has been successfully configured.
	///
	BOOLEAN                IsStarted;
	///
	/// The maximum packet size, in bytes, of the packet which the upper layer driver could feed.
	///
	UINT32                 MaxPacketSize;
	///
	/// Current configuration settings.
	///
	EFI_IP4_CONFIG_DATA    ConfigData;
	///
	/// Set to TRUE when the EFI IPv4 Protocol instance has a station address and subnet mask.
	///
	BOOLEAN                IsConfigured;
	///
	/// Number of joined multicast groups.
	///
	UINT32                 GroupCount;
	///
	/// List of joined multicast group addresses.
	///
	EFI_IPv4_ADDRESS* GroupTable;
	///
	/// Number of entries in the routing table.
	///
	UINT32                 RouteCount;
	///
	/// Routing table entries.
	///
	EFI_IP4_ROUTE_TABLE* RouteTable;
	///
	/// Number of entries in the supported ICMP types list.
	///
	UINT32                 IcmpTypeCount;
	///
	/// Array of ICMP types and codes that are supported by this EFI IPv4 Protocol driver
	///
	EFI_IP4_ICMP_TYPE* IcmpTypeList;
} EFI_IP4_MODE_DATA, *PEFI_IP4_MODE_DATA;

typedef struct _EFI_MANAGED_NETWORK_CONFIG_DATA {
	///
	/// Timeout value for a UEFI one-shot timer event. A packet that has not been removed
	/// from the MNP receive queue will be dropped if its receive timeout expires.
	///
	UINT32     ReceivedQueueTimeoutValue;
	///
	/// Timeout value for a UEFI one-shot timer event. A packet that has not been removed
	/// from the MNP transmit queue will be dropped if its receive timeout expires.
	///
	UINT32     TransmitQueueTimeoutValue;
	///
	/// Ethernet type II 16-bit protocol type in host byte order. Valid
	/// values are zero and 1,500 to 65,535.
	///
	UINT16     ProtocolTypeFilter;
	///
	/// Set to TRUE to receive packets that are sent to the network
	/// device MAC address. The startup default value is FALSE.
	///
	BOOLEAN    EnableUnicastReceive;
	///
	/// Set to TRUE to receive packets that are sent to any of the
	/// active multicast groups. The startup default value is FALSE.
	///
	BOOLEAN    EnableMulticastReceive;
	///
	/// Set to TRUE to receive packets that are sent to the network
	/// device broadcast address. The startup default value is FALSE.
	///
	BOOLEAN    EnableBroadcastReceive;
	///
	/// Set to TRUE to receive packets that are sent to any MAC address.
	/// The startup default value is FALSE.
	///
	BOOLEAN    EnablePromiscuousReceive;
	///
	/// Set to TRUE to drop queued packets when the configuration
	/// is changed. The startup default value is FALSE.
	///
	BOOLEAN    FlushQueuesOnReset;
	///
	/// Set to TRUE to timestamp all packets when they are received
	/// by the MNP. Note that timestamps may be unsupported in some
	/// MNP implementations. The startup default value is FALSE.
	///
	BOOLEAN    EnableReceiveTimestamps;
	///
	/// Set to TRUE to disable background polling in this MNP
	/// instance. Note that background polling may not be supported in
	/// all MNP implementations. The startup default value is FALSE,
	/// unless background polling is not supported.
	///
	BOOLEAN    DisableBackgroundPolling;
} EFI_MANAGED_NETWORK_CONFIG_DATA, *PEFI_MANAGED_NETWORK_CONFIG_DATA;
#define MAX_MCAST_FILTER_CNT  16
typedef struct _EFI_SIMPLE_NETWORK_MODE  {
	///
	/// Reports the current state of the network interface.
	///
	UINT32             State;
	///
	/// The size, in bytes, of the network interface's HW address.
	///
	UINT32             HwAddressSize;
	///
	/// The size, in bytes, of the network interface's media header.
	///
	UINT32             MediaHeaderSize;
	///
	/// The maximum size, in bytes, of the packets supported by the network interface.
	///
	UINT32             MaxPacketSize;
	///
	/// The size, in bytes, of the NVRAM device attached to the network interface.
	///
	UINT32             NvRamSize;
	///
	/// The size that must be used for all NVRAM reads and writes. The
	/// start address for NVRAM read and write operations and the total
	/// length of those operations, must be a multiple of this value. The
	/// legal values for this field are 0, 1, 2, 4, and 8.
	///
	UINT32             NvRamAccessSize;
	///
	/// The multicast receive filter settings supported by the network interface.
	///
	UINT32             ReceiveFilterMask;
	///
	/// The current multicast receive filter settings.
	///
	UINT32             ReceiveFilterSetting;
	///
	/// The maximum number of multicast address receive filters supported by the driver.
	///
	UINT32             MaxMCastFilterCount;
	///
	/// The current number of multicast address receive filters.
	///
	UINT32             MCastFilterCount;
	///
	/// Array containing the addresses of the current multicast address receive filters.
	///
	EFI_MAC_ADDRESS    MCastFilter[MAX_MCAST_FILTER_CNT];
	///
	/// The current HW MAC address for the network interface.
	///
	EFI_MAC_ADDRESS    CurrentAddress;
	///
	/// The current HW MAC address for broadcast packets.
	///
	EFI_MAC_ADDRESS    BroadcastAddress;
	///
	/// The permanent HW MAC address for the network interface.
	///
	EFI_MAC_ADDRESS    PermanentAddress;
	///
	/// The interface type of the network interface.
	///
	UINT8              IfType;
	///
	/// TRUE if the HW MAC address can be changed.
	///
	BOOLEAN            MacAddressChangeable;
	///
	/// TRUE if the network interface can transmit more than one packet at a time.
	///
	BOOLEAN            MultipleTxSupported;
	///
	/// TRUE if the presence of media can be determined; otherwise FALSE.
	///
	BOOLEAN            MediaPresentSupported;
	///
	/// TRUE if media are connected to the network interface; otherwise FALSE.
	///
	BOOLEAN            MediaPresent;
} EFI_SIMPLE_NETWORK_MODE, *PEFI_SIMPLE_NETWORK_MODE;

using EfiTcp4GetModeDataFn = EFI_STATUS(__cdecl)(PEFI_TCP4_PROTOCOL This, PEFI_TCP4_CONNECTION_STATE Tcp4State, PEFI_TCP4_CONFIG_DATA Tcp4ConfigData, PEFI_IP4_MODE_DATA Ip4ModeData, PEFI_MANAGED_NETWORK_CONFIG_DATA MnpConfigData, PEFI_SIMPLE_NETWORK_MODE SnpModeData);
using EfiTcp4ConfigureFn = EFI_STATUS(__cdecl)(PEFI_TCP4_PROTOCOL This, PEFI_TCP4_CONFIG_DATA Tcp4ConfigData);
using EfiTcp4RoutesFn = EFI_STATUS(__cdecl)(PEFI_TCP4_PROTOCOL This, bool DeleteRoute, PEFI_IPv4_ADDRESS SubnetAddress, PEFI_IPv4_ADDRESS SubnetMask, PEFI_IPv4_ADDRESS GatewayAddress);
using EfiTcp4ConnectFn = EFI_STATUS(__cdecl)(PEFI_TCP4_PROTOCOL This, PEFI_TCP4_CONNECTION_TOKEN ConnectionToken);
using EfiTcp4AcceptFn = EFI_STATUS(__cdecl)(PEFI_TCP4_PROTOCOL This, PEFI_TCP4_LISTEN_TOKEN ListenToken);
using EfiTcp4TransmitFn = EFI_STATUS(__cdecl)(PEFI_TCP4_PROTOCOL This, PEFI_TCP4_IO_TOKEN Token);
using EfiTcp4ReceiveFn = EFI_STATUS(__cdecl)(PEFI_TCP4_PROTOCOL This, PEFI_TCP4_IO_TOKEN Token);
using EfiTcp4CloseFn = EFI_STATUS(__cdecl)(PEFI_TCP4_PROTOCOL This, PEFI_TCP4_CLOSE_TOKEN CloseToken);
using EfiTcp4CancelFn = EFI_STATUS(__cdecl)(PEFI_TCP4_PROTOCOL This, PEFI_TCP4_COMPLETION_TOKEN Token);
using EfiTcp4PollFn = EFI_STATUS(__cdecl)(PEFI_TCP4_PROTOCOL This);

struct _EFI_TCP4_PROTOCOL
{
	EfiTcp4GetModeDataFn* GetModeData;
	EfiTcp4ConfigureFn* Configure;
	EfiTcp4RoutesFn* Routes;
	EfiTcp4ConnectFn* Connect;
	EfiTcp4AcceptFn* Accept;
	EfiTcp4TransmitFn* Transmit;
	EfiTcp4ReceiveFn* Receive;
	EfiTcp4CloseFn* Close;
	EfiTcp4CancelFn* Cancel;
	EfiTcp4PollFn* Poll;
};

constexpr CEFI_GUID gEfiTcp4ServiceBindingProtocolGuid = { 0x00720665, 0x67EB, 0x4A99, { 0xBA, 0xF7, 0xD3, 0xC3, 0x3A, 0x1C, 0x7C, 0xC9 } };
constexpr CEFI_GUID gEfiTcp4ProtocolGuid = { 0x65530BC7, 0xA359, 0x410F, { 0xB0, 0x10, 0x5A, 0xAD, 0xC7, 0xEC, 0x2B, 0x62 } };

typedef struct _EFI_TCP4_SERVICE_BINDING_PROTOCOL {
	EfiServiceBindingCreateChildFn*   CreateChild;
	EfiServiceBindingDestroyChildFn*  DestroyChild;
} EFI_TCP4_SERVICE_BINDING_PROTOCOL, *PEFI_TCP4_SERVICE_BINDING_PROTOCOL;

constexpr uint64_t EFI_PAGE_SIZE = 0x1000;

constexpr CEFI_GUID gEfiGraphicsOutputProtocolGuid = { 0x9042a9de, 0x23dc, 0x4a38, { 0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a } };

typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL EFI_GRAPHICS_OUTPUT_PROTOCOL, *PEFI_GRAPHICS_OUTPUT_PROTOCOL;

typedef struct _EFI_PIXEL_BITMASK {
	UINT32            RedMask;
	UINT32            GreenMask;
	UINT32            BlueMask;
	UINT32            ReservedMask;
} EFI_PIXEL_BITMASK, *PEFI_PIXEL_BITMASK;

typedef enum _EFI_GRAPHICS_PIXEL_FORMAT {
	PixelRedGreenBlueReserved8BitPerColor,
	PixelBlueGreenRedReserved8BitPerColor,
	PixelBitMask,
	PixelBltOnly,
	PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT, *PEFI_GRAPHICS_PIXEL_FORMAT;

typedef struct _EFI_GRAPHICS_OUTPUT_MODE_INFORMATION {
	UINT32                     Version;
	UINT32                     HorizontalResolution;
	UINT32                     VerticalResolution;
	EFI_GRAPHICS_PIXEL_FORMAT  PixelFormat;
	EFI_PIXEL_BITMASK          PixelInformation;
	UINT32                     PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION, *PEFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

using EfiGraphicsOuputProtocolQueryModeFn = EFI_STATUS(__cdecl)(PEFI_GRAPHICS_OUTPUT_PROTOCOL This, UINT32 ModeNumber, UINTN* SizeOfInfo, PEFI_GRAPHICS_OUTPUT_MODE_INFORMATION* Info);
using EfiGraphicsOuputProtocolSetModeFn = EFI_STATUS(__cdecl)(PEFI_GRAPHICS_OUTPUT_PROTOCOL This, UINT32 ModeNumber);

typedef struct _EFI_GRAPHICS_OUTPUT_BLT_PIXEL {
	UINT8 Blue;
	UINT8 Green;
	UINT8 Red;
	UINT8 Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL, *PEFI_GRAPHICS_OUTPUT_BLT_PIXEL;

typedef union _EFI_GRAPHICS_OUTPUT_BLT_PIXEL_UNION {
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL Pixel;
	UINT32                        Raw;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL_UNION, *PEFI_GRAPHICS_OUTPUT_BLT_PIXEL_UNION;

typedef enum _EFI_GRAPHICS_OUTPUT_BLT_OPERATION {
	EfiBltVideoFill,
	EfiBltVideoToBltBuffer,
	EfiBltBufferToVideo,
	EfiBltVideoToVideo,
	EfiGraphicsOutputBltOperationMax
} EFI_GRAPHICS_OUTPUT_BLT_OPERATION, *PEFI_GRAPHICS_OUTPUT_BLT_OPERATION;

using EfiGraphicsOuputProtocolBltFn = EFI_STATUS(__cdecl)(PEFI_GRAPHICS_OUTPUT_PROTOCOL This, EFI_GRAPHICS_OUTPUT_BLT_PIXEL* BltBuffer, EFI_GRAPHICS_OUTPUT_BLT_OPERATION BltOperation, UINTN SourceX, UINTN SourceY, UINTN DestinationX, UINTN DestinationY, UINTN Width, UINTN Height, UINTN Delta);

typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE {
	UINT32                                 MaxMode;
	UINT32                                 Mode;
	PEFI_GRAPHICS_OUTPUT_MODE_INFORMATION Info;
	UINTN                                  SizeOfInfo;
	UINTN                   FrameBufferBase;
	UINTN                                  FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE, *PEFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

struct _EFI_GRAPHICS_OUTPUT_PROTOCOL
{
	EfiGraphicsOuputProtocolQueryModeFn* QueryMode;
	EfiGraphicsOuputProtocolSetModeFn* SetMode;
	EfiGraphicsOuputProtocolBltFn* Blt;
	PEFI_GRAPHICS_OUTPUT_PROTOCOL_MODE Mode;
};