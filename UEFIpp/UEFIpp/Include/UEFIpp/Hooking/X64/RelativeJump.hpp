#pragma once

#include <UEFIpp/Hooking/X64/Instruction.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Memory/Memory.hpp>

namespace UEFIpp::Hooking::X64
{
	class RelativeJump
	{
	public:
		RelativeJump() = delete;

		static constexpr auto Opcode = Foundation::Uint8{ 0xE9 };
		static constexpr auto Size = Foundation::Size{ 5 };

		[[nodiscard]] static auto IsJump(const Foundation::Void* Address) -> Foundation::Bool
		{
			if (!Address)
			{
				return false;
			}

			const auto Bytes = Foundation::Cast::Auto<const Foundation::Uint8*>(Address);
			return Bytes[0] == Opcode;
		}

		[[nodiscard]] static auto Target(const Foundation::Void* Address) -> Foundation::UintPtr
		{
			if (!IsJump(Address))
			{
				return 0;
			}

			Foundation::Int32 Relative{};
			const auto Bytes = Foundation::Cast::Auto<const Foundation::Uint8*>(Address);
			Memory::Copy(&Relative, Bytes + 1, sizeof(Relative));

			return Instruction::DecodeRelative32(Foundation::Cast::Auto<Foundation::UintPtr>(Address), Relative, Size);
		}

		[[nodiscard]] static auto Make(Foundation::Uint8(&Buffer)[Size], Foundation::UintPtr Source, Foundation::UintPtr Destination) -> Foundation::Bool
		{
			Foundation::Int32 Relative{};
			if (!Instruction::EncodeRelative32(Source, Destination, Size, Relative))
			{
				Memory::Zero(Buffer, Size);
				return false;
			}

			Buffer[0] = Opcode;
			Memory::Copy(Buffer + 1, &Relative, sizeof(Relative));
			return true;
		}
	};
}