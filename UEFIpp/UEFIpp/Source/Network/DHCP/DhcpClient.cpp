#include <UEFIpp/Network/DHCP/DhcpClient.hpp>

namespace
{
    using namespace UEFIpp;

    inline constexpr auto DhcpClientPort = Foundation::Uint16{ 68 };
    inline constexpr auto DhcpServerPort = Foundation::Uint16{ 67 };
    inline constexpr auto DhcpBroadcast = Network::IPv4Address{ 255, 255, 255, 255 };
    inline constexpr auto DhcpMagicCookie = Foundation::Uint32{ 0x63825363 };
    inline constexpr auto DefaultTransactionId = Foundation::Uint32{ 0x55454649 };

    inline constexpr auto BootRequest = Foundation::Byte{ 1 };
    inline constexpr auto EthernetHardwareType = Foundation::Byte{ 1 };
    inline constexpr auto EthernetHardwareLength = Foundation::Byte{ 6 };
    inline constexpr auto BroadcastFlag = Foundation::Uint16{ 0x8000 };

    inline constexpr auto OptionSubnetMask = Foundation::Byte{ 1 };
    inline constexpr auto OptionRouter = Foundation::Byte{ 3 };
    inline constexpr auto OptionDnsServer = Foundation::Byte{ 6 };
    inline constexpr auto OptionRequestedIp = Foundation::Byte{ 50 };
    inline constexpr auto OptionLeaseTime = Foundation::Byte{ 51 };
    inline constexpr auto OptionMessageType = Foundation::Byte{ 53 };
    inline constexpr auto OptionServerIdentifier = Foundation::Byte{ 54 };
    inline constexpr auto OptionParameterRequestList = Foundation::Byte{ 55 };
    inline constexpr auto OptionRenewalTime = Foundation::Byte{ 58 };
    inline constexpr auto OptionRebindingTime = Foundation::Byte{ 59 };
    inline constexpr auto OptionEnd = Foundation::Byte{ 255 };

    auto Read16(const Library::Vector<Foundation::Byte>& Packet, Foundation::Size Offset) -> Foundation::Uint16
    {
        return Foundation::Cast::Auto<Foundation::Uint16>((Foundation::Cast::Auto<Foundation::Uint16>(Packet[Offset]) << 8) | Foundation::Cast::Auto<Foundation::Uint16>(Packet[Offset + 1]));
    }

    auto Read32(const Library::Vector<Foundation::Byte>& Packet, Foundation::Size Offset) -> Foundation::Uint32
    {
        return (Foundation::Cast::Auto<Foundation::Uint32>(Packet[Offset]) << 24) | (Foundation::Cast::Auto<Foundation::Uint32>(Packet[Offset + 1]) << 16) | (Foundation::Cast::Auto<Foundation::Uint32>(Packet[Offset + 2]) << 8) | Foundation::Cast::Auto<Foundation::Uint32>(Packet[Offset + 3]);
    }

    auto Write16(Library::Vector<Foundation::Byte>& Packet, Foundation::Uint16 Value) -> Foundation::Void
    {
        Packet.PushBack(Foundation::Cast::Auto<Foundation::Byte>((Value >> 8) & 0xFF));
        Packet.PushBack(Foundation::Cast::Auto<Foundation::Byte>(Value & 0xFF));
    }

    auto Write32(Library::Vector<Foundation::Byte>& Packet, Foundation::Uint32 Value) -> Foundation::Void
    {
        Packet.PushBack(Foundation::Cast::Auto<Foundation::Byte>((Value >> 24) & 0xFF));
        Packet.PushBack(Foundation::Cast::Auto<Foundation::Byte>((Value >> 16) & 0xFF));
        Packet.PushBack(Foundation::Cast::Auto<Foundation::Byte>((Value >> 8) & 0xFF));
        Packet.PushBack(Foundation::Cast::Auto<Foundation::Byte>(Value & 0xFF));
    }

    auto ReadAddress(const Library::Vector<Foundation::Byte>& Packet, Foundation::Size Offset) -> Network::IPv4Address
    {
        return Network::IPv4Address{ Packet[Offset], Packet[Offset + 1], Packet[Offset + 2], Packet[Offset + 3] };
    }

    auto PushAddress(Library::Vector<Foundation::Byte>& Packet, const Network::IPv4Address& Address) -> Foundation::Void
    {
        Packet.PushBack(Address.A());
        Packet.PushBack(Address.B());
        Packet.PushBack(Address.C());
        Packet.PushBack(Address.D());
    }
}

