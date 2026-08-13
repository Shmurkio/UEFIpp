# Reverse-engineering reference

`UEFIpp::Reverse` provides MSF/PDB parsing, normalized symbol storage, cross-version matching, provider composition, and convenient symbol resolution.

Focused umbrella: `UEFIpp/Reverse/Reverse.hpp`.

For workflow examples, read [PE images, PDBs, and symbol resolution](../guides/symbols.md).

## PDB and MSF

`Reverse::Pdb` contains:

- `MatchPolicy` for exact, missing-identity-tolerant, or ignored matching;
- `Error` and detailed `ErrorInfo` with stream, offset, and record kind;
- `LoadOptions` controlling parsed symbol/type/source categories and PE exports;
- `MsfFile` and borrowed `MsfStream` views;
- `PdbFile` for identity inspection and database population.

Parser objects borrow their input byte buffer. Keep it alive.

## `SymbolDatabase`

The database stores interned strings, modules, symbols, locals, types, fields, parameter types, section maps, OMAP mappings, source lines, and inline sites.

Core ID aliases are `SymbolId`, `ModuleId`, `StringId`, `TypeIndex`, and `InlineSiteId`, with explicit invalid constants. Helpers identify IPI type indices.

Classification enums describe symbol kind/visibility/flags, resolution source, type kind, and calling convention.

### Population

Set image/identity metadata, intern names, add records, then call `Finalize`. Mutation returns `Expected<…, SymbolDatabaseError>` where allocation, invalid records, duplicates, or index problems need reporting.

### Queries

The database supports:

- exact name lookup and first exact match;
- function/global/type lookup;
- containing or preceding symbol by RVA;
- source and inline-stack lookup;
- RVA source mapping in both directions;
- symbolization of VA or RVA;
- type field/parameter spans and field offsets.

Returned record pointers and spans borrow database storage and may be invalidated by mutation or reset.

## Fingerprints and matching

`FunctionFingerprintBuilder` builds a normalized function fingerprint from an image and symbol information. `CrossVersionSymbolMatcher` recovers one or many matches between old symbols and a new image.

`MatchClass`, `MatchError`, `FunctionFingerprint`, `ResolutionEvidence`, `SymbolMatch`, and `MatcherOptions` let callers inspect how a match was established and apply a confidence policy.

Heuristic matches should not be treated as equivalent to exact identity or strong signature matches without caller validation.

## Symbol providers

`SymbolQuery` describes a request and `SymbolResolution` describes a candidate. `SymbolResolutionSet` is a vector of candidates.

`SymbolProviderStub` is a non-owning type-erased provider capability. Concrete providers include:

- PDB-backed and PE-export database providers;
- signature and heuristic match providers.

`SymbolProviderChain` owns a vector of provider stubs, not provider objects. It can return all candidates or the best resolution.

## `SymbolResolver`

`SymbolResolver` combines PDB loading, a symbol database, image validation, hashed lookup, address lookup, typed function/global lookup, symbolization, and field-offset queries.

Load overloads accept a file path, byte vector, or pointer/size. `LoadDetailed` preserves `Pdb::ErrorInfo`; simple loads expose the last detail through `LastError`.

Typed `FindFunction` checks executable sections and typed `FindGlobal` checks data sections. General `FindVa<T>` does not provide the same semantic guard.

Capacity and used metrics report resolver storage policy, not image memory usage.

## Security and lifetime notes

- Treat PE and PDB contents as untrusted structured input.
- Require PDB/image identity unless a weaker policy is intentional.
- Keep the image and PDB buffers alive for every borrowed operation.
- Validate a resolved function's exact signature and calling convention before calling it.
- Provider stubs and database views do not own their sources.

## Header map

| Header | Contents |
| --- | --- |
| `Pdb.hpp` | MSF/PDB parser, errors, options |
| `SymbolDatabase.hpp` | Normalized records and queries |
| `SymbolMatcher.hpp` | Fingerprinting and cross-version matching |
| `SymbolProvider.hpp` | Provider abstraction and chain |
| `SymbolResolver.hpp` | High-level load and lookup facade |

Every path is listed in the [public-header index](header-index.md#reverse).
