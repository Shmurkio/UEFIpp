#pragma once

#include <UEFIpp/Foundation/Types.hpp>
#include <UEFIpp/Foundation/Compare.hpp>

namespace UEFIpp::Foundation
{
	class TypeId final
	{
	public:
		constexpr TypeId() noexcept = default;

		explicit constexpr TypeId(Uint64 Value) noexcept :
			Value_(Value)
		{
		}

		template<typename T>
		[[nodiscard]] static consteval auto Of() noexcept -> TypeId
		{
			return TypeId{ Hash(__FUNCSIG__) };
		}

		[[nodiscard]] constexpr auto Value() const noexcept -> Uint64
		{
			return Value_;
		}

		[[nodiscard]] constexpr explicit operator Bool() const noexcept
		{
			return Value_ != 0;
		}

		[[nodiscard]] constexpr auto operator<=>(const TypeId&) const noexcept = default;

	private:
		[[nodiscard]] static consteval auto Hash(const Char* Text) noexcept -> Uint64
		{
			Uint64 Value = 14695981039346656037ull;

			while (*Text)
			{
				Value ^= static_cast<Uint8>(*Text++);
				Value *= 1099511628211ull;
			}

			return Value ? Value : 1;
		}

		Uint64 Value_{};
	};
}
