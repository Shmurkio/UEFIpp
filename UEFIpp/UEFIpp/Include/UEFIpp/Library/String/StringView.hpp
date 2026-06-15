#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/CRT/String.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>

namespace UEFIpp::Library
{
	template<typename TChar>
	class BasicStringView
	{
	public:
		using ValueType = TChar;
		using Pointer = const TChar*;
		using ConstPointer = const TChar*;
		using Reference = const TChar&;
		using ConstReference = const TChar&;
		using SizeType = Foundation::Size;

		static constexpr SizeType NotFound = SizeType(-1);

	public:
		constexpr BasicStringView() = default;

		constexpr BasicStringView(const TChar* Data, SizeType Size) : Data_(Data), Size_(Data ? Size : 0)
		{
		}

		constexpr BasicStringView(const TChar* Text) : Data_(Text), Size_(Length(Text))
		{
		}

		[[nodiscard]] constexpr auto Data() const -> ConstPointer
		{
			return Data_;
		}

		[[nodiscard]] constexpr auto Size() const -> SizeType
		{
			return Size_;
		}

		[[nodiscard]] static constexpr auto Length(const TChar* Text) -> SizeType
		{
			if (!Text)
			{
				return 0;
			}

			SizeType Result = 0;

			while (Text[Result] != TChar{})
			{
				++Result;
			}

			return Result;
		}

		[[nodiscard]] constexpr auto Empty() const -> Foundation::Bool
		{
			return Size_ == 0;
		}

		[[nodiscard]] constexpr auto Begin() const -> ConstPointer
		{
			return Data_;
		}

		[[nodiscard]] constexpr auto End() const -> ConstPointer
		{
			return Data_ ? Data_ + Size_ : nullptr;
		}

		[[nodiscard]] constexpr auto begin() const -> ConstPointer
		{
			return Begin();
		}

		[[nodiscard]] constexpr auto end() const -> ConstPointer
		{
			return End();
		}

		[[nodiscard]] constexpr auto Front() const -> TChar
		{
			return Size_ ? Data_[0] : TChar{};
		}

		[[nodiscard]] constexpr auto Back() const -> TChar
		{
			return Size_ ? Data_[Size_ - 1] : TChar{};
		}

		[[nodiscard]] constexpr auto At(SizeType Index) const -> TChar
		{
			if (!Data_ || Index >= Size_)
			{
				return TChar{};
			}

			return Data_[Index];
		}

		[[nodiscard]] constexpr auto operator[](SizeType Index) const -> TChar
		{
			if (!Data_ || Index >= Size_)
			{
				return TChar{};
			}

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

		[[nodiscard]] constexpr auto Substr(SizeType Position, SizeType Count = NotFound) const -> BasicStringView
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

			return BasicStringView(Data_ + Position, Count);
		}

		[[nodiscard]] constexpr auto StartsWith(BasicStringView Prefix) const -> Foundation::Bool
		{
			if (Prefix.Size_ > Size_)
			{
				return false;
			}

			for (SizeType i = 0; i < Prefix.Size_; ++i)
			{
				if (Data_[i] != Prefix.Data_[i])
				{
					return false;
				}
			}

			return true;
		}

		[[nodiscard]] constexpr auto StartsWith(TChar Character) const -> Foundation::Bool
		{
			return Size_ && Data_[0] == Character;
		}

		[[nodiscard]] constexpr auto EndsWith(BasicStringView Suffix) const -> Foundation::Bool
		{
			if (Suffix.Size_ > Size_)
			{
				return false;
			}

			const auto Offset = Size_ - Suffix.Size_;

			for (SizeType i = 0; i < Suffix.Size_; ++i)
			{
				if (Data_[Offset + i] != Suffix.Data_[i])
				{
					return false;
				}
			}

			return true;
		}

		[[nodiscard]] constexpr auto EndsWith(TChar Character) const -> Foundation::Bool
		{
			return Size_ && Data_[Size_ - 1] == Character;
		}

		[[nodiscard]] constexpr auto Find(TChar Character, SizeType Position = 0) const -> SizeType
		{
			if (!Data_ || Position >= Size_)
			{
				return NotFound;
			}

			for (SizeType i = Position; i < Size_; ++i)
			{
				if (Data_[i] == Character)
				{
					return i;
				}
			}

			return NotFound;
		}

