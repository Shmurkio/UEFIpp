#include "Tcp4Client.hpp"

static constexpr auto TCP4_DEFAULT_RECEIVE_SIZE = EFI_PAGE_SIZE;

static auto ToEfiIpv4Address(const IPv4Address& Address) -> EFI_IPv4_ADDRESS
{
    EFI_IPv4_ADDRESS Result = {};
    Result.Addr[0] = Address.A();
    Result.Addr[1] = Address.B();
    Result.Addr[2] = Address.C();
    Result.Addr[3] = Address.D();
    return Result;
}

static void __cdecl Tcp4ClientTokenNotify([[maybe_unused]] EFI_EVENT Event, [[maybe_unused]] void* Context)
{
}

Tcp4Client::~Tcp4Client()
{
    Close();
}

auto Tcp4Client::CreateEvent(EfiEventNotifyFn* NotifyFunction, void* Context, EFI_EVENT& Event) -> bool
{
    Event = nullptr;
    auto Status = gBS->CreateEvent(0, TPL_CALLBACK, NotifyFunction, Context, &Event);
    return !EfiError(Status);
}

auto Tcp4Client::WaitForToken(EFI_EVENT Event, EFI_STATUS& CompletionStatus) -> bool
{
    if (!Event)
    {
        return false;
    }

    while (CompletionStatus == EFI_NOT_READY)
    {
        EFI_EVENT Events[1] = { Event };
        uint64_t Index = 0;

        auto Status = gBS->WaitForEvent(1, Events, &Index);

        if (EfiError(Status))
        {
            return false;
        }

        if (Tcp4_)
        {
            Tcp4_->Poll(Tcp4_);
        }
    }

    return !EfiError(CompletionStatus);
}

auto Tcp4Client::CreateChild() -> bool
{
    if (Tcp4_ || Tcp4Sb_ || ChildHandle_)
    {
        return true;
    }

    PEFI_HANDLE Handles = nullptr;
    uint64_t HandleCount = 0;

    auto Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiTcp4ServiceBindingProtocolGuid, nullptr, &HandleCount, &Handles);

    if (EfiError(Status) || !Handles || !HandleCount)
    {
        return false;
    }

    bool Success = false;

    for (uint64_t i = 0; i < HandleCount; ++i)
    {
        EFI_HANDLE Controller = Handles[i];
        PEFI_TCP4_SERVICE_BINDING_PROTOCOL ServiceBinding = nullptr;
        Status = gBS->OpenProtocol(Controller, &gEfiTcp4ServiceBindingProtocolGuid, Cast::To<void**>(&ServiceBinding), gImageHandle, nullptr, EFI_OPEN_PROTOCOL_GET_PROTOCOL);

        if (EfiError(Status) || !ServiceBinding)
        {
            continue;
        }

        EFI_HANDLE Child = nullptr;
        Status = ServiceBinding->CreateChild(Cast::To<PEFI_SERVICE_BINDING_PROTOCOL>(ServiceBinding), &Child);

        if (EfiError(Status) || !Child)
        {
            continue;
        }

        PEFI_TCP4_PROTOCOL Tcp4 = nullptr;

        Status = gBS->OpenProtocol(Child, &gEfiTcp4ProtocolGuid, Cast::To<void**>(&Tcp4), gImageHandle, nullptr, EFI_OPEN_PROTOCOL_GET_PROTOCOL);

        if (EfiError(Status) || !Tcp4)
        {
            ServiceBinding->DestroyChild(Cast::To<PEFI_SERVICE_BINDING_PROTOCOL>(ServiceBinding), Child);
            continue;
        }

        ControllerHandle_ = Controller;
        ChildHandle_ = Child;
        Tcp4Sb_ = ServiceBinding;
        Tcp4_ = Tcp4;
        Connected_ = false;

        Success = true;
        break;
    }

    if (Handles)
    {
		Memory::FreePool(Handles, false);
    }

    return Success;
}

auto Tcp4Client::DestroyChild() -> void
{
    if (Tcp4Sb_ && ChildHandle_)
    {
        Tcp4Sb_->DestroyChild(Cast::To<PEFI_SERVICE_BINDING_PROTOCOL>(Tcp4Sb_), ChildHandle_);
    }

    ControllerHandle_ = nullptr;
    ChildHandle_ = nullptr;
    Tcp4Sb_ = nullptr;
    Tcp4_ = nullptr;
    Connected_ = false;
}

