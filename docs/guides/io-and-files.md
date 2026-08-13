# Modern I/O, terminals, logging, and files

UEFIpp I/O is a composable byte-and-text system designed for freestanding C++, not a stateful `iostream` clone. The complete umbrella is `UEFIpp/IO/IO.hpp`.

## Standard context

`UEFI::Context::Normalize` initializes the process-wide context:

```cpp
auto& Io = IO::SystemIO();

auto ConsoleResult = IO::Println(Io.Console(), "image={}, count={}",
                                 UEFI::Context::ImageHandle(), 3);
UEFIPP_LOG(Io.Log(), IO::Severity::Info, "initialization complete");
```

`Console()` and `Serial()` are independently buffered UTF-8 writers with LF-to-CRLF conversion. `StandardOutput()` and `StandardError()` return borrowed `WriterRef` capabilities. `StandardInput()` returns the typed `Terminal`.

The checked-in application and driver entry adapters flush both buffered
writers after `Main`. Call `SystemIO().Flush()` directly, or append `IO::Flush`
to an output chain, when data must be visible earlier.

Results are never stored as hidden stream state. Check, propagate, or intentionally discard each `IO::Result`.

## Errors and partial transfers

An `IO::Error` contains:

- `Code`: semantic category such as `Closed`, `Timeout`, `InvalidEncoding`, or `Overflow`;
- `During`: `Open`, `Read`, `Write`, `Flush`, `Seek`, `Format`, and so on;
- `Status`: the exact `EFI_STATUS` when firmware produced the failure;
- `Offset` and `Transferred`: location and successfully transferred byte count.

Backends implement `WriteSome` or `ReadSome`; partial transfers are valid. `WriteAll` and `ReadExact` loop safely and reject a backend that makes no progress.

## Formatting

`Print` and `Println` use checked automatic replacement fields:

```cpp
auto Result = IO::Println(Io.Console(),
    "base={:#010X}, signed={:+8}, ratio={:.3f}, text={:?}",
    Address, Delta, Ratio, Name);
```

Format strings validate braces, argument count, and specification grammar at compile time. Formatting state never leaks between calls. Supported built-ins include integers, floating point, pointers, UTF strings/views, enums, `Optional`, GUIDs, UEFI statuses, I/O errors, ranges, tuples, and `HexDump`. Specialize `IO::Formatter<T>` for a project type.

Important specifications include `<`, `>`, and `^` alignment; fill/width; `+`/space signs; `#` alternate form; zero padding; precision; integer `b`, `o`, `x`, `X`; floating `f`, `e`, `g`, `a`; pointer `p`; and debug text `?` with quoting and escaping.

For a runtime format string, create non-owning arguments and call `VPrint`:

```cpp
const auto Count = Foundation::Uint32{7};
const IO::FormatArgument Args[]{IO::MakeFormatArgument(Count)};
auto Result = IO::VPrint(Io.Console(), "count={}", Args);
```

Arguments must outlive `VPrint`. `Format` produces an owning `U8String`; `FormatTo` writes to any sink without an intermediate string.

## Result-aware operators

`IO::Out` is an opt-in `<<` facade over the same `Formatter<T>` and writer
operations used by `Print`. It does not add formatting or error state to the
underlying sink:

```cpp
auto Output = IO::Out(Io.Console())
              << "address=" << IO::Formatted("{:#018X}", Address)
              << ", name=" << IO::Quoted(Name)
              << IO::Newline << IO::Flush;

if (!Output)
    return IO::Failure(Output.Error());
```

The short-lived `OutputChain` is `[[nodiscard]]`, retains the first error, and
does not attempt later fragments after a failure. Ordinary fragments use their
`Formatter<T>` with a default `FormatSpec`; `Formatted` applies one checked
format string to one value, `Quoted` selects debug quoting and escaping, and
`Newline` writes a UTF-8 line feed. `Flush` explicitly publishes buffered data;
when used in an operator chain, any flush failure becomes the chain's ordinary
error result. `TakeResult()` moves the final status into a regular
`IO::Result<>` when an API requires that type. No manipulator is sticky: every
fragment starts with fresh formatting state.

Newline and flushing are intentionally separate. Use `IO::Newline` when
batching several lines, and append `IO::Flush` when output must be externally
visible immediately.

Typed input uses a single atomic request rather than incrementally mutating
destinations:

```cpp
auto Values = Reader >> IO::Read<Foundation::Int32, Foundation::Bool>;
if (!Values)
    return IO::Failure(Values.Error());

Foundation::Uint32 Count{};
Foundation::Bool Enabled{};
auto Assigned = Reader >> IO::Into(Count, Enabled);
```

`Read<T>` returns `Result<T>` and `Read<T...>` returns a `Result<Tuple<T...>>`.
Use `IO::Read<T...>.With(ParseOptions)` or
`IO::Into(values...).With(ParseOptions)` for explicit parsing policy. `Into`
parses every field into temporary storage and changes its destinations only
after complete success. A seekable source is also restored on failure; a
non-seekable source may have consumed input, but destinations remain unchanged.

Classic `Reader >> First >> Second` is intentionally absent. C++ cannot detect
the end of that chain in a way that both delays destination mutation and returns
an explicit error. `Read<T...>` and `Into(...)` preserve those guarantees while
still providing concise operator syntax.

