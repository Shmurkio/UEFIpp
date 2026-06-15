#pragma once

#include <UEFIpp/Architecture/X64/Cpu/Cpu.hpp>
#include <UEFIpp/Architecture/X64/Memory/VirtualAddress.hpp>
#include <UEFIpp/Architecture/X64/Memory/PageTables.hpp>

namespace UEFIpp::Architecture::X64
{
	enum class PageWalkLevel : Foundation::Uint8
	{
		None,
		Pml4,
		Pdpt,
		Pd,
		Pt
	};

	class PageWalk
	{
	public:
		VirtualAddress Virtual{};
		PhysicalAddress Cr3{};
		PhysicalAddress Pml4Physical{};
		PhysicalAddress PdptPhysical{};
		PhysicalAddress PdPhysical{};
		PhysicalAddress PtPhysical{};

		PageTableEntry Pml4Entry{};
		PageTableEntry PdptEntry{};
		PageTableEntry PdEntry{};
		PageTableEntry PtEntry{};

		PageWalkLevel LeafLevel{ PageWalkLevel::None };

		[[nodiscard]] constexpr auto Success() const -> Foundation::Bool
		{
			return LeafLevel != PageWalkLevel::None;
		}

		[[nodiscard]] constexpr auto IsLargePage() const -> Foundation::Bool
		{
			return LeafLevel == PageWalkLevel::Pdpt || LeafLevel == PageWalkLevel::Pd;
		}
	};

	class Paging
	{
	public:
		Paging() = delete;

		[[nodiscard]] static constexpr auto Offset(Foundation::Uint64 Address, Foundation::Uint64 Mask) -> Foundation::Uint64
		{
			return Address & Mask;
		}

		[[nodiscard]] static auto CurrentCr3() -> PhysicalAddress
		{
			return PhysicalAddress{ Cr3::Address(Cpu::ReadCr3()) };
		}

		[[nodiscard]] static constexpr auto AlignDown(Foundation::Uint64 Value) -> Foundation::Uint64
		{
			return Page::AlignDown(Value);
		}

		[[nodiscard]] static constexpr auto AlignUp(Foundation::Uint64 Value) -> Foundation::Uint64
		{
			return Page::AlignUp(Value);
		}

		[[nodiscard]] static constexpr auto IsPageAligned(Foundation::Uint64 Value) -> Foundation::Bool
		{
			return Page::IsAligned(Value);
		}

		[[nodiscard]] static constexpr auto IsCanonical(Foundation::Uint64 Value) -> Foundation::Bool
		{
			return VirtualAddress{ Value }.IsCanonical();
		}

		[[nodiscard]] static constexpr auto PageCountForBytes(Foundation::Size NumberOfBytes) -> Foundation::UintN
		{
			if (!NumberOfBytes)
			{
				return 0;
			}

			return Foundation::Cast::Auto<Foundation::UintN>(Page::AlignUp(NumberOfBytes) / Page::Size);
		}

		[[nodiscard]] static constexpr auto PageFrame(Foundation::Uint64 Value) -> Foundation::Uint64
		{
			return Value & Page::FrameMask;
		}

		[[nodiscard]] static constexpr auto PhysicalFromLeaf(const PageWalk& Walk, Foundation::Uint64 VirtualValue) -> PhysicalAddress
		{
			switch (Walk.LeafLevel)
			{
			case PageWalkLevel::Pt:
			{
				return CombinePageAndOffset(Walk.PtEntry.Value, VirtualValue, Page::Mask);
			}
			case PageWalkLevel::Pd:
			{
				return CombinePageAndOffset(Walk.PdEntry.Value, VirtualValue, Page::Large2MbOffsetMask);
			}
			case PageWalkLevel::Pdpt:
			{
				return CombinePageAndOffset(Walk.PdptEntry.Value, VirtualValue, Page::Large1GbOffsetMask);
			}
			default:
			{
				return PhysicalAddress{};
			}
			}
		}

	private:
		[[nodiscard]] static constexpr auto CombinePageAndOffset(Register LeafEntryValue, Foundation::Uint64 Virtual, Foundation::Uint64 OffsetMask) -> PhysicalAddress
		{
			return PhysicalAddress{ Foundation::Bit::Clear(LeafEntryValue, OffsetMask) | Offset(Virtual, OffsetMask) };
		}
	};
}