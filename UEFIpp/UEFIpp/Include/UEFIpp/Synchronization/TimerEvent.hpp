#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Synchronization/ScopedEvent.hpp>
#include <UEFIpp/Time/Duration.hpp>
#include <UEFIpp/UEFI/Context.hpp>
#include <UEFIpp/UEFI/Status.hpp>

namespace UEFIpp::Synchronization
{
    enum class TimerMode : Foundation::Uint32
    {
        Cancel = 0,
        Periodic = 1,
        Relative = 2
    };

    class TimerEvent : public ScopedEvent
    {
    public:
        static constexpr Foundation::Uint32 EventTimer = 0x80000000;
        static constexpr UEFI::Tpl TplCallback = 8;

    public:
        constexpr TimerEvent() = default;

        explicit TimerEvent(UEFI::Event Event) : ScopedEvent(Event)
        {
        }

        [[nodiscard]] static auto Create() -> TimerEvent
        {
            UEFI::Event Event{};
            const auto Status = UEFI::Context::BootServices().CreateEvent(EventTimer, TplCallback, nullptr, nullptr, &Event);

            if (UEFI::IsError(Status))
            {
                return {};
            }

            return TimerEvent{ Event };
        }

        [[nodiscard]] auto SetRelative(Time::Duration Duration) -> UEFI::StatusCode
        {
            return Set(TimerMode::Relative, Duration);
        }

        [[nodiscard]] auto SetPeriodic(Time::Duration Interval) -> UEFI::StatusCode
        {
            return Set(TimerMode::Periodic, Interval);
        }

        [[nodiscard]] auto Cancel() -> UEFI::StatusCode
        {
            return Set(TimerMode::Cancel, Time::Duration::Zero());
        }

    private:
        [[nodiscard]] auto Set(TimerMode Mode, Time::Duration Duration) -> UEFI::StatusCode
        {
            if (!Event_)
            {
                return UEFI::StatusCode::InvalidParameter;
            }

            const auto TriggerTime = Foundation::Cast::Auto<Foundation::Uint64>(Duration.Nanoseconds() / 100);

            return UEFI::Context::BootServices().SetTimer(Event_, Foundation::Cast::Auto<UEFI::Table::TimerDelay>(Mode), TriggerTime);
        }
    };
}