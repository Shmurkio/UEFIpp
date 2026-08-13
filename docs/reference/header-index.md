# Public-header index

This is the exhaustive index of UEFIpp's public C++ headers. Use it when you know a type or subsystem name and want the narrowest include. Paths are relative to `UEFIpp/Include` and link directly to the source declarations.

Umbrella headers collect the headers beneath them; leaf headers declare the named API. The external Zydis C implementation is an internal dependency and is intentionally not part of this C++ public-header index.

## Top-level umbrella

| Header | Contents |
| --- | --- |
| [`UEFIpp/UEFIpp.hpp`](../../UEFIpp/Include/UEFIpp/UEFIpp.hpp) | Complete public library plus the namespace aliases used by samples |

## Foundation

| Header | Contents |
| --- | --- |
| [`Foundation/Foundation.hpp`](../../UEFIpp/Include/UEFIpp/Foundation/Foundation.hpp) | Foundation umbrella |
| [`Foundation/Types.hpp`](../../UEFIpp/Include/UEFIpp/Foundation/Types.hpp) | Fixed/native-width types, characters, byte, Boolean, null, size |
| [`Foundation/Traits.hpp`](../../UEFIpp/Include/UEFIpp/Foundation/Traits.hpp) | Compile-time traits and type transforms |
| [`Foundation/Concepts.hpp`](../../UEFIpp/Include/UEFIpp/Foundation/Concepts.hpp) | Concepts built on Foundation traits |
| [`Foundation/Compare.hpp`](../../UEFIpp/Include/UEFIpp/Foundation/Compare.hpp) | Three-way `Ordering` enum |
| [`Foundation/Cast.hpp`](../../UEFIpp/Include/UEFIpp/Foundation/Cast.hpp) | Value, pointer, reference, enum, bitwise, and forced casts |
| [`Foundation/Bit.hpp`](../../UEFIpp/Include/UEFIpp/Foundation/Bit.hpp) | Masks, bit fields, alignment, powers of two, rotations, counts |
| [`Foundation/Endian.hpp`](../../UEFIpp/Include/UEFIpp/Foundation/Endian.hpp) | Little/big-endian conversion and big-endian buffer access |
| [`Foundation/Flags.hpp`](../../UEFIpp/Include/UEFIpp/Foundation/Flags.hpp) | Typed enum-mask wrapper |
| [`Foundation/Utility.hpp`](../../UEFIpp/Include/UEFIpp/Foundation/Utility.hpp) | Move, forward, swap, exchange, min/max/clamp, ignore |
| [`Foundation/Atomic.hpp`](../../UEFIpp/Include/UEFIpp/Foundation/Atomic.hpp) | 32/64-bit interlocked atomic operations and memory-order enum |
| [`Foundation/SpinLock.hpp`](../../UEFIpp/Include/UEFIpp/Foundation/SpinLock.hpp) | Spin lock and scope guard |
| [`Foundation/IntrusiveList.hpp`](../../UEFIpp/Include/UEFIpp/Foundation/IntrusiveList.hpp) | Allocation-free intrusive doubly linked list |
| [`Foundation/StrongId.hpp`](../../UEFIpp/Include/UEFIpp/Foundation/StrongId.hpp) | Tagged integral identifier |
| [`Foundation/TypeId.hpp`](../../UEFIpp/Include/UEFIpp/Foundation/TypeId.hpp) | Compile-time type token |
| [`Foundation/Assertions.hpp`](../../UEFIpp/Include/UEFIpp/Foundation/Assertions.hpp) | Source location and assertion/verification backend |

## Library

