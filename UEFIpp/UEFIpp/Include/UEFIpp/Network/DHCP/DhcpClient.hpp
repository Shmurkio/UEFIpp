#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/Functional/Event.hpp>
#include <UEFIpp/Network/DHCP/DhcpLease.hpp>
#include <UEFIpp/Network/IPv4Address.hpp>
#include <UEFIpp/Network/Transport/Udp4Client.hpp>
#include <UEFIpp/UEFI/Status.hpp>

namespace UEFIpp::Network
{
    class DhcpClient
    {
    public:
        using ByteSpan = Library::Span<const Foundation::Byte>;

    public:
        Library::Event<> OnDiscoverStarted{};
        Library::Event<> OnRequestStarted{};
        Library::Event<const DhcpLease&> OnLeaseAcquired{};
        Library::Event<UEFI::StatusCode> OnError{};

        Library::Event<const DhcpLease&> OnOfferReceived{};
        Library::Event<const DhcpLease&> OnAckReceived{};
        Library::Event<const IPv4Address&> OnAddressAssigned{};
        Library::Event<const IPv4Address&> OnRouterResolved{};
        Library::Event<const IPv4Address&> OnDnsServerResolved{};

    public:
        DhcpClient() = default;

        [[nodiscard]] auto RequestLease(ByteSpan HardwareAddress, DhcpLease& Lease) -> Foundation::Bool;

        [[nodiscard]] constexpr auto LastStatus() const -> UEFI::StatusCode
        {
            return LastStatus_;
        }

    private:
        enum class MessageType : Foundation::Byte
        {
            Discover = 1,
            Offer = 2,
            Request = 3,
            Decline = 4,
            Ack = 5,
            Nak = 6,
            Release = 7,
            Inform = 8
        };

    private:
        [[nodiscard]] auto BuildDiscover(ByteSpan HardwareAddress, Library::Vector<Foundation::Byte>& Packet, Foundation::Uint32 TransactionId) -> Foundation::Bool;
        [[nodiscard]] auto BuildRequest(ByteSpan HardwareAddress, Library::Vector<Foundation::Byte>& Packet, Foundation::Uint32 TransactionId, const DhcpLease& Offer) -> Foundation::Bool;
        [[nodiscard]] auto ParsePacket(const Library::Vector<Foundation::Byte>& Packet, Foundation::Uint32 TransactionId, MessageType ExpectedType, DhcpLease& Lease) -> Foundation::Bool;

        auto WriteHeader(Library::Vector<Foundation::Byte>& Packet, ByteSpan HardwareAddress, Foundation::Uint32 TransactionId) -> Foundation::Void;
        auto WriteOptionByte(Library::Vector<Foundation::Byte>& Packet, Foundation::Byte Code, Foundation::Byte Value) -> Foundation::Void;
        auto WriteOption32(Library::Vector<Foundation::Byte>& Packet, Foundation::Byte Code, Foundation::Uint32 Value) -> Foundation::Void;
        auto WriteOptionAddress(Library::Vector<Foundation::Byte>& Packet, Foundation::Byte Code, const IPv4Address& Address) -> Foundation::Void;
        auto WriteParameterRequestList(Library::Vector<Foundation::Byte>& Packet) -> Foundation::Void;
        auto WriteEnd(Library::Vector<Foundation::Byte>& Packet) -> Foundation::Void;

    private:
        Udp4Client Udp_{};
        UEFI::StatusCode LastStatus_{ UEFI::StatusCode::Success };
    };
}