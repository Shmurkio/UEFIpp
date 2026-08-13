#pragma once

#include <intrin.h>

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Stream/Stream.hpp>

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

template <typename TSink>
auto operator<<(Stream::Output<TSink> &Stream, const TraceEntry &Entry)
    -> Stream::Output<TSink> & {
  const auto State = Stream.SaveState();

  Stream << '[' << Entry.Module << '|' << Entry.Source.Function << ':'
         << Stream::Decimal << Entry.Source.Line << ", 0x"
         << Entry.InstructionAddress << ", RIP: 0x" << Entry.ReturnAddress
         << "] ";

  Stream.RestoreState(State);
  return Stream;
}
} // namespace UEFIpp::Diagnostics

#define Trace()                                                                \
  ::UEFIpp::Diagnostics::TraceEntry {                                          \
    UEFIPP_TRACE_STRINGIZE(UEFIPP_TRACE_MODULE), UEFIPP_SOURCE_LOCATION(),     \
        ::UEFIpp::Diagnostics::CurrentInstructionAddress(), _ReturnAddress()   \
  }