| Header | Contents |
| --- | --- |
| [`Library/Library.hpp`](../../UEFIpp/Include/UEFIpp/Library/Library.hpp) | Complete general-purpose Library umbrella |
| [`Library/String/String.hpp`](../../UEFIpp/Include/UEFIpp/Library/String/String.hpp) | String umbrella |
| [`Library/String/BasicString.hpp`](../../UEFIpp/Include/UEFIpp/Library/String/BasicString.hpp) | Owning null-terminated dynamic strings and encoding aliases |
| [`Library/String/StringView.hpp`](../../UEFIpp/Include/UEFIpp/Library/String/StringView.hpp) | Non-owning string views, search, slicing, comparison |
| [`Library/Containers/Containers.hpp`](../../UEFIpp/Include/UEFIpp/Library/Containers/Containers.hpp) | Container umbrella |
| [`Library/Containers/Array.hpp`](../../UEFIpp/Include/UEFIpp/Library/Containers/Array.hpp) | Inline fixed-size array |
| [`Library/Containers/Span.hpp`](../../UEFIpp/Include/UEFIpp/Library/Containers/Span.hpp) | Non-owning contiguous view |
| [`Library/Containers/Vector.hpp`](../../UEFIpp/Include/UEFIpp/Library/Containers/Vector.hpp) | Owning dynamic contiguous vector and small algorithms |
| [`Library/Containers/Optional.hpp`](../../UEFIpp/Include/UEFIpp/Library/Containers/Optional.hpp) | Optional in-place value |
| [`Library/Containers/Expected.hpp`](../../UEFIpp/Include/UEFIpp/Library/Containers/Expected.hpp) | Value-or-error and void-or-error results |
| [`Library/Containers/Tuple.hpp`](../../UEFIpp/Include/UEFIpp/Library/Containers/Tuple.hpp) | Recursive heterogeneous tuple, `Get`, and `MakeTuple` |
| [`Library/Functional/Functional.hpp`](../../UEFIpp/Include/UEFIpp/Library/Functional/Functional.hpp) | Callable and event umbrella |
| [`Library/Functional/FunctionDetail.hpp`](../../UEFIpp/Include/UEFIpp/Library/Functional/FunctionDetail.hpp) | Internal callable invocation helper used by templates |
| [`Library/Functional/FunctionRef.hpp`](../../UEFIpp/Include/UEFIpp/Library/Functional/FunctionRef.hpp) | Non-owning callable reference |
| [`Library/Functional/Function.hpp`](../../UEFIpp/Include/UEFIpp/Library/Functional/Function.hpp) | Copyable owning callable with inline storage |
| [`Library/Functional/MoveOnlyFunction.hpp`](../../UEFIpp/Include/UEFIpp/Library/Functional/MoveOnlyFunction.hpp) | Move-only owning callable with inline storage |
| [`Library/Functional/EventResult.hpp`](../../UEFIpp/Include/UEFIpp/Library/Functional/EventResult.hpp) | Continue/stop event result |
| [`Library/Functional/EventConnection.hpp`](../../UEFIpp/Include/UEFIpp/Library/Functional/EventConnection.hpp) | Event subscription identifier |
| [`Library/Functional/Event.hpp`](../../UEFIpp/Include/UEFIpp/Library/Functional/Event.hpp) | Persistent/one-shot multicast event |
| [`Library/Functional/StopEvent.hpp`](../../UEFIpp/Include/UEFIpp/Library/Functional/StopEvent.hpp) | Event with propagation stopping |
| [`Library/Functional/ScopedEventConnection.hpp`](../../UEFIpp/Include/UEFIpp/Library/Functional/ScopedEventConnection.hpp) | RAII event disconnection |
| [`Library/Ownership/Ownership.hpp`](../../UEFIpp/Include/UEFIpp/Library/Ownership/Ownership.hpp) | Ownership umbrella |
| [`Library/Ownership/UniquePtr.hpp`](../../UEFIpp/Include/UEFIpp/Library/Ownership/UniquePtr.hpp) | Exclusive object/array ownership and deleters |
| [`Library/Ownership/UniqueResource.hpp`](../../UEFIpp/Include/UEFIpp/Library/Ownership/UniqueResource.hpp) | Exclusive arbitrary resource ownership |
| [`Library/Ownership/ScopeExit.hpp`](../../UEFIpp/Include/UEFIpp/Library/Ownership/ScopeExit.hpp) | Releaseable scope-exit action |

## UEFI

