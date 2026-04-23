#pragma once

#include "../Vector/Vector.hpp"
#include "../String/String.hpp"

template<typename T>
class MemView
{
public:
	using ValueType = T;
	using PointerType = T*;
	using ConstPointerType = const T*;
	using ReferenceType = T&;
	using ConstReferenceType = const T&;
	using SizeType = uint64_t;

private:
	PointerType Data_;
	SizeType Size_;

public:
	MemView() : Data_(nullptr), Size_(0) {}

	MemView(PointerType Data, SizeType Size) : Data_(Data), Size_(Size) {}

	template<uint64_t N>
	MemView(T(&Array)[N]) : Data_(Array), Size_(N) {}

	MemView(Vector<T>& Buffer) : Data_(Buffer.Data()), Size_(Buffer.Size()) {}

	template<typename U>
	MemView(const MemView<U>& Other) : Data_(Other.Data()), Size_(Other.Size()) {}

	auto operator=(const MemView& Other) -> MemView&
	{
		if (this == &Other)
		{
			return *this;
		}

		Data_ = Other.Data_;
		Size_ = Other.Size_;

		return *this;
	}

	auto Data() -> PointerType
	{
		return Data_;
	}

	auto Data() const -> ConstPointerType
	{
		return Data_;
	}

	auto Size() const -> SizeType
	{
		return Size_;
	}

	auto Count() const -> SizeType
	{
		return Size_;
	}

	auto SizeBytes() const -> SizeType
	{
		return Size_ * sizeof(T);
	}

	auto Empty() const -> bool
	{
		return !Size_;
	}

	auto begin() -> PointerType
	{
		return Data_;
	}

	auto end() -> PointerType
	{
		return Data_ + Size_;
	}

	auto begin() const -> ConstPointerType
	{
		return Data_;
	}

	auto end() const -> ConstPointerType
	{
		return Data_ + Size_;
	}

	auto Front() -> ReferenceType
	{
		return Data_[0];
	}

	auto Front() const -> ConstReferenceType
	{
		return Data_[0];
	}

	auto Back() -> ReferenceType
	{
		return Data_[Size_ - 1];
	}

	auto Back() const -> ConstReferenceType
	{
		return Data_[Size_ - 1];
	}

	auto At(SizeType Index) -> PointerType
	{
		if (!Data_ || Index >= Size_)
		{
			return nullptr;
		}

		return &Data_[Index];
	}

	auto At(SizeType Index) const -> ConstPointerType
	{
		if (!Data_ || Index >= Size_)
		{
			return nullptr;
		}

		return &Data_[Index];
	}

	auto operator[](SizeType Index) -> ReferenceType
	{
		return Data_[Index];
	}

	auto operator[](SizeType Index) const -> ConstReferenceType
	{
		return Data_[Index];
	}

	auto Clear() -> void
	{
		Data_ = nullptr;
		Size_ = 0;
	}

	auto Reset(PointerType Data, SizeType Size) -> void
	{
		Data_ = Data;
		Size_ = Size;
	}

	auto SubView(SizeType Offset) const -> MemView
	{
		if (Offset > Size_)
		{
			return MemView();
		}

		return MemView(Data_ + Offset, Size_ - Offset);
	}

	auto SubView(SizeType Offset, SizeType Count) const -> MemView
	{
		if (Offset > Size_ || Count > Size_ - Offset)
		{
			return MemView();
		}

		return MemView(Data_ + Offset, Count);
	}

	auto First(SizeType Count) const -> MemView
	{
		if (Count > Size_)
		{
			return MemView();
		}

		return MemView(Data_, Count);
	}

	auto Last(SizeType Count) const -> MemView
	{
		if (Count > Size_)
		{
			return MemView();
		}

		return MemView(Data_ + Size_ - Count, Count);
	}

	auto RemovePrefix(SizeType Count) -> bool
	{
		if (Count > Size_)
		{
			return false;
		}

		Data_ += Count;
		Size_ -= Count;

		return true;
	}

