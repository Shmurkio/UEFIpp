#include <UEFIpp/Network/Tcp4Client.hpp>

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
}

namespace UEFIpp::Network
{
    Tcp4Client::~Tcp4Client()
    {
        Close(true);
    }

    Tcp4Client::Tcp4Client(Tcp4Client&& Other) noexcept
    {
        MoveFrom(Other);
    }

    auto Tcp4Client::operator=(Tcp4Client&& Other) noexcept -> Tcp4Client&
    {
        if (this != &Other)
        {
            Close(true);
            MoveFrom(Other);
        }

        return *this;
    }

    auto Tcp4Client::Connect(const IPv4Address& RemoteAddress, Foundation::Uint16 RemotePort) -> Foundation::Bool
    {
        Close(true);
        LastStatus_ = CreateChild();
        Stream::Out::Serial << "CreateChild: " << Stream::Hexadecimal

            << Foundation::Cast::Underlying(LastStatus_) << Stream::Endl;
        if (UEFI::IsError(LastStatus_))
        {
            DestroyChild();
            return false;
        }

        LastStatus_ = Configure(RemoteAddress, RemotePort);
        Stream::Out::Serial << "Configure: " << Stream::Hexadecimal

            << Foundation::Cast::Underlying(LastStatus_) << Stream::Endl;
        if (UEFI::IsError(LastStatus_))
        {
            DestroyChild();
            return false;
        }

        UEFI::Event Event{};
        LastStatus_ = CreateTokenEvent(Event);

        if (UEFI::IsError(LastStatus_))
        {
            DestroyChild();
            return false;
        }

        Protocols::Tcp4ConnectionToken Token{};
        Token.CompletionToken.Event = Event;
        Token.CompletionToken.Status = UEFI::StatusCode::NotReady;

        LastStatus_ = Tcp4_->Connect(Tcp4_, &Token);
        Stream::Out::Serial << "Tcp4 Connect call: " << Stream::Hexadecimal

            << Foundation::Cast::Underlying(LastStatus_) << Stream::Endl;
        if (!UEFI::IsError(LastStatus_))
        {
            LastStatus_ = WaitForToken(Event, Token.CompletionToken.Status);
        }

        UEFI::Context::BootServices().CloseEvent(Event);

        if (UEFI::IsError(LastStatus_))
        {
            DestroyChild();
            return false;
        }

        Connected_ = true;

        return true;
    }

    auto Tcp4Client::Send(Library::Span<const Foundation::Byte> Data) -> Foundation::Bool
    {
        if (!Tcp4_ || !Connected_)
        {
            LastStatus_ = UEFI::StatusCode::NotStarted;
            return false;
        }

        if (Data.Empty())
        {
            LastStatus_ = UEFI::StatusCode::Success;
            return true;
        }

        if (Data.SizeInBytes() > Foundation::Cast::Auto<Foundation::Size>(~Foundation::Uint32{}))
        {
            LastStatus_ = UEFI::StatusCode::BadBufferSize;
            return false;
        }

        UEFI::Event Event{};
        LastStatus_ = CreateTokenEvent(Event);

        if (UEFI::IsError(LastStatus_))
        {
            return false;
        }

        Protocols::Tcp4TransmitData TxData{};
        TxData.Push = true;
        TxData.Urgent = false;
        TxData.DataLength = Foundation::Cast::Auto<Foundation::Uint32>(Data.SizeInBytes());
        TxData.FragmentCount = 1;
        TxData.FragmentTable[0].FragmentLength = TxData.DataLength;
        TxData.FragmentTable[0].FragmentBuffer = const_cast<Foundation::Byte*>(Data.Data());

        Protocols::Tcp4IoToken Token{};
        Token.CompletionToken.Event = Event;
        Token.CompletionToken.Status = UEFI::StatusCode::NotReady;
        Token.Packet.TxData = &TxData;

        LastStatus_ = Tcp4_->Transmit(Tcp4_, &Token);

        if (!UEFI::IsError(LastStatus_))
        {
            LastStatus_ = WaitForToken(Event, Token.CompletionToken.Status);
        }

        UEFI::Context::BootServices().CloseEvent(Event);

        return !UEFI::IsError(LastStatus_);
    }

