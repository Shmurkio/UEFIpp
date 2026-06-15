#include <UEFIpp/FileSystem/Time.hpp>

namespace UEFIpp::FileSystem
{
	auto Time::Valid() const -> Foundation::Bool
	{
		if (Year_ == 0 || Month_ < 1 || Month_ > 12 || Day_ < 1 || Day_ > 31)
		{
			return false;
		}

		if (Hour_ > 23 || Minute_ > 59 || Second_ > 59)
		{
			return false;
		}

		if (Nanosecond_ > 999999999)
		{
			return false;
		}

		return true;
	}

	auto Time::Year() const -> Foundation::Uint16
	{
		return Year_;
	}

	auto Time::Month() const -> Foundation::Uint8
	{
		return Month_;
	}

	auto Time::Day() const -> Foundation::Uint8
	{
		return Day_;
	}

	auto Time::Hour() const -> Foundation::Uint8
	{
		return Hour_;
	}

	auto Time::Minute() const -> Foundation::Uint8
	{
		return Minute_;
	}

	auto Time::Second() const -> Foundation::Uint8
	{
		return Second_;
	}

	auto Time::Nanosecond() const -> Foundation::Uint32
	{
		return Nanosecond_;
	}
}