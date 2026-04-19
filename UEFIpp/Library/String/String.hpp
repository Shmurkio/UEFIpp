#pragma once

#include <cstdint>
#include "../../UEFIpp.hpp"

class String
{
private:
    wchar_t* Data_;                                             // Owning wide-character buffer (source of truth)
    mutable char* CharData_;                                    // Cached narrow (char*) representation of Data_
    uint64_t Length_;                                           // Number of elements in Data_ including null terminator
    uint64_t Capacity_;                                         // Allocated size of Data_ buffer (in wchar_t, incl. terminator)
    mutable bool CharDataDirty_;                                // True if CharData_ must be rebuilt from Data_

public:
    String();                                               // Constructs empty string ("")

    String(const char* CharStr);                            // Constructs from narrow string (ASCII widened to wchar_t)
    String(const wchar_t* WcharStr);                        // Constructs from wide string

    ~String();                                              // Frees Data_ and CharData_

    String(const String& Other);                        // Deep copy (copies Data_, invalidates char cache)
    String(String&& Other) noexcept;                    // Move (steals buffers, leaves Other empty)

    auto operator=(const String& Other) -> String&;     // Deep copy assignment
    auto operator=(String&& Other) noexcept -> String&; // Move assignment (steals resources)

    auto operator+=(const char* CharStr) -> String&;        // Appends narrow string
    auto operator+=(const wchar_t* WcharStr) -> String&;    // Appends wide string
    auto operator+=(const String& Other) -> String&;    // Appends another UefiString

    auto operator==(const String& Other) const -> bool;     // Compares with another UefiString
    auto operator!=(const String& Other) const -> bool;     // Inverse of operator==
    auto operator==(const char* CharStr) const -> bool;         // Compares with narrow string
    auto operator==(const wchar_t* WcharStr) const -> bool;     // Compares with wide string

    auto CharStr() const -> const char*;                        // Returns cached narrow string (rebuilds if dirty)
    auto WcharStr() const -> const wchar_t*;                    // Returns internal wide string buffer

    auto Size() const -> uint64_t;                              // Number of visible characters (excluding null terminator)
    auto Empty() const -> bool;                                 // True if Size() == 0

    auto Assign(const char* CharStr) -> bool;                   // Replace content from narrow string
    auto Assign(const wchar_t* WcharStr) -> bool;               // Replace content from wide string

    auto Append(const char* CharStr) -> bool;                   // Append narrow string (widened)
    auto Append(const wchar_t* WcharStr) -> bool;               // Append wide string

    auto Clear() -> void;                                       // Resets string to ""
    auto At(uint64_t Index) -> wchar_t*;                        // Returns pointer to character at Index or nullptr
    auto At(uint64_t Index) const -> const wchar_t*;            // Const version of At
    auto Set(uint64_t Index, wchar_t Character) -> bool;        // Replaces character at Index
    auto Set(uint64_t Index, char Character) -> bool;           // Replaces character at Index (narrow)

    auto Reserve(uint64_t NewCapacity) -> bool;                 // Ensure capacity >= NewCapacity (may reallocate)

    auto Guid() const -> GUID;

    String(CGUID& Guid);

    auto Assign(CGUID& Guid) -> bool;
    auto operator=(CGUID& Guid) -> String&;

private:
    auto UpdateCharCache() const -> bool;                       // Rebuild CharData_ from Data_
};