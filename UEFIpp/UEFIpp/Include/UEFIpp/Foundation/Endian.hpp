#pragma once

#include <UEFIpp/Foundation/Concepts.hpp>

namespace UEFIpp::Foundation
{
	class Endian
	{
	public:
		Endian() = delete;

		[[nodiscard]] static constexpr auto Swap(Uint16 Value) -> Uint16
		{
			return static_cast<Uint16>((Value << 8) | (Value >> 8));
		}

		[[nodiscard]] static constexpr auto Swap(Uint32 Value) -> Uint32
		{
			return ((Value & 0x000000FFU) << 24) | ((Value & 0x0000FF00U) << 8) | ((Value & 0x00FF0000U) >> 8) | ((Value & 0xFF000000U) >> 24);
		}

		[[nodiscard]] static constexpr auto Swap(Uint64 Value) -> Uint64
		{
			return ((Value & 0x00000000000000FFULL) << 56) | ((Value & 0x000000000000FF00ULL) << 40) | ((Value & 0x0000000000FF0000ULL) << 24) | ((Value & 0x00000000FF000000ULL) << 8) | ((Value & 0x000000FF00000000ULL) >> 8) | ((Value & 0x0000FF0000000000ULL) >> 24) | ((Value & 0x00FF000000000000ULL) >> 40) | ((Value & 0xFF00000000000000ULL) >> 56);
		}

		template<Concepts::Integral T>
		[[nodiscard]] static constexpr auto Swap(T Value) -> T
		{
			if constexpr (sizeof(T) == 1)
			{
				return Value;
			}
			else if constexpr (sizeof(T) == 2)
			{
				return static_cast<T>(Swap(static_cast<Uint16>(Value)));
			}
			else if constexpr (sizeof(T) == 4)
			{
				return static_cast<T>(Swap(static_cast<Uint32>(Value)));
			}
			else if constexpr (sizeof(T) == 8)
			{
				return static_cast<T>(Swap(static_cast<Uint64>(Value)));
			}
			else
			{
				static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8);
			}
		}

		template<Concepts::Integral T>
		[[nodiscard]] static constexpr auto FromLittle(T Value) -> T
		{
			return Value;
		}

		template<Concepts::Integral T>
		[[nodiscard]] static constexpr auto ToLittle(T Value) -> T
		{
			return Value;
		}

		template<Concepts::Integral T>
		[[nodiscard]] static constexpr auto FromBig(T Value) -> T
		{
			return Swap(Value);
		}

		template<Concepts::Integral T>
		[[nodiscard]] static constexpr auto ToBig(T Value) -> T
		{
			return Swap(Value);
		}
	};
}