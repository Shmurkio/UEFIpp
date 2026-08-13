#pragma once

#include <UEFIpp/IO/Adapter/Adapter.hpp>
#include <UEFIpp/IO/Logging/Logging.hpp>
#include <UEFIpp/IO/Terminal/TerminalIO.hpp>
#include <UEFIpp/IO/Transport/Transport.hpp>

namespace UEFIpp::IO {
class IoContext final {
public:
  using ConsoleWriter =
      BufferedSink<NewlineSink<ConsoleSink, Newline::CarriageReturnLineFeed>,
                   1024>;
  using SerialWriter =
      BufferedSink<NewlineSink<SerialSink, Newline::CarriageReturnLineFeed>,
                   1024>;

  constexpr IoContext() noexcept = default;

  auto Attach(UEFI::Table::BootServices *BootServices,
              Protocols::SimpleTextInputEx *InputEx,
              Protocols::SimpleTextInput *Input,
              Protocols::SimpleTextOutput *Output,
              Foundation::Uint16 SerialPort) -> Foundation::Void;
  [[nodiscard]] auto Flush() -> Result<>;
  [[nodiscard]] auto PrepareExitBootServices() -> Result<>;
  auto ExitBootServices() noexcept -> Foundation::Void;

  [[nodiscard]] constexpr auto Console() noexcept -> ConsoleWriter & {
    return Console_;
  }
  [[nodiscard]] constexpr auto Serial() noexcept -> SerialWriter & {
    return Serial_;
  }
  [[nodiscard]] constexpr auto StandardOutput() noexcept -> WriterRef {
    return WriterRef{Console_};
  }
  [[nodiscard]] constexpr auto StandardError() noexcept -> WriterRef {
    return WriterRef{Serial_};
  }
  [[nodiscard]] constexpr auto StandardInput() noexcept -> Terminal & {
    return Terminal_;
  }
  [[nodiscard]] constexpr auto ConsoleTerminal() noexcept -> Terminal & {
    return Terminal_;
  }
  [[nodiscard]] constexpr auto Events() noexcept -> EventLoop & {
    return Events_;
  }
  [[nodiscard]] constexpr auto Log() noexcept -> Logger & { return Logger_; }
  [[nodiscard]] constexpr auto ConsoleTransport() noexcept -> ConsoleSink & {
    return Console_.Sink().Sink();
  }
  [[nodiscard]] constexpr auto SerialTransport() noexcept -> SerialSink & {
    return Serial_.Sink().Sink();
  }

private:
  ConsoleWriter Console_{};
  SerialWriter Serial_{};
  Terminal Terminal_{};
  EventLoop Events_{};
  Logger Logger_{};
};

[[nodiscard]] auto SystemIO() noexcept -> IoContext &;
} // namespace UEFIpp::IO
