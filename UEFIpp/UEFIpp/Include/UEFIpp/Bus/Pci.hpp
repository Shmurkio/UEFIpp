#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

#include <UEFIpp/Bus/PciDevice.hpp>

#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/Containers/Optional.hpp>

namespace UEFIpp::Bus
{
	class Pci
	{
	public:
		[[nodiscard]] static auto Enumerate() -> Library::Vector<PciDevice>;
		[[nodiscard]] static auto FindByClass(Foundation::Uint8 Class, Foundation::Uint8 SubClass, Foundation::Uint8 ProgIf = 0xFF) -> Library::Vector<PciDevice>;
		[[nodiscard]] static auto FindById(Foundation::Uint16 VendorId, Foundation::Uint16 DeviceId) -> Library::Optional<PciDevice>;
	};
}