#pragma once

#include <UEFIpp/Architecture/X64/Common/Registers.hpp>
#include <UEFIpp/Architecture/X64/Memory/Page.hpp>

namespace UEFIpp::Architecture::X64
{
	class PhysicalAddress
	{
	public:
		constexpr PhysicalAddress() = default;

		constexpr explicit PhysicalAddress(Foundation::Uint64 Value) : Value(Value)
		{
		}

		[[nodiscard]] constexpr auto IsPageAligned() const -> Foundation::Bool
		{
			return Page::IsAligned(Value);
		}

		[[nodiscard]] constexpr auto PageFrame() const -> Foundation::Uint64
		{
			return Value & Page::FrameMask;
		}

		[[nodiscard]] constexpr auto Offset() const -> Foundation::Uint64
		{
			return Value & Page::Mask;
		}

		[[nodiscard]] constexpr auto AlignDown() const -> PhysicalAddress
		{
			return PhysicalAddress{ Page::AlignDown(Value) };
		}

		[[nodiscard]] constexpr auto AlignUp() const -> PhysicalAddress
		{
			return PhysicalAddress{ Page::AlignUp(Value) };
		}

		[[nodiscard]] constexpr auto operator<=>(const PhysicalAddress&) const = default;

		Foundation::Uint64 Value{};
	};
}