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

	return UEFI::ToStatusValue(Main());
}