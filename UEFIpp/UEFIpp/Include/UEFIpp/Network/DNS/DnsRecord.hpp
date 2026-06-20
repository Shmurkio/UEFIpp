#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/String/String.hpp>

namespace UEFIpp::Network
{
    enum class DnsRecordType : Foundation::Uint16
    {
        A = 1,
        CName = 5,
        AAAA = 28
    };

    enum class DnsRecordClass : Foundation::Uint16
    {
        Internet = 1
    };

    class DnsRecord
    {
    public:
        Library::String Name{};
        DnsRecordType Type{};
        DnsRecordClass Class{};
        Foundation::Uint32 Ttl{};
        Library::Vector<Foundation::Byte> Data{};

    public:
        [[nodiscard]] constexpr auto IPv4() const -> Foundation::Bool
        {
            return Type == DnsRecordType::A &&  Class == DnsRecordClass::Internet && Data.Size() == 4;
        }
    };
}