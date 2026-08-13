#pragma once

#include <UEFIpp/IO/Adapter/Buffered.hpp>
#include <UEFIpp/IO/Text/Utf.hpp>
#include <UEFIpp/Library/String/String.hpp>

namespace UEFIpp::IO {
struct ReadTextOptions {
  Foundation::Size MaximumSize{64 * 1024};
  InvalidEncodingPolicy InvalidEncoding{InvalidEncodingPolicy::Reject};
};

struct ReadLineOptions final : ReadTextOptions {
  Foundation::Bool KeepTerminator{};
};

template <InputSource TSource, Foundation::Size Capacity = 4096>
class TextReader final {
public:
  struct Checkpoint final {
    Foundation::Uint64 Position{};
  };

  constexpr TextReader() = default;
  constexpr explicit TextReader(TSource Source,
                                Memory::AllocatorStub Allocator = {})
      : Source_(Foundation::Utility::Move(Source)), Allocator_(Allocator) {}

  [[nodiscard]] auto ReadCodePoint(
      InvalidEncodingPolicy Policy = InvalidEncodingPolicy::Reject)
      -> Result<Foundation::Char32> {
    Utf8Decoder Decoder{};
    for (;;) {
      auto Byte = Source_.Peek();
      if (!Byte) {
        if (Byte.Error().Code == ErrorCode::EndOfFile && !Decoder.Complete()) {
          auto Finished = Decoder.Finish(Policy);
          if (!Finished) return Failure(Finished.Error());
          if (Finished.Value()) return Finished.Value().Value();
        }
        return Failure(Byte.Error());
      }
      auto Decoded = Decoder.Feed(Byte.Value(), Policy);
      if (!Decoded) return Failure(Decoded.Error());
      if (Decoded.Value().Consumed) {
        auto Consumed = Source_.Consume();
        if (!Consumed) return Failure(Consumed.Error());
      }
      if (Decoded.Value().CodePoint)
        return Decoded.Value().CodePoint.Value();
    }
  }

  [[nodiscard]] auto ReadLine(ReadLineOptions Options = {})
      -> Result<Library::U8String> {
    Library::U8String Line{Allocator_};
    for (;;) {
      auto Byte = Source_.Peek();
      if (!Byte) {
        if (Byte.Error().Code == ErrorCode::EndOfFile && !Line.Empty()) {
          return Line;
        }
        return Failure(Byte.Error());
      }
      if (Byte.Value() == '\r' || Byte.Value() == '\n') {
        const auto First = Byte.Value();
        auto Result = Source_.Consume();
        if (!Result) return Failure(Result.Error());
        if (Options.KeepTerminator &&
            !Line.PushBack(Foundation::Cast::Auto<Foundation::Char8>(First))) {
          return Failure(
              Error::Semantic(ErrorCode::OutOfMemory, Operation::Allocate));
        }
        if (First == '\r') {
          auto Next = Source_.Peek();
          if (Next && Next.Value() == '\n') {
            Result = Source_.Consume();
            if (!Result) return Failure(Result.Error());
            if (Options.KeepTerminator && !Line.PushBack(u8'\n')) {
              return Failure(Error::Semantic(ErrorCode::OutOfMemory,
                                             Operation::Allocate));
            }
          }
        }
        return Line;
      }
      if (Line.Size() >= Options.MaximumSize) {
        return Failure(Error::Semantic(ErrorCode::Overflow, Operation::Read,
                                       Source_.TellBuffered()));
      }
      auto CodePoint = ReadCodePoint(Options.InvalidEncoding);
      if (!CodePoint) return Failure(CodePoint.Error());
      Foundation::Byte Encoded[4]{};
      auto Count = EncodeUtf8(CodePoint.Value(), Encoded);
      if (!Count) return Failure(Count.Error());
      if (Count.Value() > Options.MaximumSize - Line.Size()) {
        return Failure(Error::Semantic(ErrorCode::Overflow, Operation::Read,
                                       Source_.TellBuffered()));
      }
      for (Foundation::Size Index{}; Index < Count.Value(); ++Index) {
        if (!Line.PushBack(
                Foundation::Cast::Auto<Foundation::Char8>(Encoded[Index]))) {
          return Failure(
              Error::Semantic(ErrorCode::OutOfMemory, Operation::Allocate));
        }
      }
    }
  }

