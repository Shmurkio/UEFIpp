#include "String.hpp"
#include "../Memory/Memory.hpp"
#include "../Cast/Cast.hpp"

static auto CharLength(const char* Str) -> uint64_t
{
	if (!Str) return 0;

	uint64_t Length = 0;
	while (Str[Length]) ++Length;

	return Length;
}

static auto WcharLength(const wchar_t* Str) -> uint64_t
{
	if (!Str) return 0;

	uint64_t Length = 0;
	while (Str[Length]) ++Length;

	return Length;
}

static auto AllocateWideBuffer(wchar_t*& Buffer, uint64_t Capacity, bool ZeroBuffer = false) -> bool
{
	if (!Capacity)
	{
		Buffer = nullptr;
		return false;
	}

	return Memory::AllocatePool(Buffer, Capacity * sizeof(wchar_t), false, ZeroBuffer);
}

static auto AllocateCharBuffer(char*& Buffer, uint64_t Capacity, bool ZeroBuffer = false) -> bool
{
	if (!Capacity)
	{
		Buffer = nullptr;
		return false;
	}

	return Memory::AllocatePool(Buffer, Capacity * sizeof(char), false, ZeroBuffer);
}

String::String() : Data_(nullptr), CharData_(nullptr), Length_(1), Capacity_(1), CharDataDirty_(true)
{
	if (!AllocateWideBuffer(Data_, 1))
	{
		Length_ = 0;
		Capacity_ = 0;
		return;
	}

	Data_[0] = L'\0';
}

String::String(const char* CharStr) : Data_(nullptr), CharData_(nullptr), Length_(1), Capacity_(1), CharDataDirty_(true)
{
	if (!AllocateWideBuffer(Data_, 1))
	{
		Length_ = 0;
		Capacity_ = 0;
		return;
	}

	Data_[0] = L'\0';
	Assign(CharStr);
}

String::String(const wchar_t* WcharStr) : Data_(nullptr), CharData_(nullptr), Length_(1), Capacity_(1), CharDataDirty_(true)
{
	if (!AllocateWideBuffer(Data_, 1))
	{
		Length_ = 0;
		Capacity_ = 0;
		return;
	}

	Data_[0] = L'\0';
	Assign(WcharStr);
}

String::~String()
{
	Memory::FreePool(Data_, true);
	Memory::FreePool(CharData_, true);
}

String::String(const String& Other) : Data_(nullptr), CharData_(nullptr), Length_(Other.Length_), Capacity_(Other.Capacity_), CharDataDirty_(true)
{
	if (!Capacity_)
	{
		Length_ = 1;
		Capacity_ = 1;
	}

	if (!AllocateWideBuffer(Data_, Capacity_))
	{
		Length_ = 0;
		Capacity_ = 0;
		return;
	}

	auto CopyCount = Length_ < Capacity_ ? Length_ : Capacity_;
	for (uint64_t i = 0; i < CopyCount; ++i)
	{
		Data_[i] = Other.Data_ ? Other.Data_[i] : L'\0';
	}

	if (!CopyCount)
	{
		Data_[0] = L'\0';
		Length_ = 1;
	}
}

String::String(String&& Other) noexcept : Data_(Other.Data_), CharData_(Other.CharData_), Length_(Other.Length_), Capacity_(Other.Capacity_), CharDataDirty_(Other.CharDataDirty_)
{
	Other.Data_ = nullptr;
	Other.CharData_ = nullptr;
	Other.Length_ = 0;
	Other.Capacity_ = 0;
	Other.CharDataDirty_ = true;
}