    auto Tcp4Client::Receive(Library::Vector<Foundation::Byte>& Buffer, Foundation::Size Capacity) -> Foundation::Bool
    {
        if (!Tcp4_ || !Connected_)
        {
            LastStatus_ = UEFI::StatusCode::NotStarted;
            return false;
        }

        if (Capacity == 0 || Capacity > Foundation::Cast::Auto<Foundation::Size>(~Foundation::Uint32{}))
        {
            LastStatus_ = UEFI::StatusCode::BadBufferSize;
            return false;
        }

        if (!Buffer.Resize(Capacity))
        {
            LastStatus_ = UEFI::StatusCode::OutOfResources;
            return false;
        }

        UEFI::Event Event{};
        LastStatus_ = CreateTokenEvent(Event);

        if (UEFI::IsError(LastStatus_))
        {
            Buffer.Resize(0);
            return false;
        }

        Protocols::Tcp4ReceiveData RxData{};
        RxData.UrgentFlag = false;
        RxData.DataLength = Foundation::Cast::Auto<Foundation::Uint32>(Capacity);
        RxData.FragmentCount = 1;
        RxData.FragmentTable[0].FragmentLength = RxData.DataLength;
        RxData.FragmentTable[0].FragmentBuffer = Buffer.Data();

        Protocols::Tcp4IoToken Token{};
        Token.CompletionToken.Event = Event;
        Token.CompletionToken.Status = UEFI::StatusCode::NotReady;
        Token.Packet.RxData = &RxData;

        LastStatus_ = Tcp4_->Receive(Tcp4_, &Token);
        if (!UEFI::IsError(LastStatus_))
        {
            LastStatus_ = WaitForToken(Event, Token.CompletionToken.Status);
        }

        UEFI::Context::BootServices().CloseEvent(Event);

        if (UEFI::IsError(LastStatus_))
        {
            Buffer.Resize(0);
            return false;
        }

        Buffer.Resize(RxData.DataLength);

        return true;
    }

    auto Tcp4Client::Close(Foundation::Bool Abort) -> Foundation::Void
    {
        if (Tcp4_ && Connected_)
        {
            UEFI::Event Event{};
            auto Status = CreateTokenEvent(Event);

            if (!UEFI::IsError(Status))
            {
                Protocols::Tcp4CloseToken Token{};
                Token.CompletionToken.Event = Event;
                Token.CompletionToken.Status = UEFI::StatusCode::NotReady;
                Token.AbortOnClose = Abort;

                Status = Tcp4_->Close(Tcp4_, &Token);

                if (!UEFI::IsError(Status))
                {
                    Status = WaitForToken(Event, Token.CompletionToken.Status);
                }

                UEFI::Context::BootServices().CloseEvent(Event);
            }

            LastStatus_ = Status;
        }

        Connected_ = false;

        DestroyChild();
    }

    auto Tcp4Client::CreateChild() -> UEFI::StatusCode
    {
        auto& BootServices = UEFI::Context::BootServices();

        Foundation::UintN BufferSize{};
        auto Status = BootServices.LocateHandle(UEFI::Table::LocateSearchType::ByProtocol, const_cast<UEFI::Guid*>(&Protocols::ProtocolTraits<Protocols::Tcp4ServiceBinding>::Id), nullptr, &BufferSize, nullptr);

        if (Status != UEFI::StatusCode::BufferTooSmall)
        {
            return Status;
        }

        Library::Vector<UEFI::Handle> Handles;

        if (!Handles.Resize(BufferSize / sizeof(UEFI::Handle)))
        {
            return UEFI::StatusCode::OutOfResources;
        }

        Status = BootServices.LocateHandle(UEFI::Table::LocateSearchType::ByProtocol, const_cast<UEFI::Guid*>(&Protocols::ProtocolTraits<Protocols::Tcp4ServiceBinding>::Id), nullptr, &BufferSize, Handles.Data());

        if (UEFI::IsError(Status))
        {
            return Status;
        }

        for (auto Handle : Handles)
        {
            Foundation::Void* Interface{};

            Status = BootServices.HandleProtocol(Handle, const_cast<UEFI::Guid*>(&Protocols::ProtocolTraits<Protocols::Tcp4ServiceBinding>::Id), &Interface);

            if (UEFI::IsError(Status) || !Interface)
            {
                continue;
            }

            Tcp4ServiceBinding_ = static_cast<Protocols::Tcp4ServiceBinding*>(Interface);
            ControllerHandle_ = Handle;
            ChildHandle_ = nullptr;

            Status = Tcp4ServiceBinding_->CreateChild(Tcp4ServiceBinding_, &ChildHandle_);

            if (UEFI::IsError(Status))
            {
                ChildHandle_ = nullptr;
                Tcp4ServiceBinding_ = nullptr;
                ControllerHandle_ = nullptr;
                continue;
            }

            Foundation::Void* Tcp4Interface{};
            Status = BootServices.OpenProtocol(
                ChildHandle_,
                const_cast<UEFI::Guid*>(&Protocols::ProtocolTraits<Protocols::Tcp4>::Id),
                &Tcp4Interface,
                UEFI::Context::ImageHandle(),
                nullptr,
                EfiOpenProtocolGetProtocol
            );

            if (!UEFI::IsError(Status) && Tcp4Interface)
            {
                Tcp4_ = static_cast<Protocols::Tcp4*>(Tcp4Interface);
                return UEFI::StatusCode::Success;
            }

            Tcp4ServiceBinding_->DestroyChild(Tcp4ServiceBinding_, ChildHandle_);
            ChildHandle_ = nullptr;
            Tcp4ServiceBinding_ = nullptr;
            ControllerHandle_ = nullptr;
        }

        return UEFI::StatusCode::NotFound;
    }

