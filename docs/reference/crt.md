# CRT support reference

`UEFIpp::CRT` is the small compiler/runtime support layer needed by freestanding UEFIpp code. Most application code should not call it directly.

Focused umbrella: `UEFIpp/CRT/CRT.hpp`.

## Heap and allocation

`Heap` connects allocation to the normalized UEFI context. `New.hpp` declares global allocation/deallocation operators used by UEFIpp containers and ownership types when no explicit allocator capability is supplied.

Allocation depends on successful context attachment and uses the configured UEFI memory type. Code running before context setup should avoid implicit dynamic allocation or supply an explicit valid backend where the API permits it.

## C runtime primitives

`String.hpp` and `StdLib.hpp` declare/implement the memory and string routines required by the library/compiler in a no-default-libraries image. These are compatibility primitives, not a recommendation to write new application code in a C style.

Prefer `String`, `Span`, `Vector`, `Text`, and `Stream` in ordinary code.

## Runtime implementation

Library source files provide initialization/termination and compiler helpers needed by static construction and freestanding linkage. The existing linker warning about a `.CRT` section is significant: a custom EFI entry path must ensure any required static initialization policy is actually honored.

The checked-in adapters call `Context::Normalize`, which attaches the UEFIpp runtime. Do not replace them with a raw entry point without understanding initialization and teardown requirements.

## Standard-definition aliases

`StdDef.hpp` provides `size_t` and `nullptr_t` forms needed by compiler-facing declarations.

## Header map

| Header | Contents |
| --- | --- |
| `Heap.hpp` | Runtime heap facade |
| `New.hpp` | Global new/delete declarations |
| `String.hpp` | C memory/string primitive declarations |
| `StdLib.hpp` | C runtime compatibility declarations |
| `StdDef.hpp` | Fundamental compiler aliases |
| `CRT.hpp` | Focused umbrella |

Every path is listed in the [public-header index](header-index.md#crt).
