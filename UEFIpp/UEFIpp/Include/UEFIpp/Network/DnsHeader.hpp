#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Network
{
    class DnsHeader
    {
    public:
        Foundation::Uint16 Id{};
        Foundation::Uint16 Flags{};
        Foundation::Uint16 QuestionCount{};
        Foundation::Uint16 AnswerCount{};
        Foundation::Uint16 AuthorityCount{};
        Foundation::Uint16 AdditionalCount{};

    public:
        [[nodiscard]] constexpr auto Response() const -> Foundation::Bool
        {
            return (Flags & 0x8000) != 0;
        }

        [[nodiscard]] constexpr auto RecursionAvailable() const -> Foundation::Bool
        {
            return (Flags & 0x0080) != 0;
        }

        [[nodiscard]] constexpr auto ResponseCode() const -> Foundation::Uint16
        {
            return Flags & 0x000F;
        }

        [[nodiscard]] constexpr auto Success() const -> Foundation::Bool
        {
            return Response() && ResponseCode() == 0;
        }
    };
}