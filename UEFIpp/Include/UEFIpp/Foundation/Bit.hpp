#pragma once

#include <UEFIpp/Foundation/Concepts.hpp>
#include <UEFIpp/Foundation/Types.hpp>

namespace UEFIpp::Foundation::Detail::Bit
{
	template<typename T>
	using CleanType = Traits::RemoveCvType<T>;

	template<typename T, Bool = Traits::IsEnum<CleanType<T>>::Value>
	struct Storage
	{
		using Type = CleanType<T>;
	};

	template<typename T>
	struct Storage<T, true>
	{
		using Type = __underlying_type(CleanType<T>);
	};

	template<typename T>
	using StorageType = typename Storage<T>::Type;

	template<typename T>
	struct Unsigned;

	template<> struct Unsigned<Char> { using Type = Uint8; };
	template<> struct Unsigned<signed char> { using Type = unsigned char; };
	template<> struct Unsigned<unsigned char> { using Type = unsigned char; };
	template<> struct Unsigned<Char8> { using Type = Uint8; };
	template<> struct Unsigned<Char16> { using Type = Uint16; };
	template<> struct Unsigned<Char32> { using Type = Uint32; };
	template<> struct Unsigned<short> { using Type = unsigned short; };
	template<> struct Unsigned<unsigned short> { using Type = unsigned short; };
	template<> struct Unsigned<int> { using Type = unsigned int; };
	template<> struct Unsigned<unsigned int> { using Type = unsigned int; };
	template<> struct Unsigned<long> { using Type = unsigned long; };
	template<> struct Unsigned<unsigned long> { using Type = unsigned long; };
	template<> struct Unsigned<long long> { using Type = unsigned long long; };
	template<> struct Unsigned<unsigned long long> { using Type = unsigned long long; };

	template<>
	struct Unsigned<WChar>
	{
		using Type = Traits::ConditionalType<
			sizeof(WChar) == sizeof(Uint16),
			Uint16,
			Uint32
		>;
	};

	template<typename T>
	using UnsignedType = typename Unsigned<StorageType<T>>::Type;

	template<typename T>
	concept Value =
		(Concepts::Integral<CleanType<T>> || Concepts::Enum<CleanType<T>>) &&
		!Traits::IsSame<CleanType<T>, Bool>::Value;

	template<typename T>
	concept Index =
		Concepts::Integral<CleanType<T>> &&
		!Traits::IsSame<CleanType<T>, Bool>::Value;

	template<typename T>
	concept UnsignedIntegral =
		Index<T> &&
		(static_cast<CleanType<T>>(-1) > CleanType<T>{ 0 });

	template<typename T, typename U>
	concept CompatibleMask =
		Value<T> &&
		(
			Index<U> ||
			Traits::IsSame<CleanType<T>, CleanType<U>>::Value
		);

	template<Value T>
	[[nodiscard]] constexpr auto ToUnsigned(T Value) noexcept -> UnsignedType<T>
	{
		using Storage = StorageType<T>;
		using Unsigned = UnsignedType<T>;

		if constexpr (Concepts::Enum<CleanType<T>>)
		{
			return static_cast<Unsigned>(static_cast<Storage>(Value));
		}
		else
		{
			return static_cast<Unsigned>(Value);
		}
	}

	template<Value T>
	[[nodiscard]] constexpr auto FromUnsigned(UnsignedType<T> Value) noexcept -> CleanType<T>
	{
		using Result = CleanType<T>;
		using Storage = StorageType<T>;

		if constexpr (Concepts::Enum<Result>)
		{
			return static_cast<Result>(static_cast<Storage>(Value));
		}
		else
		{
			return static_cast<Result>(Value);
		}
	}

	template<Value T, typename U>
	requires CompatibleMask<T, U>
	[[nodiscard]] constexpr auto MaskToUnsigned(U Mask) noexcept -> UnsignedType<T>
	{
		if constexpr (Concepts::Enum<CleanType<U>>)
		{
			using MaskStorage = StorageType<U>;
			return static_cast<UnsignedType<T>>(static_cast<MaskStorage>(Mask));
		}
		else
		{
			return static_cast<UnsignedType<T>>(Mask);
		}
	}

	template<Index T>
	[[nodiscard]] constexpr auto IsNegative(T Value) noexcept -> Bool
	{
		if constexpr (static_cast<CleanType<T>>(-1) < CleanType<T>{ 0 })
		{
			return Value < T{ 0 };
		}
		else
		{
			return false;
		}
	}

