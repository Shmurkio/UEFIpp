#include <UEFIpp/UEFIpp.hpp>

namespace
{
	auto DriverUnload(
		[[maybe_unused]] UEFI::Handle ImageHandle
	) -> UEFI::StatusCode
	{
		Stream::Out::Serial
			<< Trace() << "SampleDxe unloaded" << Stream::Endl;

		return UEFI::StatusCode::Success;
	}
}

[[nodiscard]] auto Main() -> UEFI::MainResult
{
	Stream::Out::Serial
		<< Trace() << "SampleDxe loaded" << Stream::Endl;

	Protocols::Access Access(
		&UEFI::Context::BootServices()
	);

	auto LoadedImage = Access.Handle<Protocols::LoadedImage>(
		UEFI::Context::ImageHandle()
	);

	if (!LoadedImage)
	{
		Stream::Out::Serial
			<< Trace() << "Loaded Image protocol is unavailable" << Stream::Endl;

		return MakeUnexpected(UEFI::StatusCode::NotFound);
	}

	(*LoadedImage)->Unload = DriverUnload;

	return {};
}