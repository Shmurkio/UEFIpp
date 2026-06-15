#pragma once

#include <UEFIpp/Architecture/X64/Memory/PhysicalAddress.hpp>

namespace UEFIpp::Architecture::X64
{
	class PageTableEntryFlags
	{
	public:
		PageTableEntryFlags() = delete;

		static constexpr auto Present = Foundation::Bit::Mask<Register>(0);
		static constexpr auto Write = Foundation::Bit::Mask<Register>(1);
		static constexpr auto User = Foundation::Bit::Mask<Register>(2);
		static constexpr auto WriteThrough = Foundation::Bit::Mask<Register>(3);
		static constexpr auto CacheDisable = Foundation::Bit::Mask<Register>(4);
		static constexpr auto Accessed = Foundation::Bit::Mask<Register>(5);
		static constexpr auto Dirty = Foundation::Bit::Mask<Register>(6);
		static constexpr auto Large = Foundation::Bit::Mask<Register>(7);
		static constexpr auto Global = Foundation::Bit::Mask<Register>(8);

		static constexpr auto AvailableLowOffset = Register{ 9 };
		static constexpr auto AvailableLowWidth = Register{ 3 };
		static constexpr auto AvailableLowMask = Foundation::Bit::RangeMask<Register>(AvailableLowOffset, AvailableLowWidth);

		static constexpr auto AvailableHighOffset = Register{ 52 };
		static constexpr auto AvailableHighWidth = Register{ 11 };
		static constexpr auto AvailableHighMask = Foundation::Bit::RangeMask<Register>(AvailableHighOffset, AvailableHighWidth);

		static constexpr auto NoExecute = Foundation::Bit::Mask<Register>(63);

		static constexpr auto DefaultTable = Present | Write;

		static constexpr auto DefaultPage = Present | Write;
	};

	class PageTableEntry
	{
	public:
		constexpr PageTableEntry() = default;

		constexpr explicit PageTableEntry(Register Value)
			: Value(Value)
		{
		}

		[[nodiscard]] constexpr auto IsPresent() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(Value, PageTableEntryFlags::Present);
		}

		[[nodiscard]] constexpr auto IsWritable() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(Value, PageTableEntryFlags::Write);
		}

		[[nodiscard]] constexpr auto IsUserAccessible() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(Value, PageTableEntryFlags::User);
		}

		[[nodiscard]] constexpr auto IsWriteThrough() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(Value, PageTableEntryFlags::WriteThrough);
		}

		[[nodiscard]] constexpr auto IsCacheDisabled() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(Value, PageTableEntryFlags::CacheDisable);
		}

		[[nodiscard]] constexpr auto IsAccessed() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(Value, PageTableEntryFlags::Accessed);
		}

		[[nodiscard]] constexpr auto IsDirty() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(Value, PageTableEntryFlags::Dirty);
		}

		[[nodiscard]] constexpr auto IsLarge() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(Value, PageTableEntryFlags::Large);
		}

		[[nodiscard]] constexpr auto IsGlobal() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(Value, PageTableEntryFlags::Global);
		}

		[[nodiscard]] constexpr auto IsExecutable() const -> Foundation::Bool
		{
			return !Foundation::Bit::IsSet(Value, PageTableEntryFlags::NoExecute);
		}

		[[nodiscard]] constexpr auto IsNoExecute() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(Value, PageTableEntryFlags::NoExecute);
		}

		[[nodiscard]] constexpr auto Physical() const -> PhysicalAddress
		{
			return PhysicalAddress{ Value & Page::FrameMask };
		}

		constexpr auto SetPhysical(PhysicalAddress Address) -> Foundation::Void
		{
			Value = Foundation::Bit::Replace(Value, Page::FrameMask, Address.PageFrame());
		}

		[[nodiscard]] constexpr auto Flags() const -> Register
		{
			return Value & ~Page::FrameMask;
		}

		constexpr auto SetFlags(Register Flags) -> Foundation::Void
		{
			Value = Physical().PageFrame() | Flags;
		}

		constexpr auto AddFlags(Register Flags) -> Foundation::Void
		{
			Value = Foundation::Bit::Set(Value, Flags);
		}

		constexpr auto RemoveFlags(Register Flags) -> Foundation::Void
		{
			Value = Foundation::Bit::Clear(Value, Flags);
		}

		constexpr auto SetFlag(Register Flag, Foundation::Bool Enabled) -> Foundation::Void
		{
			Value = Foundation::Bit::Assign(Value, Flag, Enabled);
		}

		constexpr auto Clear() -> Foundation::Void
		{
			Value = 0;
		}

		[[nodiscard]] constexpr auto operator<=>(const PageTableEntry&) const = default;

		Register Value{};
	};

	class PageTable
	{
	public:
		PageTableEntry Entries[Page::EntriesPerTable]{};

		[[nodiscard]] constexpr auto operator[](Foundation::Size Index) -> PageTableEntry&
		{
			return Entries[Index];
		}

		[[nodiscard]] constexpr auto operator[](Foundation::Size Index) const -> const PageTableEntry&
		{
			return Entries[Index];
		}

		[[nodiscard]] constexpr auto Data() -> PageTableEntry*
		{
			return Entries;
		}

		[[nodiscard]] constexpr auto Data() const -> const PageTableEntry*
		{
			return Entries;
		}

		[[nodiscard]] static constexpr auto Count() -> Foundation::Size
		{
			return Page::EntriesPerTable;
		}
	};

	static_assert(sizeof(PageTableEntry) == 8);
	static_assert(sizeof(PageTable) == Page::Size);
	static_assert(Foundation::Traits::IsStandardLayout<PageTableEntry>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<PageTableEntry>::Value);
	static_assert(Foundation::Traits::IsStandardLayout<PageTable>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<PageTable>::Value);
}