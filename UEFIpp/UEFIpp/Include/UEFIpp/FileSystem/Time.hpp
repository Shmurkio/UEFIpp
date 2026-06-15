#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::FileSystem
{
	class Time
	{
	public:
		// Constructs empty time
		Time() = default;

		// Constructs a time from its components
		Time(Foundation::Uint16 Year, Foundation::Uint8 Month, Foundation::Uint8 Day, Foundation::Uint8 Hour, Foundation::Uint8 Minute, Foundation::Uint8 Second, Foundation::Uint32 Nanosecond = 0) : Year_(Year), Month_(Month), Day_(Day), Hour_(Hour), Minute_(Minute), Second_(Second), Nanosecond_(Nanosecond)
		{
		}

		// Copies another time
		Time(const Time&) = default;

		// Moves another time
		Time(Time&&) noexcept = default;

		// Destroys the time
		~Time() = default;

		// Assigns from another time
		auto operator=(const Time&) -> Time& = default;

		// Move-assigns from another time
		auto operator=(Time&&) noexcept -> Time& = default;

		// Checks whether the time value is valid
		[[nodiscard]] auto Valid() const -> Foundation::Bool;

		// Returns the year component
		[[nodiscard]] auto Year() const -> Foundation::Uint16;

		// Returns the month component (1-12)
		[[nodiscard]] auto Month() const -> Foundation::Uint8;

		// Returns the day component (1-31)
		[[nodiscard]] auto Day() const -> Foundation::Uint8;

		// Returns the hour component (0-23)
		[[nodiscard]] auto Hour() const -> Foundation::Uint8;

		// Returns the minute component (0-59)
		[[nodiscard]] auto Minute() const -> Foundation::Uint8;

		// Returns the second component (0-59)
		[[nodiscard]] auto Second() const -> Foundation::Uint8;

		// Returns the nanosecond component (0-999,999,999)
		[[nodiscard]] auto Nanosecond() const -> Foundation::Uint32;
		
		// Compares two times for equality
		[[nodiscard]] auto operator==(const Time&) const -> Foundation::Bool = default;
		
	private:
		Foundation::Uint16 Year_{};
		Foundation::Uint8 Month_{};
		Foundation::Uint8 Day_{};

		Foundation::Uint8 Hour_{};
		Foundation::Uint8 Minute_{};
		Foundation::Uint8 Second_{};
		Foundation::Uint32 Nanosecond_{};
	};
}