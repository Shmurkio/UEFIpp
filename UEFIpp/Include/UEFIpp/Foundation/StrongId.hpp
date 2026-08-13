#pragma once

#include <UEFIpp/Foundation/Concepts.hpp>
#include <UEFIpp/Foundation/Compare.hpp>

namespace UEFIpp::Foundation
{
	template<typename TTag, Concepts::Integral TValue = Uint64>
	class StrongId final
	{
	public:
		using Tag = TTag;
		using ValueType = TValue;

		constexpr StrongId() noexcept = default;

		explicit constexpr StrongId(TValue Value) noexcept :
			Value_(Value)
		{
		}

		[[nodiscard]] constexpr auto Value() const noexcept -> TValue
		{
			return Value_;
		}

		[[nodiscard]] constexpr explicit operator Bool() const noexcept
		{
			return Value_ != 0;
		}

		[[nodiscard]] constexpr auto operator<=>(const StrongId&) const noexcept = default;

	private:
		TValue Value_{};
	};
}
