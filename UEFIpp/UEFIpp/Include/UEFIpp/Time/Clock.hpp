#pragma once

#include <UEFIpp/Library/Containers/Optional.hpp>
#include <UEFIpp/Time/DateTime.hpp>
#include <UEFIpp/UEFI/Context.hpp>
#include <UEFIpp/UEFI/Status.hpp>

namespace UEFIpp::Time
{
	class Capabilities
	{
	public:
		Foundation::Uint32 Resolution{};
		Foundation::Uint32 Accuracy{};
		Foundation::Bool SetsToZero{};
	};

	class Clock
	{
	public:
		Clock() = delete;

		[[nodiscard]] static auto Now() -> Library::Optional<DateTime>
		{
			UEFI::Table::Time EfiTime{};
			UEFI::Table::TimeCapabilities EfiCapabilities{};

			const auto Status = UEFI::Context::RuntimeServices().GetTime(&EfiTime, &EfiCapabilities);

			if (UEFI::IsError(Status))
			{
				return Library::NullOpt;
			}

			return DateTime::FromEfiTime(EfiTime);
		}

		[[nodiscard]] static auto NowInstant() -> Library::Optional<Instant>
		{
			const auto Current = Now();

			if (!Current)
			{
				return Library::NullOpt;
			}

			return Current->ToInstant();
		}

		[[nodiscard]] static auto UnixNanoseconds() -> Library::Optional<Foundation::Int64>
		{
			const auto Current = NowInstant();

			if (!Current)
			{
				return Library::NullOpt;
			}

			return Current->UnixNanoseconds();
		}

		[[nodiscard]] static auto UnixMilliseconds() -> Library::Optional<Foundation::Int64>
		{
			const auto Current = NowInstant();

			if (!Current)
			{
				return Library::NullOpt;
			}

			return Current->UnixMilliseconds();
		}

		[[nodiscard]] static auto UnixSeconds() -> Library::Optional<Foundation::Int64>
		{
			const auto Current = NowInstant();

			if (!Current)
			{
				return Library::NullOpt;
			}

			return Current->UnixSeconds();
		}

		[[nodiscard]] static auto GetCapabilities() -> Library::Optional<Capabilities>
		{
			UEFI::Table::Time EfiTime{};
			UEFI::Table::TimeCapabilities EfiCapabilities{};

			const auto Status = UEFI::Context::RuntimeServices().GetTime(&EfiTime, &EfiCapabilities);

			if (UEFI::IsError(Status))
			{
				return Library::NullOpt;
			}

			return Capabilities{ EfiCapabilities.Resolution, EfiCapabilities.Accuracy, EfiCapabilities.SetsToZero };
		}

		[[nodiscard]] static auto Set(DateTime Value) -> UEFI::Status
		{
			auto EfiTime = Value.ToEfiTime();
			return UEFI::Status(UEFI::Context::RuntimeServices().SetTime(&EfiTime));
		}
	};
}