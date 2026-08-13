#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/String/StringView.hpp>
#include <UEFIpp/UEFI/Guid.hpp>
#include <UEFIpp/UEFI/Status.hpp>

namespace UEFIpp::Text
{
	class Parse
	{
	public:
		Parse() = delete;

		template<typename TChar>
		[[nodiscard]] static constexpr auto IsDigit(TChar Character) -> Foundation::Bool
		{
			return Character >= TChar('0') && Character <= TChar('9');
		}

		template<typename TChar>
		[[nodiscard]] static constexpr auto IsHexDigit(TChar Character) -> Foundation::Bool
		{
			return (Character >= TChar('0') && Character <= TChar('9')) || (Character >= TChar('A') && Character <= TChar('F')) || (Character >= TChar('a') && Character <= TChar('f'));
		}

		template<typename TChar>
		[[nodiscard]] static constexpr auto HexValue(TChar Character, Foundation::Uint8& Value) -> Foundation::Bool
		{
			if (Character >= TChar('0') && Character <= TChar('9'))
			{
				Value = Foundation::Cast::Auto<Foundation::Uint8>(Character - TChar('0'));
				return true;
			}

			if (Character >= TChar('A') && Character <= TChar('F'))
			{
				Value = Foundation::Cast::Auto<Foundation::Uint8>(Character - TChar('A') + 10);
				return true;
			}

			if (Character >= TChar('a') && Character <= TChar('f'))
			{
				Value = Foundation::Cast::Auto<Foundation::Uint8>(Character - TChar('a') + 10);
				return true;
			}

			return false;
		}

		template<typename TChar>
		[[nodiscard]] static constexpr auto UInt64(Library::BasicStringView<TChar> Text, Foundation::Uint64& Value) -> Foundation::Bool
		{
			Value = 0;

			if (Text.Empty())
			{
				return false;
			}

			constexpr auto MaxValue = ~Foundation::Uint64{};

			for (const auto Character : Text)
			{
				if (!IsDigit(Character))
				{
					return false;
				}

				const auto Digit = Foundation::Cast::Auto<Foundation::Uint64>(Character - TChar('0'));

				if (Value > (MaxValue - Digit) / 10)
				{
					return false;
				}

				Value = Value * 10 + Digit;
			}

			return true;
		}

		template<typename TChar>
		[[nodiscard]] static constexpr auto UInt32(Library::BasicStringView<TChar> Text, Foundation::Uint32& Value) -> Foundation::Bool
		{
			Foundation::Uint64 Temporary{};

			if (!UInt64(Text, Temporary) || Temporary > 0xFFFFFFFFULL)
			{
				Value = 0;
				return false;
			}

			Value = Foundation::Cast::Auto<Foundation::Uint32>(Temporary);
			return true;
		}

		template<typename TChar>
		[[nodiscard]] static constexpr auto UInt8(Library::BasicStringView<TChar> Text, Foundation::Uint8& Value) -> Foundation::Bool
		{
			Foundation::Uint64 Temporary{};

			if (!UInt64(Text, Temporary) || Temporary > 0xFF)
			{
				Value = 0;
				return false;
			}

			Value = Foundation::Cast::Auto<Foundation::Uint8>(Temporary);
			return true;
		}

		template<typename TChar>
		[[nodiscard]] static constexpr auto HexUInt64(Library::BasicStringView<TChar> Text, Foundation::Uint64& Value) -> Foundation::Bool
		{
			Value = 0;

			if (Text.Empty())
			{
				return false;
			}

			constexpr auto MaxBeforeShift = Foundation::Bit::LowMask<Foundation::Uint64>(60);

			for (const auto Character : Text)
			{
				Foundation::Uint8 Digit{};

				if (!HexValue(Character, Digit))
				{
					return false;
				}

				if (Value > MaxBeforeShift)
				{
					return false;
				}

				Value = Foundation::Cast::Auto<Foundation::Uint64>((Value << 4) | Digit);
			}

			return true;
		}

		template<typename TChar>
		[[nodiscard]] static constexpr auto HexByte(Library::BasicStringView<TChar> Text, Foundation::Uint8& Value) -> Foundation::Bool
		{
			Foundation::Uint64 Temporary{};

			if (Text.Size() != 2 || !HexUInt64(Text, Temporary) || Temporary > 0xFF)
			{
				Value = 0;
				return false;
			}

			Value = Foundation::Cast::Auto<Foundation::Uint8>(Temporary);
			return true;
		}

		template<typename TChar>
		[[nodiscard]] static constexpr auto Guid(Library::BasicStringView<TChar> Text, UEFI::Guid& Guid) -> Foundation::Bool
		{
			Guid = {};

			if (Text.Size() != 36)
			{
				return false;
			}

			if (Text[8] != TChar('-') || Text[13] != TChar('-') || Text[18] != TChar('-') || Text[23] != TChar('-'))
			{
				return false;
			}

			Foundation::Uint64 Temporary{};

			if (!HexUInt64(Text.Substr(0, 8), Temporary)) return false;
			Guid.Data1 = Foundation::Cast::Auto<Foundation::Uint32>(Temporary);

			if (!HexUInt64(Text.Substr(9, 4), Temporary)) return false;
			Guid.Data2 = Foundation::Cast::Auto<Foundation::Uint16>(Temporary);

			if (!HexUInt64(Text.Substr(14, 4), Temporary)) return false;
			Guid.Data3 = Foundation::Cast::Auto<Foundation::Uint16>(Temporary);

			if (!HexByte(Text.Substr(19, 2), Guid.Data4[0])) return false;
			if (!HexByte(Text.Substr(21, 2), Guid.Data4[1])) return false;
			if (!HexByte(Text.Substr(24, 2), Guid.Data4[2])) return false;
			if (!HexByte(Text.Substr(26, 2), Guid.Data4[3])) return false;
			if (!HexByte(Text.Substr(28, 2), Guid.Data4[4])) return false;
			if (!HexByte(Text.Substr(30, 2), Guid.Data4[5])) return false;
			if (!HexByte(Text.Substr(32, 2), Guid.Data4[6])) return false;
			if (!HexByte(Text.Substr(34, 2), Guid.Data4[7])) return false;

			return true;
		}
	};
}