| Header | Contents |
| --- | --- |
| [`UEFI/UEFI.hpp`](../../UEFIpp/Include/UEFIpp/UEFI/UEFI.hpp) | Core UEFI umbrella |
| [`UEFI/Types.hpp`](../../UEFIpp/Include/UEFIpp/UEFI/Types.hpp) | Handles, events, addresses, TPL, ABI list/time |
| [`UEFI/Guid.hpp`](../../UEFIpp/Include/UEFIpp/UEFI/Guid.hpp) | 16-byte GUID value |
| [`UEFI/Status.hpp`](../../UEFIpp/Include/UEFIpp/UEFI/Status.hpp) | Status codes, classification, names, wrapper |
| [`UEFI/Main.hpp`](../../UEFIpp/Include/UEFIpp/UEFI/Main.hpp) | `MainResult` and EFI status conversion |
| [`UEFI/Context.hpp`](../../UEFIpp/Include/UEFIpp/UEFI/Context.hpp) | Normalized global firmware context |
| [`UEFI/TableHeader.hpp`](../../UEFIpp/Include/UEFIpp/UEFI/TableHeader.hpp) | Common UEFI table header |
| [`UEFI/SystemTable.hpp`](../../UEFIpp/Include/UEFIpp/UEFI/SystemTable.hpp) | System table ABI |
| [`UEFI/ConfigurationTable.hpp`](../../UEFIpp/Include/UEFIpp/UEFI/ConfigurationTable.hpp) | Vendor configuration table entry |
| [`UEFI/BootServices.hpp`](../../UEFIpp/Include/UEFIpp/UEFI/BootServices.hpp) | Boot-services table, callbacks, and enums |
| [`UEFI/RuntimeServices.hpp`](../../UEFIpp/Include/UEFIpp/UEFI/RuntimeServices.hpp) | Runtime-services table, time/reset/capsule callbacks |
| [`UEFI/Memory.hpp`](../../UEFIpp/Include/UEFIpp/UEFI/Memory.hpp) | Allocation/memory types, descriptors, attributes |
| [`UEFI/DevicePath.hpp`](../../UEFIpp/Include/UEFIpp/UEFI/DevicePath.hpp) | Device-path node and type constants |
| [`UEFI/Capsule.hpp`](../../UEFIpp/Include/UEFIpp/UEFI/Capsule.hpp) | Capsule headers, block descriptors, table, flags |

## Protocols

| Header | Contents |
| --- | --- |
| [`Protocols/Protocols.hpp`](../../UEFIpp/Include/UEFIpp/Protocols/Protocols.hpp) | Protocol umbrella |
| [`Protocols/Traits.hpp`](../../UEFIpp/Include/UEFIpp/Protocols/Traits.hpp) | Protocol concept and built-in GUID traits |
| [`Protocols/Access.hpp`](../../UEFIpp/Include/UEFIpp/Protocols/Access.hpp) | Typed locate/handle/enumerate/install/uninstall facade |
| [`Protocols/InputKey.hpp`](../../UEFIpp/Include/UEFIpp/Protocols/InputKey.hpp) | Basic input key, scan-code constants and queries |
| [`Protocols/SimpleTextInput.hpp`](../../UEFIpp/Include/UEFIpp/Protocols/SimpleTextInput.hpp) | Basic console input ABI |
| [`Protocols/SimpleTextInputEx.hpp`](../../UEFIpp/Include/UEFIpp/Protocols/SimpleTextInputEx.hpp) | Extended key state and notification ABI |
| [`Protocols/SimpleTextOutput.hpp`](../../UEFIpp/Include/UEFIpp/Protocols/SimpleTextOutput.hpp) | Console output modes and ABI |
| [`Protocols/LoadedImage.hpp`](../../UEFIpp/Include/UEFIpp/Protocols/LoadedImage.hpp) | Loaded-image fields, load options, unload callback |
| [`Protocols/File.hpp`](../../UEFIpp/Include/UEFIpp/Protocols/File.hpp) | Raw UEFI file protocol, modes, attributes |
| [`Protocols/SimpleFileSystem.hpp`](../../UEFIpp/Include/UEFIpp/Protocols/SimpleFileSystem.hpp) | Volume root opening protocol |
| [`Protocols/PciIo.hpp`](../../UEFIpp/Include/UEFIpp/Protocols/PciIo.hpp) | PCI memory/I/O/config/DMA/BAR protocol |
| [`Protocols/ServiceBinding.hpp`](../../UEFIpp/Include/UEFIpp/Protocols/ServiceBinding.hpp) | TCP4/UDP4 child service binding |
| [`Protocols/Tcp4.hpp`](../../UEFIpp/Include/UEFIpp/Protocols/Tcp4.hpp) | TCP4 and shared IPv4/network ABI structures |
| [`Protocols/Udp4.hpp`](../../UEFIpp/Include/UEFIpp/Protocols/Udp4.hpp) | UDP4 configuration, tokens, packet ABI |