auto String::operator=(const String& Other) -> String&
{
	if (this == &Other) return *this;

	auto NewCapacity = Other.Capacity_ ? Other.Capacity_ : 1;
	wchar_t* NewData = nullptr;

	if (!AllocateWideBuffer(NewData, NewCapacity))
	{
		return *this;
	}

	auto NewLength = Other.Length_ ? Other.Length_ : 1;
	auto CopyCount = NewLength < NewCapacity ? NewLength : NewCapacity;

	for (uint64_t i = 0; i < CopyCount; ++i)
	{
		NewData[i] = Other.Data_ ? Other.Data_[i] : L'\0';
	}

	if (!CopyCount)
	{
		NewData[0] = L'\0';
		NewLength = 1;
	}

	Memory::FreePool(Data_, true);
	Data_ = NewData;
	Length_ = NewLength;
	Capacity_ = NewCapacity;

	Memory::FreePool(CharData_, true);
	CharDataDirty_ = true;

	return *this;
}

auto String::operator=(String&& Other) noexcept -> String&
{
	if (this == &Other) return *this;

	Memory::FreePool(Data_, true);
	Memory::FreePool(CharData_, true);

	Data_ = Other.Data_;
	CharData_ = Other.CharData_;
	Length_ = Other.Length_;
	Capacity_ = Other.Capacity_;
	CharDataDirty_ = Other.CharDataDirty_;

	Other.Data_ = nullptr;
	Other.CharData_ = nullptr;
	Other.Length_ = 0;
	Other.Capacity_ = 0;
	Other.CharDataDirty_ = true;

	return *this;
}

auto String::CharStr() const -> const char*
{
	if (CharDataDirty_ && !UpdateCharCache()) return "";

	return CharData_ ? CharData_ : "";
}

auto String::WcharStr() const -> const wchar_t*
{
	return Data_ ? Data_ : L"";
}

auto String::Size() const -> uint64_t
{
	return Length_ ? (Length_ - 1) : 0;
}

auto String::Empty() const -> bool
{
	return Size() == 0;
}

auto String::Assign(const char* CharStr) -> bool
{
	if (!CharStr)
	{
		if (!Reserve(1)) return false;

		Data_[0] = L'\0';
		Length_ = 1;
		CharDataDirty_ = true;

		return true;
	}

	auto SourceLength = CharLength(CharStr);
	auto RequiredLength = SourceLength + 1;

	if (!Reserve(RequiredLength)) return false;

	for (uint64_t i = 0; i < SourceLength; ++i)
	{
		Data_[i] = Cast::To<wchar_t>(CharStr[i]);
	}

	Data_[SourceLength] = L'\0';
	Length_ = RequiredLength;
	CharDataDirty_ = true;

	return true;
}

auto String::Assign(const wchar_t* WcharStr) -> bool
{
	if (!WcharStr)
	{
		if (!Reserve(1)) return false;

		Data_[0] = L'\0';
		Length_ = 1;
		CharDataDirty_ = true;

		return true;
	}

	auto SourceLength = WcharLength(WcharStr);
	auto RequiredLength = SourceLength + 1;

	if (!Reserve(RequiredLength)) return false;

	for (uint64_t i = 0; i < SourceLength; ++i)
	{
		Data_[i] = WcharStr[i];
	}

	Data_[SourceLength] = L'\0';
	Length_ = RequiredLength;
	CharDataDirty_ = true;

	return true;
}

auto String::Append(const char* CharStr) -> bool
{
	if (!CharStr) return true;

	auto SourceLength = CharLength(CharStr);
	if (!SourceLength) return true;

	auto CurrentSize = Size();
	auto NewSize = CurrentSize + SourceLength;
	auto RequiredLength = NewSize + 1;

	if (!Reserve(RequiredLength)) return false;

	for (uint64_t i = 0; i < SourceLength; ++i)
	{
		Data_[CurrentSize + i] = Cast::To<wchar_t>(CharStr[i]);
	}

	Data_[NewSize] = L'\0';
	Length_ = RequiredLength;
	CharDataDirty_ = true;

	return true;
}

