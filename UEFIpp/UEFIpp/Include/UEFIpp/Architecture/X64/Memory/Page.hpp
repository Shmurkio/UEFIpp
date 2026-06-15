#pragma once

#include <UEFIpp/Architecture/X64/Common/Registers.hpp>

namespace UEFIpp::Architecture::X64
{
	class Page
	{
	public:
		Page() = delete;

		static constexpr auto Size = Foundation::Uint64{ 0x1000 };
		static constexpr auto Shift = Foundation::Uint64{ 12 };
		static constexpr auto Mask = Size - 1;
		static constexpr auto FrameMask = Foundation::Uint64{ 0x000FFFFFFFFFF000 };

		static constexpr auto Large2MbSize = Foundation::Uint64{ 0x200000 };
		static constexpr auto Large2MbMask = Foundation::Uint64{ 0x000FFFFFFFE00000 };
		static constexpr auto Large2MbOffsetMask = Large2MbSize - 1;

		static constexpr auto Large1GbSize = Foundation::Uint64{ 0x40000000 };
		static constexpr auto Large1GbMask = Foundation::Uint64{ 0x000FFFFFC0000000 };
		static constexpr auto Large1GbOffsetMask = Large1GbSize - 1;

		static constexpr auto EntriesPerTable = Foundation::Size{ 512 };

		[[nodiscard]] static constexpr auto AlignDown(Foundation::Uint64 Value) -> Foundation::Uint64
		{
			return Foundation::Bit::AlignDown(Value, Size);
		}

		[[nodiscard]] static constexpr auto AlignUp(Foundation::Uint64 Value) -> Foundation::Uint64
		{
			return Foundation::Bit::AlignUp(Value, Size);
		}

		[[nodiscard]] static constexpr auto IsAligned(Foundation::Uint64 Value) -> Foundation::Bool
		{
			return Foundation::Bit::IsAligned(Value, Size);
		}
	};
}