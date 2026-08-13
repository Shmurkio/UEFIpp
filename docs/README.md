# UEFIpp documentation

This documentation is organized for two common ways of working:

1. Start with a task-oriented guide when you are building something new.
2. Use the module reference or header index when you already know roughly what API you need.

All examples assume the umbrella include:

```cpp
#include <UEFIpp/UEFIpp.hpp>
```

Focused headers are listed throughout the reference if you prefer tighter dependencies.

## Find something quickly

| I want to… | Read… |
| --- | --- |
| Build the repository or add a new EFI project | [Getting started](getting-started.md) |
| Understand `Main`, `EfiMain`, application arguments, or DXE unload | [Applications and drivers](entry-points.md) |
| Choose between `Bool`, `Optional`, and `Expected` | [Core programming model](programming-model.md#error-handling) |
| Print to the console or serial port | [Streams and files guide](guides/streams-and-files.md) |
| Read or write a file | [Streams and files guide](guides/streams-and-files.md#files) |
| Locate, open, install, or remove a UEFI protocol | [Protocols guide](guides/protocols.md) |
| Assemble, decode, format, or unwind x64 code | [x64 code guide](guides/x64-code.md) |
| Load a PDB or resolve a symbol | [Symbols and reverse engineering guide](guides/symbols.md) |
| Look up one exact header | [Public-header index](reference/header-index.md) |

## First steps

- [Getting started](getting-started.md) covers prerequisites, build outputs, project references, and EFI linker settings.
- [Applications and drivers](entry-points.md) explains the thin ABI adapters and the user-defined `Main` functions.
- [Core programming model](programming-model.md) explains freestanding constraints, error handling, allocation, ownership, views, and firmware lifetime.

## Guides

- [Streams, console input, and files](guides/streams-and-files.md)
- [Protocol discovery and installation](guides/protocols.md)
- [x64 assembly, disassembly, and unwinding](guides/x64-code.md)
- [PE images, PDBs, and symbol resolution](guides/symbols.md)

## Module reference

| Module | Main topics |
| --- | --- |
| [Foundation](reference/foundation.md) | Types, traits, concepts, casts, bits, flags, IDs, atomics, intrusive lists, locks, assertions |
| [Library](reference/library.md) | Strings, containers, `Optional`, `Expected`, callables, events, ownership |
| [UEFI](reference/uefi.md) | Status codes, context, tables, memory descriptors, runtime services, device paths, capsules |
| [Protocols](reference/protocols.md) | Typed access and the supported console, file, image, PCI, TCP4, and UDP4 protocols |
| [Streams](reference/streams.md) | Output/input abstractions, sinks, sources, manipulators, hex dumps, file streams |
| [File system](reference/filesystem.md) | Paths, files, metadata, attributes, and time |
| [Text](reference/text.md) | UTF conversion, ASCII conversion, number/GUID/status formatting, numeric and GUID parsing |
| [Memory](reference/memory.md) | Pool allocation, allocator stubs, executable allocation, memory access, memory views |
| [Diagnostics](reference/diagnostics.md) | `Trace()` and automatic project module names |
| [x64 architecture](reference/architecture.md) | CPU state, instructions, encoding, assembly, disassembly, and unwinding |
| [PE executable support](reference/executable.md) | PE structures, directories, image validation, RVA translation, exports |
| [Reverse engineering](reference/reverse.md) | MSF/PDB parsing, symbol database, matchers, provider chains, resolver |
| [CRT support](reference/crt.md) | Freestanding allocation, memory/string primitives, and runtime support |

The [public-header index](reference/header-index.md) lists every public `.hpp` file, including ABI-only structure definitions and specialized instruction metadata that do not need their own narrative page.

## Namespaces used in examples

The umbrella header exposes convenient aliases:

```cpp
namespace UEFI = UEFIpp::UEFI;
namespace Stream = UEFIpp::Stream;
namespace Foundation = UEFIpp::Foundation;
namespace Protocols = UEFIpp::Protocols;
namespace Architecture = UEFIpp::Architecture;
namespace X64 = Architecture::X64;
namespace Memory = UEFIpp::Memory;
namespace FileSystem = UEFIpp::FileSystem;
namespace Reverse = UEFIpp::Reverse;
namespace Executable = UEFIpp::Executable;
namespace Pe = UEFIpp::Executable::Pe;
```

It also imports `UEFIpp::Library`, which is why `String`, `Vector`, `Optional`, and `Expected` appear unqualified in the samples. In a reusable header, fully qualified names are usually easier for downstream readers and avoid leaking a using-directive through your own API.

## Reading the reference

Reference pages use a few consistent labels:

- **Owning** means the object releases its resource or allocation when destroyed.
- **View** means the object is non-owning and its source must outlive it.
- **Capability** means a small type-erased object forwards to a backend that must outlive it.
- **ABI type** means the layout mirrors a UEFI or PE structure and should not be casually reordered.

Where a call returns `Expected<T, E>`, inspect it before calling `Value()` or `Error()`. Where a call returns `Optional<T>`, an empty value usually means the requested firmware facility was not available. Details are in [Core programming model](programming-model.md#error-handling).
