#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Time/Duration.hpp>

namespace UEFIpp::Time
{
	class TimeOfDay
	{
	public:
		constexpr TimeOfDay() = default;
		constexpr TimeOfDay(const TimeOfDay&) = default;
		constexpr TimeOfDay(TimeOfDay&&) noexcept = default;

		constexpr auto operator=(const TimeOfDay&) -> TimeOfDay & = default;
		constexpr auto operator=(TimeOfDay&&) noexcept -> TimeOfDay & = default;

		~TimeOfDay() = default;

		constexpr TimeOfDay(Foundation::Uint8 Hour, Foundation::Uint8 Minute, Foundation::Uint8 Second = 0, Foundation::Uint32 Nanosecond = 0) : Hour_(Hour), Minute_(Minute), Second_(Second), Nanosecond_(Nanosecond)
		{
		}

		[[nodiscard]] static constexpr auto Midnight() -> TimeOfDay
		{
			return TimeOfDay{};
		}

		[[nodiscard]] static constexpr auto FromNanosecondsSinceMidnight(Foundation::Int64 Nanoseconds) -> TimeOfDay
		{
			if (Nanoseconds < 0)
			{
				Nanoseconds = 0;
			}

			Nanoseconds %= Duration::NanosecondsPerDay;

			const auto Hour = Nanoseconds / Duration::NanosecondsPerHour;
			Nanoseconds %= Duration::NanosecondsPerHour;

			const auto Minute = Nanoseconds / Duration::NanosecondsPerMinute;
			Nanoseconds %= Duration::NanosecondsPerMinute;

			const auto Second = Nanoseconds / Duration::NanosecondsPerSecond;
			Nanoseconds %= Duration::NanosecondsPerSecond;

			return TimeOfDay(Foundation::Cast::Auto<Foundation::Uint8>(Hour), Foundation::Cast::Auto<Foundation::Uint8>(Minute), Foundation::Cast::Auto<Foundation::Uint8>(Second), Foundation::Cast::Auto<Foundation::Uint32>(Nanoseconds));
		}

		[[nodiscard]] static constexpr auto FromDurationSinceMidnight(Duration Value) -> TimeOfDay
		{
			return FromNanosecondsSinceMidnight(Value.Nanoseconds());
		}

		[[nodiscard]] constexpr auto Hour() const -> Foundation::Uint8
		{
			return Hour_;
		}

		[[nodiscard]] constexpr auto Minute() const -> Foundation::Uint8
		{
			return Minute_;
		}

		[[nodiscard]] constexpr auto Second() const -> Foundation::Uint8
		{
			return Second_;
		}

		[[nodiscard]] constexpr auto Nanosecond() const -> Foundation::Uint32
		{
			return Nanosecond_;
		}

		[[nodiscard]] constexpr auto Valid() const -> Foundation::Bool
		{
			return Hour_ < 24 && Minute_ < 60 && Second_ < 60 && Nanosecond_ < Duration::NanosecondsPerSecond;
		}

		[[nodiscard]] constexpr auto SinceMidnight() const -> Duration
		{
			return Duration::FromHours(Hour_) + Duration::FromMinutes(Minute_) + Duration::FromSeconds(Second_) + Duration::FromNanoseconds(Nanosecond_);
		}

		[[nodiscard]] constexpr auto SecondsSinceMidnight() const -> Foundation::Int64
		{
			return SinceMidnight().Seconds();
		}

		[[nodiscard]] constexpr auto NanosecondsSinceMidnight() const -> Foundation::Int64
		{
			return SinceMidnight().Nanoseconds();
		}

		[[nodiscard]] constexpr auto operator<=>(const TimeOfDay&) const = default;

	private:
		Foundation::Uint8 Hour_{};
		Foundation::Uint8 Minute_{};
		Foundation::Uint8 Second_{};
		Foundation::Uint32 Nanosecond_{};
	};
}