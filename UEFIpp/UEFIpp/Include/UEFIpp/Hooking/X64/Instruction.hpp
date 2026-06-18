#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Hooking::X64
{
	class Instruction
	{
	public:
		Instruction() = delete;

		static constexpr auto Relative32Min = Foundation::Int64{ -0x80000000ll };
		static constexpr auto Relative32Max = Foundation::Int64{ 0x7fffffffll };

		[[nodiscard]] static constexpr auto IsRelative32Reachable(Foundation::UintPtr Source, Foundation::UintPtr Destination, Foundation::Size InstructionSize) -> Foundation::Bool
		{
			const auto SourceEnd = Foundation::Cast::Auto<Foundation::Int64>(Source + InstructionSize);
			const auto Target = Foundation::Cast::Auto<Foundation::Int64>(Destination);
			const auto Delta = Target - SourceEnd;

			return Delta >= Relative32Min && Delta <= Relative32Max;
		}

		[[nodiscard]] static constexpr auto EncodeRelative32(Foundation::UintPtr Source, Foundation::UintPtr Destination, Foundation::Size InstructionSize, Foundation::Int32& OutRelative) -> Foundation::Bool
		{
			const auto SourceEnd = Foundation::Cast::Auto<Foundation::Int64>(Source + InstructionSize);
			const auto Target = Foundation::Cast::Auto<Foundation::Int64>(Destination);
			const auto Delta = Target - SourceEnd;

			if (Delta < Relative32Min || Delta > Relative32Max)
			{
				OutRelative = 0;
				return false;
			}

			OutRelative = Foundation::Cast::Auto<Foundation::Int32>(Delta);
			return true;
		}

		[[nodiscard]] static constexpr auto DecodeRelative32(Foundation::UintPtr Source, Foundation::Int32 Relative, Foundation::Size InstructionSize) -> Foundation::UintPtr
		{
			const auto SourceEnd = Foundation::Cast::Auto<Foundation::Int64>(Source + InstructionSize);
			return Foundation::Cast::Auto<Foundation::UintPtr>(SourceEnd + Relative);
		}
	};
}