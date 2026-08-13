# Library reference

`UEFIpp::Library` is the freestanding general-purpose C++ library: owning strings, non-owning views, containers, structured results, callable wrappers, events, and ownership utilities.

Focused umbrella: `UEFIpp/Library/Library.hpp`.

The executable examples in `Samples/SampleApplication/Source/Library` exercise every public type in this module.

## Strings

### `BasicStringView<TChar>`

Aliases: `StringView`, `WideStringView`, `U8StringView`, `U16StringView`, and `U32StringView`.

This is a non-owning `(pointer, size)` view. It provides iteration, `Front`, `Back`, checked-value `At`, indexing, `RemovePrefix`, `RemoveSuffix`, `Substr`, prefix/suffix checks, forward and reverse `Find`, `Contains`, lexicographic `Compare`, relational operators, `AsSpan`, and `IsNull`.

Out-of-range character access returns a zero character rather than a pointer or exception. An empty view can be either null or a non-null zero-length slice; use `IsNull` only when that distinction matters.

### `BasicString<TChar>`

Aliases: `String`, `WideString`, `U8String`, `U16String`, and `U32String`.

This is an owning, null-terminated dynamic string backed by `Vector<TChar>`. It exposes size/capacity, allocator, views, iteration, element access, reserve/resize, assign, append, push/pop, substring, search, comparison, and `+=`.

Allocation-sensitive mutators return `Bool`: `Reserve`, `Resize`, `Assign`, `Append`, and `PushBack`. Operators such as assignment and `+=` cannot report allocation failure and keep the best result the underlying operation could produce; use the named mutator when failure handling matters.

Any reallocation invalidates pointers, iterators, and views into the string.

## Fixed and non-owning containers

### `Array<T, N>`

An inline fixed-size array with no allocation. It provides data/size queries, views, iteration, front/back, pointer-returning checked `At`, indexing, `Fill`, `Contains`, `Find`, and equality.

Do not call `Front` or `Back` for `N == 0`.

### `Span<T>`

A non-owning contiguous view. Construct it from a pointer/size pair, a C array, or a mutable-to-const span conversion. It provides iteration, front/back, indexing, prefix/suffix removal, subspans, first/last slices, search, equality, and `IsNull`.

Unlike `Array` and `Vector`, `Span::At` returns a reference and does not perform a bounds check. The name indicates indexed access, not optional access. Callers must ensure `Index < Size()`.

### `Tuple<T...>`

An inline heterogeneous value used by typed multi-value operations such as `IO::Scan<T...>`. Construct it directly or with `MakeTuple`, inspect its arity through `TupleSize`, and access fields with `Get<Index>`. C arrays decay to pointers in tuple deduction and `MakeTuple`.

## `Vector<T>`

`Vector` is an owning dynamic contiguous array with an optional `AllocatorStub`. It is copyable and movable.

### Capacity and lifetime

- `Reserve` changes capacity without changing size.
- `Resize` constructs or destroys elements.
- `Clear` destroys elements but retains allocation.
- `Release` destroys elements and frees storage.
- `ReleaseAndResetAllocator` also drops the allocator capability.
- `ShrinkToFit` reduces capacity to size.

Reallocation invalidates pointers, references, spans, and iterators.

### Mutation

`PushBack`, `EmplaceBack`, `PopBack`, `Assign`, `Append`, `Insert`, and `Erase` report failure where appropriate. `Insert` permits `Index == Size()`; `Erase` requires an existing element.

### Algorithms

`Contains`, `Find`, `Remove`, `RemoveIf`, `Reverse`, `Sort`, `LowerBound`, and `BinarySearch` operate in place or inspect current storage. `LowerBound` and `BinarySearch` require ascending sorted order under the same comparison.

`Sort` is an insertion sort, which is simple and effective for small firmware collections but not intended as a high-volume general sort.

## `Optional<T>`

`Optional` represents either no value or one in-place `T`. Construct with `NullOpt`, a value, or `InPlace`. Inspect with `HasValue` or explicit Boolean conversion; access with `Value`, `*`, or `->`.

Operations:

- `ValueOr` returns a fallback copy;
- `Emplace` replaces the current value;
- `Reset` destroys it;
- `Swap` exchanges states;
- `AndThen`, `Transform`, and `OrElse` compose optional work;
- comparisons support `NullOpt`, another optional, or a value when `T` is comparable.

