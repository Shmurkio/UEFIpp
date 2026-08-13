#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Architecture::X64
{
	using Register = Foundation::Uint64;

	class Cr0
	{
	public:
		Cr0() = delete;

		static constexpr auto ProtectionEnable = Foundation::Bit::Mask<Register>(0);
		static constexpr auto MonitorCoprocessor = Foundation::Bit::Mask<Register>(1);
		static constexpr auto Emulation = Foundation::Bit::Mask<Register>(2);
		static constexpr auto TaskSwitched = Foundation::Bit::Mask<Register>(3);
		static constexpr auto ExtensionType = Foundation::Bit::Mask<Register>(4);
		static constexpr auto NumericError = Foundation::Bit::Mask<Register>(5);
		static constexpr auto WriteProtect = Foundation::Bit::Mask<Register>(16);
		static constexpr auto AlignmentMask = Foundation::Bit::Mask<Register>(18);
		static constexpr auto NotWriteThrough = Foundation::Bit::Mask<Register>(29);
		static constexpr auto CacheDisable = Foundation::Bit::Mask<Register>(30);
		static constexpr auto Paging = Foundation::Bit::Mask<Register>(31);
	};

	class Cr3
	{
	public:
		Cr3() = delete;

		static constexpr auto PageWriteThrough = Foundation::Bit::Mask<Register>(3);
		static constexpr auto PageCacheDisable = Foundation::Bit::Mask<Register>(4);
		static constexpr auto AddressMask = Register{ 0x000FFFFFFFFFF000 };

		[[nodiscard]] static constexpr auto Address(Register Value) -> Register
		{
			return Value & AddressMask;
		}
	};

	class Cr4
	{
	public:
		Cr4() = delete;

		static constexpr auto VirtualModeExtensions = Foundation::Bit::Mask<Register>(0);
		static constexpr auto ProtectedModeVirtualInterrupts = Foundation::Bit::Mask<Register>(1);
		static constexpr auto TimeStampDisable = Foundation::Bit::Mask<Register>(2);
		static constexpr auto DebuggingExtensions = Foundation::Bit::Mask<Register>(3);
		static constexpr auto PageSizeExtensions = Foundation::Bit::Mask<Register>(4);
		static constexpr auto PhysicalAddressExtension = Foundation::Bit::Mask<Register>(5);
		static constexpr auto MachineCheckEnable = Foundation::Bit::Mask<Register>(6);
		static constexpr auto PageGlobalEnable = Foundation::Bit::Mask<Register>(7);
		static constexpr auto PerformanceMonitoringCounterEnable = Foundation::Bit::Mask<Register>(8);
		static constexpr auto OsFxsr = Foundation::Bit::Mask<Register>(9);
		static constexpr auto OsXmmExceptionSupport = Foundation::Bit::Mask<Register>(10);
		static constexpr auto UserModeInstructionPrevention = Foundation::Bit::Mask<Register>(11);
		static constexpr auto LinearAddress57 = Foundation::Bit::Mask<Register>(12);
		static constexpr auto Vmxe = Foundation::Bit::Mask<Register>(13);
		static constexpr auto Smxe = Foundation::Bit::Mask<Register>(14);
		static constexpr auto Fsgsbase = Foundation::Bit::Mask<Register>(16);
		static constexpr auto Pcide = Foundation::Bit::Mask<Register>(17);
		static constexpr auto OsXsave = Foundation::Bit::Mask<Register>(18);
		static constexpr auto Smep = Foundation::Bit::Mask<Register>(20);
		static constexpr auto Smap = Foundation::Bit::Mask<Register>(21);
		static constexpr auto ProtectionKeyEnable = Foundation::Bit::Mask<Register>(22);
	};

	class Rflags
	{
	public:
		Rflags() = delete;

		static constexpr auto Carry = Foundation::Bit::Mask<Register>(0);
		static constexpr auto Parity = Foundation::Bit::Mask<Register>(2);
		static constexpr auto AuxiliaryCarry = Foundation::Bit::Mask<Register>(4);
		static constexpr auto Zero = Foundation::Bit::Mask<Register>(6);
		static constexpr auto Sign = Foundation::Bit::Mask<Register>(7);
		static constexpr auto Trap = Foundation::Bit::Mask<Register>(8);
		static constexpr auto InterruptEnable = Foundation::Bit::Mask<Register>(9);
		static constexpr auto Direction = Foundation::Bit::Mask<Register>(10);
		static constexpr auto Overflow = Foundation::Bit::Mask<Register>(11);

		static constexpr auto IoPrivilegeLevelOffset = Register{ 12 };
		static constexpr auto IoPrivilegeLevelWidth = Register{ 2 };
		static constexpr auto IoPrivilegeLevelMask = Foundation::Bit::RangeMask<Register>(IoPrivilegeLevelOffset, IoPrivilegeLevelWidth);

		static constexpr auto NestedTask = Foundation::Bit::Mask<Register>(14);
		static constexpr auto Resume = Foundation::Bit::Mask<Register>(16);
		static constexpr auto Virtual8086 = Foundation::Bit::Mask<Register>(17);
		static constexpr auto AlignmentCheck = Foundation::Bit::Mask<Register>(18);
		static constexpr auto VirtualInterrupt = Foundation::Bit::Mask<Register>(19);
		static constexpr auto VirtualInterruptPending = Foundation::Bit::Mask<Register>(20);
		static constexpr auto Identification = Foundation::Bit::Mask<Register>(21);

		[[nodiscard]] static constexpr auto IoPrivilegeLevel(Register Value) -> Register
		{
			return Foundation::Bit::Extract(Value, IoPrivilegeLevelOffset, IoPrivilegeLevelWidth);
		}
	};

	class Efer
	{
	public:
		Efer() = delete;

		static constexpr auto SystemCallExtensions = Foundation::Bit::Mask<Register>(0);
		static constexpr auto LongModeEnable = Foundation::Bit::Mask<Register>(8);
		static constexpr auto LongModeActive = Foundation::Bit::Mask<Register>(10);
		static constexpr auto NoExecuteEnable = Foundation::Bit::Mask<Register>(11);
		static constexpr auto SecureVirtualMachineEnable = Foundation::Bit::Mask<Register>(12);
		static constexpr auto LongModeSegmentLimitEnable = Foundation::Bit::Mask<Register>(13);
		static constexpr auto FastFxsaveFxrstor = Foundation::Bit::Mask<Register>(14);
		static constexpr auto TranslationCacheExtension = Foundation::Bit::Mask<Register>(15);
	};

	class SegmentSelector
	{
	public:
		static constexpr Foundation::Uint16 IndexOffset = 3;
		static constexpr Foundation::Uint16 IndexWidth = 13;

		static constexpr Foundation::Uint16 TableOffset = 2;
		static constexpr Foundation::Uint16 RplOffset = 0;
		static constexpr Foundation::Uint16 RplWidth = 2;

		constexpr SegmentSelector() = default;
		constexpr SegmentSelector(const SegmentSelector&) = default;
		constexpr SegmentSelector(SegmentSelector&&) noexcept = default;

		constexpr auto operator=(const SegmentSelector&) -> SegmentSelector & = default;
		constexpr auto operator=(SegmentSelector&&) noexcept -> SegmentSelector & = default;

		~SegmentSelector() = default;

		explicit constexpr SegmentSelector(Foundation::Uint16 Value)
			: Value(Value)
		{
		}

		[[nodiscard]] constexpr auto Index() const -> Foundation::Uint16
		{
			return Foundation::Cast::Auto<Foundation::Uint16>(Foundation::Bit::Extract(Value, IndexOffset, IndexWidth));
		}

		[[nodiscard]] constexpr auto RequestedPrivilegeLevel() const -> Foundation::Uint16
		{
			return Foundation::Cast::Auto<Foundation::Uint16>(Foundation::Bit::Extract(Value, RplOffset, RplWidth));
		}

		[[nodiscard]] constexpr auto IsLdt() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(Value, Foundation::Bit::Mask<Foundation::Uint16>(TableOffset));
		}

		[[nodiscard]] constexpr auto operator<=>(const SegmentSelector&) const = default;

		Foundation::Uint16 Value{};
	};
}