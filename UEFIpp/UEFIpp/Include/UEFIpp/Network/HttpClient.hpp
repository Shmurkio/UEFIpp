#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Functional/Event.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Library/String/StringView.hpp>
#include <UEFIpp/Network/IPv4Address.hpp>
#include <UEFIpp/Network/Tcp4Client.hpp>
#include <UEFIpp/Network/HttpResponse.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/Network/Url.hpp>
#include <UEFIpp/Network/HttpRedirect.hpp>

namespace UEFIpp::Network
{
    class HttpClient
    {
    public:
        Library::Event<> OnConnected{};
        Library::Event<> OnDisconnected{};
        Library::Event<UEFI::StatusCode> OnError{};
        Library::Event<Library::StringView> OnRequestSent{};
        Library::Event<Library::StringView> OnRawResponseReceived{};
        Library::Event<const HttpResponse&> OnResponseReceived{};
        Library::Event<const HttpRedirect&> OnRedirected{};
        Library::Event<Library::StringView> OnResolvingHost{};
        Library::Event<const IPv4Address&> OnHostResolved{};
        Library::Event<Library::StringView> OnRequestStarted{};
        Library::Event<Library::StringView> OnRequestFinished{};

    public:
        HttpClient() = default;

        static constexpr Foundation::Uint32 DefaultMaxRedirects = 5;

        [[nodiscard]] auto Get(const IPv4Address& Address, Foundation::Uint16 Port, Library::StringView Host, Library::StringView Path, HttpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto Get(const IPv4Address& Address, Library::StringView Host, Library::StringView Path, HttpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto Get(Library::StringView Host, Library::StringView Path, HttpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto Get(const Url& Url, HttpResponse& Response) -> Foundation::Bool;

        [[nodiscard]] auto GetRedirected(const IPv4Address& Address, Foundation::Uint16 Port, Library::StringView Host, Library::StringView Path, HttpResponse& Response, Foundation::Uint32 MaxRedirects = DefaultMaxRedirects) -> Foundation::Bool;
        [[nodiscard]] auto GetRedirected(const IPv4Address& Address, Library::StringView Host, Library::StringView Path, HttpResponse& Response, Foundation::Uint32 MaxRedirects = DefaultMaxRedirects) -> Foundation::Bool;
        [[nodiscard]] auto GetRedirected(Library::StringView Host, Library::StringView Path, HttpResponse& Response, Foundation::Uint32 MaxRedirects = DefaultMaxRedirects) -> Foundation::Bool;
        [[nodiscard]] auto GetRedirected(const Url& Url, HttpResponse& Response, Foundation::Uint32 MaxRedirects = DefaultMaxRedirects) -> Foundation::Bool;

        [[nodiscard]] auto Head(const IPv4Address& Address, Foundation::Uint16 Port, Library::StringView Host, Library::StringView Path, HttpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto Head(const IPv4Address& Address, Library::StringView Host, Library::StringView Path, HttpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto Head(Library::StringView Host, Library::StringView Path, HttpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto Head(const Url& Url, HttpResponse& Response) -> Foundation::Bool;

        [[nodiscard]] auto Post(const IPv4Address& Address, Foundation::Uint16 Port, Library::StringView Host, Library::StringView Path, Library::StringView Body, HttpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto Post(const IPv4Address& Address, Library::StringView Host, Library::StringView Path, Library::StringView Body, HttpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto Post(Library::StringView Host, Library::StringView Path, Library::StringView Body, HttpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto Post(const Url& Url, Library::StringView Body, HttpResponse& Response) -> Foundation::Bool;

        [[nodiscard]] auto Put(const IPv4Address& Address, Foundation::Uint16 Port, Library::StringView Host, Library::StringView Path, Library::StringView Body, HttpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto Put(const IPv4Address& Address, Library::StringView Host, Library::StringView Path, Library::StringView Body, HttpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto Put(Library::StringView Host, Library::StringView Path, Library::StringView Body, HttpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto Put(const Url& Url, Library::StringView Body, HttpResponse& Response) -> Foundation::Bool;

        [[nodiscard]] auto Delete(const IPv4Address& Address, Foundation::Uint16 Port, Library::StringView Host, Library::StringView Path, HttpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto Delete(const IPv4Address& Address, Library::StringView Host, Library::StringView Path, HttpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto Delete(Library::StringView Host, Library::StringView Path, HttpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto Delete(const Url& Url, HttpResponse& Response) -> Foundation::Bool;

        [[nodiscard]] constexpr auto LastStatus() const -> UEFI::StatusCode
        {
            return LastStatus_;
        }

    private:
        [[nodiscard]] auto BuildRequest(Library::StringView Method, Library::StringView Host, Library::StringView Path, const HttpHeaders& Headers, Library::StringView Body = {}) -> Library::String;
        [[nodiscard]] auto Request(Library::StringView Method, const IPv4Address& Address, Foundation::Uint16 Port, Library::StringView Host, Library::StringView Path, HttpResponse& Response, const HttpHeaders& Headers = {}, Library::StringView Body = {}) -> Foundation::Bool;
        [[nodiscard]] auto ParseResponse(const Library::String& Raw, HttpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto DecodeChunkedBody(Library::StringView Body, Library::String& Decoded) -> Foundation::Bool;
        [[nodiscard]] auto IsChunked(const HttpResponse& Response) const -> Foundation::Bool;
        [[nodiscard]] auto ResolveHost(Library::StringView Host, IPv4Address& Address) -> Foundation::Bool;
        [[nodiscard]] auto PostJson(Library::StringView Host, Library::StringView Path, Library::StringView Json, HttpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto PostJson(const Url& Url, Library::StringView Json, HttpResponse& Response) -> Foundation::Bool;
        [[nodiscard]] auto DefaultHeaders() const -> HttpHeaders;

    private:
        Tcp4Client Tcp_{};
        UEFI::StatusCode LastStatus_{ UEFI::StatusCode::Success };
    };
}