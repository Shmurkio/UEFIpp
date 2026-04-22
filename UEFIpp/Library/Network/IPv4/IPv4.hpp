#pragma once

#include "../../String/String.hpp"

class IPv4Address
{
public:
    IPv4Address() = default;
    IPv4Address(uint8_t A, uint8_t B, uint8_t C, uint8_t D);
    IPv4Address(const String& Text);
    IPv4Address(const wchar_t* Text);
    IPv4Address(const char* Text);

    auto operator=(const String& Text) -> IPv4Address&;
    auto operator=(const wchar_t* Text) -> IPv4Address&;
    auto operator=(const char* Text) -> IPv4Address&;

    auto Assign(const String& Text) -> bool;
    auto Clear() -> void;
    auto ToString() const -> String;

    auto A() const -> uint8_t;
    auto B() const -> uint8_t;
    auto C() const -> uint8_t;
    auto D() const -> uint8_t;

    auto operator==(const IPv4Address& Other) const -> bool;
    auto operator!=(const IPv4Address& Other) const -> bool;

private:
    uint8_t Bytes_[4] = {};
};