auto String::Append(const wchar_t* WcharStr) -> bool
{
	if (!WcharStr) return true;

	auto SourceLength = WcharLength(WcharStr);
	if (!SourceLength) return true;

	auto SelfPointer = Data_ && WcharStr >= Data_ && WcharStr < (Data_ + Length_);

	if (SelfPointer)
	{
		String Copy(WcharStr);
		return Append(Copy.WcharStr());
	}

	auto CurrentSize = Size();
	auto NewSize = CurrentSize + SourceLength;
	auto RequiredLength = NewSize + 1;

	if (!Reserve(RequiredLength)) return false;

	for (uint64_t i = 0; i < SourceLength; ++i)
	{
		Data_[CurrentSize + i] = WcharStr[i];
	}

	Data_[NewSize] = L'\0';
	Length_ = RequiredLength;
	CharDataDirty_ = true;

	return true;
}

auto String::Reserve(uint64_t NewCapacity) -> bool
{
	if (NewCapacity <= Capacity_ && Data_)
	{
		return true;
	}

	if (!NewCapacity)
	{
		NewCapacity = 1;
	}

	wchar_t* NewData = nullptr;
	if (!AllocateWideBuffer(NewData, NewCapacity))
	{
		return false;
	}

	auto CopyCount = (Data_ && Length_) ? (Length_ < NewCapacity ? Length_ : NewCapacity) : 0;
	for (uint64_t i = 0; i < CopyCount; ++i)
	{
		NewData[i] = Data_[i];
	}

	if (!CopyCount)
	{
		NewData[0] = L'\0';
		Length_ = 1;
	}

	Memory::FreePool(Data_, true);
	Data_ = NewData;
	Capacity_ = NewCapacity;

	return true;
}

auto String::UpdateCharCache() const -> bool
{
	Memory::FreePool(CharData_, true);

	auto RequiredLength = Length_ ? Length_ : 1;
	if (!AllocateCharBuffer(CharData_, RequiredLength))
	{
		return false;
	}

	auto VisibleLength = Size();

	for (uint64_t i = 0; i < VisibleLength; ++i)
	{
		auto Character = Data_ ? Data_[i] : L'\0';

		if (Character >= 0 && Character <= 0x7F)
		{
			CharData_[i] = Cast::To<char>(Character);
		}
		else
		{
			CharData_[i] = '?';
		}
	}

	CharData_[VisibleLength] = '\0';
	CharDataDirty_ = false;

	return true;
}

auto String::operator+=(const char* CharStr) -> String&
{
	Append(CharStr);
	return *this;
}

auto String::operator+=(const wchar_t* WcharStr) -> String&
{
	Append(WcharStr);
	return *this;
}

auto String::operator+=(const String& Other) -> String&
{
	if (this == &Other)
	{
		String Copy(Other);
		Append(Copy.WcharStr());
		return *this;
	}

	Append(Other.WcharStr());
	return *this;
}

auto String::operator==(const String& Other) const -> bool
{
	if (Size() != Other.Size()) return false;

	for (uint64_t i = 0; i < Length_; ++i)
	{
		if (Data_[i] != Other.Data_[i]) return false;
	}

	return true;
}

auto String::operator!=(const String& Other) const -> bool
{
	return !(*this == Other);
}

auto String::operator==(const char* CharStr) const -> bool
{
	if (!CharStr) return Empty();

	auto OtherLength = CharLength(CharStr);
	if (Size() != OtherLength) return false;

	for (uint64_t i = 0; i < OtherLength; ++i)
	{
		if (Data_[i] != Cast::To<wchar_t>(CharStr[i])) return false;
	}

	return Data_[OtherLength] == L'\0';
}

auto String::operator==(const wchar_t* WcharStr) const -> bool
{
	if (!WcharStr) return Empty();

	auto OtherLength = WcharLength(WcharStr);
	if (Size() != OtherLength) return false;

	for (uint64_t i = 0; i < OtherLength; ++i)
	{
		if (Data_[i] != WcharStr[i]) return false;
	}

	return Data_[OtherLength] == L'\0';
}

auto String::Clear() -> void
{
	if (!Reserve(1))
	{
		return;
	}

	Data_[0] = L'\0';
	Length_ = 1;
	CharDataDirty_ = true;
}

auto String::At(uint64_t Index) -> wchar_t*
{
	if (Index >= Size()) return nullptr;
	return &Data_[Index];
}

