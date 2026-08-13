#pragma once

#include <UEFIpp/IO/Core/Operations.hpp>
#include <UEFIpp/Library/String/StringView.hpp>

namespace UEFIpp::IO {
template <OutputSink TSink>
class PrefixSink final {
public:
  constexpr PrefixSink(TSink Sink, Library::U8StringView Prefix)
      : Sink_(Foundation::Utility::Move(Sink)), Prefix_(Prefix) {}

  [[nodiscard]] auto WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size> {
    Foundation::Size Accepted{};
    for (const auto Byte : Bytes) {
      if (AtLineStart_) {
        auto Result = WriteAll(
            Sink_, {Foundation::Cast::Auto<const Foundation::Byte *>(
                        Prefix_.Data()),
                    Prefix_.Size()});
        if (!Result) return Failure(Result.Error());
        AtLineStart_ = false;
      }
      auto WriteResult = WriteAll(Sink_, {&Byte, 1});
      if (!WriteResult) {
        if (Accepted) return Accepted;
        return Failure(WriteResult.Error());
      }
      ++Accepted;
      AtLineStart_ = Byte == '\n';
    }
    return Accepted;
  }
  [[nodiscard]] auto Flush() -> Result<> { return IO::Flush(Sink_); }
  [[nodiscard]] auto Close() -> Result<> { return IO::Close(Sink_); }

private:
  TSink Sink_;
  Library::U8StringView Prefix_;
  Foundation::Bool AtLineStart_{true};
};

template <OutputSink TSink>
class HashingSink final {
public:
  constexpr explicit HashingSink(TSink Sink)
      : Sink_(Foundation::Utility::Move(Sink)) {}
  [[nodiscard]] auto WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size> {
    auto Result = Sink_.WriteSome(Bytes);
    if (!Result) return Result;
    for (Foundation::Size Index{}; Index < Result.Value(); ++Index) {
      Hash_ ^= Bytes[Index];
      Hash_ *= 1099511628211ull;
    }
    return Result;
  }
  [[nodiscard]] auto Flush() -> Result<> { return IO::Flush(Sink_); }
  [[nodiscard]] auto Close() -> Result<> { return IO::Close(Sink_); }
  [[nodiscard]] constexpr auto Hash() const noexcept -> Foundation::Uint64 {
    return Hash_;
  }

private:
  TSink Sink_;
  Foundation::Uint64 Hash_{14695981039346656037ull};
};

template <OutputSink TSink>
class FaultInjectingSink final {
public:
  constexpr FaultInjectingSink(TSink Sink, Foundation::Size FailAfter)
      : Sink_(Foundation::Utility::Move(Sink)), Remaining_(FailAfter) {}
  [[nodiscard]] auto WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size> {
    if (!Remaining_) {
      return Failure(
          Error::Semantic(ErrorCode::Firmware, Operation::Write));
    }
    const auto Limited = Bytes.Size() < Remaining_ ? Bytes.Size() : Remaining_;
    auto Result = Sink_.WriteSome(Bytes.First(Limited));
    if (Result) Remaining_ -= Result.Value();
    return Result;
  }
  [[nodiscard]] auto Flush() -> Result<> { return IO::Flush(Sink_); }
  [[nodiscard]] auto Close() -> Result<> { return IO::Close(Sink_); }

private:
  TSink Sink_;
  Foundation::Size Remaining_{};
};

template <Foundation::Size Capacity>
class RingBufferSink final {
public:
  static_assert(Capacity != 0);
  [[nodiscard]] constexpr auto
  WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size> {
    for (const auto Byte : Bytes) {
      Buffer_[(Start_ + Size_) % Capacity] = Byte;
      if (Size_ < Capacity) ++Size_;
      else Start_ = (Start_ + 1) % Capacity;
    }
    return Bytes.Size();
  }
  template <OutputSink TSink>
  [[nodiscard]] auto Replay(TSink &Sink) const -> Result<> {
    for (Foundation::Size Index{}; Index < Size_; ++Index) {
      const auto Byte = Buffer_[(Start_ + Index) % Capacity];
      auto Result = WriteAll(Sink, {&Byte, 1});
      if (!Result) return Result;
    }
    return {};
  }
  [[nodiscard]] constexpr auto Size() const noexcept -> Foundation::Size {
    return Size_;
  }
  constexpr auto Clear() noexcept -> Foundation::Void {
    Start_ = 0;
    Size_ = 0;
  }

private:
  Library::Array<Foundation::Byte, Capacity> Buffer_{};
  Foundation::Size Start_{};
  Foundation::Size Size_{};
};

template <OutputSink TSink>
class RateLimitedSink final {
public:
  using ClockFunction = auto (*)(Foundation::Void *) -> Foundation::Uint64;

