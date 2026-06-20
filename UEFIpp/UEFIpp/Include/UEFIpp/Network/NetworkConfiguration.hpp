#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Network/DHCP/DhcpLease.hpp>
#include <UEFIpp/Network/IPv4Address.hpp>

namespace UEFIpp::Network
{
    class NetworkConfiguration
    {
    public:
        IPv4Address Address{};
        IPv4Address SubnetMask{};
        IPv4Address Gateway{};
        Library::Vector<IPv4Address> DnsServers{};
        IPv4Address DhcpServer{};
        Foundation::Uint32 LeaseTime{};
        Foundation::Uint32 RenewalTime{};
        Foundation::Uint32 RebindingTime{};

    public:
        NetworkConfiguration() = default;

        explicit NetworkConfiguration(const DhcpLease& Lease)
        {
            Assign(Lease);
        }

        auto Assign(const DhcpLease& Lease) -> Foundation::Void
        {
            Address = Lease.Address;
            SubnetMask = Lease.SubnetMask;
            Gateway = Lease.Router;
            DnsServers = Lease.DnsServers;
            DhcpServer = Lease.ServerIdentifier;
            LeaseTime = Lease.LeaseTime;
            RenewalTime = Lease.RenewalTime;
            RebindingTime = Lease.RebindingTime;
        }

        [[nodiscard]] auto HasAddress() const -> Foundation::Bool
        {
            return Address != IPv4Address{};
        }

        [[nodiscard]] auto HasGateway() const -> Foundation::Bool
        {
            return Gateway != IPv4Address{};
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