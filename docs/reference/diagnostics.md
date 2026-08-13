# Diagnostics reference

Ordinary diagnostics use `IO::Logger`; `UEFIpp::Diagnostics` additionally provides low-level instruction/return-address capture.

## Structured logging

`IO::SystemIO().Log()` is initialized with serial output, a cycle timestamp source, and module `UEFIpp`. Configure a project module and emit compile-time checked messages:

```cpp
auto& Log = IO::SystemIO().Log();
Log.SetModule(u8"ControllerDriver");
UEFIPP_LOG(Log, IO::Severity::Info, "controller {} started", Controller);
```

`Logger` supports severity filtering, multiple sinks, source locations, timestamps, and named fields. See the [I/O reference](io.md#logging-and-context).

## `Diagnostics::Trace()`

`Trace()` expands at the call site into a `TraceEntry` containing the compile-time module, function, line, current instruction address, and caller return address. It has an `IO::Formatter` specialization:

```cpp
(void)IO::Println(IO::SystemIO().Serial(), "{} controller started", Trace());
```

`UEFIpp.props` defines `UEFIPP_TRACE_MODULE` from `$(ProjectName)`. Override `UEFIppTraceModule` when its display name should differ. Captured addresses are diagnostic and are not stable identifiers across builds or relocation.

## Panic path

Assertions write through `IO::PanicWriter` before breaking and halting. This path uses no allocation or lock and is configured to direct serial output. It is deliberately smaller and more dependable than the normal structured logger.

Headers: `Diagnostics/Trace.hpp`, `Diagnostics/Diagnostics.hpp`, and `IO/Logging/Logging.hpp`.
