#include <UEFIpp/Network/HTTP/HttpClient.hpp>
#include <UEFIpp/Network/DNS/DnsClient.hpp>

namespace
{
    auto AppendDecimal(UEFIpp::Library::String& Text, UEFIpp::Foundation::Size Value) -> void
    {
        UEFIpp::Foundation::Char Buffer[32]{};
        UEFIpp::Foundation::Size Index = sizeof(Buffer);

        do
        {
            Buffer[--Index] = UEFIpp::Foundation::Cast::Auto<UEFIpp::Foundation::Char>('0' + (Value % 10));
            Value /= 10;
        } while (Value);

        while (Index < sizeof(Buffer))
        {
            Text += Buffer[Index++];
        }
    }
}

namespace UEFIpp::Network
{
    auto HttpClient::Get(const IPv4Address& Address, Library::StringView Host,  Library::StringView Path, HttpResponse& Response) -> Foundation::Bool
    {
        return Get(Address, 80, Host, Path, Response);
    }

    auto HttpClient::Get(Library::StringView Host, Library::StringView Path, HttpResponse& Response) -> Foundation::Bool
    {
        IPv4Address Address{};

        if (!ResolveHost(Host, Address))
        {
            return false;
        }

        return Get(Address, 80, Host, Path, Response);
    }

    auto HttpClient::Get(const Url& RequestUrl, HttpResponse& Response) -> Foundation::Bool
    {
        if (!RequestUrl.Valid() || RequestUrl.Scheme() != UrlScheme::Http)
        {
            LastStatus_ = UEFI::StatusCode::Unsupported;
            OnError(LastStatus_);
            return false;
        }

        return Get(RequestUrl.Host(), RequestUrl.Path(), Response);
    }

    auto HttpClient::Get(const IPv4Address& Address, Foundation::Uint16 Port, Library::StringView Host, Library::StringView Path, HttpResponse& Response) -> Foundation::Bool
    {
        return Request("GET", Address, Port, Host, Path, Response, DefaultHeaders());
    }

    auto HttpClient::GetRedirected(const IPv4Address& Address, Foundation::Uint16 Port, Library::StringView Host, Library::StringView Path, HttpResponse& Response, Foundation::Uint32 MaxRedirects) -> Foundation::Bool
    {
        if (!Get(Address, Port, Host, Path, Response))
        {
            return false;
        }

        for (Foundation::Uint32 Redirect{}; Redirect < MaxRedirects && Response.Redirect(); ++Redirect)
        {
            const auto Location = Response.Location();

            if (Location.Empty())
            {
                LastStatus_ = UEFI::StatusCode::NotFound;
                OnError(LastStatus_);
                return false;
            }

            Url Next{};

            if (Location.Find("://") != Library::StringView::NotFound)
            {
                if (!Next.Parse(Location))
                {
                    LastStatus_ = UEFI::StatusCode::InvalidParameter;
                    OnError(LastStatus_);
                    return false;
                }
            }
            else
            {
                Library::String Text{};
                Text += "http://";
                Text += Host;

                if (!Location.Empty() && Location[0] != '/')
                {
                    Text += "/";
                }

                Text += Location;

                if (!Next.Parse(Text.View()))
                {
                    LastStatus_ = UEFI::StatusCode::InvalidParameter;
                    OnError(LastStatus_);
                    return false;
                }
            }

            if (!Get(Next, Response))
            {
                return false;
            }
        }

        if (Response.Redirect())
        {
            LastStatus_ = UEFI::StatusCode::Timeout;
            OnError(LastStatus_);
            return false;
        }

        return true;
    }

    auto HttpClient::GetRedirected(const IPv4Address& Address, Library::StringView Host, Library::StringView Path, HttpResponse& Response, Foundation::Uint32 MaxRedirects) -> Foundation::Bool
    {
        return GetRedirected(Address, 80, Host, Path, Response, MaxRedirects);
    }