namespace UEFIpp::Network
{
    auto DhcpClient::RequestLease(ByteSpan HardwareAddress, DhcpLease& Lease) -> Foundation::Bool
    {
        Lease = {};

        if (HardwareAddress.Size() != EthernetHardwareLength)
        {
            LastStatus_ = UEFI::StatusCode::InvalidParameter;
            OnError(LastStatus_);
            return false;
        }

        Library::Vector<Foundation::Byte> Discover{};
        Library::Vector<Foundation::Byte> Request{};
        Library::Vector<Foundation::Byte> Packet{};
        DhcpLease Offer{};

        if (!BuildDiscover(HardwareAddress, Discover, DefaultTransactionId))
        {
            LastStatus_ = UEFI::StatusCode::InvalidParameter;
            OnError(LastStatus_);
            return false;
        }

        if (!Udp_.OpenBroadcast(DhcpClientPort))
        {
            LastStatus_ = Udp_.LastStatus();
            OnError(LastStatus_);
            return false;
        }

        OnDiscoverStarted();

        if (!Udp_.SendTo(DhcpBroadcast, DhcpServerPort, { Discover.Data(), Discover.Size() }))
        {
            LastStatus_ = Udp_.LastStatus();
            OnError(LastStatus_);
            Udp_.Close();
            return false;
        }

        IPv4Address From{};
        Foundation::Uint16 FromPort{};

        if (!Udp_.ReceiveFrom(From, FromPort, Packet, 1500))
        {
            LastStatus_ = Udp_.LastStatus();
            OnError(LastStatus_);
            Udp_.Close();
            return false;
        }

        if (!ParsePacket(Packet, DefaultTransactionId, MessageType::Offer, Offer))
        {
            OnError(LastStatus_);
            Udp_.Close();
            return false;
        }

        OnOfferReceived(Offer);

        if (!BuildRequest(HardwareAddress, Request, DefaultTransactionId, Offer))
        {
            LastStatus_ = UEFI::StatusCode::InvalidParameter;
            OnError(LastStatus_);
            Udp_.Close();
            return false;
        }

        OnRequestStarted();

        if (!Udp_.SendTo(DhcpBroadcast, DhcpServerPort, { Request.Data(), Request.Size() }))
        {
            LastStatus_ = Udp_.LastStatus();
            OnError(LastStatus_);
            Udp_.Close();
            return false;
        }

        Packet.Clear();

        if (!Udp_.ReceiveFrom(From, FromPort, Packet, 1500))
        {
            LastStatus_ = Udp_.LastStatus();
            OnError(LastStatus_);
            Udp_.Close();
            return false;
        }

        Udp_.Close();

        if (!ParsePacket(Packet, DefaultTransactionId, MessageType::Ack, Lease))
        {
            OnError(LastStatus_);
            return false;
        }

        OnAckReceived(Lease);
        OnAddressAssigned(Lease.Address);

        if (Lease.HasRouter())
        {
            OnRouterResolved(Lease.Router);
        }

        for (const auto& Server : Lease.DnsServers)
        {
            OnDnsServerResolved(Server);
        }

        OnLeaseAcquired(Lease);
        LastStatus_ = UEFI::StatusCode::Success;

        return true;
    }

    auto DhcpClient::BuildDiscover(ByteSpan HardwareAddress, Library::Vector<Foundation::Byte>& Packet, Foundation::Uint32 TransactionId) -> Foundation::Bool
    {
        Packet.Clear();

        if (HardwareAddress.Size() != EthernetHardwareLength)
        {
            return false;
        }

        WriteHeader(Packet, HardwareAddress, TransactionId);
        Write32(Packet, DhcpMagicCookie);
        WriteOptionByte(Packet, OptionMessageType, Foundation::Cast::Auto<Foundation::Byte>(MessageType::Discover));
        WriteParameterRequestList(Packet);
        WriteEnd(Packet);

        return true;
    }

    auto DhcpClient::BuildRequest(ByteSpan HardwareAddress, Library::Vector<Foundation::Byte>& Packet, Foundation::Uint32 TransactionId, const DhcpLease& Offer) -> Foundation::Bool
    {
        Packet.Clear();

        if (HardwareAddress.Size() != EthernetHardwareLength || !Offer.HasAddress())
        {
            return false;
        }

        WriteHeader(Packet, HardwareAddress, TransactionId);
        Write32(Packet, DhcpMagicCookie);
        WriteOptionByte(Packet, OptionMessageType, Foundation::Cast::Auto<Foundation::Byte>(MessageType::Request));
        WriteOptionAddress(Packet, OptionRequestedIp, Offer.Address);

        if (Offer.ServerIdentifier != IPv4Address{})
        {
            WriteOptionAddress(Packet, OptionServerIdentifier, Offer.ServerIdentifier);
        }

        WriteParameterRequestList(Packet);
        WriteEnd(Packet);

        return true;
    }

