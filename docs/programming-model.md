# Core programming model

UEFIpp uses modern C++ syntax and ownership patterns in a runtime that is not a normal hosted C++ environment. Understanding the few constraints that follow makes the rest of the API predictable.

## Freestanding assumptions

Final EFI images do not link the normal platform default libraries. Exceptions and RTTI are disabled. UEFIpp supplies the small allocation, memory, string, and runtime surface required by the compiler and its own code.

This has several consequences:

- do not throw exceptions or rely on exception-based library APIs;
- do not assume the full C++ standard library is present;
- treat allocation as fallible even when a constructor cannot report it directly;
- keep firmware ABI types and callback signatures exact;
- use UEFIpp's `U8String`, containers, ownership types, and result-based I/O.

## Error handling

UEFIpp uses three main result shapes.

### `Foundation::Bool`

Use `Bool` when the caller only needs to know whether an operation happened and the API has no useful structured error:

```cpp
if (!Buffer.Reserve(4096))
{
    return MakeUnexpected(UEFI::StatusCode::OutOfResources);
}
```

Many mutable containers and high-level file helpers use this form. A `false` result should not be ignored.

### `Optional<T>`

Use `Optional` when absence is expected and no detailed reason is required. Typed protocol lookup is a good example:

```cpp
Protocols::Access Access(&UEFI::Context::BootServices());
auto FileSystem = Access.Locate<Protocols::SimpleFileSystem>();

if (!FileSystem)
{
    return MakeUnexpected(UEFI::StatusCode::NotFound);
}
```

Inspect `HasValue()` or use the explicit Boolean conversion before `Value()`, `operator*`, or `operator->`.

### `Expected<T, E>`

Use `Expected` when failure has information the caller can act on:

```cpp
auto Instruction = Decoder.Decode(Code, RuntimeAddress);

if (!Instruction)
{
    const auto Error = Instruction.Error();
    // Report or translate Error.
}
else
{
    Use(Instruction.Value());
}
```

`Expected<void, E>` represents completion without inventing a success payload. `UEFI::MainResult` is this form.

`AndThen`, `Transform`, `TransformError`, and `OrElse` are available when a short transformation chain is clearer than nested conditionals. Prefer explicit checks when a chain would hide which firmware operation failed.

## Status codes

At the UEFI ABI boundary, use `UEFI::StatusCode` or `UEFI::StatusValue`. `Status` wraps either form and exposes `Succeeded()`, `Failed()`, `Warning()`, `Code()`, and `Value()`.

Warnings are not errors. `Status::Succeeded()` returns true for success and warning statuses because `Failed()` specifically checks the UEFI error bit. Use `Warning()` if warning handling matters.

```cpp
const UEFI::Status Result{ RawStatus };

if (Result.Failed())
{
    (void)IO::Println(IO::SystemIO().Serial(),
                      "Operation failed: {}", Result);
}
```

## Owning values and views

The names communicate lifetime:

| Owning | Non-owning view |
| --- | --- |
| `String` | `StringView` |
| `Vector<T>` / `Array<T, N>` | `Span<T>` |
| `Function` / `MoveOnlyFunction` | `FunctionRef` |
| `UniquePtr` / `UniqueResource` | raw pointer, handle, or protocol pointer |

A view is cheap to copy because it does not own data. It becomes invalid when the original storage is destroyed, released, reallocated, or otherwise moved.

```cpp
String Name{ "UEFI++" };
StringView View = Name.View();

// Any operation that reallocates Name may invalidate View.
(void)Name.Append(" firmware");
```

Obtain a fresh view after mutating owning storage. The same rule applies to a `Span` into a `Vector`.

## Allocator capabilities

Many allocating types accept `Memory::AllocatorStub`. The stub is a small, non-owning type-erased capability. It forwards allocations to either an object backend or a pair of functions.

```cpp
struct Arena
{
    auto Allocate(Foundation::Size Bytes, Foundation::Size Alignment)
        -> Foundation::Void*;

    auto Free(Foundation::Void* Address)
        -> Foundation::Bool;
};

Arena Storage{};
Memory::AllocatorStub Allocator{ Storage };
Vector<Foundation::Byte> Buffer{ Allocator };
```

`Storage` must outlive `Allocator` and every container that copies it. The stub does not own the backend. A default-constructed stub tells supported types to use their normal UEFIpp allocation path.

`ExecutableAllocatorStub` has the same lifetime model but represents storage intended for generated executable code. It is a distinct type so ordinary allocation is not silently treated as executable allocation.

## Ownership wrappers

`UniquePtr<T>` and `UniquePtr<T[]>` own dynamically allocated objects. `UniqueResource<Resource, Deleter>` generalizes the pattern to handles, IDs, or any value that needs a cleanup action. `ScopeExit` runs a callable when control leaves a scope unless released.

Move these types to transfer responsibility. Do not copy or manually clean up a resource that is still owned by one of them.

```cpp
auto Guard = ScopeExit{
    [&]
    {
        RollBackPartiallyInstalledProtocols();
    }
};

if (!InitializationSucceeded())
{
    return MakeUnexpected(UEFI::StatusCode::DeviceError);
}

Guard.Release();
return {};
```

## Firmware handles and phase lifetime

UEFI handles and protocol pointers are borrowed firmware objects. `Protocols::Access` does not own them, and an `Optional<TProtocol*>` from `Locate` or `Handle` does not keep the protocol installed.

Follow these rules:

- do not use boot services after a successful `ExitBootServices`;
- do not keep a protocol pointer after the interface is uninstalled;
- close file handles and destroy service-binding children according to their protocols;
- do not assume `RuntimeServices` data is valid unless it was allocated and converted for runtime use;
- treat `Context` pointers as process-wide borrowed state.

## Attributes and return values

`[[nodiscard]]` marks values whose loss is likely a bug. Consume the result, propagate it, or explicitly cast to `void` when ignoring it is intentional:

```cpp
if (!Values.PushBack(Value))
{
    return MakeUnexpected(UEFI::StatusCode::OutOfResources);
}

(void)BestEffortDiagnosticWrite();
```

`[[maybe_unused]]` is appropriate for ABI callback parameters that must be present but are not needed by a particular implementation.

## Concurrency primitives

`Foundation::Atomic<T>` supports 32-bit and 64-bit integral or enum storage with relaxed, acquire, release, acquire-release, and sequential memory orders. `SpinLock` and `SpinLockGuard` provide mutual exclusion.

Spin locks are busy waits. Keep protected regions short, do not call arbitrary firmware code while holding a lock, and be deliberate about interrupt/task-priority interactions.

## Assertions

Use `UEFIPP_ASSERT` for programmer invariants and `UEFIPP_VERIFY` when an expression must still execute in a non-debug configuration. Assertions report a `SourceLocation` through `Foundation::Assertions`.

Assertions are not a substitute for validating firmware input. Invalid handles, malformed images, user-provided paths, and buffer sizes should be handled with ordinary results.

## Namespace style

The umbrella header provides short aliases and imports `UEFIpp::Library`, which keeps application code readable. Public headers should prefer qualified names so including them does not unexpectedly change name lookup for downstream users.

```cpp
// Fine in an application .cpp file:
Vector<String> Names{};

// Clearer in a public declaration:
auto Names() const -> UEFIpp::Library::Span<const UEFIpp::Library::String>;
```

## Where to go next

- [Library reference](reference/library.md) for the owning/view types and their operations.
- [Memory reference](reference/memory.md) for allocator and memory-view backends.
- [UEFI reference](reference/uefi.md) for status and table lifetime.
- [Protocols guide](guides/protocols.md) for typed protocol lookup.