## IO

| Header | Contents |
| --- | --- |
| [`IO/IO.hpp`](../../UEFIpp/Include/UEFIpp/IO/IO.hpp) | Complete I/O umbrella |
| [`IO/Context.hpp`](../../UEFIpp/Include/UEFIpp/IO/Context.hpp) | Standard console/serial/input/event/logging context |
| [`IO/Core/Core.hpp`](../../UEFIpp/Include/UEFIpp/IO/Core/Core.hpp) | Core umbrella |
| [`IO/Core/Error.hpp`](../../UEFIpp/Include/UEFIpp/IO/Core/Error.hpp) | Rich I/O errors and `Result<T>` |
| [`IO/Core/Concepts.hpp`](../../UEFIpp/Include/UEFIpp/IO/Core/Concepts.hpp) | Structural source, sink, and capability concepts |
| [`IO/Core/Operations.hpp`](../../UEFIpp/Include/UEFIpp/IO/Core/Operations.hpp) | Exact/all transfers, flush, close |
| [`IO/Core/Options.hpp`](../../UEFIpp/Include/UEFIpp/IO/Core/Options.hpp) | Cancellation and wait options |
| [`IO/Core/WriterRef.hpp`](../../UEFIpp/Include/UEFIpp/IO/Core/WriterRef.hpp) | Borrowed writer type erasure |
| [`IO/Core/ReaderRef.hpp`](../../UEFIpp/Include/UEFIpp/IO/Core/ReaderRef.hpp) | Borrowed reader type erasure |
| [`IO/Core/AnyIO.hpp`](../../UEFIpp/Include/UEFIpp/IO/Core/AnyIO.hpp) | Owning inline writer/reader type erasure |
| [`IO/Adapter/Adapter.hpp`](../../UEFIpp/Include/UEFIpp/IO/Adapter/Adapter.hpp) | Adapter umbrella |
| [`IO/Adapter/Buffered.hpp`](../../UEFIpp/Include/UEFIpp/IO/Adapter/Buffered.hpp) | Bounded output/input buffering and lookahead |
| [`IO/Adapter/Newline.hpp`](../../UEFIpp/Include/UEFIpp/IO/Adapter/Newline.hpp) | Explicit newline policies |
| [`IO/Adapter/Utility.hpp`](../../UEFIpp/Include/UEFIpp/IO/Adapter/Utility.hpp) | Null, counting, fixed, memory, and tee adapters |
| [`IO/Adapter/Advanced.hpp`](../../UEFIpp/Include/UEFIpp/IO/Adapter/Advanced.hpp) | Prefix, hash, rate, ANSI, fault, and ring adapters |
| [`IO/Transport/Transport.hpp`](../../UEFIpp/Include/UEFIpp/IO/Transport/Transport.hpp) | Transport umbrella |
| [`IO/Transport/Console.hpp`](../../UEFIpp/Include/UEFIpp/IO/Transport/Console.hpp) | UTF-8 console and terminal capabilities |
| [`IO/Transport/Serial.hpp`](../../UEFIpp/Include/UEFIpp/IO/Transport/Serial.hpp) | Bounded raw serial transport |
| [`IO/Transport/File.hpp`](../../UEFIpp/Include/UEFIpp/IO/Transport/File.hpp) | Partial, seekable file source and sink |
| [`IO/Text/Text.hpp`](../../UEFIpp/Include/UEFIpp/IO/Text/Text.hpp) | Text umbrella |
| [`IO/Text/Utf.hpp`](../../UEFIpp/Include/UEFIpp/IO/Text/Utf.hpp) | Incremental UTF and validated conversions |
| [`IO/Text/Writer.hpp`](../../UEFIpp/Include/UEFIpp/IO/Text/Writer.hpp) | UTF-8 writing helpers |
| [`IO/Text/Format.hpp`](../../UEFIpp/Include/UEFIpp/IO/Text/Format.hpp) | Checked formatting, runtime arguments, formatters, hex dump |
| [`IO/Text/Reader.hpp`](../../UEFIpp/Include/UEFIpp/IO/Text/Reader.hpp) | Bounded Unicode text reading |
| [`IO/Text/Scan.hpp`](../../UEFIpp/Include/UEFIpp/IO/Text/Scan.hpp) | Typed parsing and transactional scanning |
| [`IO/Text/Operators.hpp`](../../UEFIpp/Include/UEFIpp/IO/Text/Operators.hpp) | Result-aware output chaining and atomic typed extraction operators |
| [`IO/Terminal/TerminalIO.hpp`](../../UEFIpp/Include/UEFIpp/IO/Terminal/TerminalIO.hpp) | Terminal umbrella |
| [`IO/Terminal/Terminal.hpp`](../../UEFIpp/Include/UEFIpp/IO/Terminal/Terminal.hpp) | Typed key, wait, coroutine, style, and cursor API |
| [`IO/Terminal/EventLoop.hpp`](../../UEFIpp/Include/UEFIpp/IO/Terminal/EventLoop.hpp) | Fixed-capacity UEFI coroutine event loop |
| [`IO/Terminal/LineEditor.hpp`](../../UEFIpp/Include/UEFIpp/IO/Terminal/LineEditor.hpp) | Unicode-aware interactive line editor |
| [`IO/Logging/Logging.hpp`](../../UEFIpp/Include/UEFIpp/IO/Logging/Logging.hpp) | Logging umbrella |
| [`IO/Logging/Logger.hpp`](../../UEFIpp/Include/UEFIpp/IO/Logging/Logger.hpp) | Structured multi-sink logger and panic writer |

