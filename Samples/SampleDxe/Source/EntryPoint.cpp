#include <UEFIpp/UEFIpp.hpp>

[[nodiscard]] extern auto Main() -> UEFI::MainResult;

extern "C" auto EfiMain(
	UEFI::Handle ImageHandle,
	UEFI::Table::System* SystemTable
) -> UEFI::StatusValue
{
	if (!UEFI::Context::Normalize(ImageHandle, SystemTable))
	{
		return UEFI::ToStatusValue(UEFI::StatusCode::InvalidParameter);
	}

	const auto Result = Main();
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