auto String::At(uint64_t Index) const -> const wchar_t*
{
	if (Index >= Size()) return nullptr;
	return &Data_[Index];
}

auto String::operator[](uint64_t Index) const -> wchar_t
{
	if (!Data_ || Index >= Size())
	{
		return L'\0';
	}

	return Data_[Index];
}

auto String::Set(uint64_t Index, wchar_t Character) -> bool
{
	if (Index >= Size()) return false;

	Data_[Index] = Character;
	CharDataDirty_ = true;

	return true;
}

auto String::Set(uint64_t Index, char Character) -> bool
{
	return Set(Index, Cast::To<wchar_t>(Character));
}

static auto HexCharToValue(wchar_t Character, uint8_t& Value) -> bool
{
	if (Character >= L'0' && Character <= L'9')
	{
		Value = Cast::To<uint8_t>(Character - L'0');
		return true;
	}

	if (Character >= L'A' && Character <= L'F')
	{
		Value = Cast::To<uint8_t>(Character - L'A' + 10);
		return true;
	}

	if (Character >= L'a' && Character <= L'f')
	{
		Value = Cast::To<uint8_t>(Character - L'a' + 10);
		return true;
	}

	return false;
}

static auto ParseHexByte(const wchar_t* String, uint8_t& Value) -> bool
{
	if (!String)
	{
		return false;
	}

	uint8_t High = 0;
	uint8_t Low = 0;

	if (!HexCharToValue(String[0], High) || !HexCharToValue(String[1], Low))
	{
		return false;
	}

	Value = Cast::To<uint8_t>((High << 4) | Low);
	return true;
}

static auto ParseHexUint16(const wchar_t* String, uint16_t& Value) -> bool
{
	if (!String)
	{
		return false;
	}

	Value = 0;

	for (uint64_t i = 0; i < 4; ++i)
	{
		uint8_t Digit = 0;

		if (!HexCharToValue(String[i], Digit))
		{
			return false;
		}

		Value = Cast::To<uint16_t>((Value << 4) | Digit);
	}

	return true;
}

static auto ParseHexUint32(const wchar_t* String, uint32_t& Value) -> bool
{
	if (!String)
	{
		return false;
	}

	Value = 0;

	for (uint64_t i = 0; i < 8; ++i)
	{
		uint8_t Digit = 0;

		if (!HexCharToValue(String[i], Digit))
		{
			return false;
		}

		Value = (Value << 4) | Digit;
	}

	return true;
}

auto String::Guid() const -> GUID
{
	GUID Guid = {};

	auto Str = WcharStr();

	if (!Str)
	{
		return Guid;
	}

	if (Size() != 36)
	{
		return Guid;
	}

	if (Str[8] != L'-' || Str[13] != L'-' || Str[18] != L'-' || Str[23] != L'-')
	{
		return Guid;
	}

	if (!ParseHexUint32(&Str[0], Guid.Data1))
	{
		return {};
	}

	if (!ParseHexUint16(&Str[9], Guid.Data2))
	{
		return {};
	}

	if (!ParseHexUint16(&Str[14], Guid.Data3))
	{
		return {};
	}

	if (!ParseHexByte(&Str[19], Guid.Data4[0]))
	{
		return {};
	}

	if (!ParseHexByte(&Str[21], Guid.Data4[1]))
	{
		return {};
	}

	if (!ParseHexByte(&Str[24], Guid.Data4[2]))
	{
		return {};
	}

	if (!ParseHexByte(&Str[26], Guid.Data4[3]))
	{
		return {};
	}

	if (!ParseHexByte(&Str[28], Guid.Data4[4]))
	{
		return {};
	}

	if (!ParseHexByte(&Str[30], Guid.Data4[5]))
	{
		return {};
	}

	if (!ParseHexByte(&Str[32], Guid.Data4[6]))
	{
		return {};
	}

	if (!ParseHexByte(&Str[34], Guid.Data4[7]))
	{
		return {};
	}

	return Guid;
}