    auto DhcpClient::ParsePacket(const Library::Vector<Foundation::Byte>& Packet, Foundation::Uint32 TransactionId, MessageType ExpectedType, DhcpLease& Lease) -> Foundation::Bool
    {
        Lease = {};

        if (Packet.Size() < 240)
        {
            LastStatus_ = UEFI::StatusCode::InvalidParameter;
            return false;
        }

        if (Read32(Packet, 4) != TransactionId || Read32(Packet, 236) != DhcpMagicCookie)
        {
            LastStatus_ = UEFI::StatusCode::DeviceError;
            return false;
        }

        Lease.Address = ReadAddress(Packet, 16);

        MessageType Type{};
        Foundation::Bool HasMessageType{};
        Foundation::Size Offset{ 240 };

        while (Offset < Packet.Size())
        {
            const auto Code = Packet[Offset++];

            if (Code == 0)
            {
                continue;
            }

            if (Code == OptionEnd)
            {
                break;
            }

            if (Offset >= Packet.Size())
            {
                LastStatus_ = UEFI::StatusCode::InvalidParameter;
                return false;
            }

            const auto Length = Packet[Offset++];

            if (Offset + Length > Packet.Size())
            {
                LastStatus_ = UEFI::StatusCode::InvalidParameter;
                return false;
            }

            switch (Code)
            {
            case OptionMessageType:
            {
                if (Length != 1)
                {
                    LastStatus_ = UEFI::StatusCode::InvalidParameter;
                    return false;
                }

                Type = Foundation::Cast::Auto<MessageType>(Packet[Offset]);
                HasMessageType = true;
                break;
            }

            case OptionSubnetMask:
            {
                if (Length >= 4)
                {
                    Lease.SubnetMask = ReadAddress(Packet, Offset);
                }

                break;
            }

            case OptionRouter:
            {
                if (Length >= 4)
                {
                    Lease.Router = ReadAddress(Packet, Offset);
                }

                break;
            }

            case OptionDnsServer:
            {
                for (Foundation::Size Index{}; Index + 4 <= Length; Index += 4)
                {
                    Lease.DnsServers.PushBack(ReadAddress(Packet, Offset + Index));
                }

                break;
            }

            case OptionServerIdentifier:
            {
                if (Length >= 4)
                {
                    Lease.ServerIdentifier = ReadAddress(Packet, Offset);
                }

                break;
            }

            case OptionLeaseTime:
            {
                if (Length == 4)
                {
                    Lease.LeaseTime = Read32(Packet, Offset);
                }

                break;
            }

            case OptionRenewalTime:
            {
                if (Length == 4)
                {
                    Lease.RenewalTime = Read32(Packet, Offset);
                }

                break;
            }

            case OptionRebindingTime:
            {
                if (Length == 4)
                {
                    Lease.RebindingTime = Read32(Packet, Offset);
                }

                break;
            }

            default:
            {
                break;
            }
            }

            Offset += Length;
        }

        if (!HasMessageType || Type != ExpectedType || !Lease.HasAddress())
        {
            LastStatus_ = UEFI::StatusCode::DeviceError;
            return false;
        }

        LastStatus_ = UEFI::StatusCode::Success;

        return true;
    }

    auto DhcpClient::WriteHeader(Library::Vector<Foundation::Byte>& Packet, ByteSpan HardwareAddress, Foundation::Uint32 TransactionId) -> Foundation::Void
    {
        Packet.PushBack(BootRequest);
        Packet.PushBack(EthernetHardwareType);
        Packet.PushBack(EthernetHardwareLength);
        Packet.PushBack(0);

        Write32(Packet, TransactionId);
        Write16(Packet, 0);
        Write16(Packet, BroadcastFlag);

        Write32(Packet, 0);
        Write32(Packet, 0);
        Write32(Packet, 0);
        Write32(Packet, 0);

        for (Foundation::Size Index{}; Index < 16; ++Index)
        {
            Packet.PushBack(Index < HardwareAddress.Size() ? HardwareAddress[Index] : 0);
        }

        for (Foundation::Size Index{}; Index < 64; ++Index)
        {
            Packet.PushBack(0);
        }

        for (Foundation::Size Index{}; Index < 128; ++Index)
        {
            Packet.PushBack(0);
        }
    }

    auto DhcpClient::WriteOptionByte(Library::Vector<Foundation::Byte>& Packet, Foundation::Byte Code, Foundation::Byte Value) -> Foundation::Void
    {
        Packet.PushBack(Code);
        Packet.PushBack(1);
        Packet.PushBack(Value);
    }

    auto DhcpClient::WriteOption32(Library::Vector<Foundation::Byte>& Packet, Foundation::Byte Code, Foundation::Uint32 Value) -> Foundation::Void
    {
        Packet.PushBack(Code);
        Packet.PushBack(4);
        Write32(Packet, Value);
    }

    auto DhcpClient::WriteOptionAddress(Library::Vector<Foundation::Byte>& Packet, Foundation::Byte Code, const IPv4Address& Address) -> Foundation::Void
    {
        Packet.PushBack(Code);
        Packet.PushBack(4);
        PushAddress(Packet, Address);
    }

    auto DhcpClient::WriteParameterRequestList(Library::Vector<Foundation::Byte>& Packet) -> Foundation::Void
    {
        Packet.PushBack(OptionParameterRequestList);
        Packet.PushBack(7);
        Packet.PushBack(OptionSubnetMask);
        Packet.PushBack(OptionRouter);
        Packet.PushBack(OptionDnsServer);
        Packet.PushBack(OptionLeaseTime);
        Packet.PushBack(OptionServerIdentifier);
        Packet.PushBack(OptionRenewalTime);
        Packet.PushBack(OptionRebindingTime);
    }

    auto DhcpClient::WriteEnd(Library::Vector<Foundation::Byte>& Packet) -> Foundation::Void
    {
        Packet.PushBack(OptionEnd);
    }
}