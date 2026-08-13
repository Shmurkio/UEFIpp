# UEFI reference

`UEFIpp::UEFI` models the core UEFI ABI and provides the global normalized context used by higher-level modules.

Focused umbrella: `UEFIpp/UEFI/UEFI.hpp`.

## Status

`StatusCode` contains UEFI success, error, and warning values, plus network-related errors used by the library. Helpers are:

- `MakeError(code)` sets the architecture error bit;
- `ToStatusValue(code)` returns the native integer status;
- `IsError`, `IsWarning`, and `IsSuccess` classify codes;
- `StatusName` returns a readable static name or `nullptr`;
- `Status` wraps a raw value or code and exposes `Code`, `Value`, `Failed`, `Succeeded`, and `Warning`.

Warnings do not set the UEFI error bit. Consequently, `Status::Succeeded()` is true for success and warning results.

`MainResult` is `Expected<void, StatusCode>`. The result overload of `ToStatusValue` maps it back to the ABI. See [Applications and drivers](../entry-points.md).

## Context

`Context` normalizes the firmware tables, image handle, console/serial streams, and runtime allocation environment. Its API and lifetime rules are detailed in [Applications and drivers](../entry-points.md#ueficontext).

The context stores borrowed pointers. It does not own or copy the UEFI system, boot-services, runtime-services, or console tables.

## Fundamental ABI types

`Types.hpp` defines:

- `Handle` and `Event` as opaque pointers;
- `PhysicalAddress` and `VirtualAddress` as 64-bit integers;
- `Tpl` as native unsigned width;
- `ListEntry` for UEFI-style linked lists;
- a low-level `Time` structure used by ABI declarations.

These are layout types. Do not replace them with host handles or standard-library abstractions in firmware callbacks.

## GUIDs

`Guid` is a standard-layout, trivially copyable 16-byte value with component constructors, `IsValid`, component accessors, and comparison.

```cpp
inline constexpr UEFI::Guid ProtocolId
{
    0x12345678,
    0x1234,
    0x5678,
    { 0x90, 0xAB, 0xCD, 0xEF, 0x10, 0x20, 0x30, 0x40 }
};
```

Streams can print a GUID, `Text::Format::Guid` converts it to a string, and `Text::Parse::Guid` parses the canonical hyphenated form.

## System table

`Table::System` mirrors the UEFI system table and contains firmware vendor/revision, console handles and interfaces, runtime services, boot services, and configuration-table entries.

`Table::Header` mirrors the common UEFI table header with signature, revision, size, CRC32, and reserved fields.

`Configuration` stores a vendor GUID and vendor-table pointer. The system table owns the array.

Prefer `Context` accessors after normalization instead of threading the system table through every component.

## Boot services

`Table::BootServices` contains the UEFI boot-services function-pointer table. The header also defines the matching function aliases and enums:

- `TimerDelay`, `InterfaceType`, and `LocateSearchType`;
- task priority functions;
- page/pool allocation and memory-map operations;
- event and timer operations;
- protocol install, lookup, open/close, and notification operations;
- image load/start/exit/unload and `ExitBootServices`;
- monotonic count, stall, watchdog, and controller connection;
- CRC32, memory copy/set, and extended event creation.

Use `Protocols::Access` for common typed protocol operations. Use the table directly when the full UEFI service is needed.

All boot-services pointers and resources become invalid according to the UEFI phase transition after `ExitBootServices`.

## Runtime services

`Table::RuntimeServices` contains time/wakeup, virtual address conversion, variable services, monotonic count, reset, capsule, and variable-capacity APIs.

Related types include `ResetType`, `Table::Time`, and `TimeCapabilities`.

Runtime-service availability does not automatically make every pointer used by your image runtime-safe. Runtime code and data require appropriate UEFI memory types and virtual-address conversion handling.

## Memory descriptors

`AllocateType` selects fixed address, maximum address, or arbitrary pages. `MemoryType` contains every standard UEFI memory classification from reserved and loader memory through boot services, runtime services, ACPI, persistent, and unaccepted memory.

`MemoryDescriptor` stores physical/virtual starts, page count, type, and attributes. Constants cover cacheability, protection, non-volatility, reliability, read-only/executable restrictions, CPU crypto, hot-plug, ISA bits, and runtime mapping.

`MemoryDescriptorVersion` is the expected descriptor version. Firmware supplies actual descriptor size separately; do not assume an array is tightly packed at `sizeof(MemoryDescriptor)` when walking a memory map returned by firmware.

## Device paths

`DevicePathProtocol` models a device-path node header and provides node length/type/subtype inspection. Constants identify hardware, ACPI, messaging, media, BIOS boot specification, and end-node types plus end-instance and end-entire subtypes.

Device paths are variable-length node sequences. Validate every node length and the total enclosing buffer before advancing.

## Capsules

`CapsuleBlockDescriptor`, `CapsuleHeader`, and `CapsuleTable` model capsule update data. Flags represent persistence across reset, system-table population, and reset initiation.

Capsule APIs live in runtime services. Follow platform requirements for scatter/gather lists, physical addresses, persistence, and reset behavior.

## ABI rules

- Preserve class/structure field order and exact types.
- Pass writable pointers where UEFI declares writable pointers, even when the logical input is constant.
- Convert raw `StatusValue` to `Status` or `StatusCode` deliberately.
- Validate variable-length firmware buffers before traversal.
- Do not retain boot-service pointers past their valid phase.

## Header map

| Header | Contents |
| --- | --- |
| `Status.hpp` | Status values, codes, names, wrapper |
| `Main.hpp` | `MainResult` and ABI conversion |
| `Context.hpp` | Normalized process-wide firmware context |
| `Types.hpp` | Handles, events, addresses, TPL, ABI list/time |
| `Guid.hpp` | 16-byte GUID value |
| `TableHeader.hpp` | Common table header |
| `SystemTable.hpp` | System table layout |
| `ConfigurationTable.hpp` | Vendor configuration entry |
| `BootServices.hpp` | Boot-services ABI |
| `RuntimeServices.hpp` | Runtime-services ABI |
| `Memory.hpp` | Allocation/memory types and descriptors |
| `DevicePath.hpp` | Device-path nodes and constants |
| `Capsule.hpp` | Capsule structures and flags |

Every path is listed in the [public-header index](header-index.md#uefi).