	template<Value T>
	[[nodiscard]] constexpr auto Width() noexcept -> Size
	{
		return sizeof(StorageType<T>) * 8;
	}

	template<Value T>
	[[nodiscard]] constexpr auto AllOnes() noexcept -> UnsignedType<T>
	{
		return static_cast<UnsignedType<T>>(~UnsignedType<T>{ 0 });
	}

	template<Value T>
	[[nodiscard]] constexpr auto LowMask(Size Width) noexcept -> UnsignedType<T>
	{
		if (Width == 0)
		{
			return UnsignedType<T>{ 0 };
		}

		if (Width >= Detail::Bit::Width<T>())
		{
			return AllOnes<T>();
		}

		return static_cast<UnsignedType<T>>(
			(UnsignedType<T>{ 1 } << Width) - UnsignedType<T>{ 1 }
		);
	}
}

namespace UEFIpp::Foundation
{
	class Bit
	{
	public:
		Bit() = delete;

		template<Detail::Bit::Value T>
		[[nodiscard]] static constexpr auto Width() noexcept -> Size
		{
			return Detail::Bit::Width<T>();
		}

		template<Detail::Bit::Value T, Detail::Bit::Index U>
		[[nodiscard]] static constexpr auto Mask(U BitIndex) noexcept -> T
		{
			if (Detail::Bit::IsNegative(BitIndex))
			{
				return T{};
			}

			const auto Index = static_cast<Size>(BitIndex);

			if (Index >= Width<T>())
			{
				return T{};
			}

			return Detail::Bit::FromUnsigned<T>(
				static_cast<Detail::Bit::UnsignedType<T>>(
					Detail::Bit::UnsignedType<T>{ 1 } << Index
				)
			);
		}

		template<Detail::Bit::Index T>
		[[nodiscard]] static constexpr auto LowMask(T WidthValue) noexcept -> T
		{
			if (Detail::Bit::IsNegative(WidthValue))
			{
				return T{};
			}

			return Detail::Bit::FromUnsigned<T>(
				Detail::Bit::LowMask<T>(static_cast<Size>(WidthValue))
			);
		}

		template<Detail::Bit::Index T>
		[[nodiscard]] static constexpr auto RangeMask(T OffsetValue, T WidthValue) noexcept -> T
		{
			if (Detail::Bit::IsNegative(OffsetValue) || Detail::Bit::IsNegative(WidthValue))
			{
				return T{};
			}

			const auto Offset = static_cast<Size>(OffsetValue);
			const auto RequestedWidth = static_cast<Size>(WidthValue);
			const auto TypeWidth = Width<T>();

			if (Offset >= TypeWidth || RequestedWidth == 0)
			{
				return T{};
			}

			const auto AvailableWidth = TypeWidth - Offset;
			const auto EffectiveWidth = RequestedWidth < AvailableWidth
				? RequestedWidth
				: AvailableWidth;

			return Detail::Bit::FromUnsigned<T>(
				static_cast<Detail::Bit::UnsignedType<T>>(
					Detail::Bit::LowMask<T>(EffectiveWidth) << Offset
				)
			);
		}

		template<Detail::Bit::Value T, typename U>
		requires Detail::Bit::CompatibleMask<T, U>
		[[nodiscard]] static constexpr auto IsSet(T Value, U Mask) noexcept -> Bool
		{
			const auto RawMask = Detail::Bit::MaskToUnsigned<T>(Mask);
			return (Detail::Bit::ToUnsigned(Value) & RawMask) == RawMask;
		}

		template<Detail::Bit::Value T, typename U>
		requires Detail::Bit::CompatibleMask<T, U>
		[[nodiscard]] static constexpr auto HasAny(T Value, U Mask) noexcept -> Bool
		{
			return (
				Detail::Bit::ToUnsigned(Value) &
				Detail::Bit::MaskToUnsigned<T>(Mask)
			) != Detail::Bit::UnsignedType<T>{ 0 };
		}

		template<Detail::Bit::Value T, typename U>
		requires Detail::Bit::CompatibleMask<T, U>
		[[nodiscard]] static constexpr auto HasNone(T Value, U Mask) noexcept -> Bool
		{
			return !HasAny(Value, Mask);
		}