static auto NibbleToHexChar(uint8_t Value) -> wchar_t
{
	Value &= 0xF;

	if (Value < 10)
	{
		return Cast::To<wchar_t>(L'0' + Value);
	}

	return Cast::To<wchar_t>(L'A' + (Value - 10));
}

static auto WriteHexByte(wchar_t* Buffer, uint8_t Value) -> void
{
	Buffer[0] = NibbleToHexChar(Cast::To<uint8_t>(Value >> 4));
	Buffer[1] = NibbleToHexChar(Cast::To<uint8_t>(Value & 0xF));
}

static auto WriteHexUint16(wchar_t* Buffer, uint16_t Value) -> void
{
	Buffer[0] = NibbleToHexChar(Cast::To<uint8_t>((Value >> 12) & 0xF));
	Buffer[1] = NibbleToHexChar(Cast::To<uint8_t>((Value >> 8) & 0xF));
	Buffer[2] = NibbleToHexChar(Cast::To<uint8_t>((Value >> 4) & 0xF));
	Buffer[3] = NibbleToHexChar(Cast::To<uint8_t>(Value & 0xF));
}

static auto WriteHexUint32(wchar_t* Buffer, uint32_t Value) -> void
{
	Buffer[0] = NibbleToHexChar(Cast::To<uint8_t>((Value >> 28) & 0xF));
	Buffer[1] = NibbleToHexChar(Cast::To<uint8_t>((Value >> 24) & 0xF));
	Buffer[2] = NibbleToHexChar(Cast::To<uint8_t>((Value >> 20) & 0xF));
	Buffer[3] = NibbleToHexChar(Cast::To<uint8_t>((Value >> 16) & 0xF));
	Buffer[4] = NibbleToHexChar(Cast::To<uint8_t>((Value >> 12) & 0xF));
	Buffer[5] = NibbleToHexChar(Cast::To<uint8_t>((Value >> 8) & 0xF));
	Buffer[6] = NibbleToHexChar(Cast::To<uint8_t>((Value >> 4) & 0xF));
	Buffer[7] = NibbleToHexChar(Cast::To<uint8_t>(Value & 0xF));
}

String::String(CGUID& Guid) : Data_(nullptr), CharData_(nullptr), Length_(1), Capacity_(1), CharDataDirty_(true)
{
	if (!AllocateWideBuffer(Data_, 1))
	{
		Length_ = 0;
		Capacity_ = 0;
		return;
	}

	Data_[0] = L'\0';
	Assign(Guid);
}

auto String::Assign(CGUID& Guid) -> bool
{
	const uint64_t RequiredLength = 37; // 36 chars + null

	if (!Reserve(RequiredLength))
	{
		return false;
	}

	WriteHexUint32(&Data_[0], Guid.Data1);
	Data_[8] = L'-';

	WriteHexUint16(&Data_[9], Guid.Data2);
	Data_[13] = L'-';

	WriteHexUint16(&Data_[14], Guid.Data3);
	Data_[18] = L'-';

	WriteHexByte(&Data_[19], Guid.Data4[0]);
	WriteHexByte(&Data_[21], Guid.Data4[1]);
	Data_[23] = L'-';

	WriteHexByte(&Data_[24], Guid.Data4[2]);
	WriteHexByte(&Data_[26], Guid.Data4[3]);
	WriteHexByte(&Data_[28], Guid.Data4[4]);
	WriteHexByte(&Data_[30], Guid.Data4[5]);
	WriteHexByte(&Data_[32], Guid.Data4[6]);
	WriteHexByte(&Data_[34], Guid.Data4[7]);

	Data_[36] = L'\0';
	Length_ = RequiredLength;
	CharDataDirty_ = true;

	return true;
}

auto String::operator=(CGUID& Guid) -> String&
{
	Assign(Guid);
	return *this;
}

auto String::StartsWith(const wchar_t* Prefix) const -> bool
{
	if (!Prefix || !Data_)
	{
		return false;
	}

	auto PrefixLength = WcharLength(Prefix);

	if (PrefixLength > Size())
	{
		return false;
	}

	for (uint64_t i = 0; i < PrefixLength; ++i)
	{
		if (Data_[i] != Prefix[i])
		{
			return false;
		}
	}

	return true;
}

