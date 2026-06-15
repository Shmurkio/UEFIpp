#pragma once

#include <UEFIpp/Library/String/StringView.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>

namespace UEFIpp::Library
{
	template<typename TChar>
	class BasicString
	{
	public:
		using ValueType = TChar;
		using Pointer = TChar*;
		using ConstPointer = const TChar*;
		using Reference = TChar&;
		using ConstReference = const TChar&;
		using SizeType = Foundation::Size;
		using ViewType = BasicStringView<TChar>;
		using StorageType = Vector<TChar>;

		static constexpr SizeType NotFound = SizeType(-1);

	public:
		constexpr BasicString() = default;

		BasicString(const TChar* Text)
		{
			Assign(ViewType(Text));
		}

		BasicString(ViewType View)
		{
			Assign(View);
		}

		BasicString(const BasicString& Other)
		{
			Assign(Other.View());
		}

		BasicString(BasicString&& Other) noexcept : Storage_(static_cast<StorageType&&>(Other.Storage_)), Size_(Other.Size_)
		{
			Other.Size_ = 0;
		}

		~BasicString() = default;

		auto operator=(const BasicString& Other) -> BasicString&
		{
			if (this == &Other)
			{
				return *this;
			}

			Assign(Other.View());

			return *this;
		}


		auto operator=(BasicString&& Other) noexcept -> BasicString&
		{
			if (this == &Other)
			{
				return *this;
			}

			Storage_ = static_cast<StorageType&&>(Other.Storage_);
			Size_ = Other.Size_;

			Other.Size_ = 0;

			return *this;
		}

		auto operator=(const TChar* Text) -> BasicString&
		{
			Assign(ViewType(Text));
			return *this;
		}

		auto operator=(ViewType View) -> BasicString&
		{
			Assign(View);
			return *this;
		}

		[[nodiscard]] auto Data() -> Pointer
		{
			return Storage_.Data();
		}

		[[nodiscard]] auto Data() const -> ConstPointer
		{
			return Storage_.Data() ? Storage_.Data() : EmptyData();
		}

		[[nodiscard]] auto CStr() const -> ConstPointer
		{
			return Data();
		}

		[[nodiscard]] auto Size() const -> SizeType
		{
			return Size_;
		}

		[[nodiscard]] auto Length() const -> SizeType
		{
			return Size_;
		}

		[[nodiscard]] auto Capacity() const -> SizeType
		{
			return Storage_.Capacity();
		}

		[[nodiscard]] auto Empty() const -> Foundation::Bool
		{
			return Size_ == 0;
		}

		[[nodiscard]] auto View() const -> ViewType
		{
			return ViewType(Data(), Size_);
		}

		[[nodiscard]] operator ViewType() const
		{
			return View();
		}

		[[nodiscard]] auto Begin() -> Pointer
		{
			return Storage_.Data();
		}

		[[nodiscard]] auto Begin() const -> ConstPointer
		{
			return Data();
		}

		[[nodiscard]] auto End() -> Pointer
		{
			return Storage_.Data() ? Storage_.Data() + Size_ : nullptr;
		}

