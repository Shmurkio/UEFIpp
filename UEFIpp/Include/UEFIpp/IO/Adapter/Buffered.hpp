#pragma once

#include <UEFIpp/IO/Core/Operations.hpp>
#include <UEFIpp/Library/Containers/Array.hpp>
#include <UEFIpp/Memory/Memory.hpp>

namespace UEFIpp::IO {
template <OutputSink TSink, Foundation::Size Capacity = 1024>
class BufferedSink final {
  static_assert(Capacity != 0);

public:
  constexpr BufferedSink() = default;
  constexpr explicit BufferedSink(TSink Sink)
      : Sink_(Foundation::Utility::Move(Sink)) {}

  BufferedSink(const BufferedSink &) = delete;
  auto operator=(const BufferedSink &) -> BufferedSink & = delete;
  BufferedSink(BufferedSink &&Other) noexcept
      : Sink_(Foundation::Utility::Move(Other.Sink_)), Buffer_(Other.Buffer_),
        Length_(Other.Length_) {
    Other.Length_ = 0;
  }
  auto operator=(BufferedSink &&Other) noexcept -> BufferedSink & {
    if (this == &Other) return *this;
    (void)Close();
    Sink_ = Foundation::Utility::Move(Other.Sink_);
    Buffer_ = Other.Buffer_;
    Length_ = Other.Length_;
    Other.Length_ = 0;
    return *this;
  }

  [[nodiscard]] constexpr auto Sink() noexcept -> TSink & { return Sink_; }
  [[nodiscard]] constexpr auto Sink() const noexcept -> const TSink & {
    return Sink_;
  }

  [[nodiscard]] constexpr auto BufferedSize() const noexcept
      -> Foundation::Size {
    return Length_;
  }

  [[nodiscard]] auto WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size> {
    if (Bytes.Empty()) {
      return Foundation::Size{};
    }

    Foundation::Size Accepted{};

    while (Accepted < Bytes.Size()) {
      if (Length_ == Capacity) {
        auto Result = FlushBuffer();
        if (!Result) {
          if (Accepted) {
            return Accepted;
          }
          return Failure(Result.Error());
        }
      }

      const auto Available = Capacity - Length_;
      const auto Remaining = Bytes.Size() - Accepted;
      const auto Count = Available < Remaining ? Available : Remaining;
      Memory::Copy(Buffer_.Data() + Length_, Bytes.Data() + Accepted, Count);
      Length_ += Count;
      Accepted += Count;
    }

    return Accepted;
  }

  [[nodiscard]] auto Flush() -> Result<> {
    auto Result = FlushBuffer();
    if (!Result) {
      return Result;
    }
    return IO::Flush(Sink_);
  }

  [[nodiscard]] auto Close() -> Result<> {
    auto FlushResult = Flush();
    if (!FlushResult) {
      return FlushResult;
    }
    return IO::Close(Sink_);
  }

private:
  [[nodiscard]] auto FlushBuffer() -> Result<> {
    Foundation::Size Written{};

    while (Written < Length_) {
      auto Result = Sink_.WriteSome(
          {Buffer_.Data() + Written, Length_ - Written});

      if (!Result) {
        auto Error = Result.Error();
        Error.Transferred += Written;
        if (Written) {
          Memory::Move(Buffer_.Data(), Buffer_.Data() + Written,
                       Length_ - Written);
          Length_ -= Written;
        }
        return Failure(Error);
      }

      if (!Result.Value()) {
        if (Written) {
          Memory::Move(Buffer_.Data(), Buffer_.Data() + Written,
                       Length_ - Written);
          Length_ -= Written;
        }
        return Failure(Error::Semantic(ErrorCode::NoProgress,
                                       Operation::Write, 0, Written));
      }

      Written += Result.Value();
    }

    Length_ = 0;
    return {};
  }

  TSink Sink_{};
  Library::Array<Foundation::Byte, Capacity> Buffer_{};
  Foundation::Size Length_{};
};

template <InputSource TSource, Foundation::Size Capacity = 4096>
class BufferedSource final {
  static_assert(Capacity != 0);

public:
  constexpr BufferedSource() = default;
  constexpr explicit BufferedSource(TSource Source)
      : Source_(Foundation::Utility::Move(Source)) {}

