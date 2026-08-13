#pragma once

#include <UEFIpp/Foundation/Cast.hpp>
#include <UEFIpp/Foundation/Concepts.hpp>

namespace UEFIpp::Foundation
{
	template<Concepts::Enum TEnum>
	class Flags final
	{
	public:
		using Enum = TEnum;
		using ValueType = decltype(Cast::Underlying(TEnum{}));

		constexpr Flags() noexcept = default;

		constexpr Flags(TEnum Value) noexcept :
			Value_(Cast::Underlying(Value))
		{
		}

		explicit constexpr Flags(ValueType Value) noexcept :
			Value_(Value)
		{
		}

		template<typename... TValues>
		requires (sizeof...(TValues) > 1) &&
			(Concepts::Same<Traits::RemoveCvReferenceType<TValues>, TEnum> && ...)
		constexpr Flags(TValues... Values) noexcept :
			Value_((ValueType{} | ... | Cast::Underlying(Values)))
		{
		}

		[[nodiscard]] constexpr auto Value() const noexcept -> ValueType
		{
			return Value_;
		}

		[[nodiscard]] constexpr auto Empty() const noexcept -> Bool
		{
			return Value_ == 0;
		}

		[[nodiscard]] constexpr auto Has(TEnum Value) const noexcept -> Bool
		{
			const auto Mask = Cast::Underlying(Value);
			return (Value_ & Mask) == Mask;
		}

		[[nodiscard]] constexpr auto HasAll(Flags Values) const noexcept -> Bool
		{
			return (Value_ & Values.Value_) == Values.Value_;
		}

		[[nodiscard]] constexpr auto HasAny(Flags Values) const noexcept -> Bool
		{
			return (Value_ & Values.Value_) != 0;
		}

		[[nodiscard]] constexpr auto With(TEnum Value) const noexcept -> Flags
		{
			return Flags{ Value_ | Cast::Underlying(Value) };
		}

		[[nodiscard]] constexpr auto Without(TEnum Value) const noexcept -> Flags
		{
			return Flags{ Value_ & ~Cast::Underlying(Value) };
		}

		constexpr auto Add(TEnum Value) noexcept -> Flags&
		{
			Value_ |= Cast::Underlying(Value);
			return *this;
		}

		constexpr auto Remove(TEnum Value) noexcept -> Flags&
		{
			Value_ &= ~Cast::Underlying(Value);
			return *this;
		}

		constexpr auto Clear() noexcept -> Void
		{
			Value_ = 0;
		}

		[[nodiscard]] constexpr explicit operator Bool() const noexcept
		{
			return !Empty();
		}

		[[nodiscard]] constexpr auto operator<=>(const Flags&) const noexcept = default;

		[[nodiscard]] friend constexpr auto operator|(Flags Left, Flags Right) noexcept -> Flags
		{
			return Flags{ Left.Value_ | Right.Value_ };
		}

		[[nodiscard]] friend constexpr auto operator&(Flags Left, Flags Right) noexcept -> Flags
		{
			return Flags{ Left.Value_ & Right.Value_ };
		}

	private:
		ValueType Value_{};
	};
}
