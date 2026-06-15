#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>

namespace UEFIpp::Library
{
	template<typename T>
	class Vector
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
		constexpr Vector() = default;

		explicit Vector(SizeType Count)
		{
			Resize(Count);
		}

		Vector(SizeType Count, ConstReference Value)
		{
			Resize(Count, Value);
		}

		Vector(ConstViewType Values)
		{
			Assign(Values);
		}

		Vector(const Vector& Other)
		{
			Assign(Other.View());
		}

		Vector(Vector&& Other) noexcept : Data_(Other.Data_), Size_(Other.Size_), Capacity_(Other.Capacity_)
		{
			Other.Data_ = nullptr;
			Other.Size_ = 0;
			Other.Capacity_ = 0;
		}

		~Vector()
		{
			Release();
		}

		auto operator=(const Vector& Other) -> Vector&
		{
			if (this != &Other)
			{
				Assign(Other.View());
			}

			return *this;
		}

		auto operator=(Vector&& Other) noexcept -> Vector&
		{
			if (this != &Other)
			{
				Release();

				Data_ = Other.Data_;
				Size_ = Other.Size_;
				Capacity_ = Other.Capacity_;

				Other.Data_ = nullptr;
				Other.Size_ = 0;
				Other.Capacity_ = 0;
			}

			return *this;
		}

		auto operator=(ConstViewType Values) -> Vector&
		{
			Assign(Values);
			return *this;
		}

		[[nodiscard]] auto Data() -> Pointer
		{
			return Data_;
		}

		[[nodiscard]] auto Data() const -> ConstPointer
		{
			return Data_;
		}

		[[nodiscard]] auto Size() const -> SizeType
		{
			return Size_;
		}

		[[nodiscard]] auto Capacity() const -> SizeType
		{
			return Capacity_;
		}

		[[nodiscard]] auto SizeInBytes() const -> SizeType
		{
			return Size_ * sizeof(T);
		}

		[[nodiscard]] auto CapacityInBytes() const -> SizeType
		{
			return Capacity_ * sizeof(T);
		}

		[[nodiscard]] auto Empty() const -> Foundation::Bool
		{
			return Size_ == 0;
		}

		[[nodiscard]] auto View() -> ViewType
		{
			return ViewType(Data_, Size_);
		}

		[[nodiscard]] auto View() const -> ConstViewType
		{
			return ConstViewType(Data_, Size_);
		}

		[[nodiscard]] operator ViewType()
		{
			return View();
		}

		[[nodiscard]] operator ConstViewType() const
		{
			return View();
		}

		[[nodiscard]] auto Begin() -> Pointer
		{
			return Data_;
		}

		[[nodiscard]] auto Begin() const -> ConstPointer
		{
			return Data_;
		}

		[[nodiscard]] auto End() -> Pointer
		{
			return Data_ ? Data_ + Size_ : nullptr;
		}

		[[nodiscard]] auto End() const -> ConstPointer
		{
			return Data_ ? Data_ + Size_ : nullptr;
		}

		[[nodiscard]] auto begin() -> Pointer
		{
			return Begin();
		}

		[[nodiscard]] auto end() -> Pointer
		{
			return End();
		}

		[[nodiscard]] auto begin() const -> ConstPointer
		{
			return Begin();
		}

		[[nodiscard]] auto end() const -> ConstPointer
		{
			return End();
		}

		[[nodiscard]] auto Front() -> Reference
		{
			return Data_[0];
		}

		[[nodiscard]] auto Front() const -> ConstReference
		{
			return Data_[0];
		}

		[[nodiscard]] auto Back() -> Reference
		{
			return Data_[Size_ - 1];
		}

		[[nodiscard]] auto Back() const -> ConstReference
		{
			return Data_[Size_ - 1];
		}

		[[nodiscard]] auto At(SizeType Index) -> Pointer
		{
			if (Index >= Size_)
			{
				return nullptr;
			}

			return &Data_[Index];
		}

		[[nodiscard]] auto At(SizeType Index) const -> ConstPointer
		{
			if (Index >= Size_)
			{
				return nullptr;
			}

			return &Data_[Index];
		}

		[[nodiscard]] auto operator[](SizeType Index) -> Reference
		{
			return Data_[Index];
		}

		[[nodiscard]] auto operator[](SizeType Index) const -> ConstReference
		{
			return Data_[Index];
		}

		auto Clear() -> void
		{
			DestroyRange(0, Size_);
			Size_ = 0;
		}

		auto Release() -> void
		{
			Clear();

			if (Data_)
			{
				Free(Data_);
			}

			Data_ = nullptr;
			Capacity_ = 0;
		}

