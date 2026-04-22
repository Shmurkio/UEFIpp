#pragma once

#include "../../Vector/Vector.hpp"
#include "../IPv4/IPv4.hpp"
#include "../../../UEFIpp.hpp"

class Tcp4Client
{
public:
    Tcp4Client() = default;
    ~Tcp4Client();

    Tcp4Client(const Tcp4Client&) = delete;
    auto operator=(const Tcp4Client&) -> Tcp4Client & = delete;
    Tcp4Client(Tcp4Client&&) = delete;
    auto operator=(Tcp4Client&&) -> Tcp4Client & = delete;

    auto Connect(const IPv4Address& RemoteAddress, uint16_t RemotePort) -> bool;
    auto Send(const Vector<uint8_t>& Data) -> bool;
    auto Receive(Vector<uint8_t>& Buffer, uint32_t& ReceivedSize) -> bool;
    auto Close() -> void;

    auto Connected() const -> bool { return Connected_; }

private:
    auto Configure(const IPv4Address& RemoteAddress, uint16_t RemotePort) -> bool;
    auto CreateChild() -> bool;
    auto DestroyChild() -> void;
    auto CreateEvent(EfiEventNotifyFn* NotifyFunction, void* Context, EFI_EVENT& Event) -> bool;
    auto WaitForToken(EFI_EVENT Event, EFI_STATUS& CompletionStatus) -> bool;

private:
    EFI_HANDLE ControllerHandle_ = nullptr;
    EFI_HANDLE ChildHandle_ = nullptr;

    PEFI_TCP4_SERVICE_BINDING_PROTOCOL Tcp4Sb_ = nullptr;
    PEFI_TCP4_PROTOCOL Tcp4_ = nullptr;

    bool Connected_ = false;
};