  [[nodiscard]] auto ReadToken(ReadTextOptions Options = {})
      -> Result<Library::U8String> {
    for (;;) {
      auto Byte = Source_.Peek();
      if (!Byte) return Failure(Byte.Error());
      if (!IsWhitespace(Byte.Value())) break;
      auto Result = Source_.Consume();
      if (!Result) return Failure(Result.Error());
    }

    Library::U8String Token{Allocator_};
    for (;;) {
      auto Byte = Source_.Peek();
      if (!Byte) {
        if (Byte.Error().Code == ErrorCode::EndOfFile && !Token.Empty()) {
          return Token;
        }
        return Failure(Byte.Error());
      }
      if (IsWhitespace(Byte.Value())) return Token;
      if (Token.Size() >= Options.MaximumSize) {
        return Failure(Error::Semantic(ErrorCode::Overflow, Operation::Read,
                                       Source_.TellBuffered()));
      }
      auto CodePoint = ReadCodePoint(Options.InvalidEncoding);
      if (!CodePoint) return Failure(CodePoint.Error());
      Foundation::Byte Encoded[4]{};
      auto Count = EncodeUtf8(CodePoint.Value(), Encoded);
      if (!Count) return Failure(Count.Error());
      if (Count.Value() > Options.MaximumSize - Token.Size()) {
        return Failure(Error::Semantic(ErrorCode::Overflow, Operation::Read,
                                       Source_.TellBuffered()));
      }
      for (Foundation::Size Index{}; Index < Count.Value(); ++Index) {
        if (!Token.PushBack(
                Foundation::Cast::Auto<Foundation::Char8>(Encoded[Index]))) {
          return Failure(
              Error::Semantic(ErrorCode::OutOfMemory, Operation::Allocate));
        }
      }
    }
  }

  [[nodiscard]] auto Save() -> Result<Checkpoint>
    requires Seekable<TSource>
  {
    return Checkpoint{Source_.TellBuffered()};
  }

  [[nodiscard]] auto Restore(Checkpoint Value) -> Result<>
    requires Seekable<TSource>
  {
    auto Position = Source_.Seek(Value.Position);
    if (!Position) return Failure(Position.Error());
    return {};
  }

  [[nodiscard]] constexpr auto Source() noexcept
      -> BufferedSource<TSource, Capacity> & {
    return Source_;
  }

private:
  [[nodiscard]] static constexpr auto IsWhitespace(Foundation::Byte Value)
      -> Foundation::Bool {
    return Value == ' ' || Value == '\t' || Value == '\r' || Value == '\n' ||
           Value == '\f' || Value == '\v';
  }

  BufferedSource<TSource, Capacity> Source_{};
  Memory::AllocatorStub Allocator_{};
};

template <InputSource TSource>
[[nodiscard]] auto ReadAll(TSource &Source, Memory::AllocatorStub Allocator = {},
                           Foundation::Size MaximumSize = Foundation::Size(-1))
    -> Result<Library::Vector<Foundation::Byte>> {
  Library::Vector<Foundation::Byte> ResultBytes{Allocator};
  Foundation::Byte Buffer[4096]{};
  for (;;) {
    auto ReadResult = Source.ReadSome(Buffer);
    if (!ReadResult) return Failure(ReadResult.Error());
    if (ResultBytes.Size() > MaximumSize ||
        ReadResult.Value().Count > MaximumSize - ResultBytes.Size()) {
      return Failure(Error::Semantic(ErrorCode::Overflow, Operation::Read));
    }
    const auto OldSize = ResultBytes.Size();
    if (!ResultBytes.Resize(OldSize + ReadResult.Value().Count)) {
      return Failure(
          Error::Semantic(ErrorCode::OutOfMemory, Operation::Allocate));
    }
    if (ReadResult.Value().Count) {
      Memory::Copy(ResultBytes.Data() + OldSize, Buffer,
                   ReadResult.Value().Count);
    }
    if (ReadResult.Value().End) return ResultBytes;
    if (!ReadResult.Value().Count) {
      return Failure(
          Error::Semantic(ErrorCode::NoProgress, Operation::Read));
    }
  }
}
} // namespace UEFIpp::IO