		auto Reserve(SizeType NewCapacity) -> Foundation::Bool
		{
			if (NewCapacity <= Capacity_)
			{
				return true;
			}

			return Reallocate(NewCapacity);
		}

		auto Resize(SizeType NewSize) -> Foundation::Bool
		{
			if (NewSize < Size_)
			{
				DestroyRange(NewSize, Size_);
				Size_ = NewSize;
				return true;
			}

			if (!Reserve(NewSize))
			{
				return false;
			}

			while (Size_ < NewSize)
			{
				Construct(Data_ + Size_);
				++Size_;
			}

			return true;
		}

		auto Resize(SizeType NewSize, ConstReference Value) -> Foundation::Bool
		{
			if (NewSize < Size_)
			{
				DestroyRange(NewSize, Size_);
				Size_ = NewSize;
				return true;
			}

			if (!Reserve(NewSize))
			{
				return false;
			}

			while (Size_ < NewSize)
			{
				Construct(Data_ + Size_, Value);
				++Size_;
			}

			return true;
		}

		auto PushBack(ConstReference Value) -> Foundation::Bool
		{
			if (!EnsureCapacityForOneMore())
			{
				return false;
			}

			Construct(Data_ + Size_, Value);
			++Size_;

			return true;
		}

		auto PushBack(T&& Value) -> Foundation::Bool
		{
			if (!EnsureCapacityForOneMore())
			{
				return false;
			}

			Construct(Data_ + Size_, static_cast<T&&>(Value));
			++Size_;

			return true;
		}

		template<typename... TArgs>
		auto EmplaceBack(TArgs&&... Args) -> Pointer
		{
			if (!EnsureCapacityForOneMore())
			{
				return nullptr;
			}

			auto Result = Data_ + Size_;
			Construct(Result, static_cast<TArgs&&>(Args)...);
			++Size_;

			return Result;
		}

		auto PopBack() -> Foundation::Bool
		{
			if (Size_ == 0)
			{
				return false;
			}

			--Size_;
			Destroy(Data_ + Size_);

			return true;
		}

		auto Assign(ConstViewType Values) -> Foundation::Bool
		{
			Clear();

			if (!Reserve(Values.Size()))
			{
				return false;
			}

			for (SizeType i = 0; i < Values.Size(); ++i)
			{
				Construct(Data_ + i, Values[i]);
			}

			Size_ = Values.Size();
			return true;
		}

		auto Append(ConstViewType Values) -> Foundation::Bool
		{
			if (!Reserve(Size_ + Values.Size()))
			{
				return false;
			}

			for (SizeType i = 0; i < Values.Size(); ++i)
			{
				Construct(Data_ + Size_, Values[i]);
				++Size_;
			}

			return true;
		}

		auto Insert(SizeType Index, ConstReference Value) -> Foundation::Bool
		{
			if (Index > Size_)
			{
				return false;
			}

			if (!EnsureCapacityForOneMore())
			{
				return false;
			}

			for (SizeType i = Size_; i > Index; --i)
			{
				Construct(Data_ + i, static_cast<T&&>(Data_[i - 1]));
				Destroy(Data_ + i - 1);
			}

			Construct(Data_ + Index, Value);
			++Size_;

			return true;
		}

		auto Erase(SizeType Index) -> Foundation::Bool
		{
			if (Index >= Size_)
			{
				return false;
			}

			Destroy(Data_ + Index);

			for (SizeType i = Index; i + 1 < Size_; ++i)
			{
				Construct(Data_ + i, static_cast<T&&>(Data_[i + 1]));
				Destroy(Data_ + i + 1);
			}

			--Size_;

			return true;
		}

		auto Erase(SizeType Index, SizeType Count) -> Foundation::Bool
		{
			if (Index >= Size_)
			{
				return false;
			}

			if (Index + Count > Size_)
			{
				Count = Size_ - Index;
			}

			for (SizeType i = 0; i < Count; ++i)
			{
				Destroy(Data_ + Index + i);
			}

			for (SizeType i = Index; i + Count < Size_; ++i)
			{
				Construct(Data_ + i, static_cast<T&&>(Data_[i + Count]));
				Destroy(Data_ + i + Count);
			}

			Size_ -= Count;
			return true;
		}

		auto Swap(Vector& Other) -> void
		{
			auto TempData = Data_;
			auto TempSize = Size_;
			auto TempCapacity = Capacity_;

			Data_ = Other.Data_;
			Size_ = Other.Size_;
			Capacity_ = Other.Capacity_;

			Other.Data_ = TempData;
			Other.Size_ = TempSize;
			Other.Capacity_ = TempCapacity;
		}

		[[nodiscard]] auto Contains(ConstReference Value) const -> Foundation::Bool
		{
			return Find(Value) != NotFound;
		}

