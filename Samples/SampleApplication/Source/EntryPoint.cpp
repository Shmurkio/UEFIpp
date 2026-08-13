#include <UEFIpp/UEFIpp.hpp>

[[nodiscard]] extern auto Main(
	const UEFIpp::Library::Vector<UEFIpp::Library::U8String>& Args
) -> UEFI::MainResult;

namespace
{
	[[nodiscard]] auto ParseArguments(
		const Foundation::WChar* Text,
		UEFIpp::Library::Vector<UEFIpp::Library::WideString>& Args
	) -> Foundation::Bool
	{
		Args.Clear();

		if (!Text)
		{
			return true;
		}

		UEFIpp::Library::WideString Current{};
		auto InQuotes = Foundation::Bool{ false };

		while (*Text)
		{
			const auto Character = *Text++;

			if (Character == L'"')
			{
				InQuotes = !InQuotes;
				continue;
			}

			if (!InQuotes && (Character == L' ' || Character == L'\t'))
			{
				if (!Current.Empty())
				{
					if (!Args.PushBack(Current))
					{
						Args.Clear();
						return false;
					}
					Current.Clear();
				}

				continue;
			}

			if (!Current.PushBack(Character))
			{
				Args.Clear();
				return false;
			}
		}

		if (!Current.Empty())
		{
			if (!Args.PushBack(Current))
			{
				Args.Clear();
				return false;
			}
		}

		return true;
	}
}

extern "C" auto EfiMain(
	UEFI::Handle ImageHandle,
	UEFI::Table::System* SystemTable
) -> UEFI::StatusValue
{
	if (!UEFI::Context::Normalize(ImageHandle, SystemTable))
	{
		return UEFI::ToStatusValue(UEFI::StatusCode::InvalidParameter);
	}

	if (!UEFI::Context::HasConsoleIn() || !UEFI::Context::HasConsoleOut())
	{
		return UEFI::ToStatusValue(UEFI::StatusCode::Unsupported);
	}

	Protocols::Access Access(&UEFI::Context::BootServices());
	auto LoadedImage = Access.Handle<Protocols::LoadedImage>(ImageHandle);

	UEFIpp::Library::Vector<UEFIpp::Library::WideString> RawArgs{};

	if (LoadedImage && (*LoadedImage)->LoadOptions && (*LoadedImage)->LoadOptionsSize)
	{
		const auto* CommandLine = Foundation::Cast::Auto<const Foundation::WChar*>(
			(*LoadedImage)->LoadOptions
		);
		if (!ParseArguments(CommandLine, RawArgs))
		{
			return UEFI::ToStatusValue(UEFI::StatusCode::OutOfResources);
		}
	}

	UEFIpp::Library::Vector<UEFIpp::Library::U8String> Args{};

	for (const auto& Arg : RawArgs)
	{
		auto Utf8 = UEFIpp::IO::WideToUtf8(Arg.View());
		if (!Utf8 || !Args.PushBack(Foundation::Utility::Move(Utf8.Value())))
		{
			return UEFI::ToStatusValue(UEFI::StatusCode::OutOfResources);
		}
	}

	const auto Result = Main(Args);
	const auto Flushed = IO::SystemIO().Flush();

	if (!Result)
	{
		return UEFI::ToStatusValue(Result);
	}

	if (!Flushed)
	{
		return UEFI::ToStatusValue(
			Flushed.Error().Code == IO::ErrorCode::Firmware
				? Flushed.Error().Status.Code()
				: UEFI::StatusCode::DeviceError
		);
	}

	return UEFI::ToStatusValue(UEFI::StatusCode::Success);
}
