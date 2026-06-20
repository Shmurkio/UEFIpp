#include <UEFIpp/Network/Transport/Udp4Client.hpp>

#include <UEFIpp/Protocols/Traits.hpp>
#include <UEFIpp/UEFI/Context.hpp>

namespace
{
    using namespace UEFIpp;

    inline constexpr auto EfiOpenProtocolGetProtocol = Foundation::Uint32{ 0x00000002 };
    inline constexpr auto EfiEventNone = Foundation::Uint32{ 0 };
    inline constexpr auto EfiTplCallback = UEFI::Tpl{ 8 };

    auto ToRawAddress(const Network::IPv4Address& Address) -> Protocols::Ipv4RawAddress
    {
        return Protocols::Ipv4RawAddress{ { Address.A(), Address.B(), Address.C(), Address.D() } };
    }

    auto FromRawAddress(const Protocols::Ipv4RawAddress& Address) -> Network::IPv4Address
    {
        return Network::IPv4Address{ Address.Addr[0], Address.Addr[1], Address.Addr[2], Address.Addr[3] };
    }
}

namespace UEFIpp::Network
{
    Udp4Client::~Udp4Client()
    {
        Close();
    }

    Udp4Client::Udp4Client(Udp4Client&& Other) noexcept
    {
        MoveFrom(Other);
    }

    auto Udp4Client::operator=(Udp4Client&& Other) noexcept -> Udp4Client&
    {
        if (this != &Other)
        {
            Close();
            MoveFrom(Other);
        }

        return *this;
    }

    auto Udp4Client::Open(Foundation::Uint16 LocalPort) -> Foundation::Bool
    {
        Udp4Config Config{};
        Config.StationPort = LocalPort;

        return Open(Config);
    }

    auto Udp4Client::Open(const Udp4Config& Config) -> Foundation::Bool
    {
        Close();

        LastStatus_ = CreateChild();

        if (UEFI::IsError(LastStatus_))
        {
            DestroyChild();
            OnError(LastStatus_);
            return false;
        }

        LastStatus_ = Configure(Config);

        if (UEFI::IsError(LastStatus_))
        {
            DestroyChild();
            OnError(LastStatus_);
            return false;
        }

        Opened_ = true;
        OnOpened();

        return true;
    }

    auto Udp4Client::OpenBroadcast(Foundation::Uint16 LocalPort) -> Foundation::Bool
    {
        Udp4Config Config{};
        Config.StationPort = LocalPort;
        Config.AcceptBroadcast = true;
        Config.AllowDuplicatePort = true;
        Config.UseDefaultAddress = false;
        Config.StationAddress = IPv4Address{};
        Config.SubnetMask = IPv4Address{};

        return Open(Config);
    }

    auto Udp4Client::SendTo(const IPv4Address& Address, Foundation::Uint16 Port, ByteSpan Data) -> Foundation::Bool
    {
        if (!Udp4_ || !Opened_)
        {
            LastStatus_ = UEFI::StatusCode::NotStarted;
            OnError(LastStatus_);
            return false;
        }

        if (Data.Empty())
        {
            LastStatus_ = UEFI::StatusCode::Success;
            return true;
        }

        UEFI::Event Event{};
        LastStatus_ = CreateTokenEvent(Event);

        if (UEFI::IsError(LastStatus_))
        {
            OnError(LastStatus_);
            return false;
        }

        Protocols::Udp4SessionData Session{};
        Session.DestinationAddress = ToRawAddress(Address);
        Session.DestinationPort = Port;

        Protocols::Udp4TransmitData TxData{};
        TxData.UdpSessionData = &Session;
        TxData.GatewayAddress = nullptr;
        TxData.DataLength = Foundation::Cast::Auto<Foundation::Uint32>(Data.SizeInBytes());
        TxData.FragmentCount = 1;
        TxData.FragmentTable[0].FragmentLength = TxData.DataLength;
        TxData.FragmentTable[0].FragmentBuffer = Foundation::Cast::Auto<Foundation::Byte*>(Data.Data());

        Protocols::Udp4IoToken Token{};
        Token.CompletionToken.Event = Event;
        Token.CompletionToken.Status = UEFI::StatusCode::NotReady;
        Token.Packet.TxData = &TxData;

        LastStatus_ = Udp4_->Transmit(Udp4_, &Token);

        if (!UEFI::IsError(LastStatus_))
        {
            LastStatus_ = WaitForToken(Event, Token.CompletionToken.Status);
        }

        UEFI::Context::BootServices().CloseEvent(Event);

        if (UEFI::IsError(LastStatus_))
        {
            OnError(LastStatus_);
            return false;
        }

        OnDataSent(Data);

        return true;
    }

