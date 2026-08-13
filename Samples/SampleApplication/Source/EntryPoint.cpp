#include <UEFIpp/UEFIpp.hpp>

[[nodiscard]] extern auto Main(
	const UEFIpp::Library::Vector<UEFIpp::Library::String>& Args
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

	UEFIpp::Library::Vector<UEFIpp::Library::String> Args{};

	for (const auto& Arg : RawArgs)
	{
		auto Narrow = UEFIpp::Text::Encoding::WideToAscii(Arg.View());
		if (!Args.PushBack(Narrow))
		{
			return UEFI::ToStatusValue(UEFI::StatusCode::OutOfResources);
		}
	}

	return UEFI::ToStatusValue(Main(Args));
}