#pragma once

#include <UEFIpp/Library/Containers/Optional.hpp>
#include <UEFIpp/Time/Date.hpp>
#include <UEFIpp/Time/Duration.hpp>
#include <UEFIpp/Time/Instant.hpp>
#include <UEFIpp/Time/TimeOfDay.hpp>
#include <UEFIpp/Time/TimeZone.hpp>
#include <UEFIpp/UEFI/RuntimeServices.hpp>

namespace UEFIpp::Time
{
	class DateTime
	{
	public:
		constexpr DateTime() = default;
		constexpr DateTime(const DateTime&) = default;
		constexpr DateTime(DateTime&&) noexcept = default;

		constexpr auto operator=(const DateTime&) -> DateTime & = default;
		constexpr auto operator=(DateTime&&) noexcept -> DateTime & = default;

		~DateTime() = default;

		constexpr DateTime(Time::Date Date, Time::TimeOfDay TimeOfDay, Time::UtcOffset Offset = UtcOffset{}) : Date_(Date), TimeOfDay_(TimeOfDay), Offset_(Offset)
		{
		}

		[[nodiscard]] static constexpr auto FromInstant(Instant Value, UtcOffset Offset = UtcOffset::Utc()) -> DateTime
		{
			auto Nanoseconds = Value.UnixNanoseconds() + Offset.AsDuration().Nanoseconds();

			auto Days = Nanoseconds / Duration::NanosecondsPerDay;
			auto Remainder = Nanoseconds % Duration::NanosecondsPerDay;

			if (Remainder < 0)
			{
				Remainder += Duration::NanosecondsPerDay;
				--Days;
			}

			return DateTime(Date::FromDaysSinceEpoch(Days), Time::TimeOfDay::FromNanosecondsSinceMidnight(Remainder), Offset);
		}

		[[nodiscard]] static constexpr auto FromUnixSeconds(Foundation::Int64 Seconds, UtcOffset Offset = UtcOffset::Utc()) -> DateTime
		{
			return FromInstant(Instant::FromUnixSeconds(Seconds), Offset);
		}

		[[nodiscard]] static constexpr auto FromUnixMilliseconds(Foundation::Int64 Milliseconds, UtcOffset Offset = UtcOffset::Utc()) -> DateTime
		{
			return FromInstant(Instant::FromUnixMilliseconds(Milliseconds), Offset);
		}

		[[nodiscard]] static constexpr auto FromUnixNanoseconds(Foundation::Int64 Nanoseconds, UtcOffset Offset = UtcOffset::Utc()) -> DateTime
		{
			return FromInstant(Instant::FromUnixNanoseconds(Nanoseconds), Offset);
		}

		[[nodiscard]] static auto FromEfiTime(const UEFI::Table::Time& Value) -> DateTime
		{
			return DateTime(Time::Date(Value.Year, Value.Month, Value.Day), Time::TimeOfDay(Value.Hour, Value.Minute, Value.Second, Value.Nanosecond), Value.TimeZone == UtcOffset::UnspecifiedMinutes ? UtcOffset::Unspecified() : UtcOffset::FromMinutes(Value.TimeZone));
		}

		[[nodiscard]] constexpr auto Date() const -> Time::Date
		{
			return Date_;
		}

		[[nodiscard]] constexpr auto TimeOfDay() const -> Time::TimeOfDay
		{
			return TimeOfDay_;
		}

		[[nodiscard]] constexpr auto Offset() const -> Time::UtcOffset
		{
			return Offset_;
		}

		[[nodiscard]] constexpr auto Year() const -> Foundation::Int32
		{
			return Date_.Year();
		}

		[[nodiscard]] constexpr auto Month() const -> Foundation::Uint8
		{
			return Date_.Month();
		}

		[[nodiscard]] constexpr auto Day() const -> Foundation::Uint8
		{
			return Date_.Day();
		}

		[[nodiscard]] constexpr auto Hour() const -> Foundation::Uint8
		{
			return TimeOfDay_.Hour();
		}

		[[nodiscard]] constexpr auto Minute() const -> Foundation::Uint8
		{
			return TimeOfDay_.Minute();
		}

		[[nodiscard]] constexpr auto Second() const -> Foundation::Uint8
		{
			return TimeOfDay_.Second();
		}

		[[nodiscard]] constexpr auto Nanosecond() const -> Foundation::Uint32
		{
			return TimeOfDay_.Nanosecond();
		}

		[[nodiscard]] constexpr auto Valid() const -> Foundation::Bool
		{
			return Date_.Valid() && TimeOfDay_.Valid();
		}

		[[nodiscard]] constexpr auto HasSpecifiedOffset() const -> Foundation::Bool
		{
			return Offset_.Specified();
		}

		[[nodiscard]] constexpr auto ToInstant() const -> Instant
		{
			const auto Days = Date_.DaysSinceEpoch();

			auto Nanoseconds = Days * Duration::NanosecondsPerDay + TimeOfDay_.SinceMidnight().Nanoseconds();

			if (Offset_.Specified())
			{
				Nanoseconds -= Offset_.AsDuration().Nanoseconds();
			}

			return Instant::FromUnixNanoseconds(Nanoseconds);
		}

		[[nodiscard]] constexpr auto ToOffset(UtcOffset NewOffset) const -> DateTime
		{
			return FromInstant(ToInstant(), NewOffset);
		}

		[[nodiscard]] constexpr auto ToUtc() const -> DateTime
		{
			return ToOffset(UtcOffset::Utc());
		}

		[[nodiscard]] auto ToEfiTime() const -> UEFI::Table::Time
		{
			UEFI::Table::Time Result{};

			Result.Year = Foundation::Cast::Auto<Foundation::Uint16>(Year());
			Result.Month = Month();
			Result.Day = Day();
			Result.Hour = Hour();
			Result.Minute = Minute();
			Result.Second = Second();
			Result.Nanosecond = Nanosecond();
			Result.TimeZone = Offset_.Specified() ? Offset_.Minutes() : UtcOffset::UnspecifiedMinutes;

			return Result;
		}

		[[nodiscard]] friend constexpr auto operator+(DateTime Value, Duration Offset) -> DateTime
		{
			return FromInstant(Value.ToInstant() + Offset, Value.Offset_);
		}

		[[nodiscard]] friend constexpr auto operator-(DateTime Value, Duration Offset) -> DateTime
		{
			return FromInstant(Value.ToInstant() - Offset, Value.Offset_);
		}

		[[nodiscard]] friend constexpr auto operator-(DateTime Left, DateTime Right) -> Duration
		{
			return Left.ToInstant() - Right.ToInstant();
		}

		[[nodiscard]] constexpr auto operator<=>(const DateTime&) const = default;

	private:
		Time::Date Date_{};
		Time::TimeOfDay TimeOfDay_{};
		Time::UtcOffset Offset_{};
	};
}