	auto RemoveSuffix(SizeType Count) -> bool
	{
		if (Count > Size_)
		{
			return false;
		}

		Size_ -= Count;

		return true;
	}

	template<typename U>
	auto operator==(const MemView<U>& Other) const -> bool
	{
		if (Size_ != Other.Size())
		{
			return false;
		}

		for (SizeType i = 0; i < Size_; ++i)
		{
			if (Data_[i] != Other[i])
			{
				return false;
			}
		}

		return true;
	}

	template<typename U>
	auto operator!=(const MemView<U>& Other) const -> bool
	{
		return !(*this == Other);
	}

	template<typename U>
	auto operator<(const MemView<U>& Other) const -> bool
	{
		const auto Minimum = (Size_ < Other.Size()) ? Size_ : Other.Size();

		for (SizeType i = 0; i < Minimum; ++i)
		{
			if (Data_[i] < Other[i])
			{
				return true;
			}

			if (Other[i] < Data_[i])
			{
				return false;
			}
		}

		return Size_ < Other.Size();
	}

	template<typename U>
	auto operator<=(const MemView<U>& Other) const -> bool
	{
		return !(*this > Other);
	}

	template<typename U>
	auto operator>(const MemView<U>& Other) const -> bool
	{
		return Other < *this;
	}

	template<typename U>
	auto operator>=(const MemView<U>& Other) const -> bool
	{
		return !(*this < Other);
	}

	auto StartsWith(const MemView& Prefix) const -> bool
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

	auto EndsWith(const MemView& Suffix) const -> bool
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

	auto Contains(const MemView& Needle) const -> bool
	{
		return Find(Needle) != InvalidIndex();
	}

