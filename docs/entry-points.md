# Applications, drivers, and entry points

UEFI calls an exported C ABI function. Application and driver authors want a normal C++ function with structured errors. UEFIpp keeps those two concerns separate:

```text
firmware -> EfiMain adapter -> UEFI::Context -> user Main -> MainResult -> EFI_STATUS
```

The adapters live in each final `.efi` project, not in `UEFIpp.lib`, because applications and drivers have different startup requirements.

## `UEFI::MainResult`

`MainResult` is the common return type:

```cpp
using MainResult = Expected<Foundation::Void, UEFI::StatusCode>;
```

It communicates only two states:

- success, represented by `return {};`;
- a specific UEFI failure, represented by `return MakeUnexpected(Code);`.

There is no Boolean success payload. If startup succeeded, the value itself has nothing else to say. If startup failed, preserving the actual `StatusCode` is more useful than reducing it to `false`.

```cpp
[[nodiscard]] auto InitializeStorage() -> UEFI::MainResult
{
    if (!UEFI::Context::HasImageHandle())
    {
        return MakeUnexpected(UEFI::StatusCode::InvalidParameter);
    }

    return {};
}
```

The overload `UEFI::ToStatusValue(const MainResult&)` converts success to `StatusCode::Success` and failure to the contained error code.

## Application `Main`

An application receives parsed command-line arguments:

```cpp
[[nodiscard]] auto Main(
    const Vector<U8String>& Args
) -> UEFI::MainResult;
```

The application adapter performs the following work:

1. Calls `UEFI::Context::Normalize(ImageHandle, SystemTable)`.
2. Verifies that console input and output are available.
3. Looks up `Protocols::LoadedImage` on the current image handle.
4. Parses the UTF-16 `LoadOptions` command line, including quoted arguments.
5. Validates and converts arguments to UTF-8 `U8String` objects.
6. Calls `Main(Args)`.
7. Flushes the process-wide console and serial pipelines.
8. Returns the application failure, or a flush failure when `Main` succeeded,
   to firmware.

Adapter failures use meaningful firmware codes: invalid context becomes `InvalidParameter`, missing console support becomes `Unsupported`, and allocation failures become `OutOfResources`.

An application should not call `Context::Normalize` again. By the time `Main` runs, `IO::SystemIO()`, pool allocation, the system table, boot services, runtime services, and the image handle are ready.

## Driver `Main` and unload

A boot-service driver uses:

```cpp
[[nodiscard]] auto Main() -> UEFI::MainResult;
```

The driver adapter normalizes the context and calls `Main`. A driver that can be unloaded should install an unload callback in its `LoadedImage` protocol:

```cpp
namespace
{
    auto DriverUnload(
        [[maybe_unused]] UEFI::Handle ImageHandle
    ) -> UEFI::StatusCode
    {
        UEFIPP_LOG(IO::SystemIO().Log(), IO::Severity::Info,
                   "Driver unloaded");

        return UEFI::StatusCode::Success;
    }
}

[[nodiscard]] auto Main() -> UEFI::MainResult
{
    Protocols::Access Access(&UEFI::Context::BootServices());

    auto LoadedImage = Access.Handle<Protocols::LoadedImage>(
        UEFI::Context::ImageHandle()
    );

    if (!LoadedImage)
    {
        return MakeUnexpected(UEFI::StatusCode::NotFound);
    }

    (*LoadedImage)->Unload = DriverUnload;
    return {};
}
```

The driver adapter also flushes the process-wide console and serial pipelines
after `Main`. The unload callback returns a raw `StatusCode` because that
function pointer is part of the UEFI protocol ABI. `MainResult` is for the C++
boundary controlled by the project.

## `UEFI::Context`

`Context` owns no firmware tables. It stores process-wide pointers supplied by firmware and configures the library around them.

| Function | Use |
| --- | --- |
| `Normalize(ImageHandle, SystemTable, SerialPort, MemoryType)` | Full startup for an image; records the image handle and attaches the runtime |
| `Attach(SystemTable, SerialPort, MemoryType)` | Attaches to tables when no image handle is available |
| `BootServices()` | Returns the normalized boot-services table |
| `RuntimeServices()` | Returns the runtime-services table |
| `SystemTable()` | Returns the system table |
| `ConsoleIn()` / `ConsoleOut()` | Returns normalized console protocol references |
| `HasConsoleIn()` / `HasConsoleOut()` | Checks console availability before taking a reference |
| `ImageHandle()` / `HasImageHandle()` | Accesses the current image handle |
| `HasBootServices()` | Reports whether boot-service-backed capabilities remain valid |
| `IO::SystemIO().Flush()` | Publishes buffered console and serial output and returns the first failure |
| `PrepareExitBootServices()` | Flushes the standard output pipelines before invoking firmware `ExitBootServices` |
| `ExitBootServicesSucceeded()` | Invalidates boot-only I/O without firmware calls after exit succeeds |
| `CrtMemoryType()` | Reports the pool type used by the freestanding runtime |
| `IsInitialized()` | Reports whether attachment succeeded |

Do not use reference-returning accessors until initialization and the matching `Has…` condition are satisfied. Boot-services APIs are only valid while firmware boot services are active.

## Serial port and allocation type

`Normalize` and `Attach` default to serial port `0x3F8` and `MemoryType::BootServicesData` for runtime allocations:

```cpp
const auto Ready = UEFI::Context::Normalize(
    ImageHandle,
    SystemTable,
    0x3F8,
    UEFI::MemoryType::BootServicesData
);
```

The checked-in adapters use the defaults. Change them only if the platform or image lifetime requires a different choice. In particular, memory allocated as boot-services data must not be treated as valid after `ExitBootServices`.

## Logging module names

The structured logger records module, severity, timestamp, and source location. Set the module during project initialization and use `UEFIPP_LOG` to capture the call site:

```cpp
auto& Log = IO::SystemIO().Log();
Log.SetModule(u8"NetworkDriver");
UEFIPP_LOG(Log, IO::Severity::Error,
           "network initialization failed: {}", Status);
```

The default module is `UEFIpp`. A project can still format `Diagnostics::TraceEntry` when instruction/return addresses are specifically useful, but ordinary diagnostics should use structured logging.

## Keep the adapter thin

The adapter is the right place for ABI normalization and argument construction. It is not the right place for application logic. Keeping it thin has practical benefits:

- `Main` is easy to read and test;
- application failures remain typed;
- driver and application startup differences are explicit;
- changes to command-line handling do not spread through the program;
- the exported ABI remains easy to audit.

Use additional `.cpp` files when `Main` begins coordinating several independent subsystems, as demonstrated by `SampleApplication`.
