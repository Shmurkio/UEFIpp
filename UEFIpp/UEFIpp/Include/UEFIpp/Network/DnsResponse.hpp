#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Network/DnsHeader.hpp>
#include <UEFIpp/Network/DnsRecord.hpp>
#include <UEFIpp/Network/IPv4Address.hpp>

namespace UEFIpp::Network
{
    class DnsResponse
    {
    public:
        DnsHeader Header{};
        Library::Vector<DnsRecord> Answers{};
        Library::Vector<DnsRecord> Authorities{};
        Library::Vector<DnsRecord> Additionals{};

    public:
        [[nodiscard]] constexpr auto Success() const -> Foundation::Bool
        {
            return Header.Success();
        }

        [[nodiscard]] constexpr auto AnswerCount() const -> Foundation::Size
        {
            return Answers.Size();
        }

        [[nodiscard]] auto Empty() const -> Foundation::Bool
        {
            return Answers.Empty() && Authorities.Empty() && Additionals.Empty();
        }

        [[nodiscard]] auto FirstIPv4(IPv4Address& Address) const -> Foundation::Bool
        {
            for (const auto& Record : Answers)
            {
                if (!Record.IPv4())
                {
                    continue;
                }

                Address = IPv4Address{ Record.Data[0], Record.Data[1], Record.Data[2], Record.Data[3] };
                return true;
            }

            return false;
        }
    };
}