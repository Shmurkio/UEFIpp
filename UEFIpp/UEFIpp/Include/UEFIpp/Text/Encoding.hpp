#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/String/BasicString.hpp>
#include <UEFIpp/Library/String/StringView.hpp>

namespace UEFIpp::Text
{
	class Encoding
	{
	public:
		Encoding() = delete;

		inline static constexpr auto ReplacementCharacter = Foundation::Char32{ 0xFFFD };

		[[nodiscard]] static constexpr auto IsHighSurrogate(Foundation::Char16 Value) -> Foundation::Bool
		{
			return Value >= 0xD800 && Value <= 0xDBFF;
		}

		[[nodiscard]] static constexpr auto IsLowSurrogate(Foundation::Char16 Value) -> Foundation::Bool
		{
			return Value >= 0xDC00 && Value <= 0xDFFF;
		}

		[[nodiscard]] static constexpr auto IsSurrogate(Foundation::Char32 Value) -> Foundation::Bool
		{
			return Value >= 0xD800 && Value <= 0xDFFF;
		}

		[[nodiscard]] static constexpr auto IsValidCodePoint(Foundation::Char32 Value) -> Foundation::Bool
		{
			return Value <= 0x10FFFF && !IsSurrogate(Value);
		}

		static constexpr auto EncodeSurrogatePair(Foundation::Char32 CodePoint, Foundation::Char16& High, Foundation::Char16& Low) -> void
		{
			CodePoint -= 0x10000;

			High = Foundation::Cast::Auto<Foundation::Char16>(0xD800 + (CodePoint >> 10));
			Low = Foundation::Cast::Auto<Foundation::Char16>(0xDC00 + (CodePoint & SurrogatePayloadMask));
		}

		[[nodiscard]] static constexpr auto DecodeSurrogatePair(Foundation::Char16 High, Foundation::Char16 Low) -> Foundation::Char32
		{
			return Foundation::Cast::Auto<Foundation::Char32>(0x10000 + ((Foundation::Cast::Auto<Foundation::Uint32>(High) - 0xD800) << 10) + (Foundation::Cast::Auto<Foundation::Uint32>(Low) - 0xDC00));
		}

		[[nodiscard]] static auto Utf16ToUtf8(Library::U16StringView Source) -> Library::U8String
		{
			Library::U8String Result;
			Result.Reserve(Source.Size() + 1);

			for (Foundation::Size i = 0; i < Source.Size(); ++i)
			{
				auto CodePoint = Foundation::Cast::Auto<Foundation::Char32>(Source[i]);

				if (IsHighSurrogate(Source[i]))
				{
					if (i + 1 < Source.Size() && IsLowSurrogate(Source[i + 1]))
					{
						CodePoint = DecodeSurrogatePair(Source[i], Source[i + 1]);
						++i;
					}
					else
					{
						CodePoint = ReplacementCharacter;
					}
				}
				else if (IsLowSurrogate(Source[i]))
				{
					CodePoint = ReplacementCharacter;
				}

				AppendUtf8(Result, CodePoint);
			}

			return Result;
		}

		[[nodiscard]] static auto Utf8ToUtf16(Library::U8StringView Source) -> Library::U16String
		{
			Library::U16String Result;
			Result.Reserve(Source.Size() + 1);

			for (Foundation::Size i = 0; i < Source.Size();)
			{
				Foundation::Char32 CodePoint{};
				Foundation::Size Count{};

				if (!DecodeUtf8(Source, i, CodePoint, Count))
				{
					AppendUtf16(Result, ReplacementCharacter);
					++i;
					continue;
				}

				AppendUtf16(Result, CodePoint);
				i += Count;
			}

			return Result;
		}

		[[nodiscard]] static auto Utf32ToUtf8(Library::U32StringView Source) -> Library::U8String
		{
			Library::U8String Result;
			Result.Reserve(Source.Size() + 1);

			for (const auto CodePoint : Source)
			{
				AppendUtf8(Result, IsValidCodePoint(CodePoint) ? CodePoint : ReplacementCharacter);
			}

			return Result;
		}

