#pragma once

#include <UEFIpp/Protocols/SimpleTextOutput.hpp>
#include <UEFIpp/Stream/Output/Output.hpp>

namespace UEFIpp::Stream {
class ConsoleOutputSink {
public:
  static constexpr auto BufferCapacity = Foundation::Size{128};

  constexpr ConsoleOutputSink() = default;

  constexpr explicit ConsoleOutputSink(Protocols::SimpleTextOutput *Output)
      : Output_(Output) {}

  auto Put(Foundation::WChar Character) -> void;

  auto Backspace() -> void;

  auto Write(const Foundation::Char *Data, Foundation::Size Length) -> void;

  auto Write(const Foundation::WChar *Data, Foundation::Size Length) -> void;

  auto Clear() -> void;

private:
  Protocols::SimpleTextOutput *Output_{};
};

namespace Out {
extern Output<ConsoleOutputSink> Console;
}
} // namespace UEFIpp::Stream
