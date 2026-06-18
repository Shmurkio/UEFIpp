#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Bus
{
	namespace PciClass
	{
		inline constexpr Foundation::Uint8 MassStorage = 0x01;
		inline constexpr Foundation::Uint8 Network = 0x02;
		inline constexpr Foundation::Uint8 Display = 0x03;
		inline constexpr Foundation::Uint8 Multimedia = 0x04;
		inline constexpr Foundation::Uint8 Memory = 0x05;
		inline constexpr Foundation::Uint8 Bridge = 0x06;
		inline constexpr Foundation::Uint8 Communication = 0x07;
		inline constexpr Foundation::Uint8 SerialBus = 0x0C;
	}

	namespace PciSubClass
	{
		inline constexpr Foundation::Uint8 Ahci = 0x06;
		inline constexpr Foundation::Uint8 Nvme = 0x08;
		inline constexpr Foundation::Uint8 Ethernet = 0x00;
		inline constexpr Foundation::Uint8 HdAudio = 0x03;
	}
}