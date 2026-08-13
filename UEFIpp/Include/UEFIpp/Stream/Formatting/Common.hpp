#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/String/String.hpp>

namespace UEFIpp::Stream
{
	enum class Base { Dec, Hex };
	enum class HexCase { Upper, Lower };

	struct EndLine {};
	struct Hex {};
	struct Dec {};
	struct Upper {};
	struct Lower {};
	struct Flush {};
	struct Width { Foundation::UintN Value; };
	struct Fill { Foundation::Char Value; };
	struct Precision { Foundation::Uint8 Digits; };
	struct Clear {};

	inline constexpr EndLine Endl{};
	inline constexpr Flush FlushNow{};
	inline constexpr Hex Hexadecimal{};
	inline constexpr Dec Decimal{};
	inline constexpr Upper Uppercase{};
	inline constexpr Lower Lowercase{};
	inline constexpr Clear ClearScreen{};

}