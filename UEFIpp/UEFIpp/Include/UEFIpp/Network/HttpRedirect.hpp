#pragma once

#include <UEFIpp/Network/HttpResponse.hpp>
#include <UEFIpp/Network/Url.hpp>

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