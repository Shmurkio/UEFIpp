# Protocols reference

`UEFIpp::Protocols` contains ABI-compatible UEFI protocol structures and a typed access facade.

Focused umbrella: `UEFIpp/Protocols/Protocols.hpp`.

For complete usage patterns, read [Protocol discovery and installation](../guides/protocols.md).

## Typed access

`Access` borrows a `Table::BootServices` pointer and optionally an allocator. It provides:

| Operation | Result | Firmware service |
| --- | --- | --- |
| `Locate<T>()` | `Optional<T*>` | `LocateProtocol` |
| `Handle<T>(handle)` | `Optional<T*>` | `HandleProtocol` |
| `LocateHandles<T>()` | `Optional<Vector<Handle>>` | `LocateHandle` two-call pattern |
| `Install(handle*, interface*)` | `StatusCode` | `InstallProtocolInterface` |
| `Uninstall(handle, interface*)` | `StatusCode` | `UninstallProtocolInterface` |

`Raw`, `Reset`, `Valid`, and `Allocator` expose or update the facade state.

`ProtocolTraits<T>::Id` connects a C++ protocol type with its GUID. The `Protocol` concept checks for a valid specialization.

## Console input

`InputKey` stores a scan code and Unicode character. `ScanCode` defines arrows, navigation keys, F1–F10, and Escape. Helper methods distinguish Unicode and scan-code keys.

`SimpleTextInput` is the basic reset/read protocol. `SimpleTextInputEx` adds:

- `KeyShift`, `KeyToggle`, `KeyState`, and `KeyData`;
- extended reset and keystroke reads;
- toggle-state updates;
- key notification registration and unregistration.

Shift and toggle masks include validity bits. Check validity before interpreting state.

## Console output

`SimpleTextOutput` exposes reset, string output/testing, mode query/set, attribute selection, clear screen, cursor position, and cursor visibility. `SimpleTextOutputMode` holds current mode, dimensions, attributes, and cursor state.

For normal output use `Stream::Out::Console`, which handles buffering and narrow/wide conversion.

## Loaded image

`LoadedImage` describes the running image: parent and device handles, system table, file path, load options, image base/size, code/data memory types, and unload callback.

`HasLoadOptions` and `LoadOptionsText` simplify command-line inspection. The pointer is firmware-owned and the byte size remains authoritative.

## File protocols

`SimpleFileSystem::OpenRoot` obtains the volume root. `Protocols::File` exposes raw open/close/delete/read/write/position/info/flush callbacks and convenience member functions in its implementation.

`FileOpenMode` and `FileAttribute` mirror UEFI bit values. Prefer `FileSystem::File` unless protocol-level control is necessary.

## PCI I/O

`PciIo` models memory, port, and config-space access plus DMA, BAR, location, and attribute operations.

Supporting types:

- `PciIoWidth`, including scalar, FIFO, and fill variants;
- `PciIoOperation` for bus-master DMA direction/common buffer;
- `PciIoAttributeOperation` and `PciIoAttribute`;
- `PciIoAccess` and `PciIoConfigAccess` callback groups.

Buffer counts are expressed in units selected by `PciIoWidth`, not always bytes.

## Service binding

`ServiceBinding` contains `CreateChild` and `DestroyChild`. `Tcp4ServiceBinding` and `Udp4ServiceBinding` are distinct typed aliases with their own protocol GUIDs.

Child handles and child protocol interfaces remain firmware resources. Destroy children only after all operations using them are complete.

## Shared network types

`Tcp4.hpp` also declares shared IPv4 and managed-network structures used by both TCP4 and UDP4, including `Ipv4RawAddress`, `IpAddress`, `ManagedNetworkConfigData`, and the forward-declared simple-network mode.

## TCP4

`Tcp4` exposes mode query, configure, routes, connect, accept, transmit, receive, close, cancel, and poll.

The supporting structures cover:

- access points and configuration options;
- completion, connection, close, and I/O tokens;
- transmit/receive data and fragment descriptors.

All I/O is asynchronous through completion tokens. Keep events, token storage, fragment arrays, and payload buffers alive until completion or successful cancellation.

## UDP4

`Udp4` exposes mode query, configure, multicast groups, routes, transmit, receive, cancel, and poll.

Supporting structures model config data, sessions, completion/I/O tokens, receive/transmit data, and fragments. The same asynchronous lifetime rules apply.

## Header map

| Header | Contents |
| --- | --- |
| `Access.hpp` | Typed protocol facade |
| `Traits.hpp` | Protocol concept, GUID specializations |
| `InputKey.hpp` | Basic key and scan codes |
| `SimpleTextInput.hpp` | Basic console input ABI |
| `SimpleTextInputEx.hpp` | Extended keys, state, notifications |
| `SimpleTextOutput.hpp` | Console output ABI |
| `LoadedImage.hpp` | Loaded-image ABI and helpers |
| `SimpleFileSystem.hpp` | Volume opening |
| `File.hpp` | Raw file ABI |
| `PciIo.hpp` | PCI I/O ABI |
| `ServiceBinding.hpp` | Network child creation/destruction |
| `Tcp4.hpp` | TCP4 and shared IPv4 types |
| `Udp4.hpp` | UDP4 types and ABI |

Every path is listed in the [public-header index](header-index.md#protocols).
