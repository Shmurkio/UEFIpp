# UEFIpp

UEFIpp is a freestanding C++ library for x64 UEFI applications and boot-service drivers. It keeps the firmware ABI at the edge of a project and provides C++ interfaces for the parts that make firmware code pleasant to write: strings and containers, error handling, ownership, streams, files, protocol discovery, memory access, x64 assembly and disassembly, PE inspection, unwinding, and symbol resolution.

The repository builds three targets:

- `UEFIpp.lib`, the static library;
- `SampleApplication.efi`, an application and executable Library test suite;
- `SampleDxe.efi`, a boot-service driver.

## Start here

- [Documentation home](docs/README.md)
- [Getting started](docs/getting-started.md)
- [Applications, drivers, and entry points](docs/entry-points.md)
- [Core programming model](docs/programming-model.md)
- [Complete public-header index](docs/reference/header-index.md)

The [SampleApplication](Samples/SampleApplication/Entry.cpp) is useful when learning the general-purpose `Library/` APIs. It runs focused examples for every public string, container, functional, event, and ownership type. The [SampleDxe](Samples/SampleDxe/Entry.cpp) is the smallest example of a driver with an unload callback.

## A minimal application

```cpp
#include <UEFIpp/UEFIpp.hpp>

[[nodiscard]] auto Main(
    const Vector<String>& Args
) -> UEFI::MainResult
{
    Stream::Out::Console
        << "Hello from UEFI++" << Stream::Endl
        << "Arguments: " << Args.Size() << Stream::Endl;

    return {};
}
```

The project-owned entry-point adapter initializes `UEFI::Context`, converts UEFI load options to `Vector<String>`, invokes `Main`, and maps `MainResult` back to the firmware `EFI_STATUS` ABI. Application code therefore stays ordinary C++.

## Build

Open `UEFIpp.slnx` in Visual Studio and select `Debug|x64` or `Release|x64`, or build from a Visual Studio developer shell:

```powershell
msbuild UEFIpp.slnx /m /p:Configuration=Debug /p:Platform=x64
```

Outputs are written to `x64/Debug` or `x64/Release`.

The projects currently use the MSVC `v145` platform toolset, MASM, and a Windows SDK. Exceptions and RTTI are disabled, default libraries are not linked into EFI images, and `EfiMain` is the linker entry point. See [Getting started](docs/getting-started.md) for the complete project setup.

## What is included

| Area | What it provides |
| --- | --- |
| Foundation | Firmware-sized types, casts, bit operations, flags, atomics, IDs, assertions, spin locks, and low-level utilities |
| Library | Strings, views, arrays, spans, vectors, optional/expected values, callable wrappers, events, smart ownership, and scope guards |
| UEFI and Protocols | ABI-compatible tables and structures, status handling, global context, typed protocol discovery, files, console, PCI I/O, TCP4, and UDP4 |
| Stream, Text, and FileSystem | Console/serial/file streams, formatting, parsing, encoding conversion, paths, metadata, and file operations |
| Memory | Pool allocation, allocator capabilities, executable allocators, access masks, and type-erased memory views |
| Architecture | x64 registers and CPU helpers, instruction models, assembler, decoder, disassembler, formatter, and unwinder |
| Executable and Reverse | PE parsing, export enumeration, PDB/MSF loading, symbol databases, matching, providers, and resolution |
| Diagnostics and CRT | Project-aware tracing and the small runtime surface required by freestanding C++ code |

The [reference documentation](docs/README.md#module-reference) describes each area and links every public header.

## Repository layout

```text
UEFIpp/
  Include/UEFIpp/    Public headers
  Source/             Library implementations
  UEFIpp.props        Shared per-project trace configuration
Samples/
  SampleApplication/  Application and Library examples
  SampleDxe/          Boot-service driver example
docs/                 Guides and API reference
```

## Design notes

UEFIpp is intentionally explicit about failure and lifetime. Operations that can fail commonly return `Bool`, `Optional<T>`, or `Expected<T, E>`. Non-owning types such as `Span`, `StringView`, `FunctionRef`, `AllocatorStub`, and `MemoryView` do not extend the lifetime of what they reference. Firmware handles remain governed by UEFI phase and protocol rules.

Those conventions are explained in [Core programming model](docs/programming-model.md) and repeated near the APIs where they matter.
