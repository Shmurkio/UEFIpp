#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/String/StringView.hpp>
#include <UEFIpp/Text/Format.hpp>

namespace UEFIpp::Network
{
    class IPv4Address
    {
    public:
        constexpr IPv4Address() = default;

        constexpr IPv4Address(Foundation::Uint8 A, Foundation::Uint8 B, Foundation::Uint8 C, Foundation::Uint8 D) : Bytes_{ A, B, C, D }
        {
        }

        [[nodiscard]] static auto Parse(Library::StringView Text, IPv4Address& Out) -> Foundation::Bool
        {
            Foundation::Uint8 Parts[4]{};
            Foundation::Uint32 Part{};
            Foundation::Uint32 Value{};
            Foundation::Bool HaveDigit{};

            for (Foundation::Size i = 0; i <= Text.Size(); ++i)
            {
                const auto End = i == Text.Size();
                const auto Ch = End ? '.' : Text[i];

                if (Ch >= '0' && Ch <= '9')
                {
                    HaveDigit = true;
                    Value = Value * 10 + Foundation::Cast::Auto<Foundation::Uint32>(Ch - '0');

                    if (Value > 255)
                    {
                        return false;
                    }

                    continue;
                }

                if (Ch != '.' || !HaveDigit || Part >= 4)
                {
                    return false;
                }

                Parts[Part++] = Foundation::Cast::Auto<Foundation::Uint8>(Value);
                Value = 0;
                HaveDigit = false;
            }

            if (Part != 4)
            {
                return false;
            }

            Out = IPv4Address(Parts[0], Parts[1], Parts[2], Parts[3]);

            return true;
        }

        [[nodiscard]] constexpr auto A() const -> Foundation::Uint8
        {
            return Bytes_[0];
        }

        [[nodiscard]] constexpr auto B() const -> Foundation::Uint8
        {
            return Bytes_[1];
        }

        [[nodiscard]] constexpr auto C() const -> Foundation::Uint8
        {
            return Bytes_[2];
        }

        [[nodiscard]] constexpr auto D() const -> Foundation::Uint8
        {
            return Bytes_[3];
        }

        [[nodiscard]] constexpr auto Bytes() const -> const Foundation::Uint8*
        {
            return Bytes_;
        }

        constexpr auto Clear() -> Foundation::Void
        {
            Bytes_[0] = Bytes_[1] = Bytes_[2] = Bytes_[3] = 0;
        }

        [[nodiscard]] constexpr auto operator==(const IPv4Address&) const->Foundation::Bool = default;

    private:
        Foundation::Uint8 Bytes_[4]{};
    };
}