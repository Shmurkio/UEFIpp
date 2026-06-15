#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/String/String.hpp>

namespace UEFIpp::Crypto::Encoding
{
	enum class HexCase : Foundation::Uint8
	{
		Lower,
		Upper,
	};

	class Hex
	{
	public:
		using Byte = Foundation::Uint8;
		using SizeType = Foundation::Size;
		using ByteSpan = Library::Span<const Byte>;
		using MutableByteSpan = Library::Span<Byte>;
		using StringType = Library::String;
		using StringViewType = Library::StringView;
		using ByteVector = Library::Vector<Byte>;

	public:
		Hex() = delete;

		[[nodiscard]] static constexpr auto EncodedSize(SizeType ByteCount) -> SizeType
		{
			return ByteCount * 2;
		}

		[[nodiscard]] static constexpr auto DecodedSize(SizeType CharacterCount) -> SizeType
		{
			return CharacterCount / 2;
		}

		[[nodiscard]] static constexpr auto IsHexDigit(Foundation::Char Character) -> Foundation::Bool
		{
			return (Character >= '0' && Character <= '9') || (Character >= 'a' && Character <= 'f') || (Character >= 'A' && Character <= 'F');
		}

		[[nodiscard]] static constexpr auto NibbleToChar(Byte Value, HexCase Case = HexCase::Lower) -> Foundation::Char
		{
			Value &= 0x0F;

			if (Value < 10)
			{
				return Foundation::Cast::Auto<Foundation::Char>('0' + Value);
			}

			const auto Base = Case == HexCase::Upper ? 'A' : 'a';

			return Foundation::Cast::Auto<Foundation::Char>(Base + (Value - 10));
		}

		[[nodiscard]] static constexpr auto CharToNibble(Foundation::Char Character, Byte& Value) -> Foundation::Bool
		{
			if (Character >= '0' && Character <= '9')
			{
				Value = Foundation::Cast::Auto<Byte>(Character - '0');
				return true;
			}

			if (Character >= 'a' && Character <= 'f')
			{
				Value = Foundation::Cast::Auto<Byte>(10 + Character - 'a');
				return true;
			}

			if (Character >= 'A' && Character <= 'F')
			{
				Value = Foundation::Cast::Auto<Byte>(10 + Character - 'A');
				return true;
			}

			Value = 0;

			return false;
		}

		[[nodiscard]] static auto EncodeTo(ByteSpan Input, Library::Span<Foundation::Char> Output, HexCase Case = HexCase::Lower) -> Foundation::Bool
		{
			if (Output.Size() < EncodedSize(Input.Size()))
			{
				return false;
			}

			for (SizeType i = 0; i < Input.Size(); ++i)
			{
				const auto ByteValue = Input[i];
				Output[i * 2] = NibbleToChar(Foundation::Cast::Auto<Byte>(ByteValue >> 4), Case);
				Output[i * 2 + 1] = NibbleToChar(Foundation::Cast::Auto<Byte>(ByteValue & 0x0F), Case);
			}

			return true;
		}

		[[nodiscard]] static auto Encode(ByteSpan Input, HexCase Case = HexCase::Lower) -> StringType
		{
			StringType Result{};

			if (!Result.Resize(EncodedSize(Input.Size())))
			{
				return {};
			}

			EncodeTo(Input, Library::Span<Foundation::Char>{ Result.Data(), Result.Size() }, Case);

			return Result;
		}

		[[nodiscard]] static auto DecodeTo(StringViewType Input, MutableByteSpan Output) -> Foundation::Bool
		{
			if ((Input.Size() & 1) != 0 || Output.Size() < DecodedSize(Input.Size()))
			{
				return false;
			}

			for (SizeType i = 0; i < Input.Size(); i += 2)
			{
				Byte High{};
				Byte Low{};

				if (!CharToNibble(Input[i], High) || !CharToNibble(Input[i + 1], Low))
				{
					return false;
				}

				Output[i / 2] = Foundation::Cast::Auto<Byte>((High << 4) | Low);
			}

			return true;
		}

		[[nodiscard]] static auto Decode(StringViewType Input, ByteVector& Output) -> Foundation::Bool
		{
			if ((Input.Size() & 1) != 0)
			{
				return false;
			}

			if (!Output.Resize(DecodedSize(Input.Size())))
			{
				return false;
			}

			if (!DecodeTo(Input, Output.View()))
			{
				Output.Clear();
				return false;
			}

			return true;
		}

		[[nodiscard]] static auto Decode(StringViewType Input) -> ByteVector
		{
			ByteVector Result{};
			Decode(Input, Result);

			return Result;
		}
	};
}