  BufferedSource(const BufferedSource &) = delete;
  auto operator=(const BufferedSource &) -> BufferedSource & = delete;
  BufferedSource(BufferedSource &&Other) noexcept
      : Source_(Foundation::Utility::Move(Other.Source_)),
        Buffer_(Other.Buffer_), Position_(Other.Position_),
        Length_(Other.Length_), LogicalPosition_(Other.LogicalPosition_),
        End_(Other.End_) {
    Other.Position_ = 0;
    Other.Length_ = 0;
    Other.LogicalPosition_ = 0;
    Other.End_ = true;
  }
  auto operator=(BufferedSource &&Other) noexcept -> BufferedSource & {
    if (this == &Other) return *this;
    (void)IO::Close(Source_);
    Source_ = Foundation::Utility::Move(Other.Source_);
    Buffer_ = Other.Buffer_;
    Position_ = Other.Position_;
    Length_ = Other.Length_;
    LogicalPosition_ = Other.LogicalPosition_;
    End_ = Other.End_;
    Other.Position_ = 0;
    Other.Length_ = 0;
    Other.LogicalPosition_ = 0;
    Other.End_ = true;
    return *this;
  }

  [[nodiscard]] constexpr auto Source() noexcept -> TSource & { return Source_; }

  [[nodiscard]] auto ReadSome(Library::Span<Foundation::Byte> Destination)
      -> Result<ReadCount> {
    if (Destination.Empty()) {
      return ReadCount{};
    }

    if (Position_ == Length_) {
      auto Filled = Fill();
      if (!Filled) {
        return Failure(Filled.Error());
      }
      if (!Length_) {
        return ReadCount{0, End_};
      }
    }

    const auto Available = Length_ - Position_;
    const auto Count = Destination.Size() < Available ? Destination.Size()
                                                       : Available;
    Memory::Copy(Destination.Data(), Buffer_.Data() + Position_, Count);
    Position_ += Count;
    LogicalPosition_ += Count;
    return ReadCount{Count, End_ && Position_ == Length_};
  }

  [[nodiscard]] auto Peek(Foundation::Size Lookahead = 0)
      -> Result<Foundation::Byte> {
    while (Position_ + Lookahead >= Length_) {
      if (Position_ && Length_ > Position_) {
        Memory::Move(Buffer_.Data(), Buffer_.Data() + Position_,
                     Length_ - Position_);
        Length_ -= Position_;
        Position_ = 0;
      } else if (Position_ == Length_) {
        Position_ = 0;
        Length_ = 0;
      }

      if (End_ || Length_ == Capacity) {
        return Failure(Error::Semantic(
            End_ ? ErrorCode::EndOfFile : ErrorCode::Unsupported,
            Operation::Read, LogicalPosition_ + Lookahead));
      }

      auto Result = Source_.ReadSome(
          {Buffer_.Data() + Length_, Capacity - Length_});
      if (!Result) {
        return Failure(Result.Error());
      }
      Length_ += Result.Value().Count;
      End_ = Result.Value().End;
      if (!Result.Value().Count && !End_) {
        return Failure(
            Error::Semantic(ErrorCode::NoProgress, Operation::Read));
      }
    }

    return Buffer_[Position_ + Lookahead];
  }

  [[nodiscard]] auto Consume(Foundation::Size Count = 1) -> Result<> {
    for (Foundation::Size Index{}; Index < Count; ++Index) {
      auto Value = Peek();
      if (!Value) {
        return Failure(Value.Error());
      }
      ++Position_;
      ++LogicalPosition_;
    }
    return {};
  }

  [[nodiscard]] constexpr auto TellBuffered() const noexcept
      -> Foundation::Uint64 {
    return LogicalPosition_;
  }

  [[nodiscard]] auto Tell() -> Result<Foundation::Uint64> {
    return LogicalPosition_;
  }

  [[nodiscard]] auto Close() -> Result<> {
    Position_ = 0;
    Length_ = 0;
    End_ = true;
    return IO::Close(Source_);
  }

  [[nodiscard]] auto Seek(Foundation::Uint64 Position)
      -> Result<Foundation::Uint64>
    requires Seekable<TSource>
  {
    auto Result = Source_.Seek(Position);
    if (!Result) {
      return Failure(Result.Error());
    }
    Position_ = 0;
    Length_ = 0;
    LogicalPosition_ = Result.Value();
    End_ = false;
    return Result.Value();
  }

private:
  [[nodiscard]] auto Fill() -> Result<> {
    Position_ = 0;
    Length_ = 0;

    if (End_) {
      return {};
    }

    auto Result = Source_.ReadSome(Buffer_.View());
    if (!Result) {
      return Failure(Result.Error());
    }

    Length_ = Result.Value().Count;
    End_ = Result.Value().End;
    if (!Length_ && !End_) {
      return Failure(Error::Semantic(ErrorCode::NoProgress, Operation::Read));
    }
    return {};
  }

  TSource Source_{};
  Library::Array<Foundation::Byte, Capacity> Buffer_{};
  Foundation::Size Position_{};
  Foundation::Size Length_{};
  Foundation::Uint64 LogicalPosition_{};
  Foundation::Bool End_{};
};
} // namespace UEFIpp::IO
