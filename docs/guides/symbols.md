# PE images, PDBs, and symbol resolution

UEFIpp separates executable layout from debug-symbol information:

- `Executable::Pe::Image` validates and inspects a mapped PE image;
- `Reverse::Pdb` reads an in-memory MSF/PDB file;
- `Reverse::SymbolDatabase` stores normalized modules, symbols, types, fields, source lines, inline sites, and address maps;
- `Reverse::SymbolResolver` combines PDB loading with convenient name/address lookup;
- symbol providers and matchers combine several resolution strategies.

## Inspect a mapped PE image

```cpp
Pe::Image Image{ ImageBase };

if (!Image.IsValid())
{
    return MakeUnexpected(UEFI::StatusCode::LoadError);
}

Stream::Out::Console
    << "Image size: " << Image.ImageSize() << Stream::Endl
    << "Entry RVA: " << Image.EntryPointRva() << Stream::Endl
    << "Sections: " << Image.SectionCount() << Stream::Endl;
```

`Image` expects a mapped image at `Base`, not a raw file layout. It exposes the DOS header, 32/64-bit NT headers, file header, section array, data directories, RVA translation, and parsed exports.

```cpp
for (const auto& Export : Image.Exports())
{
    Stream::Out::Console
        << Export.Name << " @ 0x"
        << Stream::Hexadecimal << Export.Address
        << Stream::Decimal << Stream::Endl;
}
```

Forwarded exports set `Forwarded` and store the forwarder string instead of naming code in the current image.

`RvaToPointer<T>` returns a typed pointer only after validating the RVA against the mapped image. Still validate any structure-specific size before reading variable-length data.

## Load a PDB with `SymbolResolver`

The easiest path is `SymbolResolver`:

```cpp
Reverse::SymbolResolver Resolver{};

auto Loaded = Resolver.LoadDetailed(
    PdbBytes.Data(),
    PdbBytes.Size(),
    ImageBase
);

if (!Loaded)
{
    const auto Error = Loaded.Error();

    Stream::Out::Serial
        << Trace()
        << "PDB load failed, code="
        << Foundation::Cast::Underlying(Error.Code)
        << ", stream=" << Error.Stream
        << ", offset=" << Error.Offset
        << Stream::Endl;

    return MakeUnexpected(UEFI::StatusCode::CompromisedData);
}
```

`LoadDetailed` returns `Expected<void, Pdb::ErrorInfo>`. The simpler `Load` overloads accept a path, byte vector, or pointer/size and record details in `LastError()`.

By default, PDB identity must match the image. `Pdb::LoadOptions` controls identity policy and whether public/global/module symbols, types, source lines, inline information, and PE exports are loaded.

## Resolve names and addresses

```cpp
Foundation::Uint64 Rva{};

if (Resolver.FindRva("DriverEntry", Rva))
{
    Stream::Out::Console
        << "DriverEntry RVA: 0x"
        << Stream::Hexadecimal << Rva
        << Stream::Decimal << Stream::Endl;
}

using DriverEntryFn = auto (*)(Foundation::Void*) -> Foundation::Int32;
const auto DriverEntry = Resolver.FindFunction<DriverEntryFn>("DriverEntry");
```

`FindFunction<T>` only returns pointers that lie in executable PE sections. `FindGlobal<T>` only returns pointers in data sections. `FindVa<T>` performs a general conversion and should be used only when the expected target kind is known.

For repeated or obfuscated lookups, precompute `HashName` and use `FindRvaByHash` or `FindVaByHash`.

## Symbolize an address

```cpp
const auto Symbol = Resolver.Symbolize(Address);

if (Symbol.SymbolValue)
{
	Stream::Out::Console
		<< Resolver.Database().String(Symbol.SymbolValue->Name)
		<< "+0x" << Stream::Hexadecimal << Symbol.OffsetIntoSymbol
		<< Stream::Decimal << Stream::Endl;
}
```

Consult `SymbolizedAddress` in `SymbolDatabase.hpp` for the exact IDs and source/inline information returned. The database owns strings and records; views and record pointers remain tied to the database and are invalidated by `Reset` or rebuilding its storage.

## Query types and fields

```cpp
Foundation::Uint64 Offset{};

if (Resolver.FieldOffset("MY_TYPE", "Member", Offset))
{
    Stream::Out::Console
        << "MY_TYPE::Member = 0x"
        << Stream::Hexadecimal << Offset
        << Stream::Decimal << Stream::Endl;
}
```

For richer type inspection, use `Database().FindType`, `TypeFields`, `TypeParameters`, and `TypeByIndex`. Types use PDB type indices, including a marked IPI index form; helper functions identify and normalize those indices.

## Work directly with MSF and PDB

Use `Pdb::MsfFile` when you need raw stream access:

```cpp
Reverse::Pdb::MsfFile Msf{};
auto Opened = Msf.Open(PdbBytes.Data(), PdbBytes.Size());

if (Opened)
{
    const auto Stream = Msf.Stream(1);
    Vector<Foundation::Uint8> Contents{};
    const auto Read = Stream.ReadAll(Contents);
}
```

`MsfFile` and `PdbFile` borrow the input byte buffer. Keep it alive until the parser object and every `MsfStream` view are finished.

`PdbFile::Populate` parses a previously opened PDB into a caller-owned `SymbolDatabase`. This is useful when a program wants explicit control over the database lifetime or wants to reuse parsing components without `SymbolResolver`.

## Provider chains

The provider abstraction allows several sources to answer one `SymbolQuery`:

- `PdbSymbolProvider` queries a populated PDB-backed database;
- `PeExportSymbolProvider` exposes PE exports;
- `SignatureSymbolProvider` uses match results;
- `HeuristicSymbolProvider` uses heuristic match results;
- `SymbolProviderChain` collects providers and returns all resolutions or the best one.

`SymbolProviderStub` is a non-owning type-erased view. Every provider object added through a stub must outlive the chain and its resolution calls.

## Cross-version matching

`FunctionFingerprintBuilder` derives fingerprints from code. `CrossVersionSymbolMatcher` compares an old symbol database/image with a new image and produces `SymbolMatch` records containing match class, score/evidence, and recovered address information.

`MatcherOptions` controls the matching policy. Treat fuzzy or heuristic matches differently from exact/signature-backed matches; the `MatchClass` and evidence are there so callers can enforce their own confidence threshold.

## Common mistakes

- `Pe::Image` expects mapped-image layout, not raw disk layout.
- Keep PDB bytes alive while `MsfFile`, `PdbFile`, or stream views borrow them.
- Keep the mapped image alive while the resolver calculates virtual addresses.
- Require exact PDB identity unless you deliberately accept a weaker policy.
- Do not call a resolved address without checking its section, signature, ABI, and image lifetime.
- A provider stub does not own its provider.
- Treat malformed PE and PDB data as untrusted input; propagate detailed parser errors.

See [PE executable reference](../reference/executable.md) and [Reverse-engineering reference](../reference/reverse.md).
