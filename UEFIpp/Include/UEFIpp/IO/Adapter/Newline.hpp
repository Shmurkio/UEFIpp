#pragma once

#include <UEFIpp/IO/Core/Operations.hpp>

namespace UEFIpp::IO {
enum class Newline : Foundation::Uint8 { Preserve, LineFeed, CarriageReturnLineFeed };

template <OutputSink TSink, Newline Policy>
class NewlineSink final {
public:
  constexpr NewlineSink() = default;
  constexpr explicit NewlineSink(TSink Sink)
      : Sink_(Foundation::Utility::Move(Sink)) {}

  [[nodiscard]] constexpr auto Sink() noexcept -> TSink & { return Sink_; }

  [[nodiscard]] auto WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size> {
    Foundation::Size Accepted{};

    for (Foundation::Size Index{}; Index < Bytes.Size(); ++Index) {
      const auto Byte = Bytes[Index];
      if constexpr (Policy == Newline::CarriageReturnLineFeed) {
        if (Byte == '\n' && !PreviousWasCarriageReturn_) {
          const Foundation::Byte CarriageReturn{'\r'};
          auto WriteResult = WriteAll(Sink_, {&CarriageReturn, 1});
          if (!WriteResult) {
            if (Accepted) {
              return Accepted;
            }
            return Failure(WriteResult.Error());
          }
        }
      } else if constexpr (Policy == Newline::LineFeed) {
        if (PendingCarriageReturn_) {
          const Foundation::Byte LineFeed{'\n'};
          auto WriteResult = WriteAll(Sink_, {&LineFeed, 1});
          if (!WriteResult) {
            if (Accepted) {
              return Accepted;
            }
            return Failure(WriteResult.Error());
          }
          PendingCarriageReturn_ = false;
          if (Byte == '\n') {
            ++Accepted;
            continue;
          }
        }

        if (Byte == '\r') {
          PendingCarriageReturn_ = true;
          ++Accepted;
          continue;
        }
      }

      Foundation::Byte Output = Byte;
      auto WriteResult = WriteAll(Sink_, {&Output, 1});
      if (!WriteResult) {
        if (Accepted) {
          return Accepted;
        }
        return Failure(WriteResult.Error());
      }
      PreviousWasCarriageReturn_ = Byte == '\r';
      ++Accepted;
    }

    return Accepted;
  }

  [[nodiscard]] auto Flush() -> Result<> {
    if constexpr (Policy == Newline::LineFeed) {
      if (PendingCarriageReturn_) {
        const Foundation::Byte LineFeed{'\n'};
        auto Result = WriteAll(Sink_, {&LineFeed, 1});
        if (!Result) {
          return Result;
        }
        PendingCarriageReturn_ = false;
      }
    }
    return IO::Flush(Sink_);
  }

  [[nodiscard]] auto Close() -> Result<> {
    auto Result = Flush();
    if (!Result) return Result;
    return IO::Close(Sink_);
  }

private:
  TSink Sink_{};
  Foundation::Bool PreviousWasCarriageReturn_{};
  Foundation::Bool PendingCarriageReturn_{};
};
} // namespace UEFIpp::IO