auto Tcp4Client::Configure(const IPv4Address& RemoteAddress, uint16_t RemotePort) -> bool
{
    if (!Tcp4_)
    {
        return false;
    }

    EFI_TCP4_OPTION Options = {};
    Options.ReceiveBufferSize = 0;
    Options.SendBufferSize = 0;
    Options.MaxSynBackLog = 0;
    Options.ConnectionTimeout = 0;
    Options.DataRetries = 0;
    Options.FinTimeout = 0;
    Options.TimeWaitTimeout = 0;
    Options.KeepAliveProbes = 0;
    Options.KeepAliveTime = 0;
    Options.KeepAliveInterval = 0;
    Options.EnableNagle = true;
    Options.EnableTimeStamp = false;
    Options.EnableWindowScaling = false;
    Options.EnableSelectiveAck = false;
    Options.EnablePathMtuDiscovery = false;

    EFI_TCP4_CONFIG_DATA ConfigData = {};
    ConfigData.TypeOfService = 0;
    ConfigData.TimeToLive = 64;

    ConfigData.AccessPoint.UseDefaultAddress = true;
    ConfigData.AccessPoint.StationAddress = {};
    ConfigData.AccessPoint.SubnetMask = {};
    ConfigData.AccessPoint.StationPort = 0;
    ConfigData.AccessPoint.RemoteAddress = ToEfiIpv4Address(RemoteAddress);
    ConfigData.AccessPoint.RemotePort = RemotePort;
    ConfigData.AccessPoint.ActiveFlag = true;

    ConfigData.ControlOption = &Options;

    auto Status = Tcp4_->Configure(Tcp4_, &ConfigData);

    if (Status == EFI_NO_MAPPING)
    {
        for (uint32_t Attempt = 0; Attempt < 500 && Status == EFI_NO_MAPPING; ++Attempt)
        {
            Tcp4_->Poll(Tcp4_);
            gBS->Stall(10000);
            Status = Tcp4_->Configure(Tcp4_, &ConfigData);
        }
    }

    return !EfiError(Status);
}

auto Tcp4Client::Connect(const IPv4Address& RemoteAddress, uint16_t RemotePort) -> bool
{
    Close();

    if (!CreateChild())
    {
        return false;
    }

    if (!Configure(RemoteAddress, RemotePort))
    {
        Close();
        return false;
    }

    EFI_EVENT Event = nullptr;

    if (!CreateEvent(nullptr, nullptr, Event))
    {
        Close();
        return false;
    }

    EFI_TCP4_CONNECTION_TOKEN Token = {};
    Token.CompletionToken.Event = Event;
    Token.CompletionToken.Status = EFI_NOT_READY;

    auto Status = Tcp4_->Connect(Tcp4_, &Token);

    if (EfiError(Status))
    {
        gBS->CloseEvent(Event);
        Close();
        return false;
    }

    auto Success = WaitForToken(Event, Token.CompletionToken.Status);

    gBS->CloseEvent(Event);

    if (!Success)
    {
        Close();
        return false;
    }

    Connected_ = true;
    return true;
}

