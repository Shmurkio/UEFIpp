# Streams, console input, and files

UEFIpp uses one output model across the firmware console, serial port, and files. Once you know the `<<` interface and manipulators, moving diagnostics between sinks is straightforward.

## Console and serial output

The initialized global streams are:

```cpp
Stream::Out::Console
Stream::Out::Serial
```

They are configured by `UEFI::Context::Normalize` or `Attach`. Write values with `operator<<`:

```cpp
Stream::Out::Console
    << Stream::ClearScreen
    << "UEFI++" << Stream::Endl
    << "Ready: " << true << Stream::Endl
    << "Address: " << UEFI::Context::ImageHandle() << Stream::Endl;

Stream::Out::Serial
    << Trace() << "Initialization completed" << Stream::Endl;
```

Supported output includes narrow and wide text, UEFIpp strings and string views, characters, Booleans, integral and floating-point values, pointers, `UEFI::Guid`, `HexDump`, and trace entries.

`Endl` writes a line ending and flushes. `FlushNow` flushes without adding a line. Output also flushes automatically when its internal buffer fills.

## Formatting

The manipulators live in `UEFIpp::Stream`:

| Manipulator | Effect |
| --- | --- |
| `Hexadecimal` / `Decimal` | Select integer base |
| `Uppercase` / `Lowercase` | Select hexadecimal digit case |
| `Width{N}` | Minimum width for the next hexadecimal value |
| `Fill{'0'}` | Padding character |
| `Precision{N}` | Digits after the decimal point for floating values |
| `Endl` | New line and flush |
| `FlushNow` | Flush pending output |
| `ClearScreen` | Clear the current sink when supported |

```cpp
Stream::Out::Console
    << "Decimal: " << 48879 << Stream::Endl
    << Stream::Hexadecimal << Stream::Uppercase
    << "Hex: " << Stream::Width{ 8 } << Stream::Fill{ '0' }
    << 0xBEEF << Stream::Endl
    << Stream::Decimal
    << "Pi: " << Stream::Precision{ 3 } << 3.141592 << Stream::Endl;
```

Formatting state remains active until changed. Components that temporarily change it should use `SaveState()` and `RestoreState()`; `TraceEntry` and `HexDump` already do this.

## Hex dumps

`HexDump` renders bytes with optional addresses and ASCII:

```cpp
Array<Foundation::Byte, 8> Bytes{};

for (Foundation::Size Index{}; Index < Bytes.Size(); ++Index)
{
    Bytes[Index] = Foundation::Cast::Auto<Foundation::Byte>(Index);
}

Stream::Out::Serial
    << Stream::HexDump(Bytes.Data(), Bytes.Size(), Bytes.Data())
        .BytesPerLine(8)
        .ShowAddress(true)
        .ShowAscii(true);
```

The base address is only used for display. It does not change where bytes are read from.

## Console input

The initialized global input stream is `Stream::In::Console`. It reads extended UEFI key data through `SimpleTextInputEx` and exposes events for keys, characters, and complete lines.

```cpp
String Name{};

Stream::Out::Console
    << "Name: " << Stream::FlushNow;

Stream::In::Console >> Name;

Stream::Out::Console
    << "Hello, " << Name << Stream::Endl;
```

Useful calls are:

- `ReadKey()` and `GetKey()` for `Protocols::KeyData` without echo;
- `ReadKeyEcho()` for an echoed key;
- `ReadCharacter()` and `ReadCharacterEcho()` for Unicode-capable character keys;
- `ReadLine(String&)` or `operator>>(String&)` for an edited line with backspace support;
- `OnKey`, `OnCharacter`, and `OnLine` for event subscribers.

These operations wait on firmware console input. Check `UEFI::Context::HasConsoleIn()` before using the global in a custom startup path.

## Paths

`FileSystem::Path` owns a narrow path and understands UEFI-style backslash separators:

```cpp
FileSystem::Path Raw{ R"(\EFI//Tools\.\Logs\..\app.log)" };
const auto Path = Raw.Normalized();

Stream::Out::Console
    << "File: " << Path.FileName() << Stream::Endl
    << "Stem: " << Path.Stem() << Stream::Endl
    << "Extension: " << Path.Extension() << Stream::Endl;
```

