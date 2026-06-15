#pragma once

#include <UEFIpp/Library/Containers/Span.hpp>

namespace UEFIpp::Library
{
	template<typename T, Foundation::Size N>
	class Array
	{
	public:
		using ValueType = T;
		using Pointer = T*;
		using ConstPointer = const T*;
		using Reference = T&;
		using ConstReference = const T&;
		using SizeType = Foundation::Size;
		using ViewType = Span<T>;
		using ConstViewType = Span<const T>;

		static constexpr SizeType NotFound = SizeType(-1);

	public:
		constexpr Array() = default;

		[[nodiscard]] constexpr auto Data() -> Pointer
		{
			return Data_;
		}

		[[nodiscard]] constexpr auto Data() const -> ConstPointer
		{
			return Data_;
		}

		[[nodiscard]] constexpr auto Size() const -> SizeType
		{
			return N;
		}

		[[nodiscard]] constexpr auto SizeInBytes() const -> SizeType
		{
			return N * sizeof(T);
		}

		[[nodiscard]] constexpr auto Empty() const -> Foundation::Bool
		{
			return N == 0;
		}

		[[nodiscard]] constexpr auto View() -> ViewType
		{
			return ViewType(Data_, N);
		}

		[[nodiscard]] constexpr auto View() const -> ConstViewType
		{
			return ConstViewType(Data_, N);
		}

		[[nodiscard]] constexpr operator ViewType()
		{
			return View();
		}

		[[nodiscard]] constexpr operator ConstViewType() const
		{
			return View();
		}

		[[nodiscard]] constexpr auto Begin() -> Pointer
		{
			return Data_;
		}

		[[nodiscard]] constexpr auto Begin() const -> ConstPointer
		{
			return Data_;
		}

		[[nodiscard]] constexpr auto End() -> Pointer
		{
			return Data_ + N;
		}

		[[nodiscard]] constexpr auto End() const -> ConstPointer
		{
			return Data_ + N;
		}

		[[nodiscard]] constexpr auto begin() -> Pointer
		{
			return Begin();
		}

		[[nodiscard]] constexpr auto end() -> Pointer
		{
			return End();
		}

		[[nodiscard]] constexpr auto begin() const -> ConstPointer
		{
			return Begin();
		}

		[[nodiscard]] constexpr auto end() const -> ConstPointer
		{
			return End();
		}

		[[nodiscard]] constexpr auto Front() -> Reference
		{
			return Data_[0];
		}

		[[nodiscard]] constexpr auto Front() const -> ConstReference
		{
			return Data_[0];
		}

		[[nodiscard]] constexpr auto Back() -> Reference
		{
			return Data_[N - 1];
		}

		[[nodiscard]] constexpr auto Back() const -> ConstReference
		{
			return Data_[N - 1];
		}

		[[nodiscard]] constexpr auto At(SizeType Index) -> Pointer
		{
			if (Index >= N)
			{
				return nullptr;
			}

			return &Data_[Index];
		}

		[[nodiscard]] constexpr auto At(SizeType Index) const -> ConstPointer
		{
			if (Index >= N)
			{
				return nullptr;
			}

			return &Data_[Index];
		}

		[[nodiscard]] constexpr auto operator[](SizeType Index) -> Reference
		{
			return Data_[Index];
		}

		[[nodiscard]] constexpr auto operator[](SizeType Index) const -> ConstReference
		{
			return Data_[Index];
		}

		constexpr auto Fill(ConstReference Value) -> void
		{
			for (SizeType i = 0; i < N; ++i)
			{
				Data_[i] = Value;
			}
		}

		[[nodiscard]] constexpr auto Contains(ConstReference Value) const -> Foundation::Bool
		{
			return View().Contains(Value);
		}

		[[nodiscard]] constexpr auto Find(ConstReference Value, SizeType Position = 0) const -> SizeType
		{
			return View().Find(Value, Position);
		}

		[[nodiscard]] constexpr auto operator==(const Array& Other) const -> Foundation::Bool
		{
			return View() == Other.View();
		}

		[[nodiscard]] constexpr auto operator!=(const Array& Other) const -> Foundation::Bool
		{
			return !(*this == Other);
		}

	private:
		T Data_[N]{};
	};
}