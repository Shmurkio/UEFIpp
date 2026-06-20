#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Library/Functional/Event.hpp>
#include <UEFIpp/Network/DHCP/DhcpClient.hpp>
#include <UEFIpp/Network/NetworkConfiguration.hpp>
#include <UEFIpp/UEFI/Status.hpp>

namespace UEFIpp::Network
{
    class NetworkManager
    {
    public:
        using ByteSpan = Library::Span<const Foundation::Byte>;

    public:
        Library::Event<> OnConfigured{};
        Library::Event<const DhcpLease&> OnDhcpLeaseAcquired{};
        Library::Event<const NetworkConfiguration&> OnConfigurationChanged{};
        Library::Event<UEFI::StatusCode> OnError{};

    public:
        NetworkManager() = default;

        [[nodiscard]] auto ConfigureDhcp(ByteSpan HardwareAddress) -> Foundation::Bool;
        auto Clear() -> Foundation::Void;

        [[nodiscard]] constexpr auto LastStatus() const -> UEFI::StatusCode
        {
            return LastStatus_;
        }

        [[nodiscard]] constexpr auto Configuration() const -> const NetworkConfiguration&
        {
            return Configuration_;
        }

    public:
        static auto SetDefaultConfiguration(const NetworkConfiguration& Configuration) -> Foundation::Void;
        static auto SetDefaultConfiguration(const DhcpLease& Lease) -> Foundation::Void;
        static auto ClearDefaultConfiguration() -> Foundation::Void;

        [[nodiscard]] static auto HasDefaultConfig() -> Foundation::Bool;
        [[nodiscard]] static auto DefaultConfig() -> const NetworkConfiguration&;
        [[nodiscard]] static auto DefaultDnsServer(IPv4Address& Server) -> Foundation::Bool;
        [[nodiscard]] static auto DefaultGateway(IPv4Address& Gateway) -> Foundation::Bool;

    private:
        NetworkConfiguration Configuration_{};
        DhcpClient Dhcp_{};
        UEFI::StatusCode LastStatus_{ UEFI::StatusCode::Success };
    };
}