    auto Udp4Client::SendTo(const IPv4Address& Address, Foundation::Uint16 Port, Library::StringView Text) -> Foundation::Bool
    {
        const ByteSpan Data{ Foundation::Cast::Auto<const Foundation::Byte*>(Text.Data()), Text.Size() };

        if (!SendTo(Address, Port, Data))
        {
            return false;
        }

        OnTextSent(Text);

        return true;
    }

    auto Udp4Client::ReceiveFrom(IPv4Address& Address, Foundation::Uint16& Port, Library::Vector<Foundation::Byte>& Data, Foundation::Size Capacity) -> Foundation::Bool
    {
        Address = {};
        Port = 0;
        Data.Clear();

        if (!Udp4_ || !Opened_)
        {
            LastStatus_ = UEFI::StatusCode::NotStarted;
            OnError(LastStatus_);
            return false;
        }

        UEFI::Event Event{};
        LastStatus_ = CreateTokenEvent(Event);

        if (UEFI::IsError(LastStatus_))
        {
            OnError(LastStatus_);
            return false;
        }

        Protocols::Udp4IoToken Token{};
        Token.CompletionToken.Event = Event;
        Token.CompletionToken.Status = UEFI::StatusCode::NotReady;
        Token.Packet.RxData = nullptr;

        LastStatus_ = Udp4_->Receive(Udp4_, &Token);

        if (!UEFI::IsError(LastStatus_))
        {
            LastStatus_ = WaitForToken(Event, Token.CompletionToken.Status);
        }

        UEFI::Context::BootServices().CloseEvent(Event);

        if (UEFI::IsError(LastStatus_) || !Token.Packet.RxData)
        {
            Data.Clear();
            OnError(LastStatus_);
            return false;
        }

        auto* RxData = Token.Packet.RxData;
        Address = FromRawAddress(RxData->UdpSession.SourceAddress);
        Port = RxData->UdpSession.SourcePort;

        for (Foundation::Uint32 Fragment = 0; Fragment < RxData->FragmentCount; ++Fragment)
        {
            const auto* Buffer = Foundation::Cast::Auto<const Foundation::Byte*>(RxData->FragmentTable[Fragment].FragmentBuffer);
            const auto Length = RxData->FragmentTable[Fragment].FragmentLength;

            for (Foundation::Uint32 Index = 0; Index < Length; ++Index)
            {
                Data.PushBack(Buffer[Index]);
            }
        }

        if (RxData->RecycleSignal)
        {
            UEFI::Context::BootServices().SignalEvent(RxData->RecycleSignal);
        }

        if (Capacity && Data.Size() > Capacity)
        {
            Data.Resize(Capacity);
        }

        OnDataReceived({ Data.Data(), Data.Size() });
        return true;
    }

    auto Udp4Client::ReceiveFrom(IPv4Address& Address, Foundation::Uint16& Port, Library::String& Text, Foundation::Size Capacity) -> Foundation::Bool
    {
        Library::Vector<Foundation::Byte> Data{};

        if (!ReceiveFrom(Address, Port, Data, Capacity))
        {
            Text.Clear();
            return false;
        }

        Text.Clear();

        for (const auto Byte : Data)
        {
            Text += Foundation::Cast::Auto<char>(Byte);
        }

        OnTextReceived(Text.View());

        return true;
    }

    auto Udp4Client::Close() -> Foundation::Void
    {
        if (Udp4_)
        {
            Udp4_->Configure(Udp4_, nullptr);
        }

        const auto WasOpened = Opened_;

        DestroyChild();

        if (WasOpened)
        {
            OnClosed();
        }
    }

    auto Udp4Client::CreateChild() -> UEFI::StatusCode
    {
        Protocols::Access Access{ &UEFI::Context::BootServices() };
        auto Bindings = Access.LocateHandles<Protocols::Udp4ServiceBinding>();

        if (!Bindings)
        {
            return UEFI::StatusCode::NotFound;
        }

        for (auto Handle : Bindings.Value())
        {
            auto Binding = Access.Handle<Protocols::Udp4ServiceBinding>(Handle);

            if (!Binding)
            {
                continue;
            }

            Udp4ServiceBinding_ = Binding.Value();
            ControllerHandle_ = Handle;
            ChildHandle_ = nullptr;

            auto Status = Udp4ServiceBinding_->CreateChild(Udp4ServiceBinding_, &ChildHandle_);

            if (UEFI::IsError(Status))
            {
                ChildHandle_ = nullptr;
                Udp4ServiceBinding_ = nullptr;
                ControllerHandle_ = nullptr;
                continue;
            }

            auto Udp4 = Access.Handle<Protocols::Udp4>(ChildHandle_);

            if (Udp4)
            {
                Udp4_ = Udp4.Value();
                return UEFI::StatusCode::Success;
            }

            Udp4ServiceBinding_->DestroyChild(Udp4ServiceBinding_, ChildHandle_);

            ChildHandle_ = nullptr;
            Udp4ServiceBinding_ = nullptr;
            ControllerHandle_ = nullptr;
        }

        return UEFI::StatusCode::NotFound;
    }

