#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Reverse
{
	class PatternByte
	{
	public:
		constexpr PatternByte() = default;

		constexpr PatternByte(Foundation::Uint8 Value, Foundation::Bool Wildcard) : Value_(Value), Wildcard_(Wildcard)
		{
		}

		~PatternByte() = default;

		constexpr PatternByte(const PatternByte&) = default;
		constexpr PatternByte(PatternByte&&) noexcept = default;

		constexpr auto operator=(const PatternByte&) -> PatternByte & = default;
		constexpr auto operator=(PatternByte&&) noexcept -> PatternByte & = default;

		constexpr auto operator==(const PatternByte&) const->Foundation::Bool = default;

		[[nodiscard]] static constexpr auto Wildcard() -> PatternByte
		{
			return { 0, true };
		}

		[[nodiscard]] static constexpr auto Value(Foundation::Uint8 Byte) -> PatternByte
		{
			return { Byte, false };
		}

		[[nodiscard]] constexpr auto Matches(Foundation::Uint8 Byte) const -> Foundation::Bool
		{
			return Wildcard_ || Value_ == Byte;
		}

		[[nodiscard]] constexpr auto IsWildcard() const -> Foundation::Bool
		{
			return Wildcard_;
		}

		[[nodiscard]] constexpr auto GetValue() const -> Foundation::Uint8
		{
			return Value_;
		}

	private:
		Foundation::Uint8 Value_{};
		Foundation::Bool Wildcard_{ true };
	};
}