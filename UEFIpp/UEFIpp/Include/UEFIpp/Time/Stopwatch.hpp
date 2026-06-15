#pragma once

#include <UEFIpp/Library/Containers/Optional.hpp>
#include <UEFIpp/Time/Clock.hpp>

namespace UEFIpp::Time
{
	class Stopwatch
	{
	public:
		constexpr Stopwatch() = default;
		constexpr Stopwatch(const Stopwatch&) = default;
		constexpr Stopwatch(Stopwatch&&) noexcept = default;

		constexpr auto operator=(const Stopwatch&) -> Stopwatch & = default;
		constexpr auto operator=(Stopwatch&&) noexcept -> Stopwatch & = default;

		~Stopwatch() = default;

		explicit constexpr Stopwatch(Instant Start) : Start_(Start), Last_(Start)
		{
		}

		[[nodiscard]] static auto StartNew() -> Library::Optional<Stopwatch>
		{
			const auto Start = Clock::NowInstant();

			if (!Start)
			{
				return Library::NullOpt;
			}

			return Stopwatch(*Start);
		}

		[[nodiscard]] auto Start() const -> Instant
		{
			return Start_;
		}

		[[nodiscard]] auto Last() const -> Instant
		{
			return Last_;
		}

		[[nodiscard]] auto Elapsed() const -> Library::Optional<Duration>
		{
			const auto Now = Clock::NowInstant();

			if (!Now)
			{
				return Library::NullOpt;
			}

			return *Now - Start_;
		}

		[[nodiscard]] auto Lap() -> Library::Optional<Duration>
		{
			const auto Now = Clock::NowInstant();

			if (!Now)
			{
				return Library::NullOpt;
			}

			const auto Result = *Now - Last_;
			Last_ = *Now;

			return Result;
		}

		auto Reset() -> Foundation::Bool
		{
			const auto Now = Clock::NowInstant();

			if (!Now)
			{
				return false;
			}

			Start_ = *Now;
			Last_ = *Now;

			return true;
		}

		auto Reset(Instant Start) -> void
		{
			Start_ = Start;
			Last_ = Start;
		}

	private:
		Instant Start_{};
		Instant Last_{};
	};
}