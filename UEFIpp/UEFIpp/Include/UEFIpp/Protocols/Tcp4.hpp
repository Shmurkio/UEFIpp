#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Types.hpp>

namespace UEFIpp::Protocols
{
    class Tcp4;

    struct Ipv4RawAddress
    {
        Foundation::Uint8 Addr[4];
    };

    struct IpAddress
    {
        union
        {
            Foundation::Uint32 Addr[4];
            Ipv4RawAddress V4;
        };
    };

    struct ManagedNetworkConfigData
    {
        Foundation::Uint32 ReceivedQueueTimeoutValue;
        Foundation::Uint32 TransmitQueueTimeoutValue;
        Foundation::Uint16 ProtocolTypeFilter;
        Foundation::Bool EnableUnicastReceive;
        Foundation::Bool EnableMulticastReceive;
        Foundation::Bool EnableBroadcastReceive;
        Foundation::Bool EnablePromiscuousReceive;
        Foundation::Bool FlushQueuesOnReset;
        Foundation::Bool EnableReceiveTimestamps;
        Foundation::Bool DisableBackgroundPolling;
    };

    struct SimpleNetworkMode;

    struct Tcp4AccessPoint
    {
        Foundation::Bool UseDefaultAddress;
        Ipv4RawAddress StationAddress;
        Ipv4RawAddress SubnetMask;
        Foundation::Uint16 StationPort;
        Ipv4RawAddress RemoteAddress;
        Foundation::Uint16 RemotePort;
        Foundation::Bool ActiveFlag;
    };

    struct Tcp4Option
    {
        Foundation::Uint32 ReceiveBufferSize;
        Foundation::Uint32 SendBufferSize;
        Foundation::Uint32 MaxSynBackLog;
        Foundation::Uint32 ConnectionTimeout;
        Foundation::Uint32 DataRetries;
        Foundation::Uint32 FinTimeout;
        Foundation::Uint32 TimeWaitTimeout;
        Foundation::Uint32 KeepAliveProbes;
        Foundation::Uint32 KeepAliveTime;
        Foundation::Uint32 KeepAliveInterval;
        Foundation::Bool EnableNagle;
        Foundation::Bool EnableTimeStamp;
        Foundation::Bool EnableWindowScaling;
        Foundation::Bool EnableSelectiveAck;
        Foundation::Bool EnablePathMtuDiscovery;
    };

    struct Tcp4ConfigData
    {
        Foundation::Uint8 TypeOfService;
        Foundation::Uint8 TimeToLive;
        Tcp4AccessPoint AccessPoint;
        Tcp4Option* ControlOption;
    };

    struct Tcp4CompletionToken
    {
        UEFI::Event Event;
        UEFI::StatusCode Status;
    };

    struct Tcp4ConnectionToken
    {
        Tcp4CompletionToken CompletionToken;
    };

    struct Tcp4CloseToken
    {
        Tcp4CompletionToken CompletionToken;
        Foundation::Bool AbortOnClose;
    };

    struct Tcp4FragmentData
    {
        Foundation::Uint32 FragmentLength;
        Foundation::Void* FragmentBuffer;
    };

    struct Tcp4TransmitData
    {
        Foundation::Bool Push;
        Foundation::Bool Urgent;
        Foundation::Uint32 DataLength;
        Foundation::Uint32 FragmentCount;
        Tcp4FragmentData FragmentTable[1];
    };

    struct Tcp4ReceiveData
    {
        Foundation::Bool UrgentFlag;
        Foundation::Uint32 DataLength;
        Foundation::Uint32 FragmentCount;
        Tcp4FragmentData FragmentTable[1];
    };

    struct Tcp4IoToken
    {
        Tcp4CompletionToken CompletionToken;
        union
        {
            Tcp4ReceiveData* RxData;
            Tcp4TransmitData* TxData;
        } Packet;
    };

    using Tcp4GetModeDataFn = UEFI::StatusCode(*)(Tcp4* This, Tcp4ConfigData* Tcp4ConfigData, IpAddress* Ip4ModeData, ManagedNetworkConfigData* MnpConfigData, SimpleNetworkMode* SnpModeData);
    using Tcp4ConfigureFn = UEFI::StatusCode(*)(Tcp4* This, Tcp4ConfigData* Tcp4ConfigData);
    using Tcp4RoutesFn = UEFI::StatusCode(*)(Tcp4* This, Foundation::Bool DeleteRoute, Ipv4RawAddress* SubnetAddress, Ipv4RawAddress* SubnetMask, Ipv4RawAddress* GatewayAddress);
    using Tcp4ConnectFn = UEFI::StatusCode(*)(Tcp4* This, Tcp4ConnectionToken* ConnectionToken);
    using Tcp4AcceptFn = UEFI::StatusCode(*)(Tcp4* This, Tcp4ConnectionToken* ListenToken);
    using Tcp4TransmitFn = UEFI::StatusCode(*)(Tcp4* This, Tcp4IoToken* Token);
    using Tcp4ReceiveFn = UEFI::StatusCode(*)(Tcp4* This, Tcp4IoToken* Token);
    using Tcp4CloseFn = UEFI::StatusCode(*)(Tcp4* This, Tcp4CloseToken* CloseToken);
    using Tcp4CancelFn = UEFI::StatusCode(*)(Tcp4* This, Tcp4CompletionToken* Token);
    using Tcp4PollFn = UEFI::StatusCode(*)(Tcp4* This);

    class Tcp4
    {
    public:
        Tcp4GetModeDataFn GetModeData;
        Tcp4ConfigureFn Configure;
        Tcp4RoutesFn Routes;
        Tcp4ConnectFn Connect;
        Tcp4AcceptFn Accept;
        Tcp4TransmitFn Transmit;
        Tcp4ReceiveFn Receive;
        Tcp4CloseFn Close;
        Tcp4CancelFn Cancel;
        Tcp4PollFn Poll;
    };
}