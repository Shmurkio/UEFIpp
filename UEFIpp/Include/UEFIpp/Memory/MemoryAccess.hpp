#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Memory
{
	enum class MemoryAccess : Foundation::Uint8
	{
		None = 0,
		Read = 1u << 0,
		Write = 1u << 1,
		Execute = 1u << 2
	};

	using MemoryAccessMask = Foundation::Uint8;

	[[nodiscard]] constexpr auto operator|(
		MemoryAccess Left,
		MemoryAccess Right
	) noexcept -> MemoryAccessMask
	{
		return static_cast<MemoryAccessMask>(Left) |
			static_cast<MemoryAccessMask>(Right);
	}

	[[nodiscard]] constexpr auto operator|(
		MemoryAccessMask Left,
		MemoryAccess Right
	) noexcept -> MemoryAccessMask
	{
		return Left | static_cast<MemoryAccessMask>(Right);
	}

	[[nodiscard]] constexpr auto operator|(
		MemoryAccess Left,
		MemoryAccessMask Right
	) noexcept -> MemoryAccessMask
	{
		return static_cast<MemoryAccessMask>(Left) | Right;
	}

	constexpr auto operator|=(
		MemoryAccessMask& Left,
		MemoryAccess Right
	) noexcept -> MemoryAccessMask&
	{
		Left = Left | Right;
		return Left;
	}

	[[nodiscard]] constexpr auto HasAccess(
		MemoryAccessMask Access,
		MemoryAccess Required
	) noexcept -> Foundation::Bool
	{
		return (
			Access & static_cast<MemoryAccessMask>(Required)
		) != 0;
	}

	[[nodiscard]] constexpr auto HasAllAccess(
		MemoryAccessMask Access,
		MemoryAccessMask Required
	) noexcept -> Foundation::Bool
	{
		return (Access & Required) == Required;
	}

	inline constexpr auto ReadWriteAccess =
		MemoryAccess::Read | MemoryAccess::Write;

	inline constexpr auto ReadExecuteAccess =
		MemoryAccess::Read | MemoryAccess::Execute;

	inline constexpr auto AllMemoryAccess =
		MemoryAccess::Read | MemoryAccess::Write | MemoryAccess::Execute;

	enum class MemoryViewOperation : Foundation::Uint8
	{
		None,
		Read,
		Write,
		Probe,
		Translate
	};

	enum class MemoryViewError : Foundation::Uint8
	{
		InvalidView,
		InvalidAddress,
		InvalidRange,
		ReadUnsupported,
		WriteUnsupported,
		ProbeUnsupported,
		AccessDenied,
		Unmapped,
		TranslationFailed,
		PhysicalAccessFailed,
		PartialAccess,
		BackendFailure
	};

	// Backend-neutral diagnostic record. Architecture-specific backends may
	// flatten their native error enum values into BackendCode/BackendSubcode
	// while retaining the failing virtual/physical addresses and walk level.
	class MemoryViewErrorInfo
	{
	public:
		MemoryViewError Code{ MemoryViewError::InvalidView };
		MemoryViewOperation Operation{ MemoryViewOperation::None };
		Foundation::Uint64 Address{};
		Foundation::Size Size{};
		Foundation::Size BytesTransferred{};
		Foundation::Uint64 BackendCode{};
		Foundation::Uint64 BackendSubcode{};
		Foundation::Uint64 BackendAddress{};
		Foundation::Uint8 BackendLevel{};

		[[nodiscard]] constexpr auto operator<=> (
			const MemoryViewErrorInfo&
		) const = default;
	};

	[[nodiscard]] constexpr auto IsAddressRangeValid(
		Foundation::Uint64 Address,
		Foundation::Size Size
	) noexcept -> Foundation::Bool
	{
		if (!Size)
		{
			return false;
		}

		constexpr auto MaximumAddress = static_cast<Foundation::Uint64>(-1);
		return Size - 1 <= MaximumAddress - Address;
	}
}
