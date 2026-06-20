#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Functional/Event.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Library/String/StringView.hpp>
#include <UEFIpp/Network/DNS/DnsResponse.hpp>
#include <UEFIpp/Network/IPv4Address.hpp>
#include <UEFIpp/Network/Transport/Udp4Client.hpp>
#include <UEFIpp/UEFI/Status.hpp>

namespace UEFIpp::Network
{
    class DnsClient
    {
    public:
        Library::Event<> OnQueryStarted{};
        Library::Event<> OnQueryFinished{};
        Library::Event<UEFI::StatusCode> OnError{};

        Library::Event<Library::StringView> OnHostnameResolved{};
        Library::Event<const DnsResponse&> OnResponseReceived{};
        Library::Event<const IPv4Address&> OnAddressResolved{};

    public:
        DnsClient() = default;

        [[nodiscard]] auto Resolve(Library::StringView Hostname, IPv4Address& Address) -> Foundation::Bool;
        [[nodiscard]] auto Resolve(Library::StringView Hostname, DnsResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto Resolve(Library::StringView Hostname, IPv4Address& Address, DnsResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto Resolve(const IPv4Address& Server, Library::StringView Hostname, IPv4Address& Address) -> Foundation::Bool;
        [[nodiscard]] auto Resolve(const IPv4Address& Server, Library::StringView Hostname, DnsResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto Resolve(const IPv4Address& Server, Library::StringView Hostname, IPv4Address& Address, DnsResponse& Response) -> Foundation::Bool;

        [[nodiscard]] constexpr auto LastStatus() const -> UEFI::StatusCode
        {
            return LastStatus_;
        }

    private:
        [[nodiscard]] auto BuildQuery(Library::StringView Hostname, Library::Vector<Foundation::Byte>& Query, Foundation::Uint16 TransactionId) -> Foundation::Bool;
        [[nodiscard]] auto ParseResponse(const Library::Vector<Foundation::Byte>& Packet, DnsResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto ReadName(const Library::Vector<Foundation::Byte>& Packet, Foundation::Size& Offset, Library::String& Name) -> Foundation::Bool;
        [[nodiscard]] auto ParseRecord(const Library::Vector<Foundation::Byte>& Packet, Foundation::Size& Offset, DnsRecord& Record ) -> Foundation::Bool;

    private:
        Udp4Client Udp_{};
        UEFI::StatusCode LastStatus_{ UEFI::StatusCode::Success };
    };
}