		[[nodiscard]] static auto Utf8ToUtf32(Library::U8StringView Source) -> Library::U32String
		{
			Library::U32String Result;
			Result.Reserve(Source.Size() + 1);

			for (Foundation::Size i = 0; i < Source.Size();)
			{
				Foundation::Char32 CodePoint{};
				Foundation::Size Count{};

				if (!DecodeUtf8(Source, i, CodePoint, Count))
				{
					Result.PushBack(ReplacementCharacter);
					++i;
					continue;
				}

				Result.PushBack(CodePoint);
				i += Count;
			}

			return Result;
		}

		[[nodiscard]] static auto Utf16ToUtf32(Library::U16StringView Source) -> Library::U32String
		{
			Library::U32String Result;
			Result.Reserve(Source.Size() + 1);

			for (Foundation::Size i = 0; i < Source.Size(); ++i)
			{
				auto CodePoint = Foundation::Cast::Auto<Foundation::Char32>(Source[i]);

				if (IsHighSurrogate(Source[i]))
				{
					if (i + 1 < Source.Size() && IsLowSurrogate(Source[i + 1]))
					{
						CodePoint = DecodeSurrogatePair(Source[i], Source[i + 1]);
						++i;
					}
					else
					{
						CodePoint = ReplacementCharacter;
					}
				}
				else if (IsLowSurrogate(Source[i]))
				{
					CodePoint = ReplacementCharacter;
				}

				Result.PushBack(CodePoint);
			}

			return Result;
		}

		[[nodiscard]] static auto Utf32ToUtf16(Library::U32StringView Source) -> Library::U16String
		{
			Library::U16String Result;
			Result.Reserve(Source.Size() + 1);

			for (const auto CodePoint : Source)
			{
				AppendUtf16(Result, IsValidCodePoint(CodePoint) ? CodePoint : ReplacementCharacter);
			}

			return Result;
		}

		[[nodiscard]] static auto ToAscii(Library::WideStringView Source) -> Library::String
		{
			Library::String Result;
			Result.Reserve(Source.Size() + 1);

			for (const auto Character : Source)
			{
				if (Character <= Foundation::WChar(0x7F))
				{
					Result.PushBack(Foundation::Cast::Auto<Foundation::Char>(Character));
				}
				else
				{
					Result.PushBack('?');
				}
			}

			return Result;
		}

		[[nodiscard]] static auto ToWideAscii(Library::StringView Source) -> Library::WideString
		{
			Library::WideString Result;
			Result.Reserve(Source.Size() + 1);

			for (const auto Character : Source)
			{
				Result.PushBack(Foundation::Cast::Auto<Foundation::WChar>(Character));
			}

			return Result;
		}

		[[nodiscard]] static auto Utf16ToAscii(Library::U16StringView Source) -> Library::String
		{
			Library::String Result;
			Result.Reserve(Source.Size() + 1);

			for (const auto Character : Source)
			{
				if (Character <= Foundation::Char16(0x7F))
				{
					Result.PushBack(Foundation::Cast::Auto<Foundation::Char>(Character));
				}
				else
				{
					Result.PushBack('?');
				}
			}

			return Result;
		}

		[[nodiscard]] static auto WideToAscii(Library::WideStringView Source) -> Library::String
		{
			Library::String Result{};
			Result.Reserve(Source.Size());

			for (const auto Character : Source)
			{
				if (Character <= wchar_t(0x7F))
				{
					Result.PushBack(Foundation::Cast::Auto<Foundation::Char>(Character));
				}
				else
				{
					Result.PushBack('?');
				}
			}

			return Result;
		}

