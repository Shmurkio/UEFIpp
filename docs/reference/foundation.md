# Foundation reference

`UEFIpp::Foundation` is the lowest-level C++ layer. It supplies stable-width types, traits and concepts, casting and bit utilities, small synchronization primitives, IDs, and assertions without depending on a hosted standard library.

Focused umbrella: `UEFIpp/Foundation/Foundation.hpp`.

## Types

`Types.hpp` defines:

- signed and unsigned integers from 8 to 64 bits;
- `IntPtr`, `UintPtr`, `IntN`, `UintN`, and `Size` for the x64 native width;
- `Float32`, `Float64`, `Char`, `WChar`, `Char8`, `Char16`, `Char32`, and `Byte`;
- `Bool`, `Void`, `NullPtr`, and `PtrDiff`.

These aliases make ABI intent visible and are used consistently in UEFI structures. Prefer them in firmware-facing layouts. The current architecture fixes pointer/native-width aliases at 64 bits.

## Traits and concepts

`Traits.hpp` provides the compile-time building blocks used throughout the library: constants, enable-if, conditional selection, CV/reference/pointer removal, and predicates for identity, qualifiers, references, pointers, integral/floating/arithmetic types, enums, classes, unions, empty/final/abstract/polymorphic types, trivially copyable types, standard-layout types, scalar types, and object types.

`Concepts.hpp` wraps those predicates as readable C++ concepts: `Void`, `Integral`, `FloatingPoint`, `Arithmetic`, `Enum`, `Pointer`, reference forms, `Class`, `Union`, `Empty`, `Final`, `Abstract`, `Polymorphic`, `TriviallyCopyable`, `StandardLayout`, `Scalar`, `Object`, `Const`, `Volatile`, and `Same`.

Use concepts to constrain firmware helpers without importing the standard type-traits library:

```cpp
template<Foundation::Concepts::Integral T>
auto WriteRegister(T Value) -> Foundation::Void;
```

## Cast

`Foundation::Cast` collects explicit conversions:

| Function | Intent |
| --- | --- |
| `To<T>` | Ordinary constrained value conversion |
| `Auto<T>` | Chooses a value, pointer, reference, or forced conversion appropriate to `T` |
| `PointerToAddress<T>` | Pointer to integer address |
| `AddressToPointer<T>` | Integer address to pointer |
| `Pointer<T>` | Reinterpret pointer element type while preserving const |
| `Reference<T>` | Reinterpret a reference |
| `Underlying` | Enum to underlying value |
| `RemoveConst` | Explicitly removes pointer constness |
| `Bitwise<T>` | Same-size trivially copyable bitwise conversion |
| `Force<T>` | Last-resort explicit compiler cast |

Use the narrowest operation that describes your intent. `Force` and const removal deserve special scrutiny in review. `Bitwise` is for representation-preserving conversion, not numeric conversion.

## Bit

`Foundation::Bit` supports integral and enum storage. The API includes:

- width and mask creation: `Width`, `Mask`, `LowMask`, `RangeMask`;
- tests: `IsSet`, `HasAny`, `HasNone`, `IsBitSet`;
- updates: `Set`, `Clear`, `Toggle`, `Assign` and their single-bit forms;
- field manipulation: `ClearAndSet`, `Replace`, `Extract`, `Insert`;
- alignment: `IsValidAlignment`, `TryAlignDown`, `TryAlignUp`, `AlignDown`, `AlignUp`, `IsAligned`;
- powers of two: `IsPowerOfTwo`, `FloorPowerOfTwo`, `CeilPowerOfTwo`;
- rotations and sign extension;
- population count and leading/trailing zero count;
- indexed byte extraction and insertion.

Checked `TryAlign…` forms report invalid alignment or overflow. Use them when values can originate outside your code.

```cpp
Foundation::Uint64 Aligned{};

if (!Foundation::Bit::TryAlignUp(Size, 16, Aligned))
{
    return MakeUnexpected(UEFI::StatusCode::BadBufferSize);
}
```

