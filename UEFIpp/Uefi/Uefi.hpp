#pragma once

#include <cstdint>

#define IN
#define OUT
#define OPTIONAL
#define MAYBE_UNUSED [[maybe_unused]]

#define offsetof(Type, Member) ((size_t)&(((Type*)0)->Member))

namespace std
{
	template <typename T>
	struct remove_reference { using type = T; };

	template <typename T>
	struct remove_reference<T&> { using type = T; };

	template <typename T>
	struct remove_reference<T&&> { using type = T; };

	template <typename T>
	using remove_reference_t = typename remove_reference<T>::type;
}

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

typedef struct _EFI_GUID
{
	UINT32 Data1;
	UINT16 Data2;
	UINT16 Data3;
	UINT8 Data4[8];
} EFI_GUID, *PEFI_GUID;

using CEFI_GUID = const EFI_GUID;
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

typedef struct _EFI_DEVICE_PATH_PROTOCOL
{
	UINT8 Type;
	UINT8 SubType;
	UINT8 Length[2];
} EFI_DEVICE_PATH_PROTOCOL, *PEFI_DEVICE_PATH_PROTOCOL;

using CEFI_DEVICE_PATH_PROTOCOL = const EFI_DEVICE_PATH_PROTOCOL;
using PCEFI_DEVICE_PATH_PROTOCOL = const EFI_DEVICE_PATH_PROTOCOL*;

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