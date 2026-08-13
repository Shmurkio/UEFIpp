# Diagnostics reference

`UEFIpp::Diagnostics` currently provides structured call-site tracing.

Focused umbrella: `UEFIpp/Diagnostics/Diagnostics.hpp`.

## `Trace()`

`Trace()` expands at the call site into a `TraceEntry` containing:

- the compile-time module name;
- source function and line;
- the current instruction address;
- the caller return address.

Write it to any UEFIpp output stream:

```cpp
Stream::Out::Serial
    << Trace() << "Controller started" << Stream::Endl;
```

`operator<<` temporarily changes formatting to render addresses and restores the stream afterward.

## Automatic module names

`UEFIpp.props` defines `UEFIPP_TRACE_MODULE` from `$(ProjectName)`. Because each translation unit expands the macro itself, traces automatically identify the application, driver, or library project that contains the call.

Override `UEFIppTraceModule` in a project property group if its display name should differ from the project name. The fallback module is `UnknownModule` if the property sheet was not imported.

## Performance and safety

Trace entries capture return addresses through MSVC intrinsics and are primarily diagnostic. Avoid treating captured addresses as stable identifiers across builds or relocations.

Serial output is suitable for drivers that cannot assume a console. Like any I/O, excessive tracing can affect timing. Keep high-frequency traces behind appropriate project/configuration policy.

## Headers

- `Trace.hpp` defines `TraceEntry`, address capture, stream formatting, and the macro.
- `Diagnostics.hpp` is the focused umbrella.

See [Applications and drivers](../entry-points.md#trace-module-names) for project configuration.
