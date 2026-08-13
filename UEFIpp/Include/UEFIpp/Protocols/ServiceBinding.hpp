#pragma once

#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Types.hpp>

namespace UEFIpp::Protocols
{
    class ServiceBinding
    {
    public:
        using CreateChildFn = UEFI::StatusCode(*)(ServiceBinding* This, UEFI::Handle* ChildHandle);
        using DestroyChildFn = UEFI::StatusCode(*)(ServiceBinding* This, UEFI::Handle ChildHandle);

        CreateChildFn CreateChild;
        DestroyChildFn DestroyChild;
    };

    class Tcp4ServiceBinding : public ServiceBinding {};
    class Udp4ServiceBinding : public ServiceBinding {};
}