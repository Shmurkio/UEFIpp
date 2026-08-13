# PE executable reference

`UEFIpp::Executable::Pe` models the PE structures used by UEFI images and provides a validated mapped-image facade.

Focused umbrella: `UEFIpp/Executable/Pe/Pe.hpp`.

For usage with symbols, read [PE images, PDBs, and symbol resolution](../guides/symbols.md).

## Constants and structure types

`Types.hpp` defines DOS/NT signatures, 32/64-bit optional-header magic values, directory count, and section short-name size.

Enums:

- `SectionCharacteristic` for code/data, permissions, discardability, and alignment flags;
- `DirectoryIndex` for export, import, exception, security, relocation, debug, TLS, load-config, IAT, delay-import, CLR, and other standard directories.

ABI structures:

- `DosHeader`, `FileHeader`, `DataDirectory`;
- `OptionalHeader32`, `OptionalHeader64`;
- `NtHeaders32`, `NtHeaders64`;
- `SectionHeader` and `ExportDirectory`.

These mirror file/image layouts. Preserve packing and field order.

## `Directory`

`Directory` is a small value containing directory index, RVA, and size. `Empty` is true if RVA or size is zero.

It does not validate or translate the range by itself; obtain it from a valid `Image` and use image-aware access.

## `Image`

Construct `Image` with a mapped image base and optional allocator. It exposes:

- validation and 32/64-bit classification;
- raw base and numeric base address;
- DOS/NT/file header access;
- image size and entry-point RVA;
- directory lookup;
- section headers and count;
- RVA-to-pointer conversion;
- parsed export enumeration.

`RvaToPointer<T>` is a typed convenience. It returns `nullptr` for an invalid range.

## Exports

`Export` contains name, ordinal, RVA, resolved address, forwarder state, and forwarder string. The strings use the image object's allocator.

Export enumeration allocates a vector and strings. An empty vector can mean no exports or allocation/parsing failure; validate the image and directory if that distinction matters.

## Layout expectation

`Image` operates on a mapped image: section RVAs refer to virtual positions beneath the base. Do not pass a raw PE file buffer unless it has already been laid out as an image.

For malformed or untrusted images, validate structure-specific counts and sizes even after top-level image validation. PE contains many variable-length tables.

## Headers

- `Types.hpp`: constants, enums, and ABI structures.
- `Directory.hpp`: directory value.
- `Image.hpp`: mapped image facade and exports.
- `Pe.hpp`: focused umbrella.

Every path is listed in the [public-header index](header-index.md#executable).