	auto Find(const MemView& Needle) const -> SizeType
	{
		if (Needle.Empty())
		{
			return 0;
		}

		if (Needle.Size_ > Size_)
		{
			return InvalidIndex();
		}

		for (SizeType i = 0; i <= Size_ - Needle.Size_; ++i)
		{
			auto Match = true;

			for (SizeType j = 0; j < Needle.Size_; ++j)
			{
				if (Data_[i + j] != Needle.Data_[j])
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

		return InvalidIndex();
	}

	auto Find(ConstReferenceType Value) const -> SizeType
	{
		for (SizeType i = 0; i < Size_; ++i)
		{
			if (Data_[i] == Value)
			{
				return i;
			}
		}

		return InvalidIndex();
	}

	auto CopyTo(PointerType Destination, SizeType Count) const -> bool
	{
		if (!Destination && Count)
		{
			return false;
		}

		if (Count > Size_)
		{
			return false;
		}

		for (SizeType i = 0; i < Count; ++i)
		{
			Destination[i] = Data_[i];
		}

		return true;
	}

	auto CopyTo(MemView Destination) const -> bool
	{
		if (Destination.Size() < Size_)
		{
			return false;
		}

		for (SizeType i = 0; i < Size_; ++i)
		{
			Destination[i] = Data_[i];
		}

		return true;
	}

	auto ToVector(Vector<ValueType>& Out) const -> bool
	{
		Out.Clear();
		return Out.Append(Data_, Size_);
	}

	auto ToVector() const -> Vector<ValueType>
	{
		Vector<ValueType> Out;
		ToVector(Out);
		return Out;
	}

	auto AssignTo(Vector<ValueType>& Out) const -> bool
	{
		return Out.Assign(Data_, Size_);
	}

	static auto InvalidIndex() -> SizeType
	{
		return Cast::To<SizeType>(-1);
	}

	auto FindPattern(const String& Pattern) const -> SizeType
	{
		if (sizeof(ValueType) != 1)
		{
			return InvalidIndex();
		}

		Vector<uint8_t> PatternBytes;
		Vector<uint8_t> WildcardMask;

		if (!ParsePatternString(Pattern, PatternBytes, WildcardMask))
		{
			return InvalidIndex();
		}

		if (PatternBytes.Empty() || PatternBytes.Size() != WildcardMask.Size())
		{
			return InvalidIndex();
		}

		if (PatternBytes.Size() > Size_)
		{
			return InvalidIndex();
		}

		for (SizeType i = 0; i <= Size_ - PatternBytes.Size(); ++i)
		{
			bool Match = true;

			for (SizeType j = 0; j < PatternBytes.Size(); ++j)
			{
				if (WildcardMask[j])
				{
					continue;
				}

				if (Data_[i + j] != PatternBytes[j])
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

		return InvalidIndex();
	}

	auto FindPatternView(const String& Pattern) const -> MemView
	{
		const auto Offset = FindPattern(Pattern);

		if (Offset == InvalidIndex())
		{
			return MemView();
		}

		Vector<uint8_t> PatternBytes;
		Vector<uint8_t> WildcardMask;

		if (!ParsePatternString(Pattern, PatternBytes, WildcardMask))
		{
			return MemView();
		}

		return SubView(Offset, PatternBytes.Size());
	}

	auto FindPatternPtr(const String& Pattern) -> PointerType
	{
		const auto Offset = FindPattern(Pattern);

		if (Offset == InvalidIndex())
		{
			return nullptr;
		}

		return Data_ + Offset;
	}

	auto FindPatternPtr(const String& Pattern) const -> ConstPointerType
	{
		const auto Offset = FindPattern(Pattern);

		if (Offset == InvalidIndex())
		{
			return nullptr;
		}

		return Data_ + Offset;
	}

	auto ContainsPattern(const String& Pattern) const -> bool
	{
		return FindPattern(Pattern) != InvalidIndex();
	}

	auto ContainsPattern(const String& Pattern) -> bool
	{
		return FindPattern(Pattern) != InvalidIndex();
	}

	template<typename U>
	auto As() const -> MemView<const U>
	{
		if (SizeBytes() % sizeof(U))
		{
			return {};
		}

		return MemView<const U>(Cast::To<const U*>(Data_), SizeBytes() / sizeof(U));
	}

private:
	static auto IsHexChar(char Character) -> bool
	{
		return (Character >= '0' && Character <= '9') || (Character >= 'A' && Character <= 'F') || (Character >= 'a' && Character <= 'f');
	}

	static auto HexCharToValue(char Character, uint8_t& Value) -> bool
	{
		if (Character >= '0' && Character <= '9')
		{
			Value = Character - '0';
			return true;
		}

		if (Character >= 'A' && Character <= 'F')
		{
			Value = Character - 'A' + 10;
			return true;
		}

		if (Character >= 'a' && Character <= 'f')
		{
			Value = Character - 'a' + 10;
			return true;
		}

		return false;
	}

	static auto ParseHexByte(const char* Token, uint8_t& Value) -> bool
	{
		Value = 0;

		if (!Token)
		{
			return false;
		}

		if (!Token[0] || !Token[1] || Token[2])
		{
			return false;
		}

		uint8_t High = 0;
		uint8_t Low = 0;

		if (!HexCharToValue(Token[0], High))
		{
			return false;
		}

		if (!HexCharToValue(Token[1], Low))
		{
			return false;
		}

		Value = (High << 4) | Low;

		return true;
	}

	static auto ParsePatternString(const String& Pattern, Vector<uint8_t>& PatternBytes, Vector<uint8_t>& WildcardMask) -> bool
	{
		PatternBytes.Clear();
		WildcardMask.Clear();

		auto Tokens = Pattern.Split(' ');

		for (auto& Token : Tokens)
		{
			if (Token.Empty())
			{
				continue;
			}

			if (Token == "?" || Token == "??")
			{
				if (!PatternBytes.AppendByte(0))
				{
					return false;
				}

				if (!WildcardMask.AppendByte(1))
				{
					return false;
				}

				continue;
			}

			auto Text = Token.CharStr();
			uint8_t Byte = 0;

			if (!ParseHexByte(Text, Byte))
			{
				return false;
			}

			if (!PatternBytes.AppendByte(Byte))
			{
				return false;
			}

			if (!WildcardMask.AppendByte(0))
			{
				return false;
			}
		}

		return !PatternBytes.Empty();
	}
};