    auto Udp4Client::DestroyChild() -> Foundation::Void
    {
        if (Udp4ServiceBinding_ && ChildHandle_)
        {
            Udp4ServiceBinding_->DestroyChild(Udp4ServiceBinding_, ChildHandle_);
        }

        ControllerHandle_ = nullptr;
        ChildHandle_ = nullptr;
        Udp4ServiceBinding_ = nullptr;
        Udp4_ = nullptr;
        Opened_ = false;
    }

    auto Udp4Client::Configure(const Udp4Config& Options) -> UEFI::StatusCode
    {
        if (!Udp4_)
        {
            return UEFI::StatusCode::NotStarted;
        }

        Protocols::Udp4ConfigData Config{};
        Config.AcceptBroadcast = Options.AcceptBroadcast;
        Config.AcceptPromiscuous = Options.AcceptPromiscuous;
        Config.AcceptAnyPort = Options.AcceptAnyPort;
        Config.AllowDuplicatePort = Options.AllowDuplicatePort;
        Config.TypeOfService = Options.TypeOfService;
        Config.TimeToLive = Options.TimeToLive;
        Config.DoNotFragment = Options.DoNotFragment;
        Config.ReceiveTimeout = Options.ReceiveTimeout;
        Config.TransmitTimeout = Options.TransmitTimeout;
        Config.UseDefaultAddress = Options.UseDefaultAddress;
        Config.StationAddress = ToRawAddress(Options.StationAddress);
        Config.SubnetMask = ToRawAddress(Options.SubnetMask);
        Config.StationPort = Options.StationPort;
        Config.RemoteAddress = ToRawAddress(Options.RemoteAddress);
        Config.RemotePort = Options.RemotePort;

        auto Status = Udp4_->Configure(Udp4_, &Config);

        if (Status == UEFI::StatusCode::NoMapping && Options.UseDefaultAddress)
        {
            auto& BootServices = UEFI::Context::BootServices();

            for (Foundation::Uint32 Attempt = 0; Attempt < 500 && Status == UEFI::StatusCode::NoMapping; ++Attempt)
            {
                Udp4_->Poll(Udp4_);
                BootServices.Stall(10000);
                Status = Udp4_->Configure(Udp4_, &Config);
            }
        }

        return Status;
    }

    auto Udp4Client::CreateTokenEvent(UEFI::Event& Event) -> UEFI::StatusCode
    {
        Event = nullptr;

        return UEFI::Context::BootServices().CreateEvent(EfiEventNone, EfiTplCallback, nullptr, nullptr, &Event);
    }

    auto Udp4Client::WaitForToken(
        UEFI::Event Event,
        UEFI::StatusCode& CompletionStatus
    ) -> UEFI::StatusCode
    {
        if (!Event)
        {
            return UEFI::StatusCode::InvalidParameter;
        }

        auto& BootServices = UEFI::Context::BootServices();

        while (CompletionStatus == UEFI::StatusCode::NotReady)
        {
            UEFI::Event Events[1]{ Event };
            Foundation::UintN Index{};

            const auto Status = BootServices.WaitForEvent(1, Events, &Index);

            if (UEFI::IsError(Status))
            {
                return Status;
            }

            if (Udp4_)
            {
                Udp4_->Poll(Udp4_);
            }
        }

        return CompletionStatus;
    }

    auto Udp4Client::MoveFrom(Udp4Client& Other) -> Foundation::Void
    {
        ControllerHandle_ = Other.ControllerHandle_;
        ChildHandle_ = Other.ChildHandle_;
        Udp4ServiceBinding_ = Other.Udp4ServiceBinding_;
        Udp4_ = Other.Udp4_;
        Opened_ = Other.Opened_;
        LastStatus_ = Other.LastStatus_;

        Other.ControllerHandle_ = nullptr;
        Other.ChildHandle_ = nullptr;
        Other.Udp4ServiceBinding_ = nullptr;
        Other.Udp4_ = nullptr;
        Other.Opened_ = false;
        Other.LastStatus_ = UEFI::StatusCode::Success;
    }
}