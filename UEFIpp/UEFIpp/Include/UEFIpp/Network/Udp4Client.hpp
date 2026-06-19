#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/Functional/Event.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Library/String/StringView.hpp>
#include <UEFIpp/Network/IPv4Address.hpp>
#include <UEFIpp/Protocols/ServiceBinding.hpp>
#include <UEFIpp/Protocols/Udp4.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Types.hpp>

namespace UEFIpp::Network
{
    class Udp4Client
    {
    public:
        using ByteSpan = Library::Span<const Foundation::Byte>;

    public:
        Library::Event<> OnOpened{};
        Library::Event<> OnClosed{};
        Library::Event<UEFI::StatusCode> OnError{};
        Library::Event<ByteSpan> OnDataSent{};
        Library::Event<ByteSpan> OnDataReceived{};
        Library::Event<Library::StringView> OnTextSent{};
        Library::Event<Library::StringView> OnTextReceived{};

    public:
        Udp4Client() = default;
        ~Udp4Client();

        Udp4Client(const Udp4Client&) = delete;
        auto operator=(const Udp4Client&) -> Udp4Client & = delete;

        Udp4Client(Udp4Client&& Other) noexcept;
        auto operator=(Udp4Client&& Other) noexcept -> Udp4Client&;

        [[nodiscard]] auto Open(Foundation::Uint16 LocalPort = 0) -> Foundation::Bool;
        [[nodiscard]] auto SendTo(const IPv4Address& Address, Foundation::Uint16 Port, ByteSpan Data) -> Foundation::Bool;
        [[nodiscard]] auto SendTo(const IPv4Address& Address, Foundation::Uint16 Port, Library::StringView Text) -> Foundation::Bool;
        [[nodiscard]] auto ReceiveFrom(IPv4Address& Address, Foundation::Uint16& Port, Library::Vector<Foundation::Byte>& Data, Foundation::Size Capacity = 4096) -> Foundation::Bool;
        [[nodiscard]] auto ReceiveFrom(IPv4Address& Address, Foundation::Uint16& Port, Library::String& Text, Foundation::Size Capacity = 4096) -> Foundation::Bool;

        auto Close() -> Foundation::Void;

        [[nodiscard]] constexpr auto Opened() const -> Foundation::Bool
        {
            return Opened_;
        }

        [[nodiscard]] constexpr auto LastStatus() const -> UEFI::StatusCode
        {
            return LastStatus_;
        }

    private:
        [[nodiscard]] auto CreateChild() -> UEFI::StatusCode;
        auto DestroyChild() -> Foundation::Void;
        [[nodiscard]] auto Configure(Foundation::Uint16 LocalPort) -> UEFI::StatusCode;
        [[nodiscard]] auto CreateTokenEvent(UEFI::Event& Event) -> UEFI::StatusCode;
        [[nodiscard]] auto WaitForToken(UEFI::Event Event, UEFI::StatusCode& CompletionStatus) -> UEFI::StatusCode;
        auto MoveFrom(Udp4Client& Other) -> Foundation::Void;

    private:
        UEFI::Handle ControllerHandle_{};
        UEFI::Handle ChildHandle_{};
        Protocols::ServiceBinding* Udp4ServiceBinding_{};
        Protocols::Udp4* Udp4_{};
        Foundation::Bool Opened_{};
        UEFI::StatusCode LastStatus_{ UEFI::StatusCode::Success };
    };
}