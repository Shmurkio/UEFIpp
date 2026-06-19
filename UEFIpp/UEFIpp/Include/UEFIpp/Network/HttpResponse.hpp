#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Library/String/StringView.hpp>
#include <UEFIpp/Network/HttpHeader.hpp>

namespace UEFIpp::Network
{
    class HttpResponse
    {
    public:
        Foundation::Uint32 StatusCode{};
        Library::String ReasonPhrase{};
        Library::Vector<HttpHeader> Headers{};
        Library::String Body{};
        Library::String Raw{};

    public:
        [[nodiscard]]
        constexpr auto Ok() const -> Foundation::Bool
        {
            return StatusCode >= 200 && StatusCode < 300;
        }

        [[nodiscard]] auto Header(Library::StringView Name) const -> Library::StringView
        {
            for (const auto& Header : Headers)
            {
                if (Header.Name.View() == Name)
                {
                    return Header.Value.View();
                }
            }

            return {};
        }

        [[nodiscard]] auto HasHeader(Library::StringView Name) const -> Foundation::Bool
        {
            return !Header(Name).Empty();
        }

        [[nodiscard]] auto ContentType() const -> Library::StringView
        {
            return Header("Content-Type");
        }

        [[nodiscard]] auto Server() const -> Library::StringView
        {
            return Header("Server");
        }

        [[nodiscard]] auto ContentLength() const -> Library::StringView
        {
            return Header("Content-Length");
        }

        [[nodiscard]] auto ContentEncoding() const -> Library::StringView
        {
            return Header("Content-Encoding");
        }

        [[nodiscard]] auto TransferEncoding() const -> Library::StringView
        {
            return Header("Transfer-Encoding");
        }

        [[nodiscard]] auto Location() const -> Library::StringView
        {
            return Header("Location");
        }

        [[nodiscard]] auto Date() const -> Library::StringView
        {
            return Header("Date");
        }

        [[nodiscard]] auto LastModified() const -> Library::StringView
        {
            return Header("Last-Modified");
        }

        [[nodiscard]] auto ETag() const -> Library::StringView
        {
            return Header("ETag");
        }

        [[nodiscard]] constexpr auto Informational() const -> Foundation::Bool
        {
            return StatusCode >= 100 && StatusCode < 200;
        }

        [[nodiscard]] constexpr auto Success() const -> Foundation::Bool
        {
            return StatusCode >= 200 && StatusCode < 300;
        }

        [[nodiscard]] constexpr auto Redirect() const -> Foundation::Bool
        {
            return StatusCode >= 300 && StatusCode < 400;
        }

        [[nodiscard]] constexpr auto ClientError() const -> Foundation::Bool
        {
            return StatusCode >= 400 && StatusCode < 500;
        }

        [[nodiscard]] constexpr auto ServerError() const -> Foundation::Bool
        {
            return StatusCode >= 500;
        }

        [[nodiscard]] auto ContentTypeWithoutParameters() const -> Library::StringView
        {
            const auto Type = ContentType();
            const auto Separator = Type.Find(";");

            if (Separator == Library::StringView::NotFound)
            {
                return Type;
            }

            return Type.Substr(0, Separator);
        }

        [[nodiscard]] constexpr auto HeaderCount() const -> Foundation::Size
        {
            return Headers.Size();
        }

        [[nodiscard]] auto Empty() const -> Foundation::Bool
        {
            return StatusCode == 0 && Headers.Empty() && Body.Empty() && Raw.Empty();
        }

        [[nodiscard]] auto operator[](Library::StringView Name) const -> Library::StringView
        {
            return Header(Name);
        }
    };
}