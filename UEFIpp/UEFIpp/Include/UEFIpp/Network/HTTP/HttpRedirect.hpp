#pragma once

#include <UEFIpp/Network/HTTP/HttpResponse.hpp>
#include <UEFIpp/Network/Utility/Url.hpp>

namespace UEFIpp::Network
{
    class HttpRedirect
    {
    public:
        Url From{};
        Url To{};
        HttpResponse Response{};
    };
}