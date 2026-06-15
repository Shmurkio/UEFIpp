#pragma once

#include <UEFIpp/Foundation/Traits.hpp>

namespace UEFIpp::Foundation
{
	class Utility
	{
	public:
		Utility() = delete;

		template<typename T>
		[[nodiscard]] static constexpr auto Move(T&& Value) noexcept -> Traits::RemoveReferenceType<T>&&
		{
			return static_cast<Traits::RemoveReferenceType<T>&&>(Value);
		}

		template<typename T>
		[[nodiscard]] static constexpr auto Forward(Traits::RemoveReferenceType<T>& Value) noexcept -> T&&
		{
			return static_cast<T&&>(Value);
		}

		template<typename T>
		[[nodiscard]] static constexpr auto Forward(Traits::RemoveReferenceType<T>&& Value) noexcept -> T&&
		{
			static_assert(!Traits::IsLValueReference<T>::Value, "bad Forward call");
			return static_cast<T&&>(Value);
		}

		template<typename T>
		static constexpr auto Swap(T& A, T& B) noexcept -> void
		{
			auto Temporary = Utility::Move(A);
			A = Utility::Move(B);
			B = Utility::Move(Temporary);
		}

		template<typename T, typename U = T>
		static constexpr auto Exchange(T& Object, U&& NewValue) -> T
		{
			auto OldValue = Utility::Move(Object);
			Object = Utility::Forward<U>(NewValue);
			return OldValue;
		}

		template<typename T>
		[[nodiscard]] static constexpr auto Min(const T& A, const T& B) -> const T&
		{
			return B < A ? B : A;
		}

		template<typename T>
		[[nodiscard]] static constexpr auto Max(const T& A, const T& B) -> const T&
		{
			return A < B ? B : A;
		}

		template<typename T>
		[[nodiscard]] static constexpr auto Clamp(const T& Value, const T& Low, const T& High) -> const T&
		{
			return Value < Low ? Low : High < Value ? High : Value;
		}
	};
}