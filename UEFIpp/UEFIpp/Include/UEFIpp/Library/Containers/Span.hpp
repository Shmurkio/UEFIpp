#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Library
{
	template<typename T>
	class Span
	{
	public:
		using ValueType = T;
		using Pointer = T*;
		using ConstPointer = const T*;
		using Reference = T&;
		using ConstReference = const T&;
		using SizeType = Foundation::Size;

		static constexpr SizeType NotFound = SizeType(-1);

	public:
		constexpr Span() = default;

		constexpr Span(Pointer Data, SizeType Size) : Data_(Data), Size_(Data ? Size : 0)
		{
		}

		template<SizeType N>
		constexpr Span(T(&Array)[N]) : Data_(Array), Size_(N)
		{
		}

		template<typename U> requires Foundation::Concepts::Same<T, const U>
		constexpr Span(const Span<U>& Other) : Data_(Other.Data()), Size_(Other.Size())
		{
		}

		[[nodiscard]] constexpr auto Data() const -> Pointer
		{
			return Data_;
		}

		[[nodiscard]] constexpr auto Size() const -> SizeType
		{
			return Size_;
		}

		[[nodiscard]] constexpr auto SizeInBytes() const -> SizeType
		{
			return Size_ * sizeof(T);
		}

		[[nodiscard]] constexpr auto Empty() const -> Foundation::Bool
		{
			return Size_ == 0;
		}

		[[nodiscard]] constexpr auto Begin() const -> Pointer
		{
			return Data_;
		}

		[[nodiscard]] constexpr auto End() const -> Pointer
		{
			return Data_ ? Data_ + Size_ : nullptr;
		}

		[[nodiscard]] constexpr auto begin() const -> Pointer
		{
			return Begin();
		}

		[[nodiscard]] constexpr auto end() const -> Pointer
		{
			return End();
		}

		[[nodiscard]] constexpr auto Front() const -> Reference
		{
			return Data_[0];
		}

		[[nodiscard]] constexpr auto Back() const -> Reference
		{
			return Data_[Size_ - 1];
		}

		[[nodiscard]] constexpr auto At(SizeType Index) const -> Reference
		{
			return Data_[Index];
		}

		[[nodiscard]] constexpr auto operator[](SizeType Index) const -> Reference
		{
			return Data_[Index];
		}

		constexpr auto RemovePrefix(SizeType Count) -> void
		{
			if (Count > Size_)
			{
				Count = Size_;
			}

			if (Data_)
			{
				Data_ += Count;
			}

			Size_ -= Count;
		}

		constexpr auto RemoveSuffix(SizeType Count) -> void
		{
			if (Count > Size_)
			{
				Count = Size_;
			}

			Size_ -= Count;
		}

		[[nodiscard]] constexpr auto Subspan(SizeType Position, SizeType Count = NotFound) const -> Span
		{
			if (!Data_ || Position >= Size_)
			{
				return {};
			}

			const auto Available = Size_ - Position;

			if (Count == NotFound || Count > Available)
			{
				Count = Available;
			}

			return Span(Data_ + Position, Count);
		}

		[[nodiscard]] constexpr auto First(SizeType Count) const -> Span
		{
			return Subspan(0, Count);
		}

		[[nodiscard]] constexpr auto Last(SizeType Count) const -> Span
		{
			if (Count > Size_)
			{
				Count = Size_;
			}

			return Subspan(Size_ - Count, Count);
		}

		[[nodiscard]] constexpr auto Contains(ConstReference Value) const -> Foundation::Bool
		{
			return Find(Value) != NotFound;
		}

		[[nodiscard]] constexpr auto Find(ConstReference Value, SizeType Position = 0) const -> SizeType
		{
			if (!Data_ || Position >= Size_)
			{
				return NotFound;
			}

			for (SizeType i = Position; i < Size_; ++i)
			{
				if (Data_[i] == Value)
				{
					return i;
				}
			}

			return NotFound;
		}

		[[nodiscard]] constexpr auto IsNull() const -> Foundation::Bool
		{
			return Data_ == nullptr;
		}

		[[nodiscard]] constexpr auto operator==(const Span& Other) const -> Foundation::Bool
		{
			if (Size_ != Other.Size_)
			{
				return false;
			}

			for (SizeType i = 0; i < Size_; ++i)
			{
				if (!(Data_[i] == Other.Data_[i]))
				{
					return false;
				}
			}

			return true;
		}

		[[nodiscard]] constexpr auto operator!=(const Span& Other) const -> Foundation::Bool
		{
			return !(*this == Other);
		}

	private:
		Pointer Data_{};
		SizeType Size_{};
	};
}