## FileSystem

| Header | Contents |
| --- | --- |
| [`FileSystem/FileSystem.hpp`](../../UEFIpp/Include/UEFIpp/FileSystem/FileSystem.hpp) | File-system umbrella |
| [`FileSystem/Path.hpp`](../../UEFIpp/Include/UEFIpp/FileSystem/Path.hpp) | Owning path, normalization, components, relative operations |
| [`FileSystem/File.hpp`](../../UEFIpp/Include/UEFIpp/FileSystem/File.hpp) | Owning file handle and byte I/O |
| [`FileSystem/FileInfo.hpp`](../../UEFIpp/Include/UEFIpp/FileSystem/FileInfo.hpp) | Path, sizes, timestamps, and attributes |
| [`FileSystem/FileAttributes.hpp`](../../UEFIpp/Include/UEFIpp/FileSystem/FileAttributes.hpp) | File attribute mask wrapper |
| [`FileSystem/Time.hpp`](../../UEFIpp/Include/UEFIpp/FileSystem/Time.hpp) | File calendar timestamp value |

## Memory

| Header | Contents |
| --- | --- |
| [`Memory/Memory.hpp`](../../UEFIpp/Include/UEFIpp/Memory/Memory.hpp) | Memory umbrella |
| [`Memory/Capabilities.hpp`](../../UEFIpp/Include/UEFIpp/Memory/Capabilities.hpp) | Allocator/executable/memory-view capability umbrella |
| [`Memory/Allocator.hpp`](../../UEFIpp/Include/UEFIpp/Memory/Allocator.hpp) | Normalized UEFI pool allocation facade |
| [`Memory/AllocatorStub.hpp`](../../UEFIpp/Include/UEFIpp/Memory/AllocatorStub.hpp) | Non-owning ordinary allocation capability |
| [`Memory/ExecutableAllocatorStub.hpp`](../../UEFIpp/Include/UEFIpp/Memory/ExecutableAllocatorStub.hpp) | Non-owning executable allocation capability |
| [`Memory/MemoryAccess.hpp`](../../UEFIpp/Include/UEFIpp/Memory/MemoryAccess.hpp) | Access masks, operations, errors, range validation |
| [`Memory/MemoryView.hpp`](../../UEFIpp/Include/UEFIpp/Memory/MemoryView.hpp) | Type-erased read/write/probe memory capability |

## Diagnostics

