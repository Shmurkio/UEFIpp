#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Memory/Memory.hpp>

namespace UEFIpp::Hooking::X64
{
	class AbsoluteJump
	{
	public:
		AbsoluteJump() = delete;

		static constexpr auto Size = Foundation::Size{ 12 };

		static auto Make(Foundation::Uint8(&Buffer)[Size], Foundation::UintPtr Destination) -> Foundation::Void
		{
			Buffer[0] = 0x48;
			Buffer[1] = 0xB8;
			Memory::Copy(Buffer + 2, &Destination, sizeof(Destination));
			Buffer[10] = 0xFF;
			Buffer[11] = 0xE0;
		}
	};
}