# Text reference

`UEFIpp::Text` contains encoding conversion, value formatting, and strict parsing helpers.

Focused umbrella: `UEFIpp/Text/Text.hpp`.

## Encoding

`Text::Encoding` converts among UTF-8, UTF-16, and UTF-32:

- `Utf16ToUtf8` and `Utf8ToUtf16`;
- `Utf32ToUtf8` and `Utf8ToUtf32`;
- `Utf16ToUtf32` and `Utf32ToUtf16`.

It also provides ASCII-oriented conversion:

- `ToAscii(WideStringView)`;
- `ToWideAscii(StringView)`;
- `Utf16ToAscii`;
- `WideToAscii`.

Unicode helpers identify high/low surrogates, validate code points, and encode/decode surrogate pairs. Invalid sequences use `ReplacementCharacter` where the conversion implementation can continue.

ASCII conversion is intentionally lossy for non-ASCII input. Use a Unicode conversion when text must round-trip.

Every allocating conversion accepts an optional `AllocatorStub` and returns an owning string. An empty result can mean either empty input or allocation failure, so callers that must distinguish those cases should validate input and expected output length around the conversion.

## Format

`Text::Format` creates owning strings without streams:

- `UInt64<TString>` and `Int64<TString>`;
- `HexUInt64<TString>(value, width, uppercase)`;
- `Guid<TString>`;
- `Status<TString>`.

Narrow `String` overloads are supplied for integers, GUIDs, and statuses.

```cpp
const auto Text = UEFIpp::Text::Format::HexUInt64<String>(
    0xBEEF,
    8,
    true
);
```

`Status` uses `StatusName` when known and otherwise produces `UnknownStatus(0x…)`.

## Parse

`Text::Parse` performs strict, allocation-free parsing from any `BasicStringView<TChar>`:

- `IsDigit`, `IsHexDigit`, and `HexValue`;
- decimal `UInt64`, `UInt32`, and `UInt8`;
- `HexUInt64` and fixed-two-digit `HexByte`;
- canonical 36-character hyphenated `Guid`.

Parsers reject empty input, invalid characters, and overflow. On failure, numeric outputs are reset to zero and GUID output is reset to an empty GUID.

The hexadecimal parsers do not accept a `0x` prefix. Decimal parsers do not accept signs or whitespace. Trim and normalize input before calling them if your input format permits those features.

## Header map

| Header | Contents |
| --- | --- |
| `Encoding.hpp` | Unicode and ASCII conversion |
| `Format.hpp` | Number, GUID, and status formatting |
| `Parse.hpp` | Strict number and GUID parsing |

Every path is listed in the [public-header index](header-index.md#text).