		template<Detail::Bit::Value T, typename U>
		requires Detail::Bit::CompatibleMask<T, U>
		[[nodiscard]] static constexpr auto Set(T Value, U Mask) noexcept -> T
		{
			return Detail::Bit::FromUnsigned<T>(
				static_cast<Detail::Bit::UnsignedType<T>>(
					Detail::Bit::ToUnsigned(Value) |
					Detail::Bit::MaskToUnsigned<T>(Mask)
				)
			);
		}

		template<Detail::Bit::Value T, typename U>
		requires Detail::Bit::CompatibleMask<T, U>
		[[nodiscard]] static constexpr auto Clear(T Value, U Mask) noexcept -> T
		{
			return Detail::Bit::FromUnsigned<T>(
				static_cast<Detail::Bit::UnsignedType<T>>(
					Detail::Bit::ToUnsigned(Value) &
					~Detail::Bit::MaskToUnsigned<T>(Mask)
				)
			);
		}

		template<Detail::Bit::Value T, typename U>
		requires Detail::Bit::CompatibleMask<T, U>
		[[nodiscard]] static constexpr auto Toggle(T Value, U Mask) noexcept -> T
		{
			return Detail::Bit::FromUnsigned<T>(
				static_cast<Detail::Bit::UnsignedType<T>>(
					Detail::Bit::ToUnsigned(Value) ^
					Detail::Bit::MaskToUnsigned<T>(Mask)
				)
			);
		}

		template<Detail::Bit::Value T, typename U>
		requires Detail::Bit::CompatibleMask<T, U>
		[[nodiscard]] static constexpr auto Assign(T Value, U Mask, Bool Enabled) noexcept -> T
		{
			return Enabled ? Set(Value, Mask) : Clear(Value, Mask);
		}

		template<Detail::Bit::Value T, Detail::Bit::Index U>
		[[nodiscard]] static constexpr auto IsBitSet(T Value, U BitIndex) noexcept -> Bool
		{
			const auto BitMask = Mask<T>(BitIndex);
			return BitMask != T{} && HasAny(Value, BitMask);
		}

		template<Detail::Bit::Value T, Detail::Bit::Index U>
		[[nodiscard]] static constexpr auto SetBit(T Value, U BitIndex) noexcept -> T
		{
			return Set(Value, Mask<T>(BitIndex));
		}

		template<Detail::Bit::Value T, Detail::Bit::Index U>
		[[nodiscard]] static constexpr auto ClearBit(T Value, U BitIndex) noexcept -> T
		{
			return Clear(Value, Mask<T>(BitIndex));
		}

		template<Detail::Bit::Value T, Detail::Bit::Index U>
		[[nodiscard]] static constexpr auto ToggleBit(T Value, U BitIndex) noexcept -> T
		{
			return Toggle(Value, Mask<T>(BitIndex));
		}

		template<Detail::Bit::Value T, Detail::Bit::Index U>
		[[nodiscard]] static constexpr auto AssignBit(T Value, U BitIndex, Bool Enabled) noexcept -> T
		{
			return Assign(Value, Mask<T>(BitIndex), Enabled);
		}

		template<Detail::Bit::Value T, typename U, typename V>
		requires (
			Detail::Bit::CompatibleMask<T, U> &&
			Detail::Bit::CompatibleMask<T, V>
		)
		[[nodiscard]] static constexpr auto ClearAndSet(
			T Value,
			U ClearMask,
			V SetMask
		) noexcept -> T
		{
			return Set(Clear(Value, ClearMask), SetMask);
		}

		template<Detail::Bit::Value T, typename U, typename V>
		requires (
			Detail::Bit::CompatibleMask<T, U> &&
			Detail::Bit::CompatibleMask<T, V>
		)
		[[nodiscard]] static constexpr auto Replace(
			T Value,
			U Mask,
			V Replacement
		) noexcept -> T
		{
			const auto RawValue = Detail::Bit::ToUnsigned(Value);
			const auto RawMask = Detail::Bit::MaskToUnsigned<T>(Mask);
			const auto RawReplacement = Detail::Bit::MaskToUnsigned<T>(Replacement);

			return Detail::Bit::FromUnsigned<T>(
				static_cast<Detail::Bit::UnsignedType<T>>(
					(RawValue & ~RawMask) |
					(RawReplacement & RawMask)
				)
			);
		}