## UTF text

UTF-8 is the text boundary for I/O and application arguments. `Utf8Decoder` is incremental and validates overlong sequences, surrogates, and the Unicode maximum. Each `Utf8DecodeStep` says whether the input byte was consumed, so replacement mode preserves a valid byte that follows a malformed prefix. Invalid input can be rejected or replaced.

Use `Utf8ToUtf16`, `Utf16ToUtf8`, `Utf8ToWide`, and `WideToUtf8` at firmware boundaries. They return `IO::Result` and never silently collapse Unicode to ASCII.

`TextReader` layers bounded, validated code-point, token, and line reads over any source. `Parse<T>` handles integral, floating (including explicit infinity/NaN), Boolean, enum, GUID, and UTF-8 values with strict syntax and overflow checks. Specialize `Parser<T>` for project types. Multi-value `Scan<T...>` returns a `Tuple<T...>` and restores seekable input if any conversion fails.

## Transports and files

`ConsoleSink`, `SerialSink`, `FileSink`, and `FileSource` are direct transports. Console output incrementally transcodes UTF-8 to firmware UTF-16. Serial output has bounded polling. File I/O supports partial reads/writes, explicit current/append mode, seeking, telling, truncation, flushing, and closing.

```cpp
FileSystem::Path Path{"\\EFI\\UEFIpp\\settings.txt"};
IO::FileSink File{};

if (auto Open = File.Open(Path); !Open)
    return MakeUnexpected(Open.Error().Status.Code());

if (auto Write = IO::Println(File, "mode={}, attempts={}", "debug", 3);
    !Write)
    return MakeUnexpected(Write.Error().Status.Code());

if (auto Flush = File.Flush(); !Flush)
    return MakeUnexpected(Flush.Error().Status.Code());
```

`FileSystem::File::LastStatus()` preserves the last exact firmware result used by higher-level file operations.

## Adapters

Adapters are ordinary templates and may be nested:

- `BufferedSink` and `BufferedSource` add bounded buffering, lookahead, and seeking reset;
- `NewlineSink` normalizes LF, CRLF, or native newline policy;
- `TeeSink`, `PrefixSink`, and `HashingSink` duplicate, decorate, or hash output;
- `RateLimitedSink` applies a clock-driven byte budget;
- `AnsiTerminalSink` adds ANSI clearing and style control;
- `FixedBufferSink`, `MemorySink`, `MemorySource`, `NullSink`, and `CountingSink` support formatting, tests, and measurement;
- `FaultInjectingSink` and `RingBufferSink` support failure tests and retained diagnostics.

`WriterRef` and `ReaderRef` are non-owning, allocation-free type erasure. `AnyWriter<Capacity>` and `AnyReader<Capacity>` own a move-only backend inline and reject oversized backends at compile time.

## Terminal input and line editing

`Terminal` uses extended input when available and automatically falls back to basic UEFI console input. It provides `PollKey`, blocking `WaitKey`, timeouts, cooperative cancellation, modifier state, Unicode surrogate handling, cursor/geometry control, console colors, and `NextEventAsync` for coroutine event loops.

```cpp
IO::CancellationToken Cancel;
IO::WaitOptions Wait{};
Wait.Timeout100ns = IO::Timeout::Seconds(5);
Wait.Cancellation = &Cancel;
auto Key = Io.StandardInput().WaitKey(Wait);
```

`LineEditor` adds UTF-8 boundary-aware cursor movement, insertion/deletion, history, completion, optional masking, multiline entry, byte limits, timeout, and cancellation.

`EventLoop` waits on registered UEFI events and resumes coroutines. Its storage is fixed-capacity and cancellation remains observable even when no deadline was supplied.

## Structured logging and panic output

`Logger` applies compile-time and runtime severity filtering, formats each message once, and serializes fan-out to every configured sink. Records contain severity, module, source location, cycle timestamp, message, and optional named fields.

```cpp
auto& Log = IO::SystemIO().Log();
Log.SetModule(u8"StorageDriver");
UEFIPP_LOG(Log, IO::Severity::Warning,
           "retrying device {} after {} attempts", Device, Attempts);

(void)Log.LogFields(IO::Severity::Info, UEFIPP_SOURCE_LOCATION(),
                    u8"mounted", IO::Field(u8"volume", Volume),
                    IO::Field(u8"writable", Writable));
```

Logger fan-out attempts every sink and returns the first failure after completing the rest. `PanicWriter` is separately configured for best-effort output with no allocation or lock; assertions use it before breaking and halting.

## Boot-services lifetime

Flush boot-service-backed output before the firmware call:

```cpp
if (!UEFI::Context::PrepareExitBootServices())
    return MakeUnexpected(UEFI::StatusCode::DeviceError);
```

After firmware `ExitBootServices` succeeds, immediately call:

```cpp
UEFI::Context::ExitBootServicesSucceeded();
```

The success hook performs no firmware call: it invalidates console input/output, the terminal, and the UEFI event loop, then clears the boot-services pointer. Direct serial output and panic output remain available because they do not depend on boot services. Do not use file, console, event, allocator, or protocol backends whose firmware lifetime ended.
