#pragma once

#include <cstdint>
#include "../../UEFIpp.hpp"
#include "../Vector/Vector.hpp"

class String
{
private:
    wchar_t* Data_;                                             // Owning wide-character buffer (source of truth)
    mutable char* CharData_;                                    // Cached narrow (char*) representation of Data_
    uint64_t Length_;                                           // Number of elements in Data_ including null terminator
    uint64_t Capacity_;                                         // Allocated size of Data_ buffer (in wchar_t, incl. terminator)
    mutable bool CharDataDirty_;                                // True if CharData_ must be rebuilt from Data_

public:
    String();

    String(const char* CharStr);
    String(const wchar_t* WcharStr);

    ~String();

    String(const String& Other);
    String(String&& Other) noexcept;

    auto operator=(const String& Other) -> String&;
    auto operator=(String&& Other) noexcept -> String&;

    auto operator+=(const char* CharStr) -> String&;
    auto operator+=(const wchar_t* WcharStr) -> String&;
    auto operator+=(const String& Other) -> String&;
    auto operator+=(char Character) -> String&;
    auto operator+=(wchar_t Character) -> String&;

    auto operator==(const String& Other) const -> bool;
    auto operator!=(const String& Other) const -> bool;
    auto operator==(const char* CharStr) const -> bool;
    auto operator==(const wchar_t* WcharStr) const -> bool;
    auto operator!=(const char* CharStr) const -> bool;
    auto operator!=(const wchar_t* WcharStr) const -> bool;

    auto operator<(const String& Other) const -> bool;
    auto operator<=(const String& Other) const -> bool;
    auto operator>(const String& Other) const -> bool;
    auto operator>=(const String& Other) const -> bool;

    auto operator<(const char* CharStr) const -> bool;
    auto operator<=(const char* CharStr) const -> bool;
    auto operator>(const char* CharStr) const -> bool;
    auto operator>=(const char* CharStr) const -> bool;

    auto operator<(const wchar_t* WcharStr) const -> bool;
    auto operator<=(const wchar_t* WcharStr) const -> bool;
    auto operator>(const wchar_t* WcharStr) const -> bool;
    auto operator>=(const wchar_t* WcharStr) const -> bool;

    auto CharStr() const -> const char*;
    auto WcharStr() const -> const wchar_t*;

    auto Size() const -> uint64_t;
    auto Empty() const -> bool;

    auto Assign(const char* CharStr) -> bool;
    auto Assign(const wchar_t* WcharStr) -> bool;

    auto Append(const char* CharStr) -> bool;
    auto Append(const wchar_t* WcharStr) -> bool;

    auto Clear() -> void;
    auto At(uint64_t Index) -> wchar_t*;
    auto At(uint64_t Index) const -> const wchar_t*;
    auto operator[](uint64_t Index) const -> wchar_t;
    auto Set(uint64_t Index, wchar_t Character) -> bool;
    auto Set(uint64_t Index, char Character) -> bool;

    auto Reserve(uint64_t NewCapacity) -> bool;

    auto Guid() const -> GUID;

    String(CGUID& Guid);

    auto Assign(CGUID& Guid) -> bool;
    auto operator=(CGUID& Guid) -> String&;

    auto StartsWith(const char* Prefix) const -> bool;
    auto StartsWith(const wchar_t* Prefix) const -> bool;

    auto Find(wchar_t Character, uint64_t StartIndex = 0) const -> int64_t;
    auto Find(char Character, uint64_t StartIndex = 0) const -> int64_t;

    auto Find(const wchar_t* Str, uint64_t StartIndex = 0) const -> int64_t;
    auto Find(const char* Str, uint64_t StartIndex = 0) const -> int64_t;

    auto Substring(uint64_t StartIndex, uint64_t Length) const -> String;

    auto Split(wchar_t Delimiter) const -> Vector<String>;
    auto Split(char Delimiter) const -> Vector<String>;

    auto ToUInt32(uint32_t& Value) const -> bool;
    auto ToUInt8(uint8_t& Value) const -> bool;

    static auto IsDigit(wchar_t Character) -> bool;
    static auto IsDigit(char Character) -> bool;

    auto AppendUInt32(uint32_t Value) -> bool;
    auto Append(wchar_t Character) -> bool;
    auto Append(char Character) -> bool;

private:
    auto UpdateCharCache() const -> bool;
};

inline auto operator+(const String& Left, const String& Right) -> String
{
    String Result(Left);
    Result += Right;
    return Result;
}

inline auto operator+(const String& Left, const char* Right) -> String
{
    String Result(Left);
    Result += Right;
    return Result;
}

inline auto operator+(const char* Left, const String& Right) -> String
{
    String Result(Left);
    Result += Right;
    return Result;
}

inline auto operator+(const String& Left, const wchar_t* Right) -> String
{
    String Result(Left);
    Result += Right;
    return Result;
}

inline auto operator+(const wchar_t* Left, const String& Right) -> String
{
    String Result(Left);
    Result += Right;
    return Result;
}

inline auto operator==(const char* Left, const String& Right) -> bool
{
    return Right == Left;
}

inline auto operator==(const wchar_t* Left, const String& Right) -> bool
{
    return Right == Left;
}

inline auto operator!=(const char* Left, const String& Right) -> bool
{
    return !(Right == Left);
}

inline auto operator!=(const wchar_t* Left, const String& Right) -> bool
{
    return !(Right == Left);
}

inline auto operator<(const char* Left, const String& Right) -> bool
{
    return String(Left) < Right;
}

inline auto operator<(const wchar_t* Left, const String& Right) -> bool
{
    return String(Left) < Right;
}

inline auto operator<=(const char* Left, const String& Right) -> bool
{
    return String(Left) <= Right;
}

inline auto operator<=(const wchar_t* Left, const String& Right) -> bool
{
    return String(Left) <= Right;
}

inline auto operator>(const char* Left, const String& Right) -> bool
{
    return String(Left) > Right;
}

inline auto operator>(const wchar_t* Left, const String& Right) -> bool
{
    return String(Left) > Right;
}

inline auto operator>=(const char* Left, const String& Right) -> bool
{
    return String(Left) >= Right;
}

inline auto operator>=(const wchar_t* Left, const String& Right) -> bool
{
    return String(Left) >= Right;
}