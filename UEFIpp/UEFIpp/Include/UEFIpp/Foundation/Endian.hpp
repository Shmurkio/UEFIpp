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

		template<Concepts::Integral T>
		[[nodiscard]] static constexpr auto LoadBig(const Byte* Data) -> T
		{
			if constexpr (sizeof(T) == 1)
			{
				return static_cast<T>(Data[0]);
			}
			else if constexpr (sizeof(T) == 2)
			{
				return static_cast<T>(
					(static_cast<Uint16>(Data[0]) << 8) |
					(static_cast<Uint16>(Data[1]))
					);
			}
			else if constexpr (sizeof(T) == 4)
			{
				return static_cast<T>(
					(static_cast<Uint32>(Data[0]) << 24) |
					(static_cast<Uint32>(Data[1]) << 16) |
					(static_cast<Uint32>(Data[2]) << 8) |
					(static_cast<Uint32>(Data[3]))
					);
			}
			else if constexpr (sizeof(T) == 8)
			{
				return static_cast<T>(
					(static_cast<Uint64>(Data[0]) << 56) |
					(static_cast<Uint64>(Data[1]) << 48) |
					(static_cast<Uint64>(Data[2]) << 40) |
					(static_cast<Uint64>(Data[3]) << 32) |
					(static_cast<Uint64>(Data[4]) << 24) |
					(static_cast<Uint64>(Data[5]) << 16) |
					(static_cast<Uint64>(Data[6]) << 8) |
					(static_cast<Uint64>(Data[7]))
					);
			}
			else
			{
				static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8);
			}
		}

		template<Concepts::Integral T>
		static constexpr auto StoreBig(Byte * Data, T Value) -> void
		{
			if constexpr (sizeof(T) == 1)
			{
				Data[0] = static_cast<Byte>(Value);
			}
			else if constexpr (sizeof(T) == 2)
			{
				const auto V = static_cast<Uint16>(Value);

				Data[0] = static_cast<Byte>(V >> 8);
				Data[1] = static_cast<Byte>(V);
			}
			else if constexpr (sizeof(T) == 4)
			{
				const auto V = static_cast<Uint32>(Value);

				Data[0] = static_cast<Byte>(V >> 24);
				Data[1] = static_cast<Byte>(V >> 16);
				Data[2] = static_cast<Byte>(V >> 8);
				Data[3] = static_cast<Byte>(V);
			}
			else if constexpr (sizeof(T) == 8)
			{
				const auto V = static_cast<Uint64>(Value);

				Data[0] = static_cast<Byte>(V >> 56);
				Data[1] = static_cast<Byte>(V >> 48);
				Data[2] = static_cast<Byte>(V >> 40);
				Data[3] = static_cast<Byte>(V >> 32);
				Data[4] = static_cast<Byte>(V >> 24);
				Data[5] = static_cast<Byte>(V >> 16);
				Data[6] = static_cast<Byte>(V >> 8);
				Data[7] = static_cast<Byte>(V);
			}
			else
			{
				static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8);
			}
		}
	};
}