		template<Detail::Bit::Value T, Detail::Bit::Index U, Detail::Bit::Index V>
		[[nodiscard]] static constexpr auto Extract(
			T Value,
			U OffsetValue,
			V WidthValue
		) noexcept -> T
		{
			if (Detail::Bit::IsNegative(OffsetValue) || Detail::Bit::IsNegative(WidthValue))
			{
				return T{};
			}

			const auto Offset = static_cast<Size>(OffsetValue);
			const auto FieldWidth = static_cast<Size>(WidthValue);

			if (Offset >= Width<T>() || FieldWidth == 0)
			{
				return T{};
			}

			return Detail::Bit::FromUnsigned<T>(
				static_cast<Detail::Bit::UnsignedType<T>>(
					(Detail::Bit::ToUnsigned(Value) >> Offset) &
					Detail::Bit::LowMask<T>(FieldWidth)
				)
			);
		}

		template<Detail::Bit::Value T, typename U, Detail::Bit::Index V, Detail::Bit::Index W>
		requires Detail::Bit::CompatibleMask<T, U>
		[[nodiscard]] static constexpr auto Insert(
			T Value,
			U Field,
			V OffsetValue,
			W WidthValue
		) noexcept -> T
		{
			if (Detail::Bit::IsNegative(OffsetValue) || Detail::Bit::IsNegative(WidthValue))
			{
				return Value;
			}

			const auto Offset = static_cast<Size>(OffsetValue);
			const auto FieldWidth = static_cast<Size>(WidthValue);
			const auto TypeWidth = Width<T>();

			if (Offset >= TypeWidth || FieldWidth == 0)
			{
				return Value;
			}

			const auto AvailableWidth = TypeWidth - Offset;
			const auto EffectiveWidth = FieldWidth < AvailableWidth
				? FieldWidth
				: AvailableWidth;

			const auto RawMask = static_cast<Detail::Bit::UnsignedType<T>>(
				Detail::Bit::LowMask<T>(EffectiveWidth) << Offset
			);
			const auto ShiftedField = static_cast<Detail::Bit::UnsignedType<T>>(
				Detail::Bit::MaskToUnsigned<T>(Field) << Offset
			);

			return Detail::Bit::FromUnsigned<T>(
				static_cast<Detail::Bit::UnsignedType<T>>(
					(Detail::Bit::ToUnsigned(Value) & ~RawMask) |
					(ShiftedField & RawMask)
				)
			);
		}

		template<Detail::Bit::UnsignedIntegral T>
		[[nodiscard]] static constexpr auto IsValidAlignment(T Alignment) noexcept -> Bool
		{
			return IsPowerOfTwo(Alignment);
		}

		template<Detail::Bit::UnsignedIntegral T, Detail::Bit::UnsignedIntegral U>
		[[nodiscard]] static constexpr auto TryAlignDown(
			T Value,
			U AlignmentValue,
			T& Result
		) noexcept -> Bool
		{
			if constexpr (sizeof(U) > sizeof(T))
			{
				if (AlignmentValue > static_cast<U>(~T{ 0 }))
				{
					return false;
				}
			}

			const auto Alignment = static_cast<T>(AlignmentValue);

			if (!IsValidAlignment(Alignment))
			{
				return false;
			}

			Result = static_cast<T>(Value & ~(Alignment - T{ 1 }));
			return true;
		}

		template<Detail::Bit::UnsignedIntegral T, Detail::Bit::UnsignedIntegral U>
		[[nodiscard]] static constexpr auto TryAlignUp(
			T Value,
			U AlignmentValue,
			T& Result
		) noexcept -> Bool
		{
			if constexpr (sizeof(U) > sizeof(T))
			{
				if (AlignmentValue > static_cast<U>(~T{ 0 }))
				{
					return false;
				}
			}

			const auto Alignment = static_cast<T>(AlignmentValue);

			if (!IsValidAlignment(Alignment))
			{
				return false;
			}

			const auto Adjustment = static_cast<T>(Alignment - T{ 1 });
			const auto Maximum = static_cast<T>(~T{ 0 });

			if (Value > static_cast<T>(Maximum - Adjustment))
			{
				return false;
			}

			Result = static_cast<T>((Value + Adjustment) & ~Adjustment);
			return true;
		}

		template<Detail::Bit::UnsignedIntegral T, Detail::Bit::UnsignedIntegral U>
		[[nodiscard]] static constexpr auto AlignDown(T Value, U AlignmentValue) noexcept -> T
		{
			T Result{};
			return TryAlignDown(Value, AlignmentValue, Result) ? Result : T{};
		}