auto String::StartsWith(const char* Prefix) const -> bool
{
	if (!Prefix)
	{
		return false;
	}

	auto PrefixLength = CharLength(Prefix);

	if (PrefixLength > Size())
	{
		return false;
	}

	for (uint64_t i = 0; i < PrefixLength; ++i)
	{
		if (Data_[i] != Cast::To<wchar_t>(Prefix[i]))
		{
			return false;
		}
	}

	return true;
}

auto String::Find(wchar_t Character, uint64_t StartIndex) const -> int64_t
{
	if (!Data_ || StartIndex >= Size())
	{
		return -1;
	}

	for (uint64_t i = StartIndex; i < Size(); ++i)
	{
		if (Data_[i] == Character)
		{
			return Cast::To<int64_t>(i);
		}
	}

	return -1;
}

auto String::Find(char Character, uint64_t StartIndex) const -> int64_t
{
	return Find(Cast::To<wchar_t>(Character), StartIndex);
}

auto String::Find(const wchar_t* Str, uint64_t StartIndex) const -> int64_t
{
	if (!Str || !Data_)
	{
		return -1;
	}

	auto StrLength = WcharLength(Str);

	if (!StrLength || StartIndex >= Size())
	{
		return -1;
	}

	for (uint64_t i = StartIndex; i <= Size() - StrLength; ++i)
	{
		bool Match = true;

		for (uint64_t j = 0; j < StrLength; ++j)
		{
			if (Data_[i + j] != Str[j])
			{
				Match = false;
				break;
			}
		}

		if (Match)
		{
			return Cast::To<int64_t>(i);
		}
	}

	return -1;
}

auto String::Find(const char* Str, uint64_t StartIndex) const -> int64_t
{
	if (!Str)
	{
		return -1;
	}

	auto StrLength = CharLength(Str);

	if (!StrLength || StartIndex >= Size())
	{
		return -1;
	}

	for (uint64_t i = StartIndex; i <= Size() - StrLength; ++i)
	{
		bool Match = true;

		for (uint64_t j = 0; j < StrLength; ++j)
		{
			if (Data_[i + j] != Cast::To<wchar_t>(Str[j]))
			{
				Match = false;
				break;
			}
		}

		if (Match)
		{
			return Cast::To<int64_t>(i);
		}
	}

	return -1;
}

auto String::Substring(uint64_t StartIndex, uint64_t Length) const -> String
{
	String Result;

	if (!Data_ || StartIndex >= Size() || !Length)
	{
		return Result;
	}

	uint64_t Available = Size() - StartIndex;
	uint64_t CopyLength = (Length < Available) ? Length : Available;

	wchar_t* Buffer = nullptr;

	if (!Memory::AllocatePool(Cast::To<void*&>(Buffer), (CopyLength + 1) * sizeof(wchar_t), false, true) || !Buffer)
	{
		return Result;
	}

	for (uint64_t i = 0; i < CopyLength; ++i)
	{
		Buffer[i] = Data_[StartIndex + i];
	}

	Buffer[CopyLength] = L'\0';

	Result.Assign(Buffer);

	Memory::FreePool(Cast::To<void*&>(Buffer), true);

	return Result;
}
auto String::Split(wchar_t Delimiter) const -> Vector<String>
{
	Vector<String> Parts;

	if (!Data_)
	{
		return Parts;
	}

	uint64_t Start = 0;
	uint64_t VisibleLength = Size();

	for (uint64_t i = 0; i < VisibleLength; ++i)
	{
		if (Data_[i] == Delimiter)
		{
			Parts.PushBack(Substring(Start, i - Start));
			Start = i + 1;
		}
	}

	Parts.PushBack(Substring(Start, VisibleLength - Start));
	return Parts;
}

auto String::Split(char Delimiter) const -> Vector<String>
{
	return Split(Cast::To<wchar_t>(Delimiter));
}

