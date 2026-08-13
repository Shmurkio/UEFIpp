#pragma once

#include <UEFIpp/IO/Core/Error.hpp>
#include <UEFIpp/Library/Containers/Optional.hpp>
#include <UEFIpp/Library/String/String.hpp>

namespace UEFIpp::IO {
enum class InvalidEncodingPolicy : Foundation::Uint8 { Reject, Replace };

struct Utf8DecodeStep final {
  Library::Optional<Foundation::Char32> CodePoint{};
  Foundation::Bool Consumed{true};
};

class Utf8Decoder final {
public:
  constexpr Utf8Decoder() noexcept = default;

  [[nodiscard]] constexpr auto Complete() const noexcept -> Foundation::Bool {
    return Remaining_ == 0;
  }

  constexpr auto Reset() noexcept -> Foundation::Void {
    CodePoint_ = 0;
    Minimum_ = 0;
    Remaining_ = 0;
  }

  [[nodiscard]] auto Feed(Foundation::Byte Byte,
                          InvalidEncodingPolicy Policy =
                              InvalidEncodingPolicy::Reject)
      -> Result<Utf8DecodeStep> {
    if (!Remaining_) {
      if (Byte <= 0x7F) {
        return Utf8DecodeStep{Library::Optional<Foundation::Char32>{
                                  Foundation::Cast::Auto<Foundation::Char32>(
                                      Byte)},
                              true};
      }

      if (Byte >= 0xC2 && Byte <= 0xDF) {
        CodePoint_ = Byte & 0x1F;
        Minimum_ = 0x80;
        Remaining_ = 1;
        return Utf8DecodeStep{};
      }
      if (Byte >= 0xE0 && Byte <= 0xEF) {
        CodePoint_ = Byte & 0x0F;
        Minimum_ = 0x800;
        Remaining_ = 2;
        return Utf8DecodeStep{};
      }
      if (Byte >= 0xF0 && Byte <= 0xF4) {
        CodePoint_ = Byte & 0x07;
        Minimum_ = 0x10000;
        Remaining_ = 3;
        return Utf8DecodeStep{};
      }

      if (Policy == InvalidEncodingPolicy::Replace) {
        return Utf8DecodeStep{
            Library::Optional<Foundation::Char32>{ReplacementCharacter}, true};
      }
      return Failure(
          Error::Semantic(ErrorCode::InvalidEncoding, Operation::Decode));
    }

    if ((Byte & 0xC0) != 0x80) {
      Reset();
      if (Policy == InvalidEncodingPolicy::Replace) {
        return Utf8DecodeStep{
            Library::Optional<Foundation::Char32>{ReplacementCharacter},
            false};
      }
      return Failure(
          Error::Semantic(ErrorCode::InvalidEncoding, Operation::Decode));
    }

    CodePoint_ = (CodePoint_ << 6) | (Byte & 0x3F);
    --Remaining_;
    if (Remaining_) {
      return Utf8DecodeStep{};
    }

    const auto Value = CodePoint_;
    const auto Valid = Value >= Minimum_ && IsValidCodePoint(Value);
    Reset();
    if (!Valid) {
      if (Policy == InvalidEncodingPolicy::Replace) {
        return Utf8DecodeStep{
            Library::Optional<Foundation::Char32>{ReplacementCharacter}, true};
      }
      return Failure(
          Error::Semantic(ErrorCode::InvalidEncoding, Operation::Decode));
    }
    return Utf8DecodeStep{Library::Optional<Foundation::Char32>{Value}, true};
  }

  [[nodiscard]] auto Finish(
      InvalidEncodingPolicy Policy = InvalidEncodingPolicy::Reject)
      -> Result<Library::Optional<Foundation::Char32>> {
    if (Complete()) {
      return {};
    }
    Reset();
    if (Policy == InvalidEncodingPolicy::Replace) {
      return Library::Optional<Foundation::Char32>{ReplacementCharacter};
    }
    return Failure(
        Error::Semantic(ErrorCode::InvalidEncoding, Operation::Decode));
  }

  inline static constexpr Foundation::Char32 ReplacementCharacter{0xFFFD};