		[[nodiscard]] auto Find(ConstReference Value, SizeType Position = 0) const -> SizeType
		{
			return View().Find(Value, Position);
		}

		auto ShrinkToFit() -> Foundation::Bool
		{
			if (Size_ == Capacity_)
			{
				return true;
			}

			if (Size_ == 0)
			{
				Release();
				return true;
			}

			return Reallocate(Size_);
		}

		template<typename TPredicate>
		auto RemoveIf(TPredicate Predicate) -> SizeType
		{
			SizeType Write = 0;

			for (SizeType Read = 0; Read < Size_; ++Read)
			{
				if (!Predicate(Data_[Read]))
				{
					if (Write != Read)
					{
						Data_[Write] = static_cast<T&&>(Data_[Read]);
					}

					++Write;
				}
			}

			auto Removed = Size_ - Write;

			while (Size_ > Write)
			{
				PopBack();
			}

			return Removed;
		}

		auto Remove(ConstReference Value) -> SizeType
		{
			return RemoveIf([&](ConstReference Current)
				{
					return Current == Value;
				});
		}

		auto Reverse() -> void
		{
			for (SizeType i = 0; i < Size_ / 2; ++i)
			{
				auto Temp = static_cast<T&&>(Data_[i]);
				Data_[i] = static_cast<T&&>(Data_[Size_ - 1 - i]);
				Data_[Size_ - 1 - i] = static_cast<T&&>(Temp);
			}
		}

		template<typename TCompare>
		auto Sort(TCompare Compare) -> void
		{
			for (SizeType i = 1; i < Size_; ++i)
			{
				auto Key = static_cast<T&&>(Data_[i]);
				auto j = i;

				while (j > 0 && Compare(Key, Data_[j - 1]))
				{
					Data_[j] = static_cast<T&&>(Data_[j - 1]);
					--j;
				}

				Data_[j] = static_cast<T&&>(Key);
			}
		}

		auto Sort() -> void
		{
			Sort([](ConstReference A, ConstReference B)
				{
					return A < B;
				});
		}

		auto LowerBound(ConstReference Value) const -> SizeType
		{
			SizeType First = 0;
			SizeType Count = Size_;

			while (Count > 0)
			{
				auto Step = Count / 2;
				auto It = First + Step;

				if (Data_[It] < Value)
				{
					First = It + 1;
					Count -= Step + 1;
				}
				else
				{
					Count = Step;
				}
			}

			return First;
		}

		auto BinarySearch(ConstReference Value) const -> Foundation::Bool
		{
			auto Index = LowerBound(Value);
			return Index < Size_ && !(Value < Data_[Index]) && !(Data_[Index] < Value);
		}

		[[nodiscard]] auto operator==(const Vector& Other) const -> Foundation::Bool
		{
			return View() == Other.View();
		}

		[[nodiscard]] auto operator!=(const Vector& Other) const -> Foundation::Bool
		{
			return !(*this == Other);
		}

	private:
		[[nodiscard]] static auto Allocate(SizeType Count) -> Pointer
		{
			if (Count == 0)
			{
				return nullptr;
			}

			return static_cast<Pointer>(::operator new(Count * sizeof(T)));
		}

		static auto Free(Pointer Data) -> void
		{
			::operator delete(Data);
		}

		template<typename... TArgs>
		static auto Construct(Pointer Address, TArgs&&... Args) -> void
		{
			::new (static_cast<void*>(Address)) T(static_cast<TArgs&&>(Args)...);
		}

		static auto Destroy(Pointer Address) -> void
		{
			Address->~T();
		}

		auto DestroyRange(SizeType First, SizeType Last) -> void
		{
			for (SizeType i = First; i < Last; ++i)
			{
				Destroy(Data_ + i);
			}
		}

		[[nodiscard]] auto EnsureCapacityForOneMore() -> Foundation::Bool
		{
			if (Size_ < Capacity_)
			{
				return true;
			}

			auto NewCapacity = Capacity_ ? Capacity_ * 2 : 8;
			return Reserve(NewCapacity);
		}

		[[nodiscard]] auto Reallocate(SizeType NewCapacity) -> Foundation::Bool
		{
			auto NewData = Allocate(NewCapacity);

			if (!NewData)
			{
				return false;
			}

			for (SizeType i = 0; i < Size_; ++i)
			{
				Construct(NewData + i, static_cast<T&&>(Data_[i]));
				Destroy(Data_ + i);
			}

			if (Data_)
			{
				Free(Data_);
			}

			Data_ = NewData;
			Capacity_ = NewCapacity;

			return true;
		}

	private:
		Pointer Data_{};
		SizeType Size_{};
		SizeType Capacity_{};
	};
}