		[[nodiscard]] auto End() const -> ConstPointer
		{
			return Data() + Size_;
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

		[[nodiscard]] auto Front() const -> TChar
		{
			return Size_ ? Storage_[0] : TChar{};
		}

		[[nodiscard]] auto Back() const -> TChar
		{
			return Size_ ? Storage_[Size_ - 1] : TChar{};
		}

		[[nodiscard]] auto At(SizeType Index) -> Pointer
		{
			if (!Storage_.Data() || Index >= Size_)
			{
				return nullptr;
			}

			return &Storage_[Index];
		}

		[[nodiscard]] auto At(SizeType Index) const -> ConstPointer
		{
			if (!Storage_.Data() || Index >= Size_)
			{
				return nullptr;
			}

			return &Storage_[Index];
		}

		[[nodiscard]] auto operator[](SizeType Index) -> Reference
		{
			return Storage_[Index];
		}

		[[nodiscard]] auto operator[](SizeType Index) const -> TChar
		{
			if (!Storage_.Data() || Index >= Size_)
			{
				return TChar{};
			}

			return Storage_[Index];
		}

		[[nodiscard]] auto Reserve(SizeType NewCapacity) -> Foundation::Bool
		{
			if (!Storage_.Reserve(NewCapacity))
			{
				return false;
			}

			return EnsureNullTerminated();
		}

		[[nodiscard]] auto Resize(SizeType NewSize, TChar Character = TChar{}) -> Foundation::Bool
		{
			if (!Storage_.Resize(NewSize + 1))
			{
				return false;
			}

			if (NewSize > Size_)
			{
				for (SizeType i = Size_; i < NewSize; ++i)
				{
					Storage_[i] = Character;
				}
			}

			Size_ = NewSize;
			Storage_[Size_] = TChar{};

			return true;
		}

		auto Clear() -> void
		{
			if (Storage_.Data())
			{
				Storage_.Resize(1);
				Storage_[0] = TChar{};
			}

			Size_ = 0;
		}

		[[nodiscard]] auto Assign(ViewType View) -> Foundation::Bool
		{
			if (!Storage_.Resize(View.Size() + 1))
			{
				return false;
			}

			for (SizeType i = 0; i < View.Size(); ++i)
			{
				Storage_[i] = View[i];
			}

			Size_ = View.Size();
			Storage_[Size_] = TChar{};

			return true;
		}

		[[nodiscard]] auto Append(ViewType View) -> Foundation::Bool
		{
			if (View.Empty())
			{
				return true;
			}

			const auto OldSize = Size_;
			const auto NewSize = Size_ + View.Size();

			if (!Storage_.Resize(NewSize + 1))
			{
				return false;
			}

			for (SizeType i = 0; i < View.Size(); ++i)
			{
				Storage_[OldSize + i] = View[i];
			}

			Size_ = NewSize;
			Storage_[Size_] = TChar{};

			return true;
		}

		[[nodiscard]] auto Append(TChar Character) -> Foundation::Bool
		{
			const auto OldSize = Size_;

			if (!Storage_.Resize(Size_ + 2))
			{
				return false;
			}

			Storage_[OldSize] = Character;
			Size_ = OldSize + 1;
			Storage_[Size_] = TChar{};

			return true;
		}

		[[nodiscard]] auto PushBack(TChar Character) -> Foundation::Bool
		{
			return Append(Character);
		}

		auto PopBack() -> void
		{
			if (!Size_)
			{
				return;
			}

			--Size_;
			Storage_.Resize(Size_ + 1);
			Storage_[Size_] = TChar{};
		}

		[[nodiscard]] auto Substr(SizeType Position, SizeType Count = NotFound) const -> BasicString
		{
			return BasicString(View().Substr(Position, Count));
		}

		[[nodiscard]] auto StartsWith(ViewType Prefix) const -> Foundation::Bool
		{
			return View().StartsWith(Prefix);
		}

		[[nodiscard]] auto EndsWith(ViewType Suffix) const -> Foundation::Bool
		{
			return View().EndsWith(Suffix);
		}

		[[nodiscard]] auto Find(TChar Character, SizeType Position = 0) const -> SizeType
		{
			return View().Find(Character, Position);
		}

		[[nodiscard]] auto Find(ViewType Text, SizeType Position = 0) const -> SizeType
		{
			return View().Find(Text, Position);
		}

		[[nodiscard]] auto FindLast(TChar Character, SizeType Position = NotFound) const -> SizeType
		{
			return View().FindLast(Character, Position);
		}

		[[nodiscard]] auto FindLast(ViewType Text, SizeType Position = NotFound) const -> SizeType
		{
			return View().FindLast(Text, Position);
		}

		[[nodiscard]] auto Contains(TChar Character) const -> Foundation::Bool
		{
			return View().Contains(Character);
		}

		[[nodiscard]] auto Contains(ViewType Text) const -> Foundation::Bool
		{
			return View().Contains(Text);
		}

		[[nodiscard]] auto Compare(ViewType Other) const -> Foundation::Int32
		{
			return View().Compare(Other);
		}

		[[nodiscard]] auto operator==(ViewType Other) const -> Foundation::Bool
		{
			return View() == Other;
		}

		[[nodiscard]] auto operator==(const BasicString& Other) const -> Foundation::Bool
		{
			return View() == Other.View();
		}

		[[nodiscard]] auto operator<=>(ViewType Other) const -> Foundation::Int32
		{
			return Compare(Other);
		}

		[[nodiscard]] auto operator<=>(const BasicString& Other) const -> Foundation::Int32
		{
			return Compare(Other.View());
		}

		[[nodiscard]] auto operator<=>(const TChar* Text) const -> Foundation::Int32
		{
			return Compare(ViewType(Text));
		}

		auto operator+=(ViewType View) -> BasicString&
		{
			Append(View);
			return *this;
		}

		auto operator+=(const TChar* Text) -> BasicString&
		{
			Append(ViewType(Text));
			return *this;
		}

		auto operator+=(TChar Character) -> BasicString&
		{
			Append(Character);
			return *this;
		}

		[[nodiscard]] auto operator==(const TChar* Text) const -> Foundation::Bool
		{
			return View() == ViewType(Text);
		}

	private:
		[[nodiscard]] static constexpr auto EmptyData() -> ConstPointer
		{
			static constexpr TChar Empty[1]{};
			return Empty;
		}

		[[nodiscard]] auto EnsureNullTerminated() -> Foundation::Bool
		{
			if (!Storage_.Data())
			{
				return true;
			}

			if (Storage_.Size() < Size_ + 1)
			{
				if (!Storage_.Resize(Size_ + 1))
				{
					return false;
				}
			}

			Storage_[Size_] = TChar{};
			return true;
		}

	private:
		StorageType Storage_{};
		SizeType Size_{};
	};

	using String = BasicString<char>;
	using WideString = BasicString<wchar_t>;
	using U8String = BasicString<char8_t>;
	using U16String = BasicString<char16_t>;
	using U32String = BasicString<char32_t>;
}