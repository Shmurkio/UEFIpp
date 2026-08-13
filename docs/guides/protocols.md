# Protocol discovery and installation

UEFI is organized around GUID-identified protocols attached to handles. UEFIpp exposes the raw protocol layouts and adds `Protocols::Access`, a typed facade for the most common discovery and installation operations.

## Create an access object

After context normalization:

```cpp
Protocols::Access Access(&UEFI::Context::BootServices());

if (!Access)
{
    return MakeUnexpected(UEFI::StatusCode::InvalidParameter);
}
```

`Access` borrows the boot-services table. It can optionally carry a `Memory::AllocatorStub` for result vectors returned by `LocateHandles`.

## Locate a protocol globally

`Locate<TProtocol>()` wraps `BootServices::LocateProtocol`:

```cpp
auto FileSystem = Access.Locate<Protocols::SimpleFileSystem>();

if (!FileSystem)
{
    return MakeUnexpected(UEFI::StatusCode::NotFound);
}

Protocols::File* Root{};
const auto Status = (*FileSystem)->OpenRoot(Root);
```

The result is `Optional<TProtocol*>`. The optional owns neither the protocol nor the interface. The pointer remains governed by firmware and the protocol installation lifetime.

## Find a protocol on one handle

`Handle<TProtocol>(Handle)` wraps `BootServices::HandleProtocol`:

```cpp
auto LoadedImage = Access.Handle<Protocols::LoadedImage>(
    UEFI::Context::ImageHandle()
);

if (!LoadedImage)
{
    return MakeUnexpected(UEFI::StatusCode::NotFound);
}

(void)IO::Println(IO::SystemIO().Serial(),
                  "Image base: {}, image size: {}",
                  (*LoadedImage)->ImageBase, (*LoadedImage)->ImageSize);
```

This is the normal way to inspect the current image or install its unload callback.

## Enumerate handles

`LocateHandles<TProtocol>()` performs the two-call UEFI buffer-size pattern and returns an allocated vector:

```cpp
auto Handles = Access.LocateHandles<Protocols::PciIo>();

if (!Handles)
{
    return MakeUnexpected(UEFI::StatusCode::NotFound);
}

for (const auto Handle : *Handles)
{
    auto Pci = Access.Handle<Protocols::PciIo>(Handle);

    if (!Pci)
    {
        continue;
    }

    Foundation::UintN Segment{};
    Foundation::UintN Bus{};
    Foundation::UintN Device{};
    Foundation::UintN Function{};

    const auto Status = (*Pci)->GetLocation(
        *Pci,
        &Segment,
        &Bus,
        &Device,
        &Function
    );
}
```

Use the exact function-pointer signature from `PciIo.hpp`; protocol structures mirror the UEFI ABI and often take `This` explicitly.

## Install and uninstall a protocol

Define the protocol layout and give it a trait specialization containing its GUID:

```cpp
namespace MyDriver
{
    struct CounterProtocol
    {
        using ReadFn = auto (*)(CounterProtocol* This)
            -> Foundation::Uint64;

        ReadFn Read{};
    };
}

template<>
class UEFIpp::Protocols::ProtocolTraits<MyDriver::CounterProtocol>
{
public:
    inline static constexpr UEFI::Guid Id
    {
        0x12345678,
        0x1234,
        0x5678,
        { 0x90, 0xAB, 0xCD, 0xEF, 0x10, 0x20, 0x30, 0x40 }
    };
};
```

Then install it:

```cpp
UEFI::Handle Handle{};
MyDriver::CounterProtocol Interface{ &ReadCounter };

const auto Status = Access.Install(&Handle, &Interface);

if (UEFI::IsError(Status))
{
    return MakeUnexpected(Status);
}
```

Before unloading, remove the exact same interface pointer:

```cpp
const auto Status = Access.Uninstall(Handle, &Interface);
```

The driver owns `Interface`; firmware only publishes the pointer. Keep it alive until successful uninstallation.

## Protocol traits supplied by UEFIpp

`ProtocolTraits` specializations are included for:

- `SimpleTextInput` and `SimpleTextInputEx`;
- `SimpleTextOutput`;
- `GraphicsOutput`;
- `SimpleFileSystem` and `File`;
- `LoadedImage`;
- `PciIo`;
- `Tcp4ServiceBinding` and `Tcp4`;
- `Udp4ServiceBinding` and `Udp4`;
- `UEFI::DevicePathProtocol`.

These types satisfy the `Protocols::Protocol` concept and work directly with `Access`.

## Console and key protocols

`SimpleTextInput` is the basic keystroke protocol. `SimpleTextInputEx` adds modifier/toggle state and key notifications. UEFIpp represents extended input through `KeyData`, which combines an `InputKey` with a `KeyState`.

