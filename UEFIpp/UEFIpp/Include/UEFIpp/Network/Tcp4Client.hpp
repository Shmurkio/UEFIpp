#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Network/IPv4Address.hpp>
#include <UEFIpp/Protocols/ServiceBinding.hpp>
#include <UEFIpp/Protocols/Tcp4.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Types.hpp>

namespace UEFIpp::Network
{
    class Tcp4Client
    {
    public:
        Tcp4Client() = default;
        ~Tcp4Client();

        Tcp4Client(const Tcp4Client&) = delete;
        auto operator=(const Tcp4Client&) -> Tcp4Client& = delete;

        Tcp4Client(Tcp4Client&& Other) noexcept;
        auto operator=(Tcp4Client&& Other) noexcept -> Tcp4Client&;

        [[nodiscard]] auto Connect(const IPv4Address& RemoteAddress, Foundation::Uint16 RemotePort) -> Foundation::Bool;
        [[nodiscard]]  auto Send(Library::Span<const Foundation::Byte> Data) -> Foundation::Bool;
        [[nodiscard]] auto Receive(Library::Vector<Foundation::Byte>& Buffer, Foundation::Size Capacity = 4096)  -> Foundation::Bool;
        auto Close(Foundation::Bool Abort = false) -> Foundation::Void;

        [[nodiscard]] constexpr auto Connected() const -> Foundation::Bool { return Connected_; }
        [[nodiscard]] constexpr auto LastStatus() const -> UEFI::StatusCode { return LastStatus_; }

    private:
        [[nodiscard]] auto CreateChild() -> UEFI::StatusCode;
        auto DestroyChild() -> Foundation::Void;
        [[nodiscard]] auto Configure(const IPv4Address& RemoteAddress, Foundation::Uint16 RemotePort) -> UEFI::StatusCode;
        [[nodiscard]] auto CreateTokenEvent(UEFI::Event& Event) -> UEFI::StatusCode;
        [[nodiscard]] auto WaitForToken(UEFI::Event Event, UEFI::StatusCode& CompletionStatus) -> UEFI::StatusCode;
        auto MoveFrom(Tcp4Client& Other) -> Foundation::Void;

    private:
        UEFI::Handle ControllerHandle_{};
        UEFI::Handle ChildHandle_{};
        Protocols::ServiceBinding* Tcp4ServiceBinding_{};
        Protocols::Tcp4* Tcp4_{};
        Foundation::Bool Connected_{};
        UEFI::StatusCode LastStatus_{ UEFI::StatusCode::Success };
    };
}