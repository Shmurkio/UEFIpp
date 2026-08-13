#pragma once

#include <UEFIpp/IO/Core/Operations.hpp>
#include <UEFIpp/IO/Text/Utf.hpp>

namespace UEFIpp::IO {
template <OutputSink TSink>
[[nodiscard]] auto WriteText(TSink &Sink, Library::U8StringView Text)
    -> Result<> {
  return WriteAll(Sink, AsBytes(Text));
}

template <OutputSink TSink>
[[nodiscard]] auto WriteText(TSink &Sink, Library::StringView Utf8Text)
    -> Result<> {
  return WriteText(Sink, AsUtf8(Utf8Text));
}
} // namespace UEFIpp::IO
