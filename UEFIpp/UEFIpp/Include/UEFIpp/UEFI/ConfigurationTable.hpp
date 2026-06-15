#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Guid.hpp>

namespace UEFIpp::UEFI::Table
{
	class Configuration
	{
	public:
		Guid VendorGuid;
		Foundation::Void* VendorTable;

		[[nodiscard]] constexpr auto operator<=>(const Configuration&) const = default;
	};

	static_assert(sizeof(Configuration) == 24);
	static_assert(Foundation::Traits::IsStandardLayout<Configuration>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<Configuration>::Value);
}