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

auto String::Set(uint64_t Index, wchar_t Character) -> bool
{
	if (Index >= Size()) return false;

	Data_[Index] = Character;
	CharDataDirty_ = true;

	return true;
}

auto String::Set(uint64_t Index, char Character) -> bool
{
	return Set(Index, static_cast<wchar_t>(static_cast<unsigned char>(Character)));
}

static auto HexCharToValue(wchar_t Character, uint8_t& Value) -> bool
{
	if (Character >= L'0' && Character <= L'9')
	{
		Value = static_cast<uint8_t>(Character - L'0');
		return true;
	}

	if (Character >= L'A' && Character <= L'F')
	{
		Value = static_cast<uint8_t>(Character - L'A' + 10);
		return true;
	}

	if (Character >= L'a' && Character <= L'f')
	{
		Value = static_cast<uint8_t>(Character - L'a' + 10);
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

	Value = static_cast<uint8_t>((High << 4) | Low);
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

		Value = static_cast<uint16_t>((Value << 4) | Digit);
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
		return static_cast<wchar_t>(L'0' + Value);
	}

	return static_cast<wchar_t>(L'A' + (Value - 10));
}

static auto WriteHexByte(wchar_t* Buffer, uint8_t Value) -> void
{
	Buffer[0] = NibbleToHexChar(static_cast<uint8_t>(Value >> 4));
	Buffer[1] = NibbleToHexChar(static_cast<uint8_t>(Value & 0xF));
}

static auto WriteHexUint16(wchar_t* Buffer, uint16_t Value) -> void
{
	Buffer[0] = NibbleToHexChar(static_cast<uint8_t>((Value >> 12) & 0xF));
	Buffer[1] = NibbleToHexChar(static_cast<uint8_t>((Value >> 8) & 0xF));
	Buffer[2] = NibbleToHexChar(static_cast<uint8_t>((Value >> 4) & 0xF));
	Buffer[3] = NibbleToHexChar(static_cast<uint8_t>(Value & 0xF));
}

static auto WriteHexUint32(wchar_t* Buffer, uint32_t Value) -> void
{
	Buffer[0] = NibbleToHexChar(static_cast<uint8_t>((Value >> 28) & 0xF));
	Buffer[1] = NibbleToHexChar(static_cast<uint8_t>((Value >> 24) & 0xF));
	Buffer[2] = NibbleToHexChar(static_cast<uint8_t>((Value >> 20) & 0xF));
	Buffer[3] = NibbleToHexChar(static_cast<uint8_t>((Value >> 16) & 0xF));
	Buffer[4] = NibbleToHexChar(static_cast<uint8_t>((Value >> 12) & 0xF));
	Buffer[5] = NibbleToHexChar(static_cast<uint8_t>((Value >> 8) & 0xF));
	Buffer[6] = NibbleToHexChar(static_cast<uint8_t>((Value >> 4) & 0xF));
	Buffer[7] = NibbleToHexChar(static_cast<uint8_t>(Value & 0xF));
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