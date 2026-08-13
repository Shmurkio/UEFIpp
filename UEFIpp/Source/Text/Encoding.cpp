#include <UEFIpp/Text/Encoding.hpp>

namespace UEFIpp::Text {
auto Encoding::Utf16ToUtf8(Library::U16StringView Source,
                           Memory::AllocatorStub Allocator)
    -> Library::U8String {
  Library::U8String Result{Allocator};
  if (!Result.Reserve(Source.Size() + 1)) {
    return Result;
  }

  for (Foundation::Size i = 0; i < Source.Size(); ++i) {
    auto CodePoint = Foundation::Cast::Auto<Foundation::Char32>(Source[i]);

    if (IsHighSurrogate(Source[i])) {
      if (i + 1 < Source.Size() && IsLowSurrogate(Source[i + 1])) {
        CodePoint = DecodeSurrogatePair(Source[i], Source[i + 1]);
        ++i;
      } else {
        CodePoint = ReplacementCharacter;
      }
    } else if (IsLowSurrogate(Source[i])) {
      CodePoint = ReplacementCharacter;
    }

    if (!AppendUtf8(Result, CodePoint)) {
      Result.Clear();
      return Result;
    }
  }

  return Result;
}

auto Encoding::Utf8ToUtf16(Library::U8StringView Source,
                           Memory::AllocatorStub Allocator)
    -> Library::U16String {
  Library::U16String Result{Allocator};
  if (!Result.Reserve(Source.Size() + 1)) {
    return Result;
  }

  for (Foundation::Size i = 0; i < Source.Size();) {
    Foundation::Char32 CodePoint{};
    Foundation::Size Count{};

    if (!DecodeUtf8(Source, i, CodePoint, Count)) {
      if (!AppendUtf16(Result, ReplacementCharacter)) {
        Result.Clear();
        return Result;
      }
      ++i;
      continue;
    }

    if (!AppendUtf16(Result, CodePoint)) {
      Result.Clear();
      return Result;
    }
    i += Count;
  }

  return Result;
}

auto Encoding::Utf32ToUtf8(Library::U32StringView Source,
                           Memory::AllocatorStub Allocator)
    -> Library::U8String {
  Library::U8String Result{Allocator};
  if (!Result.Reserve(Source.Size() + 1)) {
    return Result;
  }

  for (const auto CodePoint : Source) {
    if (!AppendUtf8(
            Result,
            IsValidCodePoint(CodePoint) ? CodePoint : ReplacementCharacter)) {
      Result.Clear();
      return Result;
    }
  }

  return Result;
}

auto Encoding::Utf8ToUtf32(Library::U8StringView Source,
                           Memory::AllocatorStub Allocator)
    -> Library::U32String {
  Library::U32String Result{Allocator};
  if (!Result.Reserve(Source.Size() + 1)) {
    return Result;
  }

  for (Foundation::Size i = 0; i < Source.Size();) {
    Foundation::Char32 CodePoint{};
    Foundation::Size Count{};

    if (!DecodeUtf8(Source, i, CodePoint, Count)) {
      if (!Result.PushBack(ReplacementCharacter)) {
        Result.Clear();
        return Result;
      }
      ++i;
      continue;
    }

    if (!Result.PushBack(CodePoint)) {
      Result.Clear();
      return Result;
    }
    i += Count;
  }

  return Result;
}

auto Encoding::Utf16ToUtf32(Library::U16StringView Source,
                            Memory::AllocatorStub Allocator)
    -> Library::U32String {
  Library::U32String Result{Allocator};
  if (!Result.Reserve(Source.Size() + 1)) {
    return Result;
  }

  for (Foundation::Size i = 0; i < Source.Size(); ++i) {
    auto CodePoint = Foundation::Cast::Auto<Foundation::Char32>(Source[i]);

    if (IsHighSurrogate(Source[i])) {
      if (i + 1 < Source.Size() && IsLowSurrogate(Source[i + 1])) {
        CodePoint = DecodeSurrogatePair(Source[i], Source[i + 1]);
        ++i;
      } else {
        CodePoint = ReplacementCharacter;
      }
    } else if (IsLowSurrogate(Source[i])) {
      CodePoint = ReplacementCharacter;
    }

    if (!Result.PushBack(CodePoint)) {
      Result.Clear();
      return Result;
    }
  }

  return Result;
}

auto Encoding::Utf32ToUtf16(Library::U32StringView Source,
                            Memory::AllocatorStub Allocator)
    -> Library::U16String {
  Library::U16String Result{Allocator};
  if (!Result.Reserve(Source.Size() + 1)) {
    return Result;
  }

  for (const auto CodePoint : Source) {
    if (!AppendUtf16(
            Result,
            IsValidCodePoint(CodePoint) ? CodePoint : ReplacementCharacter)) {
      Result.Clear();
      return Result;
    }
  }

  return Result;
}

auto Encoding::ToAscii(Library::WideStringView Source,
                       Memory::AllocatorStub Allocator) -> Library::String {
  Library::String Result{Allocator};
  if (!Result.Reserve(Source.Size() + 1)) {
    return Result;
  }

  for (const auto Character : Source) {
    if (!Result.PushBack(
            Character <= Foundation::WChar(0x7F)
                ? Foundation::Cast::Auto<Foundation::Char>(Character)
                : '?')) {
      Result.Clear();
      return Result;
    }
  }

  return Result;
}

auto Encoding::ToWideAscii(Library::StringView Source,
                           Memory::AllocatorStub Allocator)
    -> Library::WideString {
  Library::WideString Result{Allocator};
  if (!Result.Reserve(Source.Size() + 1)) {
    return Result;
  }

  for (const auto Character : Source) {
    if (!Result.PushBack(
            Foundation::Cast::Auto<Foundation::WChar>(Character))) {
      Result.Clear();
      return Result;
    }
  }

  return Result;
}

auto Encoding::Utf16ToAscii(Library::U16StringView Source,
                            Memory::AllocatorStub Allocator)
    -> Library::String {
  Library::String Result{Allocator};
  if (!Result.Reserve(Source.Size() + 1)) {
    return Result;
  }

  for (const auto Character : Source) {
    if (!Result.PushBack(
            Character <= Foundation::Char16(0x7F)
                ? Foundation::Cast::Auto<Foundation::Char>(Character)
                : '?')) {
      Result.Clear();
      return Result;
    }
  }

  return Result;
}

auto Encoding::WideToAscii(Library::WideStringView Source,
                           Memory::AllocatorStub Allocator) -> Library::String {
  Library::String Result{Allocator};
  if (!Result.Reserve(Source.Size())) {
    return Result;
  }

  for (const auto Character : Source) {
    if (!Result.PushBack(
            Character <= wchar_t(0x7F)
                ? Foundation::Cast::Auto<Foundation::Char>(Character)
                : '?')) {
      Result.Clear();
      return Result;
    }
  }

  return Result;
}

auto Encoding::AppendUtf8(Library::U8String &Result,
                          Foundation::Char32 CodePoint) -> Foundation::Bool {
  if (!IsValidCodePoint(CodePoint)) {
    CodePoint = ReplacementCharacter;
  }

  if (CodePoint <= 0x7F) {
    return Result.PushBack(
        Foundation::Cast::Auto<Foundation::Char8>(CodePoint));
  } else if (CodePoint <= 0x7FF) {
    return Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(
               0xC0 | (CodePoint >> 6))) &&
           Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(
               0x80 | (CodePoint & ContinuationMask)));
  } else if (CodePoint <= 0xFFFF) {
    return Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(
               0xE0 | (CodePoint >> 12))) &&
           Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(
               0x80 | ((CodePoint >> 6) & ContinuationMask))) &&
           Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(
               0x80 | (CodePoint & ContinuationMask)));
  }

  return Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(
             0xF0 | (CodePoint >> 18))) &&
         Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(
             0x80 | ((CodePoint >> 12) & ContinuationMask))) &&
         Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(
             0x80 | ((CodePoint >> 6) & ContinuationMask))) &&
         Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(
             0x80 | (CodePoint & ContinuationMask)));
}

