# File-system reference

`UEFIpp::FileSystem` provides owning paths, file handles, metadata, attributes, and calendar timestamps above the raw UEFI file protocols.

Focused umbrella: `UEFIpp/FileSystem/FileSystem.hpp`.

For copyable examples, read [Streams, console input, and files](../guides/streams-and-files.md#files).

## `Path`

`Path` owns a narrow `String` and supports:

- construction/assignment from string, C string, or view;
- `String`, `View`, `Allocator`, and `Empty`;
- absolute/root tests and component splitting;
- copy/in-place normalization;
- static and member `Join`;
- `Parent`, `FileName`, `Extension`, `Stem`, `NameCount`, and `HasExtension`;
- extension replacement/removal;
- `RelativeTo` and equality.

Paths use backslash as the canonical separator. An absolute path starts with `\`; root is a single `\`.

Returned string views borrow the path. Mutation or destruction invalidates them. `Components()` allocates a vector of borrowed views, so both the vector and original path must remain appropriate for use.

## `File`

`File` owns a `Protocols::File` handle and caches `FileInfo`. It is movable and not copyable.

Open operations:

- `Open(Path, Mode)` uses the current image file system;
- `Open(Path, FsHandle, Mode)` selects a specific file system;
- `OpenAny(Path, Mode)` searches simple-file-system handles.

`FileOpenMode` values are `Read`, `Write`, `Create`, `ReadWrite`, and `CreateReadWrite`.

I/O includes vector and raw-buffer reads, raw/vector writes at current or explicit positions, append, position query, flush, close, and delete. `FileSystemHandle` reports the volume handle used.

The raw allocated-buffer read must be paired with `FreeReadBuffer` from the same `File` allocator path.

## `FileInfo`

`FileInfo` stores:

- full path and derived name/stem/extension views;
- logical and physical sizes;
- creation, modification, and access times;
- attributes and convenience classification.

It offers `IsFile`, `IsDirectory`, `IsReadOnly`, `IsHidden`, `IsSystem`, and `IsArchive`.

## `FileAttributes`

`FileAttribute` values are `None`, `ReadOnly`, `Hidden`, `System`, `Directory`, and `Archive`. `FileAttributes` wraps the mask and provides `Has`, `Set`, `Clear`, `Toggle`, and named queries.

## `Time`

`Time` stores year, month, day, hour, minute, second, and nanosecond. `Valid` checks component ranges. This is file metadata time, distinct from raw UEFI runtime-service time structures.

It does not store timezone/daylight state and does not perform comparisons beyond equality or calendar arithmetic.

## Failure model

The high-level file API returns `Bool`. A failure may represent protocol absence, path resolution failure, firmware I/O failure, metadata failure, or allocation failure. Translate to an appropriate `StatusCode` at your component boundary and log enough context for diagnosis.

Use the raw protocol API when exact firmware status preservation is required.

## Header map

| Header | Contents |
| --- | --- |
| `Path.hpp` | Owning normalized path |
| `File.hpp` | Owning high-level file handle |
| `FileInfo.hpp` | Cached metadata |
| `FileAttributes.hpp` | Attribute mask wrapper |
| `Time.hpp` | File calendar time |

Every path is listed in the [public-header index](header-index.md#filesystem).