Use `ScanCode` constants for non-character keys such as arrows, function keys, and Escape. `InputKey::HasUnicode`, `HasScanCode`, `IsScanCode`, and `IsEscape` keep key handling readable.

The high-level `IO::Terminal` prefers extended input, falls back to basic input, and adds typed events, Unicode handling, timeouts, cancellation, and line editing.

## Graphics output

`GraphicsOutput` models the UEFI Graphics Output Protocol (GOP). Locate the
firmware-owned interface through the ordinary typed access facade:

```cpp
Protocols::Access Access(&UEFI::Context::BootServices());
auto Graphics = Access.Locate<Protocols::GraphicsOutput>();

if (!Graphics || !(*Graphics)->CurrentMode() ||
    !(*Graphics)->CurrentMode()->Info)
{
    return MakeUnexpected(UEFI::StatusCode::NotFound);
}

const auto* Mode = (*Graphics)->CurrentMode();
(void)IO::Println(IO::SystemIO().Console(), "GOP mode: {}x{}",
                  Mode->Info->HorizontalResolution,
                  Mode->Info->VerticalResolution);
```

Use `SetCurrentMode` to select a supported mode and `BlockTransfer` for fill,
screen-to-buffer, buffer-to-screen, and screen-to-screen operations. A solid
fill reads only the first BLT pixel:

```cpp
Protocols::GraphicsOutputBltPixel Blue{ 0xFF, 0x00, 0x00, 0x00 };
const auto Status = (*Graphics)->BlockTransfer(
    &Blue,
    Protocols::GraphicsOutputBltOperation::VideoFill,
    0, 0,
    0, 0,
    Mode->Info->HorizontalResolution,
    Mode->Info->VerticalResolution
);
```

`GraphicsOutputModeInformation`, `GraphicsOutputProtocolMode`, pixel formats,
bit masks, BLT pixels, and BLT operations mirror the firmware ABI. A mode with
`GraphicsPixelFormat::BltOnly` has no usable linear framebuffer. For other
modes, honor `PixelsPerScanLine` rather than assuming it equals horizontal
resolution.

`GraphicsOutput::Query` forwards GOP's allocating `QueryMode` operation. On a
successful query, the caller owns the returned mode-information buffer and must
release it with `UEFI::Context::BootServices().FreePool(Info)` before boot
services end. The protocol pointer, current-mode pointer, and framebuffer remain
firmware-owned and phase-bound.

## File protocols

`Protocols::File` and `SimpleFileSystem` expose the raw UEFI ABI. Use them for protocol-level info GUIDs or handle ownership. Use `IO::FileSource` and `IO::FileSink` for result-based partial I/O, seeking, flushing, and truncation.

## PCI I/O

`PciIo` exposes:

- memory, port-I/O, and PCI configuration access structures;
- polling and copy operations;
- DMA map/unmap;
- buffer allocation and release;
- attribute queries and changes;
- BAR attribute access;
- location discovery.

Choose `PciIoWidth` carefully. FIFO and fill variants alter address or buffer advancement and are not aliases for ordinary scalar access.

## TCP4 and UDP4

The network headers model the UEFI asynchronous token APIs. A typical flow is:

1. Locate the matching service-binding protocol.
2. Call `CreateChild`.
3. Get `Tcp4` or `Udp4` from the child handle.
4. Configure local/remote address data.
5. Submit connection or I/O tokens containing completion events.
6. Poll or wait for completion according to the firmware environment.
7. Cancel outstanding work, close if needed, unconfigure, and destroy the child.

Token fragment arrays are trailing variable-length structures. Allocate enough storage for the declared fragment count and keep all packet buffers alive until firmware completes the token.

## When to call raw boot services

`Protocols::Access` intentionally covers common typed operations, not every boot-service call. Use `UEFI::Context::BootServices()` directly for `OpenProtocol`, `CloseProtocol`, event creation, image loading, controller connection, memory-map operations, or other services that need their full UEFI parameters.

## Common mistakes

- `Optional<TProtocol*>` is not an owning smart pointer.
- `Locate` finds one matching interface; use `LocateHandles` when every handle matters.
- Install and uninstall with the same protocol GUID, handle, and interface pointer.
- Do not destroy a service-binding child while asynchronous tokens are outstanding.
- Do not use boot-service protocols after boot services have exited.
- Protocol function members are firmware callbacks. Pass the correct `This` pointer and preserve ABI types.

See [Protocols reference](../reference/protocols.md) and [UEFI reference](../reference/uefi.md) for the complete layouts.
