#include <UEFIpp/Network/NetworkManager.hpp>

namespace
{
    UEFIpp::Network::NetworkConfiguration DefaultConfiguration{};
    UEFIpp::Foundation::Bool HasDefaultConfiguration{};
}

namespace UEFIpp::Network
{
    auto NetworkManager::ConfigureDhcp(ByteSpan HardwareAddress) -> Foundation::Bool
    {
        DhcpLease Lease{};

        Dhcp_.OnError.Subscribe([this](UEFI::StatusCode Status)
            {
                LastStatus_ = Status;
                OnError(Status);
            });

        if (!Dhcp_.RequestLease(HardwareAddress, Lease))
        {
            LastStatus_ = Dhcp_.LastStatus();
            OnError(LastStatus_);
            return false;
        }

        Configuration_.Assign(Lease);
        SetDefaultConfiguration(Configuration_);

        OnDhcpLeaseAcquired(Lease);
        OnConfigurationChanged(Configuration_);
        OnConfigured();

        LastStatus_ = UEFI::StatusCode::Success;
        return true;
    }

    auto NetworkManager::Clear() -> Foundation::Void
    {
        Configuration_ = {};
    }

    auto NetworkManager::SetDefaultConfiguration(const NetworkConfiguration& Configuration) -> Foundation::Void
    {
        DefaultConfiguration = Configuration;
        HasDefaultConfiguration = true;
    }

    auto NetworkManager::SetDefaultConfiguration(const DhcpLease& Lease) -> Foundation::Void
    {
        NetworkConfiguration Configuration{ Lease };
        SetDefaultConfiguration(Configuration);
    }

    auto NetworkManager::ClearDefaultConfiguration() -> Foundation::Void
    {
        DefaultConfiguration = {};
        HasDefaultConfiguration = false;
    }

    auto NetworkManager::HasDefaultConfig() -> Foundation::Bool
    {
        return HasDefaultConfiguration;
    }

    auto NetworkManager::DefaultConfig() -> const NetworkConfiguration&
    {
        return ::DefaultConfiguration;
    }

    auto NetworkManager::DefaultDnsServer(IPv4Address& Server) -> Foundation::Bool
    {
        if (!HasDefaultConfiguration)
        {
            return false;
        }

        return DefaultConfiguration.FirstDnsServer(Server);
    }

    auto NetworkManager::DefaultGateway(IPv4Address& Gateway) -> Foundation::Bool
    {
        if (!HasDefaultConfiguration || !DefaultConfiguration.HasGateway())
        {
            return false;
        }

        Gateway = DefaultConfiguration.Gateway;
        return true;
    }
}