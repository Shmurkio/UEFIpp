#pragma once

#include <UEFIpp/Memory/AllocatorStub.hpp>

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/String/BasicString.hpp>
#include <UEFIpp/Library/String/StringView.hpp>

namespace UEFIpp::Text {
class Encoding {
public:
  Encoding() = delete;

  inline static constexpr auto ReplacementCharacter =
      Foundation::Char32{0xFFFD};

  [[nodiscard]] static constexpr auto IsHighSurrogate(Foundation::Char16 Value)
      -> Foundation::Bool {
    return Value >= 0xD800 && Value <= 0xDBFF;
  }

  [[nodiscard]] static constexpr auto IsLowSurrogate(Foundation::Char16 Value)
      -> Foundation::Bool {
    return Value >= 0xDC00 && Value <= 0xDFFF;
  }

  [[nodiscard]] static constexpr auto IsSurrogate(Foundation::Char32 Value)
      -> Foundation::Bool {
    return Value >= 0xD800 && Value <= 0xDFFF;
  }

  [[nodiscard]] static constexpr auto IsValidCodePoint(Foundation::Char32 Value)
      -> Foundation::Bool {
    return Value <= 0x10FFFF && !IsSurrogate(Value);
  }

  static constexpr auto EncodeSurrogatePair(Foundation::Char32 CodePoint,
                                            Foundation::Char16 &High,
                                            Foundation::Char16 &Low) -> void {
    CodePoint -= 0x10000;
    High =
        Foundation::Cast::Auto<Foundation::Char16>(0xD800 + (CodePoint >> 10));
    Low = Foundation::Cast::Auto<Foundation::Char16>(
        0xDC00 + (CodePoint & SurrogatePayloadMask));
  }

  [[nodiscard]] static constexpr auto
  DecodeSurrogatePair(Foundation::Char16 High, Foundation::Char16 Low)
      -> Foundation::Char32 {
    return Foundation::Cast::Auto<Foundation::Char32>(
        0x10000 +
        ((Foundation::Cast::Auto<Foundation::Uint32>(High) - 0xD800) << 10) +
        (Foundation::Cast::Auto<Foundation::Uint32>(Low) - 0xDC00));
  }

  [[nodiscard]] static auto Utf16ToUtf8(Library::U16StringView Source,
                                        Memory::AllocatorStub Allocator = {})
      -> Library::U8String;

  [[nodiscard]] static auto Utf8ToUtf16(Library::U8StringView Source,
                                        Memory::AllocatorStub Allocator = {})
      -> Library::U16String;

  [[nodiscard]] static auto Utf32ToUtf8(Library::U32StringView Source,
                                        Memory::AllocatorStub Allocator = {})
      -> Library::U8String;

  [[nodiscard]] static auto Utf8ToUtf32(Library::U8StringView Source,
                                        Memory::AllocatorStub Allocator = {})
      -> Library::U32String;

  [[nodiscard]] static auto Utf16ToUtf32(Library::U16StringView Source,
                                         Memory::AllocatorStub Allocator = {})
      -> Library::U32String;

  [[nodiscard]] static auto Utf32ToUtf16(Library::U32StringView Source,
                                         Memory::AllocatorStub Allocator = {})
      -> Library::U16String;

  [[nodiscard]] static auto ToAscii(Library::WideStringView Source,
                                    Memory::AllocatorStub Allocator = {})
      -> Library::String;

  [[nodiscard]] static auto ToWideAscii(Library::StringView Source,
                                        Memory::AllocatorStub Allocator = {})
      -> Library::WideString;

  [[nodiscard]] static auto Utf16ToAscii(Library::U16StringView Source,
                                         Memory::AllocatorStub Allocator = {})
      -> Library::String;

  [[nodiscard]] static auto WideToAscii(Library::WideStringView Source,
                                        Memory::AllocatorStub Allocator = {})
      -> Library::String;

private:
  [[nodiscard]] static auto AppendUtf8(Library::U8String &Result,
                                       Foundation::Char32 CodePoint)
      -> Foundation::Bool;

  [[nodiscard]] static auto AppendUtf16(Library::U16String &Result,
                                        Foundation::Char32 CodePoint)
      -> Foundation::Bool;

  [[nodiscard]] static auto
  DecodeUtf8(Library::U8StringView Source, Foundation::Size Position,
             Foundation::Char32 &CodePoint, Foundation::Size &Count)
      -> Foundation::Bool;

  inline static constexpr auto ContinuationMask =
      Foundation::Bit::LowMask<Foundation::Uint8>(6);
  inline static constexpr auto Utf8Payload1 =
      Foundation::Bit::LowMask<Foundation::Uint8>(7);
  inline static constexpr auto Utf8Payload2 =
      Foundation::Bit::LowMask<Foundation::Uint8>(5);
  inline static constexpr auto Utf8Payload3 =
      Foundation::Bit::LowMask<Foundation::Uint8>(4);
  inline static constexpr auto Utf8Payload4 =
      Foundation::Bit::LowMask<Foundation::Uint8>(3);
  inline static constexpr auto SurrogatePayloadMask =
      Foundation::Bit::LowMask<Foundation::Uint32>(10);
};
} // namespace UEFIpp::Text
