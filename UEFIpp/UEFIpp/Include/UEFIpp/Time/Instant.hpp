#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Time/Duration.hpp>

namespace UEFIpp::Time
{
	class Instant
	{
	public:
		using Rep = Foundation::Int64;

	public:
		constexpr Instant() = default;
		constexpr Instant(const Instant&) = default;
		constexpr Instant(Instant&&) noexcept = default;

		constexpr auto operator=(const Instant&) -> Instant & = default;
		constexpr auto operator=(Instant&&) noexcept -> Instant & = default;

		~Instant() = default;

		explicit constexpr Instant(Rep NanosecondsSinceEpoch)
			: NanosecondsSinceEpoch_(NanosecondsSinceEpoch)
		{
		}

		[[nodiscard]] static constexpr auto Epoch() -> Instant
		{
			return Instant{};
		}

		[[nodiscard]] static constexpr auto FromUnixSeconds(Rep Seconds) -> Instant
		{
			return Instant(Seconds * Duration::NanosecondsPerSecond);
		}

		[[nodiscard]] static constexpr auto FromUnixMilliseconds(Rep Milliseconds) -> Instant
		{
			return Instant(Milliseconds * Duration::NanosecondsPerMillisecond);
		}

		[[nodiscard]] static constexpr auto FromUnixMicroseconds(Rep Microseconds) -> Instant
		{
			return Instant(Microseconds * Duration::NanosecondsPerMicrosecond);
		}

		[[nodiscard]] static constexpr auto FromUnixNanoseconds(Rep Nanoseconds) -> Instant
		{
			return Instant(Nanoseconds);
		}

		[[nodiscard]] constexpr auto UnixNanoseconds() const -> Rep
		{
			return NanosecondsSinceEpoch_;
		}

		[[nodiscard]] constexpr auto UnixMicroseconds() const -> Rep
		{
			return NanosecondsSinceEpoch_ / Duration::NanosecondsPerMicrosecond;
		}

		[[nodiscard]] constexpr auto UnixMilliseconds() const -> Rep
		{
			return NanosecondsSinceEpoch_ / Duration::NanosecondsPerMillisecond;
		}

		[[nodiscard]] constexpr auto UnixSeconds() const -> Rep
		{
			return NanosecondsSinceEpoch_ / Duration::NanosecondsPerSecond;
		}

		constexpr auto operator+=(Duration Value) -> Instant&
		{
			NanosecondsSinceEpoch_ += Value.Nanoseconds();
			return *this;
		}

		constexpr auto operator-=(Duration Value) -> Instant&
		{
			NanosecondsSinceEpoch_ -= Value.Nanoseconds();
			return *this;
		}

		[[nodiscard]] friend constexpr auto operator+(Instant Value, Duration Offset) -> Instant
		{
			return Instant(Value.NanosecondsSinceEpoch_ + Offset.Nanoseconds());
		}

		[[nodiscard]] friend constexpr auto operator+(Duration Offset, Instant Value) -> Instant
		{
			return Value + Offset;
		}

		[[nodiscard]] friend constexpr auto operator-(Instant Value, Duration Offset) -> Instant
		{
			return Instant(Value.NanosecondsSinceEpoch_ - Offset.Nanoseconds());
		}

		[[nodiscard]] friend constexpr auto operator-(Instant Left, Instant Right) -> Duration
		{
			return Duration::FromNanoseconds(
				Left.NanosecondsSinceEpoch_ - Right.NanosecondsSinceEpoch_
			);
		}

		[[nodiscard]] constexpr auto operator<=>(const Instant&) const = default;

	private:
		Rep NanosecondsSinceEpoch_{};
	};
}