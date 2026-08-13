#include <UEFIpp/UEFIpp.hpp>

namespace
{
	auto DriverUnload(
		[[maybe_unused]] UEFI::Handle ImageHandle
	) -> UEFI::StatusCode
	{
		UEFIPP_LOG(IO::SystemIO().Log(), IO::Severity::Info,
			"SampleDxe unloaded");

		return UEFI::StatusCode::Success;
	}
}

[[nodiscard]] auto Main() -> UEFI::MainResult
{
	UEFIPP_LOG(IO::SystemIO().Log(), IO::Severity::Info, "SampleDxe loaded");

	Protocols::Access Access(
		&UEFI::Context::BootServices()
	);

	auto LoadedImage = Access.Handle<Protocols::LoadedImage>(
		UEFI::Context::ImageHandle()
	);

	if (!LoadedImage)
	{
		UEFIPP_LOG(IO::SystemIO().Log(), IO::Severity::Error,
			"Loaded Image protocol is unavailable");

		return MakeUnexpected(UEFI::StatusCode::NotFound);
	}

	(*LoadedImage)->Unload = DriverUnload;

	return {};
}
