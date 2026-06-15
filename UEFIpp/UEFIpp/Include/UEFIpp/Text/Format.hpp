#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/String/BasicString.hpp>
#include <UEFIpp/UEFI/Guid.hpp>
#include <UEFIpp/UEFI/Status.hpp>

namespace UEFIpp::Text
{
	class Format
	{
	public:
		Format() = delete;

		template<typename TChar>
		[[nodiscard]] static constexpr auto HexCharacter(Foundation::Uint8 Value, Foundation::Bool Uppercase = true) -> TChar
		{
			Value &= Foundation::Bit::LowMask<Foundation::Uint8>(4);

			if (Value < 10)
			{
				return Foundation::Cast::Auto<TChar>(TChar('0') + Value);
			}

			return Foundation::Cast::Auto<TChar>((Uppercase ? TChar('A') : TChar('a')) + (Value - 10));
		}

		template<typename TString>
		[[nodiscard]] static auto UInt64(Foundation::Uint64 Value) -> TString
		{
			using CharType = typename TString::ValueType;

			TString Result;

			if (Value == 0)
			{
				Result.PushBack(CharType('0'));
				return Result;
			}

			CharType Buffer[20]{};
			Foundation::Size Count{};

			while (Value)
			{
				Buffer[Count++] = Foundation::Cast::Auto<CharType>(CharType('0') + (Value % 10));
				Value /= 10;
			}

			while (Count)
			{
				Result.PushBack(Buffer[--Count]);
			}

			return Result;
		}

		template<typename TString>
		[[nodiscard]] static auto Int64(Foundation::Int64 Value) -> TString
		{
			using CharType = typename TString::ValueType;

			TString Result;

			if (Value < 0)
			{
				Result.PushBack(CharType('-'));
				Result += UInt64<TString>(Foundation::Cast::Auto<Foundation::Uint64>(-(Value + 1)) + 1);
				return Result;
			}

			return UInt64<TString>(Foundation::Cast::Auto<Foundation::Uint64>(Value));
		}

		template<typename TString>
		[[nodiscard]] static auto HexUInt64(Foundation::Uint64 Value, Foundation::Size Width = 0, Foundation::Bool Uppercase = true) -> TString
		{
			using CharType = typename TString::ValueType;

			TString Result;
			CharType Buffer[16]{};

			constexpr auto NibbleMask = Foundation::Bit::LowMask<Foundation::Uint64>(4);

			Foundation::Size Count{};

			if (!Value)
			{
				Buffer[Count++] = CharType('0');
			}
			else
			{
				while (Value)
				{
					const auto Nibble = Foundation::Cast::Auto<Foundation::Uint8>(Value & NibbleMask);
					Buffer[Count++] = HexCharacter<CharType>(Nibble, Uppercase);
					Value >>= 4;
				}
			}

			for (Foundation::Size i = Count; i < Width; ++i)
			{
				Result.PushBack(CharType('0'));
			}

			while (Count)
			{
				Result.PushBack(Buffer[--Count]);
			}

			return Result;
		}

		template<typename TString>
		[[nodiscard]] static auto Guid(const UEFI::Guid& Value) -> TString
		{
			using CharType = typename TString::ValueType;

			TString Result;
			Result.Reserve(37);

			Result += HexUInt64<TString>(Value.Data1, 8);
			Result.PushBack(CharType('-'));

			Result += HexUInt64<TString>(Value.Data2, 4);
			Result.PushBack(CharType('-'));

			Result += HexUInt64<TString>(Value.Data3, 4);
			Result.PushBack(CharType('-'));

			Result += HexUInt64<TString>(Value.Data4[0], 2);
			Result += HexUInt64<TString>(Value.Data4[1], 2);
			Result.PushBack(CharType('-'));

			for (Foundation::Size i = 2; i < 8; ++i)
			{
				Result += HexUInt64<TString>(Value.Data4[i], 2);
			}

			return Result;
		}

		template<typename TString>
		[[nodiscard]] static auto Status(UEFI::Status Value) -> TString
		{
			using CharType = typename TString::ValueType;

			if (const auto* Name = UEFI::StatusName(Value.Code()))
			{
				TString Result;

				while (*Name)
				{
					Result.PushBack(Foundation::Cast::Auto<CharType>(*Name++));
				}

				return Result;
			}

			TString Result;
			const auto* Prefix = "UnknownStatus(0x";

			while (*Prefix)
			{
				Result.PushBack(Foundation::Cast::Auto<CharType>(*Prefix++));
			}

			Result += HexUInt64<TString>(Value.Value());
			Result.PushBack(CharType(')'));

			return Result;
		}

		[[nodiscard]] static auto UInt64(Foundation::Uint64 Value) -> Library::String
		{
			return UInt64<Library::String>(Value);
		}

		[[nodiscard]] static auto Int64(Foundation::Int64 Value) -> Library::String
		{
			return Int64<Library::String>(Value);
		}

		[[nodiscard]] static auto Guid(const UEFI::Guid& Value) -> Library::String
		{
			return Guid<Library::String>(Value);
		}

		[[nodiscard]] static auto Status(UEFI::Status Value) -> Library::String
		{
			return Status<Library::String>(Value);
		}
	};
}