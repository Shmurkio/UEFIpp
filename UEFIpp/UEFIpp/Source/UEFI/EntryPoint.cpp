#include <UEFIpp/UEFIpp.hpp>

extern "C" void __chkstk()
{
}

extern "C" void _chkstk()
{
}

extern "C" int atexit(void (*)())
{
	return 0;
}

extern auto Main(const UEFIpp::Library::Vector<UEFIpp::Library::String>& Args) -> Foundation::Bool;

static auto ParseArguments(const Foundation::WChar* Text) -> UEFIpp::Library::Vector<UEFIpp::Library::WideString>
{
	UEFIpp::Library::Vector<UEFIpp::Library::WideString> Args{};

	if (!Text)
	{
		return Args;
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
				Args.PushBack(Current);
				Current.Clear();
			}

			continue;
		}

		Current.PushBack(Character);
	}

	if (!Current.Empty())
	{
		Args.PushBack(Current);
	}

	return Args;
}

auto EfipMain(UEFI::Handle ImageHandle, UEFI::Table::System* SystemTable) -> UEFI::StatusValue
{
	if (!UEFI::Context::Normalize(ImageHandle, SystemTable))
	{
		return UEFI::ToStatusValue(UEFI::StatusCode::InvalidParameter);
	}

	Protocols::Access Access(&UEFI::Context::BootServices());
	auto LoadedImage = Access.Handle<Protocols::LoadedImage>(ImageHandle);

	UEFIpp::Library::Vector<UEFIpp::Library::WideString> RawArgs{};

	if (LoadedImage && (*LoadedImage)->LoadOptions && (*LoadedImage)->LoadOptionsSize)
	{
		const auto* CommandLine = Foundation::Cast::Auto<const Foundation::WChar*>((*LoadedImage)->LoadOptions);
		RawArgs = ParseArguments(CommandLine);
	}

	UEFIpp::Library::Vector<UEFIpp::Library::String> Args{};

	for (const auto& Arg : RawArgs)
	{
		auto Narrow = UEFIpp::Text::Encoding::WideToAscii(Arg.View());
		Args.PushBack(Narrow);
	}

	if (!Main(Args))
	{
		return UEFI::ToStatusValue(UEFI::StatusCode::LoadError);
	}

	return UEFI::ToStatusValue(UEFI::StatusCode::Success);
}