		template<Detail::Bit::UnsignedIntegral T, Detail::Bit::UnsignedIntegral U>
		[[nodiscard]] static constexpr auto AlignUp(T Value, U AlignmentValue) noexcept -> T
		{
			T Result{};
			return TryAlignUp(Value, AlignmentValue, Result) ? Result : T{};
		}

		template<Detail::Bit::UnsignedIntegral T, Detail::Bit::UnsignedIntegral U>
		[[nodiscard]] static constexpr auto IsAligned(T Value, U AlignmentValue) noexcept -> Bool
		{
			if constexpr (sizeof(U) > sizeof(T))
			{
				if (AlignmentValue > static_cast<U>(~T{ 0 }))
				{
					return false;
				}
			}

			const auto Alignment = static_cast<T>(AlignmentValue);

			return IsValidAlignment(Alignment) &&
				(Value & (Alignment - T{ 1 })) == T{ 0 };
		}

		template<Detail::Bit::Value T>
		[[nodiscard]] static constexpr auto IsPowerOfTwo(T Value) noexcept -> Bool
		{
			const auto Raw = Detail::Bit::ToUnsigned(Value);
			return Raw != Detail::Bit::UnsignedType<T>{ 0 } &&
				(Raw & (Raw - Detail::Bit::UnsignedType<T>{ 1 })) == Detail::Bit::UnsignedType<T>{ 0 };
		}

		template<Detail::Bit::Value T>
		[[nodiscard]] static constexpr auto FloorPowerOfTwo(T Value) noexcept -> T
		{
			auto Raw = Detail::Bit::ToUnsigned(Value);

			if (Raw == Detail::Bit::UnsignedType<T>{ 0 })
			{
				return T{};
			}

			for (Size Shift = 1; Shift < Width<T>(); Shift <<= 1)
			{
				Raw = static_cast<Detail::Bit::UnsignedType<T>>(Raw | (Raw >> Shift));
			}

			return Detail::Bit::FromUnsigned<T>(
				static_cast<Detail::Bit::UnsignedType<T>>(Raw - (Raw >> 1))
			);
		}

		template<Detail::Bit::Value T>
		[[nodiscard]] static constexpr auto CeilPowerOfTwo(T Value) noexcept -> T
		{
			const auto Raw = Detail::Bit::ToUnsigned(Value);

			if (Raw <= Detail::Bit::UnsignedType<T>{ 1 })
			{
				return Detail::Bit::FromUnsigned<T>(Detail::Bit::UnsignedType<T>{ 1 });
			}

			const auto Floor = Detail::Bit::ToUnsigned(FloorPowerOfTwo(Value));

			if (Floor > (Detail::Bit::AllOnes<T>() >> 1))
			{
				return T{};
			}

			return Detail::Bit::FromUnsigned<T>(
				static_cast<Detail::Bit::UnsignedType<T>>(Floor << 1)
			);
		}

		template<Detail::Bit::Value T, Detail::Bit::Index U>
		[[nodiscard]] static constexpr auto RotateLeft(T Value, U CountValue) noexcept -> T
		{
			if (Detail::Bit::IsNegative(CountValue))
			{
				return Value;
			}

			const auto TypeWidth = Width<T>();
			const auto Count = static_cast<Size>(CountValue) % TypeWidth;

			if (Count == 0)
			{
				return Value;
			}

			const auto Raw = Detail::Bit::ToUnsigned(Value);

			return Detail::Bit::FromUnsigned<T>(
				static_cast<Detail::Bit::UnsignedType<T>>(
					(Raw << Count) |
					(Raw >> (TypeWidth - Count))
				)
			);
		}

		template<Detail::Bit::Value T, Detail::Bit::Index U>
		[[nodiscard]] static constexpr auto RotateRight(T Value, U CountValue) noexcept -> T
		{
			if (Detail::Bit::IsNegative(CountValue))
			{
				return Value;
			}

			const auto TypeWidth = Width<T>();
			const auto Count = static_cast<Size>(CountValue) % TypeWidth;

			if (Count == 0)
			{
				return Value;
			}

			const auto Raw = Detail::Bit::ToUnsigned(Value);

			return Detail::Bit::FromUnsigned<T>(
				static_cast<Detail::Bit::UnsignedType<T>>(
					(Raw >> Count) |
					(Raw << (TypeWidth - Count))
				)
			);
		}

