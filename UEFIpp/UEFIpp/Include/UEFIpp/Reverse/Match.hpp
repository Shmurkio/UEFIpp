#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Reverse
{
	class Match
	{
	public:
		using Byte = Foundation::Uint8;
		using AddressType = const Byte*;

	public:
		constexpr Match() = default;

		constexpr Match(AddressType Address, Foundation::Size Offset) : Address_(Address), Offset_(Offset)
		{
		}

		~Match() = default;

		constexpr Match(const Match&) = default;
		constexpr Match(Match&&) noexcept = default;

		constexpr auto operator=(const Match&) -> Match & = default;
		constexpr auto operator=(Match&&) noexcept -> Match & = default;

		constexpr auto operator==(const Match&) const->Foundation::Bool = default;

	public:
		[[nodiscard]] constexpr auto Valid() const -> Foundation::Bool
		{
			return Address_ != nullptr;
		}

		[[nodiscard]] constexpr auto Address() const -> AddressType
		{
			return Address_;
		}

		[[nodiscard]] constexpr auto Offset() const -> Foundation::Size
		{
			return Offset_;
		}

		template<typename T>
		[[nodiscard]] constexpr auto As() const -> const T*
		{
			return reinterpret_cast<const T*>(Address_);
		}

	private:
		AddressType Address_{};
		Foundation::Size Offset_{};
	};
}