    auto HttpClient::GetRedirected(Library::StringView Host, Library::StringView Path, HttpResponse& Response, Foundation::Uint32 MaxRedirects) -> Foundation::Bool
    {
        Url Url{};

        Library::String Text{};
        Text += "http://";
        Text += Host;

        if (Path.Empty() || Path[0] != '/')
        {
            Text += "/";
        }

        Text += Path.Empty() ? "/" : Path;

        if (!Url.Parse(Text.View()))
        {
            LastStatus_ = UEFI::StatusCode::InvalidParameter;
            OnError(LastStatus_);
            return false;
        }

        return GetRedirected(Url, Response, MaxRedirects);
    }

    auto HttpClient::GetRedirected(const Url& RequestUrl, HttpResponse& Response, Foundation::Uint32 MaxRedirects) -> Foundation::Bool
    {
        if (!RequestUrl.Valid() || RequestUrl.Scheme() != UrlScheme::Http)
        {
            LastStatus_ = UEFI::StatusCode::Unsupported;
            OnError(LastStatus_);
            return false;
        }

        Url Current = RequestUrl;

        for (Foundation::Uint32 Redirect{}; Redirect <= MaxRedirects; ++Redirect)
        {
            if (!Get(Current, Response))
            {
                return false;
            }

            if (!Response.Redirect())
            {
                return true;
            }

            if (Redirect == MaxRedirects)
            {
                break;
            }

            const auto Location = Response.Location();

            if (Location.Empty())
            {
                LastStatus_ = UEFI::StatusCode::NotFound;
                OnError(LastStatus_);
                return false;
            }

            Url Next{};

            if (Location.Find("://") != Library::StringView::NotFound)
            {
                if (!Next.Parse(Location))
                {
                    LastStatus_ = UEFI::StatusCode::InvalidParameter;
                    OnError(LastStatus_);
                    return false;
                }
            }
            else
            {
                Library::String Text{};
                Text += "http://";
                Text += Current.Host();

                if (!Location.Empty() && Location[0] != '/')
                {
                    Text += "/";
                }

                Text += Location;

                if (!Next.Parse(Text.View()))
                {
                    LastStatus_ = UEFI::StatusCode::InvalidParameter;
                    OnError(LastStatus_);
                    return false;
                }
            }

            if (Next.Secure())
            {
                LastStatus_ = UEFI::StatusCode::Unsupported;
                OnError(LastStatus_);
                return false;
            }

            HttpRedirect ParsedRedirect{};
            ParsedRedirect.From = Current;
            ParsedRedirect.To = Next;
            ParsedRedirect.Response = Response;

            OnRedirected(ParsedRedirect);

            Current = Next;
        }

        LastStatus_ = UEFI::StatusCode::Timeout;
        OnError(LastStatus_);

        return false;
    }

    auto HttpClient::Head(const IPv4Address& Address, Library::StringView Host, Library::StringView Path, HttpResponse& Response) -> Foundation::Bool
    {
        return Head(Address, 80, Host, Path, Response);
    }

    auto HttpClient::Head(Library::StringView Host, Library::StringView Path, HttpResponse& Response) -> Foundation::Bool
    {
        IPv4Address Address{};

        if (!ResolveHost(Host, Address))
        {
            return false;
        }

        return Head(Address, 80, Host, Path, Response);
    }

    auto HttpClient::Head(const Url& Url, HttpResponse& Response) -> Foundation::Bool
    {
        if (!Url.Valid() || Url.Scheme() != UrlScheme::Http)
        {
            LastStatus_ = UEFI::StatusCode::Unsupported;
            OnError(LastStatus_);
            return false;
        }

        return Head(Url.Host(), Url.Path(), Response);
    }

    auto HttpClient::Head(const IPv4Address& Address, Foundation::Uint16 Port, Library::StringView Host, Library::StringView Path, HttpResponse& Response) -> Foundation::Bool
    {
        return Request("HEAD", Address, Port, Host, Path, Response, DefaultHeaders());
    }

