#include <UEFIpp/Network/DNS/DnsClient.hpp>
#include <UEFIpp/Network/NetworkManager.hpp>

namespace
{
    using namespace UEFIpp;

    inline constexpr auto FallbackDnsServer = Network::IPv4Address{ 192, 168, 2, 1 };
    inline constexpr auto DnsPort = Foundation::Uint16{ 53 };
    inline constexpr auto DefaultTransactionId = Foundation::Uint16{ 0x1234 };

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
}

namespace UEFIpp::Network
{
    auto DnsClient::Resolve(Library::StringView Hostname, IPv4Address& Address) -> Foundation::Bool
    {
        IPv4Address Server{};

        if (!NetworkManager::DefaultDnsServer(Server))
        {
            Server = FallbackDnsServer;
        }

        return Resolve(Server, Hostname, Address);
    }

    auto DnsClient::Resolve(Library::StringView Hostname, DnsResponse& Response) -> Foundation::Bool
    {
        IPv4Address Server{};

        if (!NetworkManager::DefaultDnsServer(Server))
        {
            Server = FallbackDnsServer;
        }

        return Resolve(Server, Hostname, Response);
    }

    auto DnsClient::Resolve(Library::StringView Hostname, IPv4Address& Address, DnsResponse& Response) -> Foundation::Bool
    {
        IPv4Address Server{};

        if (!NetworkManager::DefaultDnsServer(Server))
        {
            Server = FallbackDnsServer;
        }

        return Resolve(Server, Hostname, Address, Response);
    }

    auto DnsClient::Resolve(const IPv4Address& Server, Library::StringView Hostname, IPv4Address& Address) -> Foundation::Bool
    {
        DnsResponse Response{};

        return Resolve(Server, Hostname, Address, Response);
    }

    auto DnsClient::Resolve(const IPv4Address& Server, Library::StringView Hostname, DnsResponse& Response) -> Foundation::Bool
    {
        IPv4Address Address{};

        return Resolve(Server, Hostname, Address, Response);
    }

    auto DnsClient::Resolve(const IPv4Address& Server, Library::StringView Hostname, IPv4Address& Address, DnsResponse& Response) -> Foundation::Bool
    {
        Address = {};
        Response = {};

        Library::Vector<Foundation::Byte> Query{};

        if (!BuildQuery(Hostname, Query, DefaultTransactionId))
        {
            LastStatus_ = UEFI::StatusCode::InvalidParameter;
            OnError(LastStatus_);
            return false;
        }

        OnQueryStarted();

        if (!Udp_.Open())
        {
            LastStatus_ = Udp_.LastStatus();
            OnError(LastStatus_);
            return false;
        }

        if (!Udp_.SendTo(Server, DnsPort, { Query.Data(), Query.Size() }))
        {
            LastStatus_ = Udp_.LastStatus();
            OnError(LastStatus_);
            Udp_.Close();
            return false;
        }

        OnHostnameResolved(Hostname);

        IPv4Address From{};
        Foundation::Uint16 FromPort{};
        Library::Vector<Foundation::Byte> Packet{};

        if (!Udp_.ReceiveFrom(From, FromPort, Packet, 512))
        {
            LastStatus_ = Udp_.LastStatus();
            OnError(LastStatus_);
            Udp_.Close();
            return false;
        }

        Udp_.Close();

        if (!ParseResponse(Packet, Response))
        {
            OnError(LastStatus_);
            return false;
        }

        OnResponseReceived(Response);

        if (!Response.FirstIPv4(Address))
        {
            LastStatus_ = UEFI::StatusCode::NotFound;
            OnError(LastStatus_);
            return false;
        }

        OnAddressResolved(Address);
        OnQueryFinished();

        LastStatus_ = UEFI::StatusCode::Success;

        return true;
    }

    auto DnsClient::BuildQuery(Library::StringView Hostname, Library::Vector<Foundation::Byte>& Query, Foundation::Uint16 TransactionId) -> Foundation::Bool
    {
        Query.Clear();

        if (Hostname.Empty())
        {
            return false;
        }

        Write16(Query, TransactionId);
        Write16(Query, 0x0100);
        Write16(Query, 1);
        Write16(Query, 0);
        Write16(Query, 0);
        Write16(Query, 0);

        Foundation::Size LabelStart{};

        for (Foundation::Size Index{}; Index <= Hostname.Size(); ++Index)
        {
            if (Index != Hostname.Size() && Hostname[Index] != '.')
            {
                continue;
            }

            const auto LabelLength = Index - LabelStart;

            if (LabelLength == 0 || LabelLength > 63)
            {
                return false;
            }

            Query.PushBack(Foundation::Cast::Auto<Foundation::Byte>(LabelLength));

            for (Foundation::Size LabelIndex = LabelStart; LabelIndex < Index; ++LabelIndex)
            {
                Query.PushBack(Foundation::Cast::Auto<Foundation::Byte>(Hostname[LabelIndex]));
            }

            LabelStart = Index + 1;
        }

        Query.PushBack(0);

        Write16(Query, Foundation::Cast::Auto<Foundation::Uint16>(DnsRecordType::A));
        Write16(Query, Foundation::Cast::Auto<Foundation::Uint16>(DnsRecordClass::Internet));

        return true;
    }

