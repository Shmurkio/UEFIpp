#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Protocols/Tcp4.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Types.hpp>

namespace UEFIpp::Protocols
{
    class Udp4;

    struct Udp4ConfigData
    {
        Foundation::Bool AcceptBroadcast;
        Foundation::Bool AcceptPromiscuous;
        Foundation::Bool AcceptAnyPort;
        Foundation::Bool AllowDuplicatePort;
        Foundation::Uint8 TypeOfService;
        Foundation::Uint8 TimeToLive;
        Foundation::Bool DoNotFragment;
        Foundation::Uint32 ReceiveTimeout;
        Foundation::Uint32 TransmitTimeout;
        Foundation::Bool UseDefaultAddress;
        Ipv4RawAddress StationAddress;
        Ipv4RawAddress SubnetMask;
        Foundation::Uint16 StationPort;
        Ipv4RawAddress RemoteAddress;
        Foundation::Uint16 RemotePort;
    };

    struct Udp4CompletionToken
    {
        UEFI::Event Event;
        UEFI::StatusCode Status;
    };

    struct Udp4FragmentData
    {
        Foundation::Uint32 FragmentLength;
        Foundation::Void* FragmentBuffer;
    };

    struct Udp4SessionData
    {
        Ipv4RawAddress SourceAddress;
        Foundation::Uint16 SourcePort;
        Ipv4RawAddress DestinationAddress;
        Foundation::Uint16 DestinationPort;
    };

    struct Udp4ReceiveData
    {
        UEFI::Time TimeStamp;
        UEFI::Event RecycleSignal;
        Udp4SessionData UdpSession;
        Foundation::Uint32 DataLength;
        Foundation::Uint32 FragmentCount;
        Udp4FragmentData FragmentTable[1];
    };

    struct Udp4TransmitData
    {
        Udp4SessionData* UdpSessionData;
        Ipv4RawAddress* GatewayAddress;
        Foundation::Uint32 DataLength;
        Foundation::Uint32 FragmentCount;
        Udp4FragmentData FragmentTable[1];
    };

    struct Udp4IoToken
    {
        Udp4CompletionToken CompletionToken;

        union
        {
            Udp4ReceiveData* RxData;
            Udp4TransmitData* TxData;
        } Packet;
    };

    using Udp4GetModeDataFn = UEFI::StatusCode(*)(Udp4* This, Udp4ConfigData* Udp4ConfigData, IpAddress* Ip4ModeData, ManagedNetworkConfigData* MnpConfigData, SimpleNetworkMode* SnpModeData);
    using Udp4ConfigureFn = UEFI::StatusCode(*)(Udp4* This, Udp4ConfigData* Udp4ConfigData);
    using Udp4GroupsFn = UEFI::StatusCode(*)(Udp4* This, Foundation::Bool JoinFlag, Ipv4RawAddress* MulticastAddress);
    using Udp4RoutesFn = UEFI::StatusCode(*)(Udp4* This, Foundation::Bool DeleteRoute, Ipv4RawAddress* SubnetAddress, Ipv4RawAddress* SubnetMask, Ipv4RawAddress* GatewayAddress);
    using Udp4TransmitFn = UEFI::StatusCode(*)(Udp4* This, Udp4IoToken* Token);
    using Udp4ReceiveFn = UEFI::StatusCode(*)(Udp4* This, Udp4IoToken* Token);
    using Udp4CancelFn = UEFI::StatusCode(*)(Udp4* This, Udp4CompletionToken* Token);
    using Udp4PollFn = UEFI::StatusCode(*)(Udp4* This);

    class Udp4
    {
    public:
        Udp4GetModeDataFn GetModeData;
        Udp4ConfigureFn Configure;
        Udp4GroupsFn Groups;
        Udp4RoutesFn Routes;
        Udp4TransmitFn Transmit;
        Udp4ReceiveFn Receive;
        Udp4CancelFn Cancel;
        Udp4PollFn Poll;
    };

    static_assert(sizeof(Udp4FragmentData) == 16);
    static_assert(sizeof(Udp4SessionData) == 12);
    static_assert(sizeof(Udp4CompletionToken) == 16);
    static_assert(sizeof(Udp4IoToken) == 24);
}