auto String::ToUInt32(uint32_t& Value) const -> bool
{
	Value = 0;

	if (!Data_ || Empty())
	{
		return false;
	}

	for (uint64_t i = 0; i < Size(); ++i)
	{
		wchar_t Character = Data_[i];

		if (!IsDigit(Character))
		{
			return false;
		}

		uint32_t Digit = Cast::To<uint32_t>(Character - L'0');

		if (Value > ((0xFFFFFFFFu - Digit) / 10u))
		{
			return false;
		}

		Value = Value * 10u + Digit;
	}

	return true;
}

auto String::ToUInt8(uint8_t& Value) const -> bool
{
	Value = 0;

	uint32_t Parsed = 0;
	if (!ToUInt32(Parsed) || Parsed > 255u)
	{
		return false;
	}

	Value = Cast::To<uint8_t>(Parsed);
	return true;
}

auto String::IsDigit(wchar_t Character) -> bool
{
	return Character >= L'0' && Character <= L'9';
}

auto String::IsDigit(char Character) -> bool
{
	return Character >= '0' && Character <= '9';
}

auto String::Append(wchar_t Character) -> bool
{
	wchar_t Buffer[2] = { Character, L'\0' };
	return Append(Buffer);
}

auto String::Append(char Character) -> bool
{
	return Append(Cast::To<wchar_t>(Character));
}

auto String::AppendUInt32(uint32_t Value) -> bool
{
	wchar_t Buffer[11] = {};
	uint32_t Index = 0;

	if (Value == 0)
	{
		return Append(L'0');
	}

	while (Value > 0)
	{
		Buffer[Index++] = static_cast<wchar_t>(L'0' + (Value % 10));
		Value /= 10;
	}

	for (int32_t i = static_cast<int32_t>(Index) - 1; i >= 0; --i)
	{
		if (!Append(Buffer[i]))
		{
			return false;
		}
	}

	return true;
}

auto String::operator!=(const char* CharStr) const -> bool
{
	return !(*this == CharStr);
}

auto String::operator!=(const wchar_t* WcharStr) const -> bool
{
	return !(*this == WcharStr);
}

auto String::operator<(const String& Other) const -> bool
{
	const uint64_t ThisSize = Size();
	const uint64_t OtherSize = Other.Size();
	const uint64_t Minimum = (ThisSize < OtherSize) ? ThisSize : OtherSize;

	for (uint64_t Index = 0; Index < Minimum; ++Index)
	{
		if (Data_[Index] < Other.Data_[Index])
		{
			return true;
		}

		if (Data_[Index] > Other.Data_[Index])
		{
			return false;
		}
	}

	return ThisSize < OtherSize;
}

auto String::operator<=(const String& Other) const -> bool
{
	return !(*this > Other);
}

auto String::operator>(const String& Other) const -> bool
{
	return Other < *this;
}

auto String::operator>=(const String& Other) const -> bool
{
	return !(*this < Other);
}

auto String::operator<(const char* CharStr) const -> bool
{
	return *this < String(CharStr);
}

auto String::operator<=(const char* CharStr) const -> bool
{
	return *this <= String(CharStr);
}

auto String::operator>(const char* CharStr) const -> bool
{
	return *this > String(CharStr);
}

auto String::operator>=(const char* CharStr) const -> bool
{
	return *this >= String(CharStr);
}

auto String::operator<(const wchar_t* WcharStr) const -> bool
{
	return *this < String(WcharStr);
}

auto String::operator<=(const wchar_t* WcharStr) const -> bool
{
	return *this <= String(WcharStr);
}

auto String::operator>(const wchar_t* WcharStr) const -> bool
{
	return *this > String(WcharStr);
}

auto String::operator>=(const wchar_t* WcharStr) const -> bool
{
	return *this >= String(WcharStr);
}

auto String::operator+=(char Character) -> String&
{
	Append(Character);
	return *this;
}

auto String::operator+=(wchar_t Character) -> String&
{
	Append(Character);
	return *this;
}