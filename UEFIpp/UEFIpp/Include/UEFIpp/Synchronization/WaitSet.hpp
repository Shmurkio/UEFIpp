#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Expected.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Synchronization/WaitableEvent.hpp>
#include <UEFIpp/UEFI/Context.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Types.hpp>

namespace UEFIpp::Synchronization
{
    class WaitSet
    {
    public:
        [[nodiscard]] auto Add(WaitableEvent Event) -> Foundation::Bool
        {
            if (!Event)
            {
                return false;
            }

            Events_.PushBack(Event.Raw());

            return true;
        }

        auto Clear() -> Foundation::Void
        {
            Events_.Clear();
        }

        [[nodiscard]] auto Empty() const -> Foundation::Bool
        {
            return Events_.Empty();
        }

        [[nodiscard]] auto Count() const -> Foundation::Size
        {
            return Events_.Size();
        }

        [[nodiscard]] auto Wait() -> Library::Expected<Foundation::Size, UEFI::StatusCode>
        {
            if (Events_.Empty())
            {
                return Library::Unexpected{ UEFI::StatusCode::InvalidParameter };
            }

            Foundation::UintN Index{};
            const auto Status = UEFI::Context::BootServices().WaitForEvent(Foundation::Cast::Auto<Foundation::UintN>(Events_.Size()), Events_.Data(), &Index);

            if (UEFI::IsError(Status))
            {
                return Library::Unexpected{ Status };
            }

            return Foundation::Cast::Auto<Foundation::Size>(Index);
        }

    private:
        Library::Vector<UEFI::Event> Events_{};
    };
}