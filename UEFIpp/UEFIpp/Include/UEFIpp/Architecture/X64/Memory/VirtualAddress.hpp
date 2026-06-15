#pragma once

#include <UEFIpp/Architecture/X64/Common/Registers.hpp>
#include <UEFIpp/Architecture/X64/Memory/Page.hpp>

namespace UEFIpp::Architecture::X64
{
	class VirtualAddress
	{
	public:
		static constexpr auto OffsetOffset = Foundation::Uint64{ 0 };
		static constexpr auto OffsetWidth = Foundation::Uint64{ 12 };

		static constexpr auto PtIndexOffset = Foundation::Uint64{ 12 };
		static constexpr auto PtIndexWidth = Foundation::Uint64{ 9 };

		static constexpr auto PdIndexOffset = Foundation::Uint64{ 21 };
		static constexpr auto PdIndexWidth = Foundation::Uint64{ 9 };

		static constexpr auto PdptIndexOffset = Foundation::Uint64{ 30 };
		static constexpr auto PdptIndexWidth = Foundation::Uint64{ 9 };

		static constexpr auto Pml4IndexOffset = Foundation::Uint64{ 39 };
		static constexpr auto Pml4IndexWidth = Foundation::Uint64{ 9 };

		static constexpr auto SignExtensionOffset = Foundation::Uint64{ 48 };
		static constexpr auto SignExtensionWidth = Foundation::Uint64{ 16 };

		constexpr VirtualAddress() = default;

		constexpr explicit VirtualAddress(Foundation::Uint64 Value) : Value(Value)
		{
		}

		[[nodiscard]] constexpr auto IsCanonical() const -> Foundation::Bool
		{
			return Foundation::Bit::SignExtend(Value, Foundation::Uint64{ 48 }) == Value;
		}

		[[nodiscard]] constexpr auto IsPageAligned() const -> Foundation::Bool
		{
			return Page::IsAligned(Value);
		}

		[[nodiscard]] constexpr auto Offset() const -> Foundation::Uint16
		{
			return Foundation::Cast::Auto<Foundation::Uint16>(Foundation::Bit::Extract(Value, OffsetOffset, OffsetWidth));
		}

		[[nodiscard]] constexpr auto PtIndex() const -> Foundation::Uint16
		{
			return Foundation::Cast::Auto<Foundation::Uint16>(Foundation::Bit::Extract(Value, PtIndexOffset, PtIndexWidth));
		}

		[[nodiscard]] constexpr auto PdIndex() const -> Foundation::Uint16
		{
			return Foundation::Cast::Auto<Foundation::Uint16>(Foundation::Bit::Extract(Value, PdIndexOffset, PdIndexWidth));
		}

		[[nodiscard]] constexpr auto PdptIndex() const -> Foundation::Uint16
		{
			return Foundation::Cast::Auto<Foundation::Uint16>(Foundation::Bit::Extract(Value, PdptIndexOffset, PdptIndexWidth));
		}

		[[nodiscard]] constexpr auto Pml4Index() const -> Foundation::Uint16
		{
			return Foundation::Cast::Auto<Foundation::Uint16>(Foundation::Bit::Extract(Value, Pml4IndexOffset, Pml4IndexWidth));
		}

		[[nodiscard]] constexpr auto AlignDown() const -> VirtualAddress
		{
			return VirtualAddress{ Page::AlignDown(Value) };
		}

		[[nodiscard]] constexpr auto AlignUp() const -> VirtualAddress
		{
			return VirtualAddress{ Page::AlignUp(Value) };
		}

		[[nodiscard]] constexpr auto operator<=>(const VirtualAddress&) const = default;

		Foundation::Uint64 Value{};
	};
}