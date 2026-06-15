#pragma once

#include <UEFIpp/Stream/In/Common.hpp>
#include <UEFIpp/Stream/Out/Out.hpp>
#include <UEFIpp/UEFI/BootServices.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/Protocols/SimpleTextInputEx.hpp>

namespace UEFIpp::Stream
{
	class ConsoleInputSource
	{
	public:
		constexpr ConsoleInputSource() = default;

		constexpr ConsoleInputSource(
			UEFI::Table::BootServices* BootServices,
			Protocols::SimpleTextInputEx* Input
		) : BootServices_(BootServices),
			Input_(Input)
		{
		}

		[[nodiscard]] constexpr auto Valid() const -> Foundation::Bool
		{
			return BootServices_ && Input_;
		}

		[[nodiscard]] constexpr explicit operator Foundation::Bool() const
		{
			return Valid();
		}

		auto Reset(
			UEFI::Table::BootServices* BootServices,
			Protocols::SimpleTextInputEx* Input
		) -> Foundation::Void
		{
			BootServices_ = BootServices;
			Input_ = Input;
			LineLength_ = 0;
		}

		[[nodiscard]] auto ReadKey() -> Protocols::KeyData
		{
			return ReadKey(false);
		}

		[[nodiscard]] auto ReadKeyEcho() -> Protocols::KeyData
		{
			return ReadKey(true);
		}

		[[nodiscard]] auto ReadKey(Foundation::Bool Echo) -> Protocols::KeyData
		{
			if (!Valid())
			{
				return {};
			}

			for (;;)
			{
				Foundation::UintN Index{};
				BootServices_->WaitForEvent(1, &Input_->WaitForKeyEx, &Index);

				Protocols::KeyData Key{};
				const auto Status = Input_->ReadKey(Key);

				if (UEFI::Status(Status).Succeeded())
				{
					if (Echo)
					{
						EchoKey(Key);
					}

					return Key;
				}
			}
		}

	private:
		auto EchoKey(const Protocols::KeyData& Key) -> Foundation::Void
		{
			if (!Key.HasUnicode())
			{
				return;
			}

			const auto Character = Key.Key.UnicodeChar;

			if (Character == L'\b')
			{
				if (LineLength_)
				{
					--LineLength_;
					Stream::Out::Console << "\b \b" << Stream::FlushNow;
				}

				return;
			}

			if (Character == L'\r' || Character == L'\n')
			{
				LineLength_ = 0;
				Stream::Out::Console << Stream::Endl;
				return;
			}

			++LineLength_;
			Stream::Out::Console << Character << Stream::FlushNow;
		}

	private:
		UEFI::Table::BootServices* BootServices_{};
		Protocols::SimpleTextInputEx* Input_{};
		Foundation::Size LineLength_{};
	};

	namespace In
	{
		inline Input<ConsoleInputSource> Console{};
	}
}