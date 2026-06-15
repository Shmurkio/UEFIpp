#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Time
{
	class Duration
	{
	public:
		using Rep = Foundation::Int64;

		static constexpr Rep NanosecondsPerMicrosecond = 1000;
		static constexpr Rep NanosecondsPerMillisecond = 1000 * NanosecondsPerMicrosecond;
		static constexpr Rep NanosecondsPerSecond = 1000 * NanosecondsPerMillisecond;
		static constexpr Rep NanosecondsPerMinute = 60 * NanosecondsPerSecond;
		static constexpr Rep NanosecondsPerHour = 60 * NanosecondsPerMinute;
		static constexpr Rep NanosecondsPerDay = 24 * NanosecondsPerHour;

	public:
		constexpr Duration() = default;
		constexpr Duration(const Duration&) = default;
		constexpr Duration(Duration&&) noexcept = default;

		constexpr auto operator=(const Duration&) -> Duration & = default;
		constexpr auto operator=(Duration&&) noexcept -> Duration & = default;

		~Duration() = default;

		explicit constexpr Duration(Rep Nanoseconds) : Nanoseconds_(Nanoseconds)
		{
		}

		[[nodiscard]] static constexpr auto Zero() -> Duration
		{
			return Duration{};
		}

		[[nodiscard]] static constexpr auto FromNanoseconds(Rep Value) -> Duration
		{
			return Duration(Value);
		}

		[[nodiscard]] static constexpr auto FromMicroseconds(Rep Value) -> Duration
		{
			return Duration(Value * NanosecondsPerMicrosecond);
		}

		[[nodiscard]] static constexpr auto FromMilliseconds(Rep Value) -> Duration
		{
			return Duration(Value * NanosecondsPerMillisecond);
		}

		[[nodiscard]] static constexpr auto FromSeconds(Rep Value) -> Duration
		{
			return Duration(Value * NanosecondsPerSecond);
		}

		[[nodiscard]] static constexpr auto FromMinutes(Rep Value) -> Duration
		{
			return Duration(Value * NanosecondsPerMinute);
		}

		[[nodiscard]] static constexpr auto FromHours(Rep Value) -> Duration
		{
			return Duration(Value * NanosecondsPerHour);
		}

		[[nodiscard]] static constexpr auto FromDays(Rep Value) -> Duration
		{
			return Duration(Value * NanosecondsPerDay);
		}

		[[nodiscard]] constexpr auto Nanoseconds() const -> Rep
		{
			return Nanoseconds_;
		}

		[[nodiscard]] constexpr auto Microseconds() const -> Rep
		{
			return Nanoseconds_ / NanosecondsPerMicrosecond;
		}

		[[nodiscard]] constexpr auto Milliseconds() const -> Rep
		{
			return Nanoseconds_ / NanosecondsPerMillisecond;
		}

		[[nodiscard]] constexpr auto Seconds() const -> Rep
		{
			return Nanoseconds_ / NanosecondsPerSecond;
		}

		[[nodiscard]] constexpr auto Minutes() const -> Rep
		{
			return Nanoseconds_ / NanosecondsPerMinute;
		}

		[[nodiscard]] constexpr auto Hours() const -> Rep
		{
			return Nanoseconds_ / NanosecondsPerHour;
		}

		[[nodiscard]] constexpr auto Days() const -> Rep
		{
			return Nanoseconds_ / NanosecondsPerDay;
		}

		[[nodiscard]] constexpr auto operator+() const -> Duration
		{
			return *this;
		}

		[[nodiscard]] constexpr auto operator-() const -> Duration
		{
			return Duration(-Nanoseconds_);
		}

		constexpr auto operator+=(Duration Other) -> Duration&
		{
			Nanoseconds_ += Other.Nanoseconds_;
			return *this;
		}

		constexpr auto operator-=(Duration Other) -> Duration&
		{
			Nanoseconds_ -= Other.Nanoseconds_;
			return *this;
		}

		constexpr auto operator*=(Rep Factor) -> Duration&
		{
			Nanoseconds_ *= Factor;
			return *this;
		}

		constexpr auto operator/=(Rep Divisor) -> Duration&
		{
			Nanoseconds_ /= Divisor;
			return *this;
		}

		[[nodiscard]] friend constexpr auto operator+(Duration Left, Duration Right) -> Duration
		{
			return Duration(Left.Nanoseconds_ + Right.Nanoseconds_);
		}

		[[nodiscard]] friend constexpr auto operator-(Duration Left, Duration Right) -> Duration
		{
			return Duration(Left.Nanoseconds_ - Right.Nanoseconds_);
		}

		[[nodiscard]] friend constexpr auto operator*(Duration Value, Rep Factor) -> Duration
		{
			return Duration(Value.Nanoseconds_ * Factor);
		}

		[[nodiscard]] friend constexpr auto operator*(Rep Factor, Duration Value) -> Duration
		{
			return Value * Factor;
		}

		[[nodiscard]] friend constexpr auto operator/(Duration Value, Rep Divisor) -> Duration
		{
			return Duration(Value.Nanoseconds_ / Divisor);
		}

		[[nodiscard]] friend constexpr auto operator/(Duration Left, Duration Right) -> Rep
		{
			return Left.Nanoseconds_ / Right.Nanoseconds_;
		}

		[[nodiscard]] constexpr auto operator<=>(const Duration&) const = default;

	private:
		Rep Nanoseconds_{};
	};
}