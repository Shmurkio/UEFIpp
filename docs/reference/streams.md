# Streams reference

`UEFIpp::Stream` provides buffered output, interactive input, console/serial/file adapters, formatting manipulators, and file streams.

Focused umbrella: `UEFIpp/Stream/Stream.hpp`.

For practical examples, read [Streams, console input, and files](../guides/streams-and-files.md).

## Output model

`Output<TSink>` owns a sink and a 256-byte narrow output buffer. It supports direct `Put` and `Write`, signed/unsigned integer rendering, pointers, GUIDs, floating-point formatting, sink replacement, flush, clear, and formatting-state save/restore.

A sink must provide compatible narrow/wide `Write` and `Clear` operations. The supplied sinks are:

- `ConsoleOutputSink`, backed by `SimpleTextOutput`;
- `SerialSink`, backed by an I/O serial port;
- `FileOutputSink`, backed by `FileSystem::File`.

The initialized globals are `Out::Console` and `Out::Serial`.

## Output values

Overloads support:

- C strings, characters, Booleans, integral and floating values;
- `BasicString<TChar>` and `StringView`;
- wide text and `U16String`;
- pointers and `UEFI::Guid`;
- manipulators, `HexDump`, and `Diagnostics::TraceEntry`.

Narrow newlines are converted to firmware-style CRLF. Wide writes are flushed directly to the sink.

## Formatting state

`Output::State` contains `Base`, `HexCase`, width, fill, and precision. `SaveState`/`RestoreState` let formatters avoid leaking local choices.

Manipulators are defined in `Formatting/Common.hpp`: `Endl`, `FlushNow`, `Hexadecimal`, `Decimal`, `Uppercase`, `Lowercase`, `Width`, `Fill`, `Precision`, and `ClearScreen`.

Width applies to hexadecimal rendering and is reset after the value. Base, case, fill, and precision persist.

## Hex dumps

`HexDump` is a non-owning description of a byte region. It stores data, byte count, optional display base, bytes per line, and address/ASCII visibility. Its fluent setters return the dump by reference.

The data must remain valid for the duration of output.

## Input model

`Input<TSource>` owns a source and exposes:

- `ReadKey`, `ReadKeyEcho`, and `GetKey`;
- `ReadCharacter`, `ReadCharacterEcho`, and `Get`;
- `ReadLine` and extraction operators;
- `OnKey`, `OnCharacter`, and `OnLine` events;
- source and allocator access.

`ConsoleInputSource` uses boot services and `SimpleTextInputEx` to wait for keys and optionally echo them. The initialized global is `In::Console`.

Line input handles carriage return/newline and backspace. Allocation failure while extending a line currently yields the best partial string because `ReadLine` has no error return; reserve an expected maximum size in advance when truncation is unacceptable.

## File output

`FileOutputStream` owns a `FileSystem::File`, a `FileOutputSink`, and its formatter. It supports construction/open with path, mode, auto-flush, and allocator; `Valid`, `Write`, `Flush`, `Close`, and `operator<<`.

It is intended for formatted or raw output. Always inspect `Valid` and `Flush` when correctness depends on persistence.

## File input

`FileInputStream` owns a file and an in-memory byte vector. It supports:

- `Open`, `Close`, `Valid`, Boolean conversion, `Empty`, and `Eof`;
- `Size`, `Tell`, `Seek`, and `Rewind`;
- `Peek`, `Get`, raw `Read`, `ReadLine`, and `ReadToken`;
- access to the file, buffer, and allocator;
- extraction operators and free `GetLine`.

Opening reads the file into memory, so `Size` and seeking apply to the buffered contents. A large file requires enough pool memory for the full contents.

## Header map

| Header | Contents |
| --- | --- |
| `Formatting/Common.hpp` | State enums and manipulators |
| `Formatting/HexDump.hpp` | Configurable byte dump |
| `Output/Output.hpp` | Generic buffered formatter |
| `Output/ConsoleSink.hpp` | Firmware console sink and global |
| `Output/SerialSink.hpp` | Serial-port sink and global |
| `Output/FileSink.hpp` | File-backed sink |
| `Input/Input.hpp` | Generic input and events |
| `Input/ConsoleSource.hpp` | Firmware console source and global |
| `File/FileInputStream.hpp` | Buffered file input |
| `File/FileOutputStream.hpp` | Formatted/raw file output |

Every path is listed in the [public-header index](header-index.md#stream).