		template<Detail::Bit::Value T, Detail::Bit::Index U>
		[[nodiscard]] static constexpr auto SignExtend(T Value, U WidthValue) noexcept -> T
		{
			if (Detail::Bit::IsNegative(WidthValue))
			{
				return T{};
			}

			const auto FieldWidth = static_cast<Size>(WidthValue);

			if (FieldWidth == 0)
			{
				return T{};
			}

			if (FieldWidth >= Width<T>())
			{
				return Value;
			}

			const auto FieldMask = Detail::Bit::LowMask<T>(FieldWidth);
			auto Raw = static_cast<Detail::Bit::UnsignedType<T>>(
				Detail::Bit::ToUnsigned(Value) & FieldMask
			);
			const auto SignBit = static_cast<Detail::Bit::UnsignedType<T>>(
				Detail::Bit::UnsignedType<T>{ 1 } << (FieldWidth - 1)
			);

			if ((Raw & SignBit) != 0)
			{
				Raw = static_cast<Detail::Bit::UnsignedType<T>>(Raw | ~FieldMask);
			}

			return Detail::Bit::FromUnsigned<T>(Raw);
		}

		template<Detail::Bit::Value T>
		[[nodiscard]] static constexpr auto CountOnes(T Value) noexcept -> Size
		{
			auto Raw = Detail::Bit::ToUnsigned(Value);
			Size Count = 0;

			while (Raw != Detail::Bit::UnsignedType<T>{ 0 })
			{
				Raw = static_cast<Detail::Bit::UnsignedType<T>>(Raw & (Raw - 1));
				++Count;
			}

			return Count;
		}

		template<Detail::Bit::Value T>
		[[nodiscard]] static constexpr auto CountTrailingZeros(T Value) noexcept -> Size
		{
			auto Raw = Detail::Bit::ToUnsigned(Value);

			if (Raw == Detail::Bit::UnsignedType<T>{ 0 })
			{
				return Width<T>();
			}

			Size Count = 0;

			while ((Raw & Detail::Bit::UnsignedType<T>{ 1 }) == 0)
			{
				Raw >>= 1;
				++Count;
			}

			return Count;
		}

		template<Detail::Bit::Value T>
		[[nodiscard]] static constexpr auto CountLeadingZeros(T Value) noexcept -> Size
		{
			const auto Raw = Detail::Bit::ToUnsigned(Value);

			if (Raw == Detail::Bit::UnsignedType<T>{ 0 })
			{
				return Width<T>();
			}

			Size Count = 0;
			auto TestMask = static_cast<Detail::Bit::UnsignedType<T>>(
				Detail::Bit::UnsignedType<T>{ 1 } << (Width<T>() - 1)
			);

			while ((Raw & TestMask) == 0)
			{
				TestMask >>= 1;
				++Count;
			}

			return Count;
		}

		template<Detail::Bit::Value T, Detail::Bit::Index U>
		[[nodiscard]] static constexpr auto Byte(T Value, U IndexValue) noexcept -> Foundation::Byte
		{
			if (Detail::Bit::IsNegative(IndexValue))
			{
				return Foundation::Byte{};
			}

			const auto Index = static_cast<Size>(IndexValue);

			if (Index >= sizeof(Detail::Bit::StorageType<T>))
			{
				return Foundation::Byte{};
			}

			return static_cast<Foundation::Byte>(
				(Detail::Bit::ToUnsigned(Value) >> (Index * 8)) &
				Detail::Bit::UnsignedType<T>{ 0xFF }
			);
		}

		template<Detail::Bit::Value T, Detail::Bit::Index U>
		[[nodiscard]] static constexpr auto FromByte(
			Foundation::Byte Value,
			U IndexValue
		) noexcept -> T
		{
			if (Detail::Bit::IsNegative(IndexValue))
			{
				return T{};
			}

			const auto Index = static_cast<Size>(IndexValue);

			if (Index >= sizeof(Detail::Bit::StorageType<T>))
			{
				return T{};
			}

			return Detail::Bit::FromUnsigned<T>(
				static_cast<Detail::Bit::UnsignedType<T>>(
					static_cast<Detail::Bit::UnsignedType<T>>(Value) << (Index * 8)
				)
			);
		}
	};
}