    auto HttpClient::ParseResponse(const Library::String& Raw, HttpResponse& Response) -> Foundation::Bool
    {
        const auto HeaderEnd = Raw.View().Find("\r\n\r\n");

        if (HeaderEnd == Library::StringView::NotFound)
        {
            LastStatus_ = UEFI::StatusCode::InvalidParameter;
            return false;
        }

        const auto HeaderText = Raw.View().Substr(0, HeaderEnd);
        const auto BodyText = Raw.View().Substr(HeaderEnd + 4);

        const auto StatusLineEnd = HeaderText.Find("\r\n");

        if (StatusLineEnd == Library::StringView::NotFound)
        {
            LastStatus_ = UEFI::StatusCode::InvalidParameter;
            return false;
        }

        const auto StatusLine = HeaderText.Substr(0, StatusLineEnd);

        if (StatusLine.Size() < 12)
        {
            LastStatus_ = UEFI::StatusCode::InvalidParameter;
            return false;
        }

        Response.StatusCode = (StatusLine[9] - '0') * 100 + (StatusLine[10] - '0') * 10 + (StatusLine[11] - '0');

        if (StatusLine.Size() > 13)
        {
            Response.ReasonPhrase = StatusLine.Substr(13);
        }

        Foundation::Size Offset = StatusLineEnd + 2;

        while (Offset < HeaderText.Size())
        {
            const auto LineEnd = HeaderText.Find("\r\n", Offset);
            const auto Line = LineEnd == Library::StringView::NotFound ? HeaderText.Substr(Offset) : HeaderText.Substr(Offset, LineEnd - Offset);
            const auto Colon = Line.Find(":");

            if (Colon != Library::StringView::NotFound)
            {
                HttpHeader Header{};
                Header.Name = Line.Substr(0, Colon);

                auto ValueStart = Colon + 1;

                while (ValueStart < Line.Size() && Line[ValueStart] == ' ')
                {
                    ++ValueStart;
                }

                Header.Value = Line.Substr(ValueStart);
                Response.Headers.PushBack(Foundation::Utility::Move(Header));
            }

            if (LineEnd == Library::StringView::NotFound)
            {
                break;
            }

            Offset = LineEnd + 2;
        }

        if (IsChunked(Response))
        {
            if (!DecodeChunkedBody(BodyText, Response.Body))
            {
                LastStatus_ = UEFI::StatusCode::InvalidParameter;
                return false;
            }
        }
        else
        {
            Response.Body = BodyText;
        }

        LastStatus_ = UEFI::StatusCode::Success;

        return true;
    }

    auto HttpClient::IsChunked(const HttpResponse& Response) const -> Foundation::Bool
    {
        for (const auto& Header : Response.Headers)
        {
            if (Header.Name == "Transfer-Encoding" && Header.Value.Find("chunked") != Library::StringView::NotFound)
            {
                return true;
            }
        }

        return false;
    }

    auto HttpClient::DecodeChunkedBody(Library::StringView Body, Library::String& Decoded) -> Foundation::Bool
    {
        Decoded.Clear();

        Foundation::Size Offset{};

        while (Offset < Body.Size())
        {
            const auto LineEnd = Body.Find("\r\n", Offset);

            if (LineEnd == Library::StringView::NotFound)
            {
                return false;
            }

            const auto SizeText = Body.Substr(Offset, LineEnd - Offset);
            Foundation::Uint32 ChunkSize{};

            for (const auto Ch : SizeText)
            {
                ChunkSize <<= 4;

                if (Ch >= '0' && Ch <= '9')
                {
                    ChunkSize += Ch - '0';
                }
                else if (Ch >= 'a' && Ch <= 'f')
                {
                    ChunkSize += Ch - 'a' + 10;
                }
                else if (Ch >= 'A' && Ch <= 'F')
                {
                    ChunkSize += Ch - 'A' + 10;
                }
                else
                {
                    return false;
                }
            }

            Offset = LineEnd + 2;

            if (ChunkSize == 0)
            {
                return true;
            }

            if (Offset + ChunkSize > Body.Size())
            {
                return false;
            }

            Decoded += Body.Substr(Offset, ChunkSize);
            Offset += ChunkSize;

            if (Offset + 2 > Body.Size())
            {
                return false;
            }

            Offset += 2;
        }

        return true;
    }

