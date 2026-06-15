#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Time/Duration.hpp>

namespace UEFIpp::Time
{
	class UtcOffset
	{
	public:
		using Rep = Foundation::Int16;

		static constexpr Rep UnspecifiedMinutes = 2047;
		static constexpr Rep MinutesPerHour = 60;

	public:
		constexpr UtcOffset() = default;
		constexpr UtcOffset(const UtcOffset&) = default;
		constexpr UtcOffset(UtcOffset&&) noexcept = default;

		constexpr auto operator=(const UtcOffset&) -> UtcOffset & = default;
		constexpr auto operator=(UtcOffset&&) noexcept -> UtcOffset & = default;

		~UtcOffset() = default;

		explicit constexpr UtcOffset(Rep MinutesEastOfUtc) : MinutesEastOfUtc_(MinutesEastOfUtc)
		{
		}

		[[nodiscard]] static constexpr auto Unspecified() -> UtcOffset
		{
			return UtcOffset(UnspecifiedMinutes);
		}

		[[nodiscard]] static constexpr auto Utc() -> UtcOffset
		{
			return UtcOffset(0);
		}

		[[nodiscard]] static constexpr auto FromMinutes(Rep MinutesEastOfUtc) -> UtcOffset
		{
			return UtcOffset(MinutesEastOfUtc);
		}

		[[nodiscard]] static constexpr auto FromHours(Rep HoursEastOfUtc) -> UtcOffset
		{
			return UtcOffset(Foundation::Cast::Auto<Rep>(HoursEastOfUtc * MinutesPerHour));
		}

		[[nodiscard]] constexpr auto Specified() const -> Foundation::Bool
		{
			return MinutesEastOfUtc_ != UnspecifiedMinutes;
		}

		[[nodiscard]] constexpr auto IsUtc() const -> Foundation::Bool
		{
			return Specified() && MinutesEastOfUtc_ == 0;
		}

		[[nodiscard]] constexpr auto Minutes() const -> Rep
		{
			return MinutesEastOfUtc_;
		}

		[[nodiscard]] constexpr auto Hours() const -> Rep
		{
			return Foundation::Cast::Auto<Rep>(MinutesEastOfUtc_ / MinutesPerHour);
		}

		[[nodiscard]] constexpr auto AsDuration() const -> Duration
		{
			return Specified() ? Duration::FromMinutes(MinutesEastOfUtc_) : Duration{};
		}

		[[nodiscard]] constexpr auto operator<=>(const UtcOffset&) const = default;

	private:
		Rep MinutesEastOfUtc_{ UnspecifiedMinutes };
	};

	enum class Daylight : Foundation::Uint8
	{
		None = 0,
		InDaylight = 1,
		Adjusted = 2,
	};
}