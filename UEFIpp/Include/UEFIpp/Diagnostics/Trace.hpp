#pragma once

#include <intrin.h>

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/IO/Text/Format.hpp>

#ifndef UEFIPP_TRACE_MODULE
#define UEFIPP_TRACE_MODULE UnknownModule
#endif

#define UEFIPP_TRACE_STRINGIZE_INNER(Value) #Value
#define UEFIPP_TRACE_STRINGIZE(Value) UEFIPP_TRACE_STRINGIZE_INNER(Value)

namespace UEFIpp::Diagnostics {
struct TraceEntry final {
  const Foundation::Char *Module{};
  Foundation::SourceLocation Source{nullptr, nullptr, 0};
  const Foundation::Void *InstructionAddress{};
  const Foundation::Void *ReturnAddress{};
};

[[nodiscard]]
__declspec(noinline) auto CurrentInstructionAddress() noexcept
    -> const Foundation::Void *;

} // namespace UEFIpp::Diagnostics

namespace UEFIpp::IO {
template <>
struct Formatter<Diagnostics::TraceEntry> {
  [[nodiscard]] auto Format(WriterRef Writer,
                            const Diagnostics::TraceEntry &Entry,
                            const FormatSpec &) const -> Result<> {
    return Print(Writer, "[{}|{}:{}, {}, RIP: {}] ", Entry.Module,
                 Entry.Source.Function, Entry.Source.Line,
                 Entry.InstructionAddress, Entry.ReturnAddress);
  }
};
} // namespace UEFIpp::IO

#define Trace()                                                                \
  ::UEFIpp::Diagnostics::TraceEntry {                                          \
    UEFIPP_TRACE_STRINGIZE(UEFIPP_TRACE_MODULE), UEFIPP_SOURCE_LOCATION(),     \
        ::UEFIpp::Diagnostics::CurrentInstructionAddress(), _ReturnAddress()   \
  }
