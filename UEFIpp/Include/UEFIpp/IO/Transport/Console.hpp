#pragma once

#include <UEFIpp/IO/Core/Concepts.hpp>
#include <UEFIpp/IO/Text/Utf.hpp>
#include <UEFIpp/Library/Containers/Array.hpp>
#include <UEFIpp/Protocols/SimpleTextOutput.hpp>

namespace UEFIpp::IO {
enum class ConsoleColor : Foundation::Uint8 {
  Black,
  Blue,
  Green,
  Cyan,
  Red,
  Magenta,
  Brown,
  LightGray,
  DarkGray,
  LightBlue,
  LightGreen,
  LightCyan,
  LightRed,
  LightMagenta,
  Yellow,
  White
};

struct ConsoleStyle final {
  ConsoleColor Foreground{ConsoleColor::LightGray};
  ConsoleColor Background{ConsoleColor::Black};
};

struct ConsoleGeometry final {
  Foundation::UintN Columns{};
  Foundation::UintN Rows{};
  Foundation::UintN Column{};
  Foundation::UintN Row{};
};

class ConsoleSink final {
public:
  static constexpr Foundation::Size WideBufferCapacity = 128;

  constexpr ConsoleSink() noexcept = default;
  constexpr explicit ConsoleSink(
      Protocols::SimpleTextOutput *Output,
      InvalidEncodingPolicy InvalidPolicy = InvalidEncodingPolicy::Replace)
      : Output_(Output), InvalidPolicy_(InvalidPolicy) {}

  constexpr auto Reset(
      Protocols::SimpleTextOutput *Output,
      InvalidEncodingPolicy InvalidPolicy = InvalidEncodingPolicy::Replace)
      noexcept -> Foundation::Void {
    Output_ = Output;
    InvalidPolicy_ = InvalidPolicy;
    Decoder_.Reset();
    WideLength_ = 0;
  }

  [[nodiscard]] constexpr auto Valid() const noexcept -> Foundation::Bool {
    return Output_ != nullptr;
  }

  [[nodiscard]] auto WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size>;
  [[nodiscard]] auto Flush() -> Result<>;
  [[nodiscard]] auto ClearTerminal() -> Result<>;
  [[nodiscard]] auto SetAttribute(Foundation::UintN Attribute) -> Result<>;
  [[nodiscard]] auto SetStyle(ConsoleStyle Style) -> Result<>;
  [[nodiscard]] auto SetCursor(Foundation::UintN Column,
                               Foundation::UintN Row) -> Result<>;
  [[nodiscard]] auto SetCursorVisible(Foundation::Bool Visible) -> Result<>;
  [[nodiscard]] auto Geometry() const -> Result<ConsoleGeometry>;

private:
  [[nodiscard]] auto Append(Foundation::Char32 CodePoint) -> Result<>;
  [[nodiscard]] auto FlushWide() -> Result<>;

  Protocols::SimpleTextOutput *Output_{};
  InvalidEncodingPolicy InvalidPolicy_{InvalidEncodingPolicy::Replace};
  Utf8Decoder Decoder_{};
  Library::Array<Foundation::WChar, WideBufferCapacity + 1> WideBuffer_{};
  Foundation::Size WideLength_{};
};
} // namespace UEFIpp::IO
