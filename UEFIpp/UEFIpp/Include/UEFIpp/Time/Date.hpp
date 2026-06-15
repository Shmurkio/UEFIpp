#pragma once

#include <UEFIpp/Time/Calendar.hpp>

namespace UEFIpp::Time
{
	class Date
	{
	public:
		constexpr Date() = default;
		constexpr Date(const Date&) = default;
		constexpr Date(Date&&) noexcept = default;

		constexpr auto operator=(const Date&) -> Date & = default;
		constexpr auto operator=(Date&&) noexcept -> Date & = default;

		~Date() = default;

		constexpr Date(Foundation::Int32 Year, Foundation::Uint8 Month, Foundation::Uint8 Day) : Year_(Year), Month_(Month), Day_(Day)
		{
		}

		constexpr Date(Foundation::Int32 Year, Time::Month Month, Foundation::Uint8 Day) : Date(Year, Foundation::Cast::Auto<Foundation::Uint8>(Month), Day)
		{
		}

		[[nodiscard]] static constexpr auto UnixEpoch() -> Date
		{
			return Date(1970, 1, 1);
		}

		[[nodiscard]] static constexpr auto FromDaysSinceEpoch(Foundation::Int64 Days) -> Date
		{
			const auto Parts = CivilFromDays(Days);
			return Date(Parts.Year, Parts.Month, Parts.Day);
		}

		[[nodiscard]] constexpr auto Year() const -> Foundation::Int32
		{
			return Year_;
		}

		[[nodiscard]] constexpr auto Month() const -> Foundation::Uint8
		{
			return Month_;
		}

		[[nodiscard]] constexpr auto MonthEnum() const -> Time::Month
		{
			return Foundation::Cast::Auto<Time::Month>(Month_);
		}

		[[nodiscard]] constexpr auto Day() const -> Foundation::Uint8
		{
			return Day_;
		}

		[[nodiscard]] constexpr auto Valid() const -> Foundation::Bool
		{
			return IsValidDate(Year_, Month_, Day_);
		}

		[[nodiscard]] constexpr auto DaysSinceEpoch() const -> Foundation::Int64
		{
			return DaysFromCivil(Year_, Month_, Day_);
		}

		[[nodiscard]] constexpr auto DayOfYear() const -> Foundation::Uint16
		{
			return Time::DayOfYear(Year_, Month_, Day_);
		}

		[[nodiscard]] constexpr auto Weekday() const -> Time::Weekday
		{
			return WeekdayFromDays(DaysSinceEpoch());
		}

		[[nodiscard]] constexpr auto IsLeapYear() const -> Foundation::Bool
		{
			return Time::IsLeapYear(Year_);
		}

		[[nodiscard]] constexpr auto DaysInMonth() const -> Foundation::Uint8
		{
			return Time::DaysInMonth(Year_, Month_);
		}

		[[nodiscard]] friend constexpr auto operator+(Date Value, Foundation::Int64 Days) -> Date
		{
			return Date::FromDaysSinceEpoch(Value.DaysSinceEpoch() + Days);
		}

		[[nodiscard]] friend constexpr auto operator-(Date Value, Foundation::Int64 Days) -> Date
		{
			return Date::FromDaysSinceEpoch(Value.DaysSinceEpoch() - Days);
		}

		[[nodiscard]] friend constexpr auto operator-(Date Left, Date Right) -> Foundation::Int64
		{
			return Left.DaysSinceEpoch() - Right.DaysSinceEpoch();
		}

		[[nodiscard]] constexpr auto operator<=>(const Date&) const = default;

	private:
		Foundation::Int32 Year_{};
		Foundation::Uint8 Month_{};
		Foundation::Uint8 Day_{};
	};
}