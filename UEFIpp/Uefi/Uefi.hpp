// To-do:
// - EFI_DEVICE_PATH_PROTOCOL

#pragma once

#include <cstdint>

#define IN
#define OUT
#define OPTIONAL
#define MAYBE_UNUSED [[maybe_unused]]

using UINT8 = uint8_t;
using CUINT8 = const UINT8;
using PUINT8 = UINT8*;
using PCUINT8 = const UINT8;

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
using STR = CHAR*;
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

typedef struct _EFI_GUID
{
	UINT32 Data1;
	UINT16 Data2;
	UINT16 Data3;
	UINT8 Data4[8];
} EFI_GUID, *PEFI_GUID;

using PCEFI_GUID = const PEFI_GUID;

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
} EFI_MEMORY_DESCRIPTOR, * PEFI_MEMORY_DESCRIPTOR;

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

using EfiInstallProtocolInterfaceFn = EFI_STATUS(__cdecl)(PEFI_HANDLE Handle, PEFI_GUID Protocol, EFI_INTERFACE_TYPE InterfaceType, PVOID Interface);
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

extern PEFI_SYSTEM_TABLE gST;
extern EFI_HANDLE gImageHandle;
extern PEFI_BOOT_SERVICES gBS;

#include "Memory/Memory.hpp"
#include "Console/Console.hpp"
#include "Serial/Serial.hpp"