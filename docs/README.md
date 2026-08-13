# UEFIpp documentation

All examples may use the umbrella header:

```cpp
#include <UEFIpp/UEFIpp.hpp>
```

Focused headers are listed in the module references and [public-header index](reference/header-index.md).

## Find something quickly

| Goal | Read |
| --- | --- |
| Build UEFIpp or add an EFI target | [Getting started](getting-started.md) |
| Understand `Main`, UTF-8 arguments, or DXE unload | [Applications and drivers](entry-points.md) |
| Print, scan, edit a line, log, or access a file | [Modern I/O and files](guides/io-and-files.md) |
| Choose `Bool`, `Optional`, `Expected`, or `IO::Result` | [Core programming model](programming-model.md#error-handling) |
| Locate or install a protocol | [Protocols guide](guides/protocols.md) |
| Work with x64 code | [x64 code guide](guides/x64-code.md) |
| Load PDB data or resolve symbols | [Symbols guide](guides/symbols.md) |

## Guides

- [Modern I/O, terminals, logging, and files](guides/io-and-files.md)
- [Protocol discovery and installation](guides/protocols.md)
- [x64 assembly, disassembly, and unwinding](guides/x64-code.md)
- [PE images, PDBs, and symbol resolution](guides/symbols.md)

## Module reference

| Module | Main topics |
| --- | --- |
| [Foundation](reference/foundation.md) | Types, traits, concepts, casts, bits, atomics, locks, assertions |
| [Library](reference/library.md) | Strings, containers, tuples, `Optional`, `Expected`, callables, events, ownership |
| [I/O](reference/io.md) | Concepts, errors, transports, adapters, UTF, format/scan, terminal input, logging |
| [UEFI](reference/uefi.md) | Status codes, context, tables, memory descriptors, services, device paths |
| [Protocols](reference/protocols.md) | Typed access and supported console, file, image, PCI, TCP4, and UDP4 protocols |
| [File system](reference/filesystem.md) | Paths, files, metadata, attributes, time, exact firmware status |
| [Memory](reference/memory.md) | Allocation, capabilities, executable storage, access, memory views |
| [Diagnostics](reference/diagnostics.md) | Trace records and structured logging |
| [x64 architecture](reference/architecture.md) | CPU state, instructions, encoding, assembly, disassembly, unwinding |
| [PE support](reference/executable.md) | PE validation, directories, RVA translation, exports |
| [Reverse engineering](reference/reverse.md) | MSF/PDB parsing, symbols, matchers, provider chains, resolver |
| [CRT support](reference/crt.md) | Freestanding allocation, memory/string primitives, runtime support |

## Namespace aliases

`UEFIpp.hpp` exposes `UEFI`, `IO`, `Foundation`, `Diagnostics`, `Protocols`, `Architecture`, `X64`, `Memory`, `FileSystem`, `Reverse`, `Executable`, and `Pe`. It also imports `UEFIpp::Library`, so application code can use `U8String`, `Vector`, `Tuple`, `Optional`, and `Expected` directly. Reusable public headers should generally use qualified names.

## Result and lifetime vocabulary

- **Owning** objects release their resource or allocation.
- **View** objects borrow storage that must outlive them.
- **Capability reference** objects such as `WriterRef` borrow an implementation without allocation.
- **ABI types** mirror firmware layouts and must retain exact layout.
- **`IO::Result<T>`** is `Expected<T, IO::Error>` and preserves semantic or firmware failure context.

Never call `Value()` or `Error()` without first inspecting an `Expected`. Never retain a view, writer/reader reference, protocol pointer, or event past the lifetime and firmware phase of its backend.