## Flags

`Flags<TEnum>` is a typed enum-mask wrapper. Construct it from an enum or raw mask, query with `Has`, `HasAll`, and `HasAny`, create copies with `With`/`Without`, or mutate with `Add`, `Remove`, and `Clear`.

It does not assume every enum value is a single bit; callers are responsible for defining meaningful masks.

## Endian

`Endian` swaps 16-, 32-, and 64-bit values, converts from/to little or big endian, and loads/stores big-endian values from byte buffers.

The target is x64 little endian. `FromLittle` and `ToLittle` are therefore identity operations, while big-endian conversion swaps. Use `LoadBig` and `StoreBig` for network-format integer fields rather than unaligned pointer casts.

## Utility

`Utility` provides `Move`, `Forward`, `Swap`, `Exchange`, `Min`, `Max`, `Clamp`, and `Ignore`. These are the freestanding counterparts used by containers and ownership types.

`Ignore` is useful for an intentionally discarded result. It should signal a deliberate best-effort operation, not hide a failure that needs propagation.

## Ordering

`Ordering` contains `Less`, `Equal`, and `Greater` for APIs that need a small explicit three-way result.

## Atomic

`Atomic<T>` accepts 32-bit or 64-bit integral types. It provides `Load`, `Store`, `Exchange`, `CompareExchange`, `FetchAdd`, and `FetchSub`, with a `MemoryOrder` parameter.

The implementation uses MSVC interlocked intrinsics. The order parameter documents caller intent; current operations use interlocked primitives for all order choices. Do not assume a weaker code-generation distinction without checking the implementation.

## Spin locks

`SpinLock` provides `TryLock`, `Lock`, `Unlock`, and `Locked`. `SpinLockGuard` acquires a lock for a scope and releases it on destruction.

Keep critical sections short and never hold a spin lock across arbitrary protocol calls or blocking console input.

## Intrusive lists

`IntrusiveListNode<T>` stores previous/next pointers and link state inside `T`. `IntrusiveList<T, &T::Node>` then provides `PushBack`, `Remove`, `Front`, `Back`, `Size`, and `Empty` without allocation.

An object can only be linked once through a given node member. Remove it before destruction. The list does not own its elements.

## Strong and type IDs

`StrongId<Tag, Value>` wraps an integral ID so unrelated ID domains cannot be mixed accidentally. Zero is the invalid/default value, exposed through explicit Boolean conversion.

`TypeId::Of<T>()` computes a compile-time hash of the MSVC function signature for `T`. It is useful as an in-build type token, but its exact numeric value is compiler/signature dependent and should not be used as a stable file or wire format.

## Assertions

`SourceLocation` stores file, function, and line. `Assertions` backs the `UEFIPP_ASSERT`, `UEFIPP_VERIFY`, and `UEFIPP_SOURCE_LOCATION` macros.

- Assert programmer invariants.
- Verify fallible cleanup whose failure should be visible while still executing in release.
- Validate external data with normal control flow rather than assertions.

## Header map

| Header | Contents |
| --- | --- |
| `Types.hpp` | Fundamental aliases |
| `Traits.hpp` | Compile-time traits and transforms |
| `Concepts.hpp` | Concepts built from traits |
| `Cast.hpp` | Explicit conversion helpers |
| `Bit.hpp` | Masks, fields, alignment, rotations, bit counts |
| `Endian.hpp` | Byte-order conversion |
| `Flags.hpp` | Typed flag masks |
| `Utility.hpp` | Move/forward/swap/exchange/min/max/clamp |
| `Compare.hpp` | `Ordering` |
| `Atomic.hpp` | 32/64-bit atomics |
| `SpinLock.hpp` | Spin lock and guard |
| `IntrusiveList.hpp` | Non-owning intrusive list |
| `StrongId.hpp` | Tagged integral identifiers |
| `TypeId.hpp` | Compile-time type token |
| `Assertions.hpp` | Source locations and assertion backend |

Every path is listed in the [public-header index](header-index.md#foundation).