auto Encoding::AppendUtf16(Library::U16String &Result,
                           Foundation::Char32 CodePoint) -> Foundation::Bool {
  if (!IsValidCodePoint(CodePoint)) {
    CodePoint = ReplacementCharacter;
  }

  if (CodePoint <= 0xFFFF) {
    return Result.PushBack(
        Foundation::Cast::Auto<Foundation::Char16>(CodePoint));
  }

  Foundation::Char16 High{};
  Foundation::Char16 Low{};
  EncodeSurrogatePair(CodePoint, High, Low);
  return Result.PushBack(High) && Result.PushBack(Low);
}

auto Encoding::DecodeUtf8(Library::U8StringView Source,
                          Foundation::Size Position,
                          Foundation::Char32 &CodePoint,
                          Foundation::Size &Count) -> Foundation::Bool {
  if (Position >= Source.Size()) {
    return false;
  }

  const auto First =
      Foundation::Cast::Auto<Foundation::Uint8>(Source[Position]);

  if ((First & 0x80) == 0) {
    CodePoint = First & Utf8Payload1;
    Count = 1;
    return true;
  }

  if ((First & 0xE0) == 0xC0) {
    CodePoint = First & Utf8Payload2;
    Count = 2;
  } else if ((First & 0xF0) == 0xE0) {
    CodePoint = First & Utf8Payload3;
    Count = 3;
  } else if ((First & 0xF8) == 0xF0) {
    CodePoint = First & Utf8Payload4;
    Count = 4;
  } else {
    return false;
  }

  if (Position + Count > Source.Size()) {
    return false;
  }

  for (Foundation::Size i = 1; i < Count; ++i) {
    const auto Byte =
        Foundation::Cast::Auto<Foundation::Uint8>(Source[Position + i]);

    if ((Byte & 0xC0) != 0x80) {
      return false;
    }

    CodePoint = (CodePoint << 6) | (Byte & ContinuationMask);
  }

  if (!IsValidCodePoint(CodePoint)) {
    return false;
  }

  if ((Count == 2 && CodePoint <= 0x7F) || (Count == 3 && CodePoint <= 0x7FF) ||
      (Count == 4 && CodePoint <= 0xFFFF)) {
    return false;
  }

  return true;
}
} // namespace UEFIpp::Text
