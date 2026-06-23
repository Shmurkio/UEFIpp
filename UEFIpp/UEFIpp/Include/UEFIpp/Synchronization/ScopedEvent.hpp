#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Synchronization/WaitableEvent.hpp>
#include <UEFIpp/UEFI/Context.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Types.hpp>

namespace UEFIpp::Synchronization
{
    class ScopedEvent : public WaitableEvent
    {
    public:
        static constexpr Foundation::Uint32 EventNone = 0;
        static constexpr UEFI::Tpl TplCallback = 8;

    public:
        constexpr ScopedEvent() = default;

        explicit ScopedEvent(UEFI::Event Event) : WaitableEvent(Event)
        {
        }

        ScopedEvent(const ScopedEvent&) = delete;
        auto operator=(const ScopedEvent&) -> ScopedEvent & = delete;

        ScopedEvent(ScopedEvent&& Other) noexcept
        {
            Event_ = Other.Event_;
            Other.Event_ = nullptr;
        }

        auto operator=(ScopedEvent&& Other) noexcept -> ScopedEvent&
        {
            if (this != &Other)
            {
                Close();

                Event_ = Other.Event_;
                Other.Event_ = nullptr;
            }

            return *this;
        }

        ~ScopedEvent()
        {
            Close();
        }

        [[nodiscard]] static auto Create() -> ScopedEvent
        {
            UEFI::Event Event{};
            const auto Status = UEFI::Context::BootServices().CreateEvent(EventNone, TplCallback, nullptr, nullptr, &Event);

            if (UEFI::IsError(Status))
            {
                return {};
            }

            return ScopedEvent{ Event };
        }

        auto Close() -> Foundation::Void
        {
            if (Event_)
            {
                UEFI::Context::BootServices().CloseEvent(Event_);
                Event_ = nullptr;
            }
        }

        [[nodiscard]] auto Release() -> UEFI::Event
        {
            const auto Event = Event_;
            Event_ = nullptr;
            return Event;
        }
    };
}