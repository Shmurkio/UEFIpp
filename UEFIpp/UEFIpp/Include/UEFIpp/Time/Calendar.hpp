#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Time
{
	enum class Month : Foundation::Uint8
	{
		January = 1,
		February,
		March,
		April,
		May,
		June,
		July,
		August,
		September,
		October,
		November,
		December,
	};

	enum class Weekday : Foundation::Uint8
	{
		Sunday = 0,
		Monday,
		Tuesday,
		Wednesday,
		Thursday,
		Friday,
		Saturday,
	};

	class CivilDateParts
	{
	public:
		Foundation::Int32 Year{};
		Foundation::Uint8 Month{};
		Foundation::Uint8 Day{};
	};

	[[nodiscard]] constexpr auto IsValidMonth(Foundation::Uint8 Value) -> Foundation::Bool
	{
		return Value >= 1 && Value <= 12;
	}

	[[nodiscard]] constexpr auto IsLeapYear(Foundation::Int32 Year) -> Foundation::Bool
	{
		return (Year % 4 == 0 && Year % 100 != 0) || (Year % 400 == 0);
	}

	[[nodiscard]] constexpr auto DaysInMonth(Foundation::Int32 Year, Foundation::Uint8 MonthValue) -> Foundation::Uint8
	{
		switch (MonthValue)
		{
		case 1: return 31;
		case 2: return IsLeapYear(Year) ? 29 : 28;
		case 3: return 31;
		case 4: return 30;
		case 5: return 31;
		case 6: return 30;
		case 7: return 31;
		case 8: return 31;
		case 9: return 30;
		case 10: return 31;
		case 11: return 30;
		case 12: return 31;
		default: return 0;
		}
	}

	[[nodiscard]] constexpr auto DaysInMonth(Foundation::Int32 Year, Month MonthValue) -> Foundation::Uint8
	{
		return DaysInMonth(Year, Foundation::Cast::Auto<Foundation::Uint8>(MonthValue));
	}

	[[nodiscard]] constexpr auto IsValidDate(Foundation::Int32 Year, Foundation::Uint8 MonthValue, Foundation::Uint8 Day) -> Foundation::Bool
	{
		const auto Days = DaysInMonth(Year, MonthValue);
		return Days != 0 && Day >= 1 && Day <= Days;
	}

	[[nodiscard]] constexpr auto DayOfYear(Foundation::Int32 Year, Foundation::Uint8 MonthValue, Foundation::Uint8 Day) -> Foundation::Uint16
	{
		Foundation::Uint16 Result{};

		for (Foundation::Uint8 Current = 1; Current < MonthValue; ++Current)
		{
			Result = Foundation::Cast::Auto<Foundation::Uint16>(Result + DaysInMonth(Year, Current));
		}

		return Foundation::Cast::Auto<Foundation::Uint16>(Result + Day - 1);
	}

	[[nodiscard]] constexpr auto DaysFromCivil(Foundation::Int32 Year, Foundation::Uint32 MonthValue, Foundation::Uint32 Day) -> Foundation::Int64
	{
		Year -= MonthValue <= 2;

		const auto Era = (Year >= 0 ? Year : Year - 399) / 400;
		const auto YearOfEra = Foundation::Cast::Auto<Foundation::Uint32>(Year - Era * 400);

		const auto DayOfYearValue = (153 * (MonthValue + (MonthValue > 2 ? -3 : 9)) + 2) / 5 + Day - 1;

		const auto DayOfEra = YearOfEra * 365 + YearOfEra / 4 - YearOfEra / 100 + DayOfYearValue;

		return Foundation::Cast::Auto<Foundation::Int64>(Era) * 146097 + Foundation::Cast::Auto<Foundation::Int64>(DayOfEra) - 719468;
	}

	[[nodiscard]] constexpr auto CivilFromDays(Foundation::Int64 Days) -> CivilDateParts
	{
		Days += 719468;

		const auto Era = (Days >= 0 ? Days : Days - 146096) / 146097;
		const auto DayOfEra = Foundation::Cast::Auto<Foundation::Uint32>(Days - Era * 146097);

		const auto YearOfEra = (DayOfEra - DayOfEra / 1460 + DayOfEra / 36524 - DayOfEra / 146096) / 365;

		auto Year = Foundation::Cast::Auto<Foundation::Int32>(YearOfEra) + Foundation::Cast::Auto<Foundation::Int32>(Era) * 400;

		const auto DayOfYearValue = DayOfEra - (365 * YearOfEra + YearOfEra / 4 - YearOfEra / 100);

		const auto MonthPrime = (5 * DayOfYearValue + 2) / 153;

		const auto Day = DayOfYearValue - (153 * MonthPrime + 2) / 5 + 1;

		const auto MonthValue = MonthPrime + (MonthPrime < 10 ? 3 : -9);

		Year += MonthValue <= 2;

		return { Year, Foundation::Cast::Auto<Foundation::Uint8>(MonthValue), Foundation::Cast::Auto<Foundation::Uint8>(Day) };
	}

	[[nodiscard]] constexpr auto WeekdayFromDays(Foundation::Int64 Days) -> Weekday
	{
		const auto Value = (Days >= -4) ? (Days + 4) % 7 : (Days + 5) % 7 + 6;
		return Foundation::Cast::Auto<Weekday>(Value);
	}

	[[nodiscard]] constexpr auto WeekdayFromCivil(Foundation::Int32 Year, Foundation::Uint8 MonthValue, Foundation::Uint8 Day) -> Weekday
	{
		return WeekdayFromDays(DaysFromCivil(Year, MonthValue, Day));
	}
}