| Header | Contents |
| --- | --- |
| [`Diagnostics/Diagnostics.hpp`](../../UEFIpp/Include/UEFIpp/Diagnostics/Diagnostics.hpp) | Diagnostics umbrella |
| [`Diagnostics/Trace.hpp`](../../UEFIpp/Include/UEFIpp/Diagnostics/Trace.hpp) | Project-aware call-site trace entry and formatting |

## Executable

| Header | Contents |
| --- | --- |
| [`Executable/Pe/Pe.hpp`](../../UEFIpp/Include/UEFIpp/Executable/Pe/Pe.hpp) | PE support umbrella |
| [`Executable/Pe/Types.hpp`](../../UEFIpp/Include/UEFIpp/Executable/Pe/Types.hpp) | PE constants, enums, and ABI structures |
| [`Executable/Pe/Directory.hpp`](../../UEFIpp/Include/UEFIpp/Executable/Pe/Directory.hpp) | Data-directory value |
| [`Executable/Pe/Image.hpp`](../../UEFIpp/Include/UEFIpp/Executable/Pe/Image.hpp) | Mapped-image validation, headers, RVAs, exports |

## Reverse

| Header | Contents |
| --- | --- |
| [`Reverse/Reverse.hpp`](../../UEFIpp/Include/UEFIpp/Reverse/Reverse.hpp) | Reverse-engineering umbrella |
| [`Reverse/Pdb.hpp`](../../UEFIpp/Include/UEFIpp/Reverse/Pdb.hpp) | MSF/PDB parsing, options, detailed errors |
| [`Reverse/SymbolDatabase.hpp`](../../UEFIpp/Include/UEFIpp/Reverse/SymbolDatabase.hpp) | Normalized symbol/type/source database and queries |
| [`Reverse/SymbolMatcher.hpp`](../../UEFIpp/Include/UEFIpp/Reverse/SymbolMatcher.hpp) | Function fingerprints and cross-version recovery |
| [`Reverse/SymbolProvider.hpp`](../../UEFIpp/Include/UEFIpp/Reverse/SymbolProvider.hpp) | Provider stubs, concrete providers, provider chain |
| [`Reverse/SymbolResolver.hpp`](../../UEFIpp/Include/UEFIpp/Reverse/SymbolResolver.hpp) | PDB load, typed address lookup, symbolization |

## CRT

| Header | Contents |
| --- | --- |
| [`CRT/CRT.hpp`](../../UEFIpp/Include/UEFIpp/CRT/CRT.hpp) | Freestanding CRT umbrella |
| [`CRT/Heap.hpp`](../../UEFIpp/Include/UEFIpp/CRT/Heap.hpp) | Runtime heap facade |
| [`CRT/New.hpp`](../../UEFIpp/Include/UEFIpp/CRT/New.hpp) | Global new/delete declarations |
| [`CRT/StdDef.hpp`](../../UEFIpp/Include/UEFIpp/CRT/StdDef.hpp) | Compiler-facing size/null aliases |
| [`CRT/StdLib.hpp`](../../UEFIpp/Include/UEFIpp/CRT/StdLib.hpp) | C runtime compatibility declarations |
| [`CRT/String.hpp`](../../UEFIpp/Include/UEFIpp/CRT/String.hpp) | C memory/string primitives |

## Architecture

### Umbrellas

| Header | Contents |
| --- | --- |
| [`Architecture/Architecture.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/Architecture.hpp) | Architecture umbrella |
| [`Architecture/X64/X64.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/X64.hpp) | Complete x64 umbrella |

### CPU and common state

| Header | Contents |
| --- | --- |
| [`Architecture/X64/Common/Msr.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Common/Msr.hpp) | MSR indices, selected field decoders, read/write helpers |
| [`Architecture/X64/Common/Registers.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Common/Registers.hpp) | CR0/CR3/CR4/RFLAGS/EFER/segment bit helpers |
| [`Architecture/X64/Cpu/Cpu.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Cpu/Cpu.hpp) | Control registers, CPUID, TSC, interrupts, halt/pause |
| [`Architecture/X64/Cpu/Random.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Cpu/Random.hpp) | RDRAND and RDSEED wrappers |

### Instruction model