`Value()` assumes a value is present. Check first.

## `Expected<T, E>`

`Expected` stores either `T` or `E`. Construct failures with `MakeUnexpected(error)` or `Expected(Unexpect, args...)`.

```cpp
Expected<Foundation::Uint64, ParseError> ParseValue(StringView Text);

auto Value = ParseValue(Input);

if (!Value)
{
    return MakeUnexpected(Value.Error());
}
```

Value operations include `Value`, `*`, `->`, and `ValueOr`. Error operations include `Error` and `ErrorOr`. `Emplace` and `EmplaceError` replace state. Composition uses `AndThen`, `Transform`, `TransformError`, and `OrElse`.

`Expected<void, E>` supports success without storage for a value. It provides `Value()` as a checked no-op plus the error and composition operations appropriate to a void result.

Accessing the inactive alternative triggers a UEFIpp assertion.

## Callable wrappers

| Type | Owns callable | Copyable | Best use |
| --- | --- | --- | --- |
| `FunctionRef<R(Args...)>` | No | Yes | Temporary call into a callable guaranteed to outlive the reference |
| `Function<R(Args...)>` | Yes | Yes | Stored callback that may be copied |
| `MoveOnlyFunction<R(Args...)>` | Yes | Move only | Callback capturing a move-only resource |

`Function` and `MoveOnlyFunction` keep small callables inline and allocate larger ones. Both can carry an allocator. Check their Boolean conversion before invocation. `Reset` or assignment from `nullptr` clears them.

`FunctionRef` never extends lifetime. Do not construct it from a callable that will disappear before invocation.

## Events

### `Event<Args...>`

An event owns a vector of `Function<void(Args...)>` handlers. Subscribe with `Subscribe`, `SubscribeOnce`, `Connect`, `ConnectOnce`, or `operator+=`. Emit with `Emit` or `operator()`.

Handlers can be enabled, disabled, disconnected, queried, and cleared. Disconnecting during emission marks the handler for removal and compacts afterward, which keeps iteration stable.

`EventConnection` is an ID, not ownership. Resetting a copied connection does not disconnect the event.

### `ScopedEventConnection<EventType>`

This wrapper binds an event reference and connection ID, disconnecting on destruction. The event must outlive the scoped connection. Use `Disconnect` for early removal and `Connected` to inspect state.

### `StopEvent<Args...>`

Handlers return `EventResult::Continue` or `EventResult::Stop`. Emission stops at the first `Stop`. It supports regular and one-shot subscriptions, disconnection, clearing, containment, and size queries.

## Ownership

### `UniquePtr<T>` and `UniquePtr<T[]>`

Exclusive ownership for objects and arrays. Supports custom deleters, move transfer, `Get`, `GetDeleter`, Boolean conversion, dereference/indexing, `Release`, `Reset`, and `Swap`.

Use the array specialization for memory created with `new[]`. `Release` transfers cleanup responsibility to the caller.

### `UniqueResource<Resource, Deleter>`

Exclusive ownership of an arbitrary resource value. `Get` accesses it while owned, `Release` transfers it, `Reset` closes it, `Reset(newResource)` replaces it, and `Swap` exchanges ownership.

The deleter receives the resource by non-const reference. A default-constructed `UniqueResource` owns nothing.

### `ScopeExit<Function>`

Runs a stored function at scope exit while active. `Release` cancels it. Moving transfers activity and deactivates the source.

Use it for rollback and local cleanup, particularly before a resource can be placed in a dedicated ownership wrapper.

## Header map

| Area | Headers |
| --- | --- |
| Strings | `String/BasicString.hpp`, `String/StringView.hpp`, `String/String.hpp` |
| Containers | `Containers/Array.hpp`, `Span.hpp`, `Vector.hpp`, `Tuple.hpp`, `Optional.hpp`, `Expected.hpp`, `Containers.hpp` |
| Functional | `FunctionRef.hpp`, `Function.hpp`, `MoveOnlyFunction.hpp`, `Event.hpp`, `StopEvent.hpp`, connection/result headers, `Functional.hpp` |
| Ownership | `UniquePtr.hpp`, `UniqueResource.hpp`, `ScopeExit.hpp`, `Ownership.hpp` |

`FunctionDetail.hpp` is an implementation helper exposed only because the callable templates include it. Application code should use the public wrappers rather than depending on `Library::Detail`.

Every path is listed in the [public-header index](header-index.md#library).