  [[nodiscard]] static constexpr auto IsValidCodePoint(
      Foundation::Char32 Value) noexcept -> Foundation::Bool {
    return Value <= 0x10FFFF && !(Value >= 0xD800 && Value <= 0xDFFF);
  }

private:
  Foundation::Char32 CodePoint_{};
  Foundation::Char32 Minimum_{};
  Foundation::Uint8 Remaining_{};
};

[[nodiscard]] constexpr auto EncodeUtf8(
    Foundation::Char32 CodePoint, Foundation::Byte (&Output)[4])
    -> Result<Foundation::Size> {
  if (!Utf8Decoder::IsValidCodePoint(CodePoint)) {
    return Failure(
        Error::Semantic(ErrorCode::InvalidEncoding, Operation::Encode));
  }

  if (CodePoint <= 0x7F) {
    Output[0] = Foundation::Cast::Auto<Foundation::Byte>(CodePoint);
    return Foundation::Size{1};
  }
  if (CodePoint <= 0x7FF) {
    Output[0] = Foundation::Cast::Auto<Foundation::Byte>(0xC0 | (CodePoint >> 6));
    Output[1] = Foundation::Cast::Auto<Foundation::Byte>(0x80 | (CodePoint & 0x3F));
    return Foundation::Size{2};
  }
  if (CodePoint <= 0xFFFF) {
    Output[0] = Foundation::Cast::Auto<Foundation::Byte>(0xE0 | (CodePoint >> 12));
    Output[1] = Foundation::Cast::Auto<Foundation::Byte>(0x80 | ((CodePoint >> 6) & 0x3F));
    Output[2] = Foundation::Cast::Auto<Foundation::Byte>(0x80 | (CodePoint & 0x3F));
    return Foundation::Size{3};
  }
  Output[0] = Foundation::Cast::Auto<Foundation::Byte>(0xF0 | (CodePoint >> 18));
  Output[1] = Foundation::Cast::Auto<Foundation::Byte>(0x80 | ((CodePoint >> 12) & 0x3F));
  Output[2] = Foundation::Cast::Auto<Foundation::Byte>(0x80 | ((CodePoint >> 6) & 0x3F));
  Output[3] = Foundation::Cast::Auto<Foundation::Byte>(0x80 | (CodePoint & 0x3F));
  return Foundation::Size{4};
}

[[nodiscard]] constexpr auto EncodeUtf16(
    Foundation::Char32 CodePoint, Foundation::Char16 (&Output)[2])
    -> Result<Foundation::Size> {
  if (!Utf8Decoder::IsValidCodePoint(CodePoint)) {
    return Failure(
        Error::Semantic(ErrorCode::InvalidEncoding, Operation::Encode));
  }
  if (CodePoint <= 0xFFFF) {
    Output[0] = Foundation::Cast::Auto<Foundation::Char16>(CodePoint);
    return Foundation::Size{1};
  }
  CodePoint -= 0x10000;
  Output[0] = Foundation::Cast::Auto<Foundation::Char16>(0xD800 + (CodePoint >> 10));
  Output[1] = Foundation::Cast::Auto<Foundation::Char16>(0xDC00 + (CodePoint & 0x3FF));
  return Foundation::Size{2};
}

[[nodiscard]] constexpr auto AsBytes(Library::U8StringView Text) noexcept
    -> Library::Span<const Foundation::Byte> {
  return {Foundation::Cast::Auto<const Foundation::Byte *>(Text.Data()),
          Text.Size()};
}

[[nodiscard]] constexpr auto AsUtf8(Library::StringView Text) noexcept
    -> Library::U8StringView {
  return {Foundation::Cast::Auto<const Foundation::Char8 *>(Text.Data()),
          Text.Size()};
}

[[nodiscard]] inline auto Utf16ToUtf8(
    Library::U16StringView Source, Memory::AllocatorStub Allocator = {},
    InvalidEncodingPolicy Policy = InvalidEncodingPolicy::Reject)
    -> Result<Library::U8String> {
  Library::U8String Output{Allocator};
  if (!Output.Reserve(Source.Size() + 1)) {
    return Failure(
        Error::Semantic(ErrorCode::OutOfMemory, Operation::Allocate));
  }
  for (Foundation::Size Index{}; Index < Source.Size(); ++Index) {
    auto CodePoint = Foundation::Cast::Auto<Foundation::Char32>(Source[Index]);
    if (Source[Index] >= 0xD800 && Source[Index] <= 0xDBFF) {
      if (Index + 1 < Source.Size() && Source[Index + 1] >= 0xDC00 &&
          Source[Index + 1] <= 0xDFFF) {
        const auto High =
            Foundation::Cast::Auto<Foundation::Char32>(Source[Index]);
        const auto Low =
            Foundation::Cast::Auto<Foundation::Char32>(Source[Index + 1]);
        CodePoint = 0x10000 + ((High - 0xD800) << 10) + (Low - 0xDC00);
        ++Index;
      } else if (Policy == InvalidEncodingPolicy::Replace) {
        CodePoint = Utf8Decoder::ReplacementCharacter;
      } else {
        return Failure(Error::Semantic(ErrorCode::InvalidEncoding,
                                       Operation::Decode, Index));
      }
    } else if (Source[Index] >= 0xDC00 && Source[Index] <= 0xDFFF) {
      if (Policy == InvalidEncodingPolicy::Replace) {
        CodePoint = Utf8Decoder::ReplacementCharacter;
      } else {
        return Failure(Error::Semantic(ErrorCode::InvalidEncoding,
                                       Operation::Decode, Index));
      }
    }
    Foundation::Byte Encoded[4]{};
    auto Count = EncodeUtf8(CodePoint, Encoded);
    if (!Count) return Failure(Count.Error());
    for (Foundation::Size Byte{}; Byte < Count.Value(); ++Byte) {
      if (!Output.PushBack(
              Foundation::Cast::Auto<Foundation::Char8>(Encoded[Byte]))) {
        return Failure(
            Error::Semantic(ErrorCode::OutOfMemory, Operation::Allocate));
      }
    }
  }
  return Output;
}

[[nodiscard]] inline auto Utf8ToUtf16(
    Library::U8StringView Source, Memory::AllocatorStub Allocator = {},
    InvalidEncodingPolicy Policy = InvalidEncodingPolicy::Reject)
    -> Result<Library::U16String> {
  Library::U16String Output{Allocator};
  if (!Output.Reserve(Source.Size() + 1)) {
    return Failure(
        Error::Semantic(ErrorCode::OutOfMemory, Operation::Allocate));
  }
  Utf8Decoder Decoder{};
  const auto AppendCodePoint = [&](Foundation::Char32 CodePoint) -> Result<> {
    Foundation::Char16 Encoded[2]{};
    auto Count = EncodeUtf16(CodePoint, Encoded);
    if (!Count) return Failure(Count.Error());
    for (Foundation::Size Unit{}; Unit < Count.Value(); ++Unit) {
      if (!Output.PushBack(Encoded[Unit])) {
        return Failure(
            Error::Semantic(ErrorCode::OutOfMemory, Operation::Allocate));
      }
    }
    return {};
  };
  for (Foundation::Size Index{}; Index < Source.Size(); ++Index) {
    Foundation::Bool Consumed{};
    while (!Consumed) {
      auto Decoded = Decoder.Feed(
          Foundation::Cast::Auto<Foundation::Byte>(Source[Index]), Policy);
      if (!Decoded) {
        auto Error = Decoded.Error();
        Error.Offset = Index;
        return Failure(Error);
      }
      Consumed = Decoded.Value().Consumed;
      if (!Decoded.Value().CodePoint) continue;
      auto Appended = AppendCodePoint(Decoded.Value().CodePoint.Value());
      if (!Appended) return Failure(Appended.Error());
    }
  }
  auto Finished = Decoder.Finish(Policy);
  if (!Finished) return Failure(Finished.Error());
  if (Finished.Value()) {
    auto Appended = AppendCodePoint(Finished.Value().Value());
    if (!Appended) return Failure(Appended.Error());
  }
  return Output;
}

[[nodiscard]] inline auto WideToUtf8(
    Library::WideStringView Source, Memory::AllocatorStub Allocator = {},
    InvalidEncodingPolicy Policy = InvalidEncodingPolicy::Reject)
    -> Result<Library::U8String> {
  static_assert(sizeof(Foundation::WChar) == sizeof(Foundation::Char16));
  return Utf16ToUtf8(
      {Foundation::Cast::Auto<const Foundation::Char16 *>(Source.Data()),
       Source.Size()},
      Allocator, Policy);
}

[[nodiscard]] inline auto Utf8ToWide(
    Library::U8StringView Source, Memory::AllocatorStub Allocator = {},
    InvalidEncodingPolicy Policy = InvalidEncodingPolicy::Reject)
    -> Result<Library::WideString> {
  auto Converted = Utf8ToUtf16(Source, Allocator, Policy);
  if (!Converted) return Failure(Converted.Error());
  Library::WideString Output{Allocator};
  if (!Output.Assign(
          {Foundation::Cast::Auto<const Foundation::WChar *>(
               Converted.Value().Data()),
           Converted.Value().Size()})) {
    return Failure(
        Error::Semantic(ErrorCode::OutOfMemory, Operation::Allocate));
  }
  return Output;
}
} // namespace UEFIpp::IO
