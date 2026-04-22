#include "IPv4.hpp"

IPv4Address::IPv4Address(uint8_t A, uint8_t B, uint8_t C, uint8_t D)
{
    Bytes_[0] = A;
    Bytes_[1] = B;
    Bytes_[2] = C;
    Bytes_[3] = D;
}

IPv4Address::IPv4Address(const String& Text)
{
    Assign(Text);
}

IPv4Address::IPv4Address(const wchar_t* Text)
{
    Assign(String(Text));
}

IPv4Address::IPv4Address(const char* Text)
{
    Assign(String(Text));
}

auto IPv4Address::operator=(const String& Text) -> IPv4Address&
{
    Assign(Text);
    return *this;
}

auto IPv4Address::operator=(const wchar_t* Text) -> IPv4Address&
{
    Assign(String(Text));
    return *this;
}

auto IPv4Address::operator=(const char* Text) -> IPv4Address&
{
    Assign(String(Text));
    return *this;
}

auto IPv4Address::Clear() -> void
{
    Bytes_[0] = 0;
    Bytes_[1] = 0;
    Bytes_[2] = 0;
    Bytes_[3] = 0;
}

auto IPv4Address::A() const -> uint8_t { return Bytes_[0]; }
auto IPv4Address::B() const -> uint8_t { return Bytes_[1]; }
auto IPv4Address::C() const -> uint8_t { return Bytes_[2]; }
auto IPv4Address::D() const -> uint8_t { return Bytes_[3]; }

auto IPv4Address::Assign(const String& Text) -> bool
{
    auto Parts = Text.Split(L'.');

    if (Parts.Size() != 4)
    {
        return false;
    }

    uint8_t NewBytes[4] = {};

    for (uint32_t i = 0; i < 4; ++i)
    {
        uint8_t Value = 0;

        if (!Parts[i].ToUInt8(Value))
        {
            return false;
        }

        NewBytes[i] = Value;
    }

    Bytes_[0] = NewBytes[0];
    Bytes_[1] = NewBytes[1];
    Bytes_[2] = NewBytes[2];
    Bytes_[3] = NewBytes[3];

    return true;
}

auto IPv4Address::ToString() const -> String
{
    String Result;

    Result.AppendUInt32(A());
    Result.Append(L'.');
    Result.AppendUInt32(B());
    Result.Append(L'.');
    Result.AppendUInt32(C());
    Result.Append(L'.');
    Result.AppendUInt32(D());

    return Result;
}

auto IPv4Address::operator==(const IPv4Address& Other) const -> bool
{
    return Bytes_[0] == Other.Bytes_[0] && Bytes_[1] == Other.Bytes_[1] && Bytes_[2] == Other.Bytes_[2] && Bytes_[3] == Other.Bytes_[3];
}

auto IPv4Address::operator!=(const IPv4Address& Other) const -> bool
{
    return !(*this == Other);
}