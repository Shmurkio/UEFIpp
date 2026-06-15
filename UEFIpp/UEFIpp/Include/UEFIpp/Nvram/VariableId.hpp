#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Guid.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Library/String/StringView.hpp>

namespace UEFIpp::Nvram
{
	class VariableId
	{
	public:
		VariableId() = default;

		VariableId(Library::StringView Name, const UEFI::Guid& VendorGuid) : Name_(Name), VendorGuid_(VendorGuid)
		{
		}

		[[nodiscard]] auto Name() const -> const Library::String&
		{
			return Name_;
		}

		[[nodiscard]] auto VendorGuid() const -> const UEFI::Guid&
		{
			return VendorGuid_;
		}

		[[nodiscard]] auto Empty() const -> Foundation::Bool
		{
			return Name_.Empty();
		}

	private:
		Library::String Name_{};
		UEFI::Guid VendorGuid_{};
	};
}