    auto HttpClient::BuildRequest(Library::StringView Method, Library::StringView Host, Library::StringView Path, const HttpHeaders& Headers, Library::StringView Body) -> Library::String
    {
        Library::String Request{};

        Request += Method;
        Request += " ";
        Request += Path.Empty() ? "/" : Path;
        Request += " HTTP/1.1\r\n";

        Request += "Host: ";
        Request += Host;
        Request += "\r\n";

        for (const auto& Header : Headers)
        {
            if (!Header.Valid())
            {
                continue;
            }

            Request += Header.Name;
            Request += ": ";
            Request += Header.Value;
            Request += "\r\n";
        }

        if (!Body.Empty() && !Headers.Contains("Content-Length"))
        {
            Request += "Content-Length: ";
            AppendDecimal(Request, Body.Size());
            Request += "\r\n";
        }

        if (!Headers.Contains("Connection"))
        {
            Request += "Connection: close\r\n";
        }

        Request += "\r\n";

        if (!Body.Empty())
        {
            Request += Body;
        }

        return Request;
    }

    auto HttpClient::Request(Library::StringView Method, const IPv4Address& Address, Foundation::Uint16 Port, Library::StringView Host, Library::StringView Path, HttpResponse& Response, const HttpHeaders& Headers, Library::StringView Body) -> Foundation::Bool
    {
        OnRequestStarted(Method);
        Response = {};

        if (!Tcp_.Connect(Address, Port))
        {
            LastStatus_ = Tcp_.LastStatus();
            OnError(LastStatus_);
            OnRequestFinished(Method);
            return false;
        }

        OnConnected();
        const auto RequestText = BuildRequest(Method, Host, Path, Headers, Body);

        if (!Tcp_.Send(RequestText.View()))
        {
            LastStatus_ = Tcp_.LastStatus();
            OnError(LastStatus_);
            Tcp_.Close(true);
            OnDisconnected();
            OnRequestFinished(Method);
            return false;
        }

        OnRequestSent(RequestText.View());

        Library::String Raw{};

        for (;;)
        {
            Library::String Chunk{};

            if (!Tcp_.Receive(Chunk, 4096))
            {
                LastStatus_ = Tcp_.LastStatus();
                break;
            }

            Raw += Chunk;
            OnRawResponseReceived(Chunk.View());
        }

        Tcp_.Close();
        OnDisconnected();

        Response.Raw = Raw;

        if (!ParseResponse(Raw, Response))
        {
            OnError(LastStatus_);
            OnRequestFinished(Method);
            return false;
        }

        OnResponseReceived(Response);
        OnRequestFinished(Method);

        return true;
    }

    auto HttpClient::ResolveHost(Library::StringView Host, IPv4Address& Address) -> Foundation::Bool
    {
		OnResolvingHost(Host);
        DnsClient Dns{};

        if (!Dns.Resolve(Host, Address))
        {
            LastStatus_ = Dns.LastStatus();
            OnError(LastStatus_);
            return false;
        }

		OnHostResolved(Address);

        return true;
    }

    auto HttpClient::Post(const IPv4Address& Address, Library::StringView Host, Library::StringView Path, Library::StringView Body, HttpResponse& Response) -> Foundation::Bool
    {
        return Post(Address, 80, Host, Path, Body, Response);
    }

    auto HttpClient::Post(Library::StringView Host, Library::StringView Path, Library::StringView Body, HttpResponse& Response) -> Foundation::Bool
    {
        IPv4Address Address{};

        if (!ResolveHost(Host, Address))
        {
            return false;
        }

        return Post(Address, 80, Host, Path, Body, Response);
    }

    auto HttpClient::Post(const Url& RequestUrl, Library::StringView Body, HttpResponse& Response) -> Foundation::Bool
    {
        if (!RequestUrl.Valid() || RequestUrl.Scheme() != UrlScheme::Http)
        {
            LastStatus_ = UEFI::StatusCode::Unsupported;
            OnError(LastStatus_);
            return false;
        }

        return Post(RequestUrl.Host(), RequestUrl.Path(), Body, Response);
    }