  constexpr RateLimitedSink(TSink Sink, Foundation::Size BytesPerWindow,
                            Foundation::Uint64 WindowTicks,
                            Foundation::Void *ClockContext,
                            ClockFunction Clock) noexcept
      : Sink_(Foundation::Utility::Move(Sink)),
        BytesPerWindow_(BytesPerWindow), Remaining_(BytesPerWindow),
        WindowTicks_(WindowTicks), ClockContext_(ClockContext), Clock_(Clock) {}

  [[nodiscard]] auto WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size> {
    if (Bytes.Empty()) return Foundation::Size{};
    const auto Now = Clock_ ? Clock_(ClockContext_) : Foundation::Uint64{};
    if (!Started_) {
      WindowStart_ = Now;
      Started_ = true;
    } else if (WindowTicks_ && Now - WindowStart_ >= WindowTicks_) {
      WindowStart_ = Now;
      Remaining_ = BytesPerWindow_;
    }
    if (!Remaining_) {
      return Failure(
          Error::Semantic(ErrorCode::WouldBlock, Operation::Write));
    }
    const auto Count = Bytes.Size() < Remaining_ ? Bytes.Size() : Remaining_;
    auto Result = Sink_.WriteSome(Bytes.First(Count));
    if (Result) Remaining_ -= Result.Value();
    return Result;
  }
  [[nodiscard]] auto Flush() -> Result<> { return IO::Flush(Sink_); }
  [[nodiscard]] auto Close() -> Result<> { return IO::Close(Sink_); }

private:
  TSink Sink_;
  Foundation::Size BytesPerWindow_{};
  Foundation::Size Remaining_{};
  Foundation::Uint64 WindowTicks_{};
  Foundation::Uint64 WindowStart_{};
  Foundation::Void *ClockContext_{};
  ClockFunction Clock_{};
  Foundation::Bool Started_{};
};

enum class AnsiColor : Foundation::Uint8 {
  Black,
  Red,
  Green,
  Yellow,
  Blue,
  Magenta,
  Cyan,
  White,
  Default
};

struct AnsiStyle final {
  AnsiColor Foreground{AnsiColor::Default};
  AnsiColor Background{AnsiColor::Default};
  Foundation::Bool Bright{};
  Foundation::Bool Underline{};
};

template <OutputSink TSink>
class AnsiTerminalSink final {
public:
  constexpr explicit AnsiTerminalSink(TSink Sink)
      : Sink_(Foundation::Utility::Move(Sink)) {}

  [[nodiscard]] auto WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size> {
    return Sink_.WriteSome(Bytes);
  }
  [[nodiscard]] auto Flush() -> Result<> { return IO::Flush(Sink_); }
  [[nodiscard]] auto Close() -> Result<> { return IO::Close(Sink_); }
  [[nodiscard]] constexpr auto Sink() noexcept -> TSink & { return Sink_; }
  [[nodiscard]] auto ClearTerminal() -> Result<> {
    constexpr Foundation::Byte Sequence[]{0x1B, '[', '2', 'J',
                                           0x1B, '[', 'H'};
    return WriteAll(Sink_, Sequence);
  }
  [[nodiscard]] auto ResetStyle() -> Result<> {
    constexpr Foundation::Byte Sequence[]{0x1B, '[', '0', 'm'};
    return WriteAll(Sink_, Sequence);
  }
  [[nodiscard]] auto SetStyle(AnsiStyle Style) -> Result<> {
    Foundation::Byte Sequence[32]{0x1B, '['};
    Foundation::Size Length{2};
    const auto AppendNumber = [&](Foundation::Uint8 Value) {
      if (Value >= 10) Sequence[Length++] = '0' + Value / 10;
      Sequence[Length++] = '0' + Value % 10;
    };
    AppendNumber(Style.Bright ? 1 : 22);
    Sequence[Length++] = ';';
    AppendNumber(Style.Underline ? 4 : 24);
    Sequence[Length++] = ';';
    AppendNumber(Style.Foreground == AnsiColor::Default
                     ? 39
                     : Foundation::Cast::Auto<Foundation::Uint8>(30) +
                           Foundation::Cast::Underlying(Style.Foreground));
    Sequence[Length++] = ';';
    AppendNumber(Style.Background == AnsiColor::Default
                     ? 49
                     : Foundation::Cast::Auto<Foundation::Uint8>(40) +
                           Foundation::Cast::Underlying(Style.Background));
    Sequence[Length++] = 'm';
    return WriteAll(Sink_, {Sequence, Length});
  }

private:
  TSink Sink_;
};
} // namespace UEFIpp::IO