auto Tcp4Client::Send(const Vector<uint8_t>& Data) -> bool
{
    if (!Tcp4_ || !Connected_ || !Data.Size())
    {
        return false;
    }

    uint8_t* Payload = nullptr;
    
    if (!Memory::AllocatePool(Payload, Data.Size(), false, true) || !Payload)
    {
        return false;
    }

    for (uint64_t i = 0; i < Data.Size(); ++i)
    {
        Payload[i] = Data[i];
    }

    auto TxSize = sizeof(EFI_TCP4_TRANSMIT_DATA) + (sizeof(EFI_TCP4_FRAGMENT_DATA) * 0);

    PEFI_TCP4_TRANSMIT_DATA TxData = nullptr;
    
    if (!Memory::AllocatePool(TxData, TxSize, false, true) || !TxData)
    {
        Memory::FreePool(Payload, false);
        return false;
    }

    TxData->Push = true;
    TxData->Urgent = false;
    TxData->DataLength = static_cast<uint32_t>(Data.Size());
    TxData->FragmentCount = 1;
    TxData->FragmentTable[0].FragmentLength = static_cast<uint32_t>(Data.Size());
    TxData->FragmentTable[0].FragmentBuffer = Payload;

    EFI_EVENT Event = nullptr;

    if (!CreateEvent(nullptr, nullptr, Event))
    {
        Memory::FreePool(TxData, false);
        Memory::FreePool(Payload, false);
        return false;
    }

    EFI_TCP4_IO_TOKEN Token = {};
    Token.CompletionToken.Event = Event;
    Token.CompletionToken.Status = EFI_NOT_READY;
    Token.Packet.TxData = TxData;

    auto Status = Tcp4_->Transmit(Tcp4_, &Token);

    if (EfiError(Status))
    {
        gBS->CloseEvent(Event);
        Memory::FreePool(TxData, false);
        Memory::FreePool(Payload, false);
        return false;
    }

    bool Success = WaitForToken(Event, Token.CompletionToken.Status);

    gBS->CloseEvent(Event);
    Memory::FreePool(TxData, false);
    Memory::FreePool(Payload, false);

    return Success;
}

auto Tcp4Client::Receive(Vector<uint8_t>& Buffer, uint32_t& ReceivedSize) -> bool
{
    ReceivedSize = 0;

    if (!Tcp4_ || !Connected_)
    {
        return false;
    }

    uint8_t* Payload = nullptr;
    
    if (!Memory::AllocatePool(Payload, TCP4_DEFAULT_RECEIVE_SIZE, false, true) || !Payload)
    {
        return false;
    }

    auto RxSize = sizeof(EFI_TCP4_RECEIVE_DATA) + (sizeof(EFI_TCP4_FRAGMENT_DATA) * 0);

    PEFI_TCP4_RECEIVE_DATA RxData = nullptr;

    if (!Memory::AllocatePool(RxData, RxSize, false, true) || !RxData)
    {
        Memory::FreePool(Payload, false);
        return false;
    }

    RxData->UrgentFlag = false;
    RxData->DataLength = TCP4_DEFAULT_RECEIVE_SIZE;
    RxData->FragmentCount = 1;
    RxData->FragmentTable[0].FragmentLength = TCP4_DEFAULT_RECEIVE_SIZE;
    RxData->FragmentTable[0].FragmentBuffer = Payload;

    EFI_EVENT Event = nullptr;

    if (!CreateEvent(nullptr, nullptr, Event))
    {
		Memory::FreePool(RxData, false);
		Memory::FreePool(Payload, false);
        return false;
    }

    EFI_TCP4_IO_TOKEN Token = {};
    Token.CompletionToken.Event = Event;
    Token.CompletionToken.Status = EFI_NOT_READY;
    Token.Packet.RxData = RxData;

    auto Status = Tcp4_->Receive(Tcp4_, &Token);

    if (EfiError(Status))
    {
        gBS->CloseEvent(Event);
		Memory::FreePool(RxData, false);
		Memory::FreePool(Payload, false);
        return false;
    }

    auto Success = WaitForToken(Event, Token.CompletionToken.Status);

    if (Success)
    {
        ReceivedSize = RxData->DataLength;
        Buffer.Clear();

        for (uint32_t i = 0; i < ReceivedSize; ++i)
        {
            Buffer.PushBack(Payload[i]);
        }
    }

    gBS->CloseEvent(Event);
    Memory::FreePool(RxData, false);
    Memory::FreePool(Payload, false);

    return Success;
}

auto Tcp4Client::Close() -> void
{
    if (Tcp4_)
    {
        EFI_EVENT Event = nullptr;

        if (CreateEvent(nullptr, nullptr, Event))
        {
            EFI_TCP4_CLOSE_TOKEN Token = {};
            Token.CompletionToken.Event = Event;
            Token.CompletionToken.Status = EFI_NOT_READY;
            Token.AbortOnClose = false;

            auto Status = Tcp4_->Close(Tcp4_, &Token);

            if (!EfiError(Status))
            {
                WaitForToken(Event, Token.CompletionToken.Status);
            }

            gBS->CloseEvent(Event);
        }

        Tcp4_->Configure(Tcp4_, nullptr);
    }

    DestroyChild();
}