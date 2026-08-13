# UEFIpp

UEFIpp is a freestanding, modern C++ library for x64 UEFI applications and boot-service drivers. It keeps the firmware ABI at the edge while providing typed errors, UTF-8 text, composable I/O, containers, ownership, protocol discovery, files, x64 code tooling, PE inspection, unwinding, and symbol resolution.

The repository builds `UEFIpp.lib`, `SampleApplication.efi`, and `SampleDxe.efi`.

## Start here

- [Documentation home](docs/README.md)
- [Getting started](docs/getting-started.md)
- [Modern I/O and files](docs/guides/io-and-files.md)
- [Applications, drivers, and entry points](docs/entry-points.md)
- [Public-header index](docs/reference/header-index.md)

## Minimal application

```cpp
#include <UEFIpp/UEFIpp.hpp>

[[nodiscard]] auto Main(const Vector<U8String>& Args) -> UEFI::MainResult
{
    auto& Io = IO::SystemIO();

    if (auto Result = IO::Println(
            Io.Console(), "Hello from UEFI++; arguments: {}", Args.Size());
        !Result)
    {
        return MakeUnexpected(Result.Error().Status.Failed()
            ? Result.Error().Status.Code()
            : UEFI::StatusCode::DeviceError);
    }

    UEFIPP_LOG(Io.Log(), IO::Severity::Info, "application started");
    return {};
}
```

The project-owned entry adapter attaches `UEFI::Context`, initializes
`IO::SystemIO()`, converts UTF-16 load options to validated UTF-8 arguments,
invokes `Main`, flushes the standard console and serial pipelines, and maps the
application or flush result back to `EFI_STATUS`.

## I/O design

The I/O system is intentionally not an imitation of `std::iostream`:

- `OutputSink` and `InputSource` are structural concepts with partial-transfer semantics.
- Every fallible operation returns `IO::Result<T>` with operation, exact firmware status, offset, and transferred-byte context.
- `Print` uses compile-time checked, stateless format strings; `VPrint` supports runtime format arguments.
- The optional `Out(...) <<` and `Reader >> Read<T...>`/`Into(...)` facade preserves explicit results, per-value formatting, and transactional extraction without persistent stream state.
- UTF-8 is the library text boundary. UTF-16 conversion is explicit and validated at firmware boundaries.
- Buffering, newline conversion, teeing, prefixing, hashing, rate limiting, fault injection, and terminal styling are composable adapters.
- `WriterRef`/`ReaderRef` provide allocation-free borrowed type erasure; `AnyWriter`/`AnyReader` provide owning small-buffer type erasure.
- Terminal input exposes typed key events, timeouts, cancellation, coroutines, and a Unicode-aware line editor.
- Structured logging formats once and fans out to multiple sinks; panic output avoids allocation and locks.

See the [I/O guide](docs/guides/io-and-files.md) and
[I/O reference](docs/reference/io.md).

## Build

Open `UEFIpp.slnx` in Visual Studio or build from a Visual Studio developer shell:

```powershell
msbuild UEFIpp.slnx /m /p:Configuration=Debug /p:Platform=x64
```

Use `Release` for the optimized configuration. Outputs are written below `x64/Debug` or `x64/Release`. The projects use the MSVC `v145` toolset, `/std:c++latest`, the conforming preprocessor, UTF-8 source/execution encoding, MASM, no exceptions or RTTI, and no default libraries in EFI images.

## Main modules

| Area | What it provides |
| --- | --- |
| Foundation | Firmware-sized types, traits, casts, atomics, source locations, assertions, and locks |
| Library | UTF string types, views, spans, arrays, vectors, tuples, expected/optional values, callables, events, and ownership |
| IO | Result-based byte I/O, transports, adapters, UTF conversion, formatting, scanning, terminals, coroutines, and logging |
| UEFI and Protocols | ABI-compatible tables, status handling, global context, typed protocol discovery, console, graphics output, files, PCI, and networking |
| FileSystem | Paths, metadata, file ownership, resizing, and exact last firmware status |
| Memory | Pool allocation, allocator capabilities, executable allocators, access masks, and memory views |
| Architecture | x64 registers, CPU helpers, assembly, decoding, formatting, disassembly, and unwinding |
| Executable and Reverse | PE parsing, exports, MSF/PDB loading, symbol databases, matching, providers, and resolution |

UEFIpp is explicit about failure and lifetime. Non-owning views and capability
references never extend backend lifetimes. Flush the standard I/O pipelines
with `UEFI::Context::PrepareExitBootServices()` before firmware exit and
invalidate boot-service-backed I/O with `ExitBootServicesSucceeded()` afterward.