		[[nodiscard]] constexpr auto Find(BasicStringView Text, SizeType Position = 0) const -> SizeType
		{
			if (!Data_ || !Text.Data_ || Position > Size_)
			{
				return NotFound;
			}

			if (Text.Empty())
			{
				return Position;
			}

			if (Text.Size_ > Size_ || Position > Size_ - Text.Size_)
			{
				return NotFound;
			}

			for (SizeType i = Position; i <= Size_ - Text.Size_; ++i)
			{
				Foundation::Bool Match = true;

				for (SizeType j = 0; j < Text.Size_; ++j)
				{
					if (Data_[i + j] != Text.Data_[j])
					{
						Match = false;
						break;
					}
				}

				if (Match)
				{
					return i;
				}
			}

			return NotFound;
		}

		[[nodiscard]] constexpr auto Contains(TChar Character) const -> Foundation::Bool
		{
			return Find(Character) != NotFound;
		}

		[[nodiscard]] constexpr auto Contains(BasicStringView Text) const -> Foundation::Bool
		{
			return Find(Text) != NotFound;
		}

		[[nodiscard]] constexpr auto Compare(BasicStringView Other) const -> Foundation::Int32
		{
			const auto Count = Size_ < Other.Size_ ? Size_ : Other.Size_;

			for (SizeType i = 0; i < Count; ++i)
			{
				if (Data_[i] < Other.Data_[i])
				{
					return -1;
				}

				if (Data_[i] > Other.Data_[i])
				{
					return 1;
				}
			}

			if (Size_ < Other.Size_)
			{
				return -1;
			}

			if (Size_ > Other.Size_)
			{
				return 1;
			}

			return 0;
		}

		[[nodiscard]] constexpr auto AsSpan() const -> Span<const TChar>
		{
			return Span<const TChar>(Data_, Size_);
		}

		[[nodiscard]] constexpr auto operator==(BasicStringView Other) const -> Foundation::Bool
		{
			return Compare(Other) == 0;
		}

		[[nodiscard]] constexpr auto operator!=(BasicStringView Other) const -> Foundation::Bool
		{
			return !(*this == Other);
		}

		[[nodiscard]] constexpr auto operator<(BasicStringView Other) const -> Foundation::Bool
		{
			return Compare(Other) < 0;
		}

		[[nodiscard]] constexpr auto operator<=(BasicStringView Other) const -> Foundation::Bool
		{
			return Compare(Other) <= 0;
		}

		[[nodiscard]] constexpr auto operator>(BasicStringView Other) const -> Foundation::Bool
		{
			return Compare(Other) > 0;
		}

		[[nodiscard]] constexpr auto operator>=(BasicStringView Other) const -> Foundation::Bool
		{
			return Compare(Other) >= 0;
		}

		[[nodiscard]] constexpr auto IsNull() const -> Foundation::Bool
		{
			return Data_ == nullptr;
		}

		[[nodiscard]] constexpr auto FindLast(TChar Character, SizeType Position = NotFound) const -> SizeType
		{
			if (Empty())
			{
				return NotFound;
			}

			auto i = Position == NotFound || Position >= Size_ ? Size_ : Position + 1;

			while (i > 0)
			{
				--i;

				if (Data_[i] == Character)
				{
					return i;
				}
			}

			return NotFound;
		}

		[[nodiscard]] constexpr auto FindLast(BasicStringView Text, SizeType Position = NotFound) const -> SizeType
		{
			if (Text.Empty())
			{
				return Position < Size_ ? Position : Size_;
			}

			if (Text.Size_ > Size_)
			{
				return NotFound;
			}

			auto Start = Size_ - Text.Size_;

			if (Position != NotFound && Position < Start)
			{
				Start = Position;
			}

			for (auto i = Start + 1; i > 0; --i)
			{
				auto Current = i - 1;

				Foundation::Bool Match = true;

				for (SizeType j = 0; j < Text.Size_; ++j)
				{
					if (Data_[Current + j] != Text[j])
					{
						Match = false;
						break;
					}
				}

				if (Match)
				{
					return Current;
				}
			}

			return NotFound;
		}

	private:
		ConstPointer Data_{};
		SizeType Size_{};
	};

	using StringView = BasicStringView<char>;
	using WideStringView = BasicStringView<wchar_t>;
	using U8StringView = BasicStringView<char8_t>;
	using U16StringView = BasicStringView<char16_t>;
	using U32StringView = BasicStringView<char32_t>;
}