    auto Tcp4Client::DestroyChild() -> Foundation::Void
    {
        if (Tcp4ServiceBinding_ && ChildHandle_)
        {
            Tcp4ServiceBinding_->DestroyChild(Tcp4ServiceBinding_, ChildHandle_);
        }

        ControllerHandle_ = nullptr;
        ChildHandle_ = nullptr;
        Tcp4ServiceBinding_ = nullptr;
        Tcp4_ = nullptr;
        Connected_ = false;
    }

    auto Tcp4Client::Configure(const IPv4Address& RemoteAddress, Foundation::Uint16 RemotePort) -> UEFI::StatusCode
    {
        if (!Tcp4_)
        {
            return UEFI::StatusCode::NotStarted;
        }

        Protocols::Tcp4Option Options{};
        Options.EnableNagle = true;

        Protocols::Tcp4ConfigData Config{};
        Config.TypeOfService = 0;
        Config.TimeToLive = 64;
        Config.AccessPoint.UseDefaultAddress = true;
        Config.AccessPoint.StationPort = 0;
        Config.AccessPoint.RemoteAddress = ToRawAddress(RemoteAddress);
        Config.AccessPoint.RemotePort = RemotePort;
        Config.AccessPoint.ActiveFlag = true;
        Config.ControlOption = &Options;

        auto Status = Tcp4_->Configure(Tcp4_, &Config);

        if (Status == UEFI::StatusCode::NoMapping)
        {
            auto& BootServices = UEFI::Context::BootServices();

            for (Foundation::Uint32 Attempt = 0;
                Attempt < 500 && Status == UEFI::StatusCode::NoMapping;
                ++Attempt)
            {
                Tcp4_->Poll(Tcp4_);
                BootServices.Stall(10000);
                Status = Tcp4_->Configure(Tcp4_, &Config);
            }
        }

        return Status;
    }

    auto Tcp4Client::CreateTokenEvent(UEFI::Event& Event) -> UEFI::StatusCode
    {
        Event = nullptr;

        return UEFI::Context::BootServices().CreateEvent(
            EfiEventNone,
            EfiTplCallback,
            nullptr,
            nullptr,
            &Event
        );
    }

    auto Tcp4Client::WaitForToken(
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

            auto Status = BootServices.WaitForEvent(1, Events, &Index);

            if (UEFI::IsError(Status))
            {
                return Status;
            }

            if (Tcp4_)
            {
                Tcp4_->Poll(Tcp4_);
            }
        }

        return CompletionStatus;
    }

    auto Tcp4Client::MoveFrom(Tcp4Client& Other) -> Foundation::Void
    {
        ControllerHandle_ = Other.ControllerHandle_;
        ChildHandle_ = Other.ChildHandle_;
        Tcp4ServiceBinding_ = Other.Tcp4ServiceBinding_;
        Tcp4_ = Other.Tcp4_;
        Connected_ = Other.Connected_;
        LastStatus_ = Other.LastStatus_;

        Other.ControllerHandle_ = nullptr;
        Other.ChildHandle_ = nullptr;
        Other.Tcp4ServiceBinding_ = nullptr;
        Other.Tcp4_ = nullptr;
        Other.Connected_ = false;
        Other.LastStatus_ = UEFI::StatusCode::Success;
    }
}