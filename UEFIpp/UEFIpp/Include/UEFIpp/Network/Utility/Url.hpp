#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Library/String/StringView.hpp>

namespace UEFIpp::Network
{
    enum class UrlScheme : Foundation::Uint8
    {
        Unknown,
        Http,
        Https
    };

    class Url
    {
    public:
        Url() = default;

        explicit Url(Library::StringView Text)
        {
            Parse(Text);
        }

        [[nodiscard]] auto Parse(Library::StringView Text) -> Foundation::Bool
        {
            Clear();

            auto Rest = Text;

            const auto SchemeEnd = Rest.Find("://");

            if (SchemeEnd != Library::StringView::NotFound)
            {
                const auto SchemeText = Rest.Substr(0, SchemeEnd);

                if (SchemeText == "http")
                {
                    Scheme_ = UrlScheme::Http;
                    Port_ = 80;
                }
                else if (SchemeText == "https")
                {
                    Scheme_ = UrlScheme::Https;
                    Port_ = 443;
                }
                else
                {
                    return false;
                }

                Rest = Rest.Substr(SchemeEnd + 3);
            }
            else
            {
                Scheme_ = UrlScheme::Http;
                Port_ = 80;
            }

            const auto PathStart = Rest.Find("/");
            Library::StringView Authority{};

            if (PathStart == Library::StringView::NotFound)
            {
                Authority = Rest;
                Path_ = "/";
            }
            else
            {
                Authority = Rest.Substr(0, PathStart);
                Path_ = Rest.Substr(PathStart);
            }

            if (Authority.Empty())
            {
                Clear();
                return false;
            }

            const auto PortStart = Authority.Find(":");

            if (PortStart == Library::StringView::NotFound)
            {
                Host_ = Authority;
            }
            else
            {
                Host_ = Authority.Substr(0, PortStart);

                if (Host_.Empty())
                {
                    Clear();
                    return false;
                }

                Foundation::Uint16 Port{};
                const auto PortText = Authority.Substr(PortStart + 1);

                if (PortText.Empty())
                {
                    Clear();
                    return false;
                }

                for (const auto Ch : PortText)
                {
                    if (Ch < '0' || Ch > '9')
                    {
                        Clear();
                        return false;
                    }

                    const auto Digit = Foundation::Cast::Auto<Foundation::Uint16>(Ch - '0');

                    if (Port > 6553 || (Port == 6553 && Digit > 5))
                    {
                        Clear();
                        return false;
                    }

                    Port = Foundation::Cast::Auto<Foundation::Uint16>(Port * 10 + Digit);
                }

                Port_ = Port;
            }

            if (Path_.Empty())
            {
                Path_ = "/";
            }

            Valid_ = true;

            return true;
        }

        auto Clear() -> Foundation::Void
        {
            Scheme_ = UrlScheme::Unknown;
            Host_.Clear();
            Path_.Clear();
            Port_ = 0;
            Valid_ = false;
        }

        [[nodiscard]] constexpr auto Valid() const -> Foundation::Bool
        {
            return Valid_;
        }

        [[nodiscard]] constexpr explicit operator Foundation::Bool() const
        {
            return Valid();
        }

        [[nodiscard]] constexpr auto Scheme() const -> UrlScheme
        {
            return Scheme_;
        }

        [[nodiscard]] auto SchemeText() const -> Library::StringView
        {
            switch (Scheme_)
            {
            case UrlScheme::Http:
            {
                return "http";
            }
            case UrlScheme::Https:
            {
                return "https";
            }
            default:
            {
                return {};
            }
            }
        }

        [[nodiscard]] constexpr auto Secure() const -> Foundation::Bool
        {
            return Scheme_ == UrlScheme::Https;
        }

        [[nodiscard]] auto Host() const -> Library::StringView
        {
            return Host_.View();
        }

        [[nodiscard]] auto Path() const -> Library::StringView
        {
            return Path_.View();
        }

        [[nodiscard]] constexpr auto Port() const -> Foundation::Uint16
        {
            return Port_;
        }

    private:
        UrlScheme Scheme_{ UrlScheme::Unknown };
        Library::String Host_{};
        Library::String Path_{};
        Foundation::Uint16 Port_{};
        Foundation::Bool Valid_{};
    };
}