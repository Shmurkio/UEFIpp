#pragma once

#include <UEFIpp/Library/Containers/Expected.hpp>
#include <UEFIpp/UEFI/Status.hpp>

namespace UEFIpp::UEFI
{
	using MainResult = Library::Expected<Foundation::Void, StatusCode>;

	[[nodiscard]] inline auto ToStatusValue(
		const MainResult& Result
	) -> StatusValue
	{
		return Result
			? ToStatusValue(StatusCode::Success)
			: ToStatusValue(Result.Error());
	}
}