	private:
		static auto AppendUtf8(Library::U8String& Result, Foundation::Char32 CodePoint) -> void
		{
			if (!IsValidCodePoint(CodePoint))
			{
				CodePoint = ReplacementCharacter;
			}

			if (CodePoint <= 0x7F)
			{
				Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(CodePoint));
			}
			else if (CodePoint <= 0x7FF)
			{
				Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(0xC0 | (CodePoint >> 6)));
				Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(0x80 | (CodePoint & ContinuationMask)));
			}
			else if (CodePoint <= 0xFFFF)
			{
				Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(0xE0 | (CodePoint >> 12)));
				Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(0x80 | ((CodePoint >> 6) & ContinuationMask)));
				Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(0x80 | (CodePoint & ContinuationMask)));
			}
			else
			{
				Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(0xF0 | (CodePoint >> 18)));
				Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(0x80 | ((CodePoint >> 12) & ContinuationMask)));
				Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(0x80 | ((CodePoint >> 6) & ContinuationMask)));
				Result.PushBack(Foundation::Cast::Auto<Foundation::Char8>(0x80 | (CodePoint & ContinuationMask)));
			}
		}

		static auto AppendUtf16(Library::U16String& Result, Foundation::Char32 CodePoint) -> void
		{
			if (!IsValidCodePoint(CodePoint))
			{
				CodePoint = ReplacementCharacter;
			}

			if (CodePoint <= 0xFFFF)
			{
				Result.PushBack(Foundation::Cast::Auto<Foundation::Char16>(CodePoint));
				return;
			}

			Foundation::Char16 High{};
			Foundation::Char16 Low{};

			EncodeSurrogatePair(CodePoint, High, Low);

			Result.PushBack(High);
			Result.PushBack(Low);
		}

		[[nodiscard]] static auto DecodeUtf8(Library::U8StringView Source, Foundation::Size Position, Foundation::Char32& CodePoint, Foundation::Size& Count) -> Foundation::Bool
		{
			if (Position >= Source.Size())
			{
				return false;
			}

			const auto First = Foundation::Cast::Auto<Foundation::Uint8>(Source[Position]);

			if ((First & 0x80) == 0)
			{
				CodePoint = First & Utf8Payload1;
				Count = 1;
				return true;
			}

			if ((First & 0xE0) == 0xC0)
			{
				CodePoint = First & Utf8Payload2;
				Count = 2;
			}
			else if ((First & 0xF0) == 0xE0)
			{
				CodePoint = First & Utf8Payload3;
				Count = 3;
			}
			else if ((First & 0xF8) == 0xF0)
			{
				CodePoint = First & Utf8Payload4;
				Count = 4;
			}
			else
			{
				return false;
			}

			if (Position + Count > Source.Size())
			{
				return false;
			}

			for (Foundation::Size i = 1; i < Count; ++i)
			{
				const auto Byte = Foundation::Cast::Auto<Foundation::Uint8>(Source[Position + i]);

				if ((Byte & 0xC0) != 0x80)
				{
					return false;
				}

				CodePoint = (CodePoint << 6) | (Byte & ContinuationMask);
			}

			if (!IsValidCodePoint(CodePoint))
			{
				return false;
			}

			if ((Count == 2 && CodePoint <= 0x7F) || (Count == 3 && CodePoint <= 0x7FF) || (Count == 4 && CodePoint <= 0xFFFF))
			{
				return false;
			}

			return true;
		}

		inline static constexpr auto ContinuationMask = Foundation::Bit::LowMask<Foundation::Uint8>(6);
		inline static constexpr auto Utf8Payload1 = Foundation::Bit::LowMask<Foundation::Uint8>(7);
		inline static constexpr auto Utf8Payload2 = Foundation::Bit::LowMask<Foundation::Uint8>(5);
		inline static constexpr auto Utf8Payload3 = Foundation::Bit::LowMask<Foundation::Uint8>(4);
		inline static constexpr auto Utf8Payload4 = Foundation::Bit::LowMask<Foundation::Uint8>(3);
		inline static constexpr auto SurrogatePayloadMask = Foundation::Bit::LowMask<Foundation::Uint32>(10);
	};
}