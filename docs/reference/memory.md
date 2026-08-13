# Memory reference

`UEFIpp::Memory` provides pool allocation, type-erased allocator capabilities, access masks, and backend-neutral memory views.

Focused umbrella: `UEFIpp/Memory/Memory.hpp`.

## UEFI pool allocation

`Allocator` is a static facade over the normalized UEFI pool allocator:

- `AllocatePool(bytes, memoryType, zeroMemory)`;
- `FreePool(address)`.

The default memory type is `BootServicesData`. The returned memory follows UEFI pool and phase lifetime. Match every successful allocation with `FreePool` unless ownership has been transferred to a component that does so.

## `AllocatorStub`

`AllocatorStub` is a three-pointer, standard-layout, trivially copyable, non-owning capability. It forwards `Allocate` and `Free` to:

- an object satisfying `AllocatorBackend`;
- explicit context/function pointers;
- stateless functions through `FromFunctions`.

It also provides checked typed `AllocateStorage<T>(Count)`, validity, context access, and reset.

The backend must outlive every stub and every container/function that copies it. A stub does not destroy its backend.

## `ExecutableAllocatorStub`

This has the same representation and binding patterns as `AllocatorStub`, but forms a separate capability domain for executable storage.

The interface alone cannot enforce page permissions, cache synchronization, or platform security policy. A backend must actually provide memory suitable for the generated-code use case.

## Access masks and errors

`MemoryAccess` has `Read`, `Write`, and `Execute` bits. Helpers compose masks and test one or all permissions. Common masks are `ReadWriteAccess`, `ReadExecuteAccess`, and `AllMemoryAccess`.

`MemoryViewOperation` identifies read, write, probe, or translation work. `MemoryViewError` covers invalid view/address/range, unsupported capabilities, access denial, unmapped/translation/physical failures, partial access, and backend failure.

`MemoryViewErrorInfo` adds failing address, requested size, bytes transferred, and backend-specific code/address/level fields.

## `MemoryView`

`MemoryView` is a non-owning type-erased capability. A backend must implement `ReadPartial`; write and probe support are detected when available.

```cpp
struct Backend
{
    auto ReadPartial(
        Foundation::Uint64 Address,
        Span<Foundation::Byte> Destination
    ) -> Memory::MemoryTransferResult;

    auto Probe(
        Foundation::Uint64 Address,
        Foundation::Size Size,
        Memory::MemoryAccessMask Access
    ) -> Memory::MemoryViewStatus;
};

Backend Source{};
Memory::MemoryView View{ Source };
```

Capability queries are `IsValid`, `CanRead`, `CanWrite`, and `CanProbe`.

Operations include:

- partial and exact `Read`/`Write`;
- `Probe` by mask or one access bit;
- typed `ReadObject` and `WriteObject` for trivially copyable values;
- `ReadArray` and `WriteArray` through spans;
- `ReadAddress` for a 64-bit address;
- `Reset`.

Exact operations fail with `PartialAccess` if the backend transfers fewer bytes. Partial operations return the actual byte count. Empty arrays are invalid ranges rather than successful no-ops.

The bound backend must outlive every copy of the view. The view owns no address space or transport.

## Capabilities umbrella

`Capabilities.hpp` groups `AllocatorStub`, `ExecutableAllocatorStub`, and `MemoryView`. Include it when an API accepts several injectable capabilities but does not need UEFI pool allocation itself.

## Header map

| Header | Contents |
| --- | --- |
| `Allocator.hpp` | Normalized UEFI pool facade |
| `AllocatorStub.hpp` | Ordinary allocation capability |
| `ExecutableAllocatorStub.hpp` | Executable allocation capability |
| `MemoryAccess.hpp` | Permission masks, operations, errors |
| `MemoryView.hpp` | Type-erased read/write/probe view |
| `Capabilities.hpp` | Capability-only umbrella |

Every path is listed in the [public-header index](header-index.md#memory).
