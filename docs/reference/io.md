# I/O reference

Focused umbrella: `UEFIpp/IO/IO.hpp`.

## Core

| Header | Public surface |
| --- | --- |
| `IO/Core/Error.hpp` | `Operation`, `ErrorCode`, `Error`, `Result<T>`, `ReadCount` |
| `IO/Core/Concepts.hpp` | `OutputSink`, `InputSource`, `Flushable`, `Closable`, `Seekable`, `Truncatable`, `TerminalOutput` |
| `IO/Core/Operations.hpp` | `WriteAll`, `ReadExact`, capability-aware `Flush` operation object and `Close` |
| `IO/Core/WriterRef.hpp`, `ReaderRef.hpp` | Borrowed allocation-free type erasure |
| `IO/Core/AnyIO.hpp` | Owning move-only inline `AnyWriter` and `AnyReader` |
| `IO/Core/Options.hpp` | Atomic cancellation token and wait options |

`WriteSome` returns accepted bytes. `ReadSome` returns byte count plus end-of-input state. Zero progress without end-of-input is an error in the exact/all operations.

## Adapters

`BufferedSink`, `BufferedSource`, `NewlineSink`, `TeeSink`, `PrefixSink`, `HashingSink`, `RateLimitedSink`, `AnsiTerminalSink`, `FaultInjectingSink`, `RingBufferSink`, `FixedBufferSink`, `MemorySink`, `MemorySource`, `CountingSink`, and `NullSink` live below `IO/Adapter`.

Adapters own their nested backend unless they are instantiated with a capability such as `WriterRef`.

## Text

| Header | Public surface |
| --- | --- |
| `IO/Text/Utf.hpp` | Incremental UTF-8 decoder/steps, UTF-8/16 encoders, validated owning conversions |
| `IO/Text/Writer.hpp` | UTF-8 byte writing helpers |
| `IO/Text/Format.hpp` | `FormatString`, `FormatSpec`, `Formatter<T>`, `Print`, `Println`, `FormatTo`, `Format`, `VPrint`, `HexDump` |
| `IO/Text/Reader.hpp` | Bounded `TextReader`, `ReadAll` |
| `IO/Text/Scan.hpp` | `ParseOptions`, customizable `Parser<T>`, typed `Parse`, single/multiple `Scan` |
| `IO/Text/Operators.hpp` | Result-aware `Out`/`OutputChain`, `Formatted`, `Quoted`, `Newline`, chainable `Flush`, typed `Read<T...>`, transactional `Into(...)` |

Formatting is stateless. `FormatString<T...>` is immediate/compile-time checked. `FormatArgument` is borrowed runtime type erasure and must not outlive its value. User formatters implement:

```cpp
template <>
struct IO::Formatter<MyType>
{
    [[nodiscard]] auto Format(IO::WriterRef Writer,
                              const MyType& Value,
                              const IO::FormatSpec& Spec) const
        -> IO::Result<>;
};
```

The optional operator facade retains the same error model. `IO::Out(Sink) <<`
fragments returns a `[[nodiscard]]` result-like chain whose failure belongs only
to that expression, never to `Sink`. `Reader >> IO::Read<T...>` returns values;
`Reader >> IO::Into(destinations...)` commits destinations only after all
parsing succeeds. Seekable readers restore their position on failure. There are
no persistent format flags and no incremental `Reader >> a >> b` extraction.

## Transports

- `ConsoleSink`: validated UTF-8 to UEFI UTF-16, styles, clearing, cursor, visibility, geometry.
- `SerialSink`: raw bytes, bounded UART polling, ANSI clear.
- `FileSource`: open, partial read, seek, tell, close.
- `FileSink`: open, current/append write, positioned write, seek, tell, truncate, flush, close.

## Terminal

`Terminal` exposes `KeyEvent`, key codes/modifiers, polling, waits with deadline/cancellation, output-sink operations, styles, geometry, cursor control, and coroutine awaiting. `LineEditor` provides editable UTF-8 lines, history, completion, masking, multiline input, and bounded storage. `EventLoop` owns a fixed array of event/coroutine registrations.

## Logging and context

`Logger` supports multiple `WriterRef` sinks, minimum severity, module name, timestamp source, compile-time severity calls, format messages, and named structured fields. `UEFIPP_LOG` captures the source location. `PanicWriter` is a no-allocation, no-lock emergency path.

`IoContext` owns the standard console and serial pipelines, terminal, event loop,
and logger. Use `SystemIO()` for the initialized process-wide instance and
`SystemIO().Flush()` to publish both buffered writers. The checked-in image
entry adapters flush after `Main`. Call `UEFI::Context::PrepareExitBootServices()`
before firmware exit and `ExitBootServicesSucceeded()` only after it succeeds.
