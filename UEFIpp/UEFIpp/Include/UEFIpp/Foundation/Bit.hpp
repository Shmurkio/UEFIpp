#pragma once

namespace UEFIpp::Foundation
{
	class Bit
	{
	public:
		Bit() = delete;

		template<typename T, typename U>
		[[nodiscard]] static constexpr auto Mask(U BitIndex) -> T
		{
			return static_cast<T>(T{ 1 } << BitIndex);
		}

		template<typename T>
		[[nodiscard]] static constexpr auto LowMask(T Width) -> T
		{
			return Width == T{ 0 } ? T{ 0 } : static_cast<T>((T{ 1 } << Width) - T{ 1 });
		}

		template<typename T>
		[[nodiscard]] static constexpr auto RangeMask(T Offset, T Width) -> T
		{
			return static_cast<T>(LowMask(Width) << Offset);
		}

		template<typename T>
		[[nodiscard]] static constexpr auto IsSet(T Value, T Mask) -> bool
		{
			return (Value & Mask) == Mask;
		}

		template<typename T>
		[[nodiscard]] static constexpr auto HasAny(T Value, T Mask) -> bool
		{
			return (Value & Mask) != T{ 0 };
		}

		template<typename T>
		[[nodiscard]] static constexpr auto Set(T Value, T Mask) -> T
		{
			return static_cast<T>(Value | Mask);
		}

		template<typename T>
		[[nodiscard]] static constexpr auto Clear(T Value, T Mask) -> T
		{
			return static_cast<T>(Value & ~Mask);
		}

		template<typename T>
		[[nodiscard]] static constexpr auto Toggle(T Value, T Mask) -> T
		{
			return static_cast<T>(Value ^ Mask);
		}

		template<typename T>
		[[nodiscard]] static constexpr auto Assign(T Value, T Mask, bool Enabled) -> T
		{
			return Enabled ? Set(Value, Mask) : Clear(Value, Mask);
		}

		template<typename T>
		[[nodiscard]] static constexpr auto ClearAndSet(T Value, T ClearMask, T SetMask) -> T
		{
			return Set(Clear(Value, ClearMask), SetMask);
		}

		template<typename T>
		[[nodiscard]] static constexpr auto Replace(T Value, T Mask, T Replacement) -> T
		{
			return static_cast<T>((Value & ~Mask) | (Replacement & Mask));
		}

		template<typename T>
		[[nodiscard]] static constexpr auto Extract(T Value, T Offset, T Width) -> T
		{
			return static_cast<T>((Value >> Offset) & LowMask(Width));
		}

		template<typename T>
		[[nodiscard]] static constexpr auto Insert(T Value, T Field, T Offset, T Width) -> T
		{
			const auto Mask = RangeMask(Offset, Width);
			return Replace(Value, Mask, static_cast<T>(Field << Offset));
		}

		template<typename T>
		[[nodiscard]] static constexpr auto AlignDown(T Value, T Alignment) -> T
		{
			return static_cast<T>(Value & ~(Alignment - T{ 1 }));
		}

		template<typename T>
		[[nodiscard]] static constexpr auto AlignUp(T Value, T Alignment) -> T
		{
			return AlignDown(static_cast<T>(Value + Alignment - T{ 1 }), Alignment);
		}

		template<typename T>
		[[nodiscard]] static constexpr auto IsAligned(T Value, T Alignment) -> bool
		{
			return (Value & (Alignment - T{ 1 })) == T{ 0 };
		}

		template<typename T>
		[[nodiscard]] static constexpr auto IsPowerOfTwo(T Value) -> bool
		{
			return Value != T{ 0 } && (Value & (Value - T{ 1 })) == T{ 0 };
		}

		template<typename T, typename U>
		[[nodiscard]] static constexpr auto RotateLeft(T Value, U Count) -> T
		{
			constexpr auto Bits = static_cast<U>(sizeof(T) * 8);
			Count %= Bits;
			return Count == U{ 0 } ? Value : static_cast<T>((Value << Count) | (Value >> (Bits - Count)));
		}

		template<typename T>
		[[nodiscard]] static constexpr auto SignExtend(T Value, T Width) -> T
		{
			if (Width == T{ 0 })
			{
				return T{ 0 };
			}

			const auto SignBit = Mask<T>(Width - T{ 1 });

			if (!HasAny(Value, SignBit))
			{
				return Value;
			}

			return Set(Value, ~LowMask(Width));
		}

		template<typename T, typename U>
		[[nodiscard]] static constexpr auto RotateRight(T Value, U Count) -> T
		{
			constexpr auto Bits = static_cast<U>(sizeof(T) * 8);
			Count %= Bits;
			return Count == U{ 0 } ? Value : static_cast<T>((Value >> Count) | (Value << (Bits - Count)));
		}

		template<typename T, typename U>
		[[nodiscard]] static constexpr auto Byte(T Value, U Index) -> Foundation::Byte
		{
			return static_cast<Foundation::Byte>((Value >> (Index * U{ 8 })) & T { 0xFF });
		}

		template<typename T, typename U>
		[[nodiscard]] static constexpr auto FromByte(Foundation::Byte Value, U Index) -> T
		{
			return static_cast<T>(static_cast<T>(Value) << (Index * U{ 8 }));
		}
	};
}