| Header | Contents |
| --- | --- |
| [`Architecture/X64/Instruction/InstructionSet.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Instruction/InstructionSet.hpp) | Instruction model umbrella |
| [`Architecture/X64/Instruction/Mnemonic.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Instruction/Mnemonic.hpp) | Mnemonic enum and count |
| [`Architecture/X64/Instruction/Register.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Instruction/Register.hpp) | Register IDs/classes/value and named constants |
| [`Architecture/X64/Instruction/RegisterSet.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Instruction/RegisterSet.hpp) | Fixed modeled-register set |
| [`Architecture/X64/Instruction/Operand.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Instruction/Operand.hpp) | Register/memory/immediate/pointer operands and helpers |
| [`Architecture/X64/Instruction/Instruction.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Instruction/Instruction.hpp) | Mnemonic, operands, and encoding options for one instruction |
| [`Architecture/X64/Instruction/Condition.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Instruction/Condition.hpp) | Conditional branch conditions |
| [`Architecture/X64/Instruction/Attributes.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Instruction/Attributes.hpp) | Category, ISA, encoding, control-flow, attribute flags |
| [`Architecture/X64/Instruction/Encoding.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Instruction/Encoding.hpp) | Encoding hints, prefixes, EVEX/MVEX options |
| [`Architecture/X64/Instruction/DecodedInstruction.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Instruction/DecodedInstruction.hpp) | Decoded semantics and raw encoding records |

### Assembly

| Header | Contents |
| --- | --- |
| [`Architecture/X64/Assembly/Assembly.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Assembly/Assembly.hpp) | Assembly umbrella |
| [`Architecture/X64/Assembly/Error.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Assembly/Error.hpp) | Encode and assembly errors |
| [`Architecture/X64/Assembly/Label.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Assembly/Label.hpp) | Program label ID |
| [`Architecture/X64/Assembly/Section.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Assembly/Section.hpp) | Section kind/ID and section storage |
| [`Architecture/X64/Assembly/Node.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Assembly/Node.hpp) | Instruction/label/data/alignment program node |
| [`Architecture/X64/Assembly/Fixup.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Assembly/Fixup.hpp) | Internal label fixup description |
| [`Architecture/X64/Assembly/Relocation.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Assembly/Relocation.hpp) | Final relocation kind, target, and record |
| [`Architecture/X64/Assembly/CodeBuffer.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Assembly/CodeBuffer.hpp) | Owning byte buffer for generated code |
| [`Architecture/X64/Assembly/Program.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Assembly/Program.hpp) | Sections, nodes, labels, and current-section state |
| [`Architecture/X64/Assembly/Encoder.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Assembly/Encoder.hpp) | Single-instruction encoder |
| [`Architecture/X64/Assembly/AssembledCode.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Assembly/AssembledCode.hpp) | Final bytes, section layout, relocations, base |
| [`Architecture/X64/Assembly/Assembler.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Assembly/Assembler.hpp) | High-level program construction and final assembly |

### Disassembly

| Header | Contents |
| --- | --- |
| [`Architecture/X64/Disassembly/Disassembly.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Disassembly/Disassembly.hpp) | Materialized decoded instruction collection |
| [`Architecture/X64/Disassembly/Error.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Disassembly/Error.hpp) | Decode, decode-at, format, disassembly errors |
| [`Architecture/X64/Disassembly/Decoder.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Disassembly/Decoder.hpp) | One-instruction byte/memory decoder |
| [`Architecture/X64/Disassembly/InstructionRange.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Disassembly/InstructionRange.hpp) | Lazy memory-backed instruction iterator |
| [`Architecture/X64/Disassembly/Disassembler.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Disassembly/Disassembler.hpp) | Complete span disassembly facade |
| [`Architecture/X64/Disassembly/Formatter.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Disassembly/Formatter.hpp) | Intel/AT&T instruction text formatting |

### Unwind

| Header | Contents |
| --- | --- |
| [`Architecture/X64/Unwind/Unwind.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Unwind/Unwind.hpp) | Unwind umbrella |
| [`Architecture/X64/Unwind/Context.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Unwind/Context.hpp) | x64 register context and capture declaration |
| [`Architecture/X64/Unwind/Unwinder.hpp`](../../UEFIpp/Include/UEFIpp/Architecture/X64/Unwind/Unwinder.hpp) | PE runtime functions and virtual unwinding |