    auto DnsClient::ParseResponse(const Library::Vector<Foundation::Byte>& Packet, DnsResponse& Response) -> Foundation::Bool
    {
        Response = {};

        if (Packet.Size() < 12)
        {
            LastStatus_ = UEFI::StatusCode::InvalidParameter;
            return false;
        }

        Response.Header.Id = Read16(Packet, 0);
        Response.Header.Flags = Read16(Packet, 2);
        Response.Header.QuestionCount = Read16(Packet, 4);
        Response.Header.AnswerCount = Read16(Packet, 6);
        Response.Header.AuthorityCount = Read16(Packet, 8);
        Response.Header.AdditionalCount = Read16(Packet, 10);

        if (!Response.Header.Success())
        {
            LastStatus_ = UEFI::StatusCode::DeviceError;
            return false;
        }

        Foundation::Size Offset{ 12 };

        for (Foundation::Uint16 Question{}; Question < Response.Header.QuestionCount; ++Question)
        {
            Library::String Name{};

            if (!ReadName(Packet, Offset, Name))
            {
                LastStatus_ = UEFI::StatusCode::InvalidParameter;
                return false;
            }

            if (Offset + 4 > Packet.Size())
            {
                LastStatus_ = UEFI::StatusCode::InvalidParameter;
                return false;
            }

            Offset += 4;
        }

        for (Foundation::Uint16 Answer{}; Answer < Response.Header.AnswerCount; ++Answer)
        {
            DnsRecord Record{};

            if (!ParseRecord(Packet, Offset, Record))
            {
                LastStatus_ = UEFI::StatusCode::InvalidParameter;
                return false;
            }

            Response.Answers.PushBack(Foundation::Utility::Move(Record));
        }

        for (Foundation::Uint16 Authority{}; Authority < Response.Header.AuthorityCount; ++Authority)
        {
            DnsRecord Record{};

            if (!ParseRecord(Packet, Offset, Record))
            {
                LastStatus_ = UEFI::StatusCode::InvalidParameter;
                return false;
            }

            Response.Authorities.PushBack(Foundation::Utility::Move(Record));
        }

        for (Foundation::Uint16 Additional{}; Additional < Response.Header.AdditionalCount; ++Additional)
        {
            DnsRecord Record{};

            if (!ParseRecord(Packet, Offset, Record))
            {
                LastStatus_ = UEFI::StatusCode::InvalidParameter;
                return false;
            }

            Response.Additionals.PushBack(Foundation::Utility::Move(Record));
        }

        LastStatus_ = UEFI::StatusCode::Success;

        return true;
    }

    auto DnsClient::ReadName(const Library::Vector<Foundation::Byte>& Packet, Foundation::Size& Offset, Library::String& Name) -> Foundation::Bool
    {
        Name.Clear();

        Foundation::Size Current = Offset;
        Foundation::Bool Jumped{};
        Foundation::Size Jumps{};

        while (Current < Packet.Size())
        {
            const auto Length = Packet[Current];

            if (Length == 0)
            {
                ++Current;

                if (!Jumped)
                {
                    Offset = Current;
                }

                return true;
            }

            if ((Length & 0xC0) == 0xC0)
            {
                if (Current + 1 >= Packet.Size())
                {
                    return false;
                }

                const auto Pointer = Foundation::Cast::Auto<Foundation::Size>(((Length & 0x3F) << 8) | Packet[Current + 1]);

                if (Pointer >= Packet.Size())
                {
                    return false;
                }

                if (!Jumped)
                {
                    Offset = Current + 2;
                }

                Current = Pointer;
                Jumped = true;

                if (++Jumps > 16)
                {
                    return false;
                }

                continue;
            }

            if ((Length & 0xC0) != 0 || Length > 63)
            {
                return false;
            }

            ++Current;

            if (Current + Length > Packet.Size())
            {
                return false;
            }

            if (!Name.Empty())
            {
                Name += ".";
            }

            for (Foundation::Size Index{}; Index < Length; ++Index)
            {
                Name.PushBack(Foundation::Cast::Auto<Library::String::ValueType>(Packet[Current + Index]));
            }

            Current += Length;

            if (!Jumped)
            {
                Offset = Current;
            }
        }

        return false;
    }

    auto DnsClient::ParseRecord(const Library::Vector<Foundation::Byte>& Packet, Foundation::Size& Offset, DnsRecord& Record) -> Foundation::Bool
    {
        Record = {};

        if (!ReadName(Packet, Offset, Record.Name))
        {
            return false;
        }

        if (Offset + 10 > Packet.Size())
        {
            return false;
        }

        Record.Type = Foundation::Cast::Auto<DnsRecordType>(Read16(Packet, Offset));
        Offset += 2;

        Record.Class = Foundation::Cast::Auto<DnsRecordClass>(Read16(Packet, Offset));
        Offset += 2;

        Record.Ttl = Read32(Packet, Offset);
        Offset += 4;

        const auto DataLength = Read16(Packet, Offset);
        Offset += 2;

        if (Offset + DataLength > Packet.Size())
        {
            return false;
        }

        Record.Data.Clear();

        for (Foundation::Size Index{}; Index < DataLength; ++Index)
        {
            Record.Data.PushBack(Packet[Offset + Index]);
        }

        Offset += DataLength;

        return true;
    }
}