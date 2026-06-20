#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Network/IPv4Address.hpp>

namespace UEFIpp::Network
{
    class DhcpLease
    {
    public:
        IPv4Address Address{};
        IPv4Address SubnetMask{};
        IPv4Address Router{};
        Library::Vector<IPv4Address> DnsServers{};
        IPv4Address ServerIdentifier{};
        Foundation::Uint32 LeaseTime{};
        Foundation::Uint32 RenewalTime{};
        Foundation::Uint32 RebindingTime{};

    public:
        [[nodiscard]] auto HasAddress() const -> Foundation::Bool
        {
            return Address != IPv4Address{};
        }

        [[nodiscard]] auto HasRouter() const -> Foundation::Bool
        {
            return Router != IPv4Address{};
        }

        [[nodiscard]] auto HasDnsServer() const -> Foundation::Bool
        {
            return !DnsServers.Empty();
        }

        [[nodiscard]] auto FirstDnsServer(IPv4Address& Server) const -> Foundation::Bool
        {
            if (DnsServers.Empty())
            {
                return false;
            }

            Server = DnsServers[0];
            return true;
        }
    };
}