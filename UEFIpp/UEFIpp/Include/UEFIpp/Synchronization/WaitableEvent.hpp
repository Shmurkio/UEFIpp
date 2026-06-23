#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Context.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Types.hpp>

namespace UEFIpp::Synchronization
{
    class WaitableEvent
    {
    public:
        constexpr WaitableEvent() = default;

        explicit constexpr WaitableEvent(UEFI::Event Event) : Event_(Event)
        {
        }

        [[nodiscard]] constexpr auto Valid() const -> Foundation::Bool
        {
            return Event_ != nullptr;
        }

        [[nodiscard]] constexpr explicit operator Foundation::Bool() const
        {
            return Valid();
        }

        [[nodiscard]] constexpr auto Raw() const -> UEFI::Event
        {
            return Event_;
        }

        [[nodiscard]] auto Wait() const -> UEFI::StatusCode
        {
            if (!Event_)
            {
                return UEFI::StatusCode::InvalidParameter;
            }

            UEFI::Event Event = Event_;
            Foundation::UintN Index{};

            return UEFI::Context::BootServices().WaitForEvent(1, &Event, &Index);
        }

    protected:
        UEFI::Event Event_{};
    };
}