`Normalized()` returns a copy; `Normalize()` changes the current object. Normalization converts `/` to `\`, collapses separators, removes `.`, resolves `..`, and removes trailing separators except for root.

Other useful operations are `Components`, `Join`, `Parent`, `NameCount`, `ReplaceExtension`, `RemoveExtension`, and `RelativeTo`.

## Files

### Stream interface

Use `FileOutputStream` for formatted text and `FileInputStream` for buffered reads:

```cpp
const FileSystem::Path Path{ "settings.txt" };

{
    Stream::FileOutputStream Output{ Path };

    if (!Output.Valid())
    {
        return MakeUnexpected(UEFI::StatusCode::DeviceError);
    }

    Output
        << "mode=debug" << Stream::Endl
        << "attempts=" << 3 << Stream::Endl;

    if (!Output.Flush())
    {
        return MakeUnexpected(UEFI::StatusCode::DeviceError);
    }
}

Stream::FileInputStream Input{ Path };

if (!Input.Valid())
{
    return MakeUnexpected(UEFI::StatusCode::NotFound);
}

String Line{};

while (Input.ReadLine(Line))
{
    Stream::Out::Console << Line << Stream::Endl;
}
```

`FileInputStream` loads file contents into an internal byte vector. It supports `Size`, `Tell`, `Seek`, `Rewind`, `Peek`, `Get`, raw `Read`, `ReadLine`, `ReadToken`, and extraction operators. Views or pointers into its `Buffer()` remain tied to the stream's buffer lifetime.

`FileOutputStream` uses a `FileOutputSink` and `Output` formatter. `Open` accepts a mode and an `AutoFlush` choice. Call `Flush()` whenever persistence before destruction matters.

### Raw file interface

`FileSystem::File` gives direct byte-oriented access:

```cpp
FileSystem::File File{};

if (!File.Open(
    FileSystem::Path{ "payload.bin" },
    FileSystem::FileOpenMode::Read
))
{
    return MakeUnexpected(UEFI::StatusCode::NotFound);
}

Vector<Foundation::Uint8> Data{};

if (!File.Read(Data))
{
    return MakeUnexpected(UEFI::StatusCode::DeviceError);
}
```

Open modes are `Read`, `Write`, `Create`, `ReadWrite`, and `CreateReadWrite`. The class owns its firmware file handle and closes it on destruction. It is movable but not copyable.

`Open` uses the current image's file system. `Open(Path, FsHandle, Mode)` targets a specific file-system handle. `OpenAny` searches available simple file systems.

The raw-buffer `Read(Uint8*&, Uint64&)` allocates through the file's allocator. Release that buffer with `FreeReadBuffer`, not an unrelated allocator.

## File metadata

After a successful open, `File::Info()` exposes `FileInfo`:

```cpp
const auto& Info = File.Info();

Stream::Out::Console
    << "Name: " << Info.FileName() << Stream::Endl
    << "Size: " << Info.Size() << Stream::Endl
    << "Directory: " << Info.IsDirectory() << Stream::Endl;
```

`FileInfo` contains the path, logical and physical sizes, creation/modification/access times, and `FileAttributes`. `FileAttributes` can query, set, clear, or toggle `ReadOnly`, `Hidden`, `System`, `Directory`, and `Archive` bits.

`FileSystem::Time::Valid()` checks the component ranges. It stores calendar fields but does not perform timezone or duration arithmetic.

## Common mistakes

- Do not assume a console exists in a driver. Prefer serial output for driver diagnostics.
- Check `Valid()` immediately after constructing a file stream.
- Treat `Flush()` as fallible.
- A relative path is resolved by the selected firmware file-system context, not the host operating system.
- Do not free a raw file-read buffer with `delete[]`; use `FreeReadBuffer`.
- A `StringView` returned by `Path::FileName`, `Extension`, or `Stem` refers to the path's owned string and is invalidated by path mutation or destruction.

See [Streams reference](../reference/streams.md) and [File-system reference](../reference/filesystem.md) for the complete type list.