    auto HttpClient::Post(const IPv4Address& Address, Foundation::Uint16 Port,  Library::StringView Host, Library::StringView Path, Library::StringView Body, HttpResponse& Response) -> Foundation::Bool
    {
        auto Headers = DefaultHeaders();
        Headers.Add("Content-Type", "application/octet-stream");

        return Request("POST", Address, Port, Host, Path, Response, Headers, Body);
    }

    auto HttpClient::PostJson(Library::StringView Host, Library::StringView Path, Library::StringView Json, HttpResponse& Response) -> Foundation::Bool
    {
        IPv4Address Address{};

        if (!ResolveHost(Host, Address))
        {
            return false;
        }

        auto Headers = DefaultHeaders();
        Headers.Add("Content-Type", "application/json");

        return Request("POST", Address, 80, Host, Path, Response, Headers, Json);
    }

    auto HttpClient::PostJson(const Url& RequestUrl, Library::StringView Json, HttpResponse& Response) -> Foundation::Bool
    {
        if (!RequestUrl.Valid() || RequestUrl.Scheme() != UrlScheme::Http)
        {
            LastStatus_ = UEFI::StatusCode::Unsupported;
            OnError(LastStatus_);
            return false;
        }

        return PostJson(RequestUrl.Host(), RequestUrl.Path(), Json, Response);
    }

    auto HttpClient::DefaultHeaders() const -> HttpHeaders
    {
        HttpHeaders Headers{};

        Headers.Add("User-Agent", "UEFIpp/1.0");
        Headers.Add("Accept", "*/*");

        return Headers;
    }

    auto HttpClient::Put(const IPv4Address& Address, Library::StringView Host, Library::StringView Path, Library::StringView Body, HttpResponse& Response) -> Foundation::Bool
    {
        return Put(Address, 80, Host, Path, Body, Response);
    }

    auto HttpClient::Put(Library::StringView Host, Library::StringView Path, Library::StringView Body, HttpResponse& Response) -> Foundation::Bool
    {
        IPv4Address Address{};

        if (!ResolveHost(Host, Address))
        {
            return false;
        }

        return Put(Address, 80, Host, Path, Body, Response);
    }

    auto HttpClient::Put(const Url& RequestUrl, Library::StringView Body, HttpResponse& Response) -> Foundation::Bool
    {
        if (!RequestUrl.Valid() || RequestUrl.Scheme() != UrlScheme::Http)
        {
            LastStatus_ = UEFI::StatusCode::Unsupported;
            OnError(LastStatus_);
            return false;
        }

        return Put(RequestUrl.Host(), RequestUrl.Path(), Body, Response);
    }

    auto HttpClient::Put(const IPv4Address& Address, Foundation::Uint16 Port, Library::StringView Host, Library::StringView Path, Library::StringView Body, HttpResponse& Response) -> Foundation::Bool
    {
        auto Headers = DefaultHeaders();
        Headers.Add("Content-Type", "application/octet-stream");

        return Request("PUT", Address, Port, Host, Path, Response, Headers, Body);
    }

    auto HttpClient::Delete(const IPv4Address& Address, Foundation::Uint16 Port, Library::StringView Host, Library::StringView Path, HttpResponse& Response) -> Foundation::Bool
    {
        return Request("DELETE", Address, Port, Host, Path, Response, DefaultHeaders());
    }

    auto HttpClient::Delete(const IPv4Address& Address, Library::StringView Host, Library::StringView Path, HttpResponse& Response) -> Foundation::Bool
    {
        return Delete(Address, 80, Host, Path, Response);
    }

    auto HttpClient::Delete(Library::StringView Host, Library::StringView Path, HttpResponse& Response) -> Foundation::Bool
    {
        IPv4Address Address{};

        if (!ResolveHost(Host, Address))
        {
            return false;
        }

        return Delete(Address, 80, Host, Path, Response);
    }

    auto HttpClient::Delete(const Url& RequestUrl, HttpResponse& Response) -> Foundation::Bool
    {
        if (!RequestUrl.Valid() || RequestUrl.Scheme() != UrlScheme::Http)
        {
            LastStatus_ = UEFI::StatusCode::Unsupported;
            OnError(LastStatus_);
            return false;
        }

        return Delete(RequestUrl.Host(), RequestUrl.Path(), Response);
    }
}