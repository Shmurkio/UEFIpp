#pragma once

#include <UEFIpp/Architecture/X64/Instruction/Register.hpp>

namespace UEFIpp::Architecture::X64::InstructionSet
{
	enum class EncodableEncoding : Foundation::Uint32
	{
		Default = 0,
		Legacy = 1u << 0,
		Amd3DNow = 1u << 1,
		Xop = 1u << 2,
		Vex = 1u << 3,
		Evex = 1u << 4,
		Mvex = 1u << 5,
		All = Legacy | Amd3DNow | Xop | Vex | Evex | Mvex
	};

	enum class BranchType : Foundation::Uint8
	{
		Automatic,
		Short,
		Near,
		Far
	};

	enum class BranchWidth : Foundation::Uint8
	{
		Automatic,
		Width8,
		Width16,
		Width32,
		Width64
	};

	enum class AddressSizeHint : Foundation::Uint8
	{
		Automatic,
		Width16,
		Width32,
		Width64
	};

	enum class OperandSizeHint : Foundation::Uint8
	{
		Automatic,
		Width8,
		Width16,
		Width32,
		Width64
	};

	enum class InstructionPrefix : Foundation::Uint32
	{
		None = 0,
		Lock = 1u << 0,
		Rep = 1u << 1,
		Repe = 1u << 2,
		Repne = 1u << 3,
		Bound = 1u << 4,
		XAcquire = 1u << 5,
		XRelease = 1u << 6,
		BranchTaken = 1u << 7,
		BranchNotTaken = 1u << 8,
		NoTrack = 1u << 9,
		SegmentCs = 1u << 10,
		SegmentSs = 1u << 11,
		SegmentDs = 1u << 12,
		SegmentEs = 1u << 13,
		SegmentFs = 1u << 14,
		SegmentGs = 1u << 15
	};

	using InstructionPrefixes = Foundation::Uint32;

	[[nodiscard]] constexpr auto operator|(
		EncodableEncoding Left,
		EncodableEncoding Right
	) noexcept -> EncodableEncoding
	{
		return static_cast<EncodableEncoding>(
			static_cast<Foundation::Uint32>(Left) |
			static_cast<Foundation::Uint32>(Right)
		);
	}

	constexpr auto operator|=(
		EncodableEncoding& Left,
		EncodableEncoding Right
	) noexcept -> EncodableEncoding&
	{
		Left = Left | Right;
		return Left;
	}

	[[nodiscard]] constexpr auto operator|(
		InstructionPrefix Left,
		InstructionPrefix Right
	) noexcept -> InstructionPrefixes
	{
		return static_cast<InstructionPrefixes>(Left) |
			static_cast<InstructionPrefixes>(Right);
	}

	[[nodiscard]] constexpr auto operator|(
		InstructionPrefixes Left,
		InstructionPrefix Right
	) noexcept -> InstructionPrefixes
	{
		return Left | static_cast<InstructionPrefixes>(Right);
	}

	constexpr auto operator|=(
		InstructionPrefixes& Left,
		InstructionPrefix Right
	) noexcept -> InstructionPrefixes&
	{
		Left = Left | Right;
		return Left;
	}

	[[nodiscard]] constexpr auto HasPrefix(
		InstructionPrefixes Prefixes,
		InstructionPrefix Prefix
	) noexcept -> Foundation::Bool
	{
		return (
			Prefixes & static_cast<InstructionPrefixes>(Prefix)
		) != 0;
	}

	enum class PrefixType : Foundation::Uint8
	{
		Ignored,
		Effective,
		Mandatory
	};

	enum class MaskMode : Foundation::Uint8
	{
		Invalid,
		Disabled,
		Merging,
		Zeroing,
		Control,
		ControlZeroing
	};

	enum class BroadcastMode : Foundation::Uint8
	{
		Invalid,
		OneTo2,
		OneTo4,
		OneTo8,
		OneTo16,
		OneTo32,
		OneTo64,
		TwoTo4,
		TwoTo8,
		TwoTo16,
		FourTo8,
		FourTo16,
		EightTo16
	};

	enum class RoundingMode : Foundation::Uint8
	{
		Invalid,
		Nearest,
		Down,
		Up,
		TowardZero
	};

	enum class SwizzleMode : Foundation::Uint8
	{
		Invalid,
		Dcba,
		Cdab,
		Badc,
		Dacb,
		Aaaa,
		Bbbb,
		Cccc,
		Dddd
	};

	enum class ConversionMode : Foundation::Uint8
	{
		Invalid,
		Float16,
		SignedInt8,
		UnsignedInt8,
		SignedInt16,
		UnsignedInt16
	};

	struct EvexEncodingOptions
	{
		BroadcastMode Broadcast{ BroadcastMode::Invalid };
		RoundingMode Rounding{ RoundingMode::Invalid };
		Foundation::Bool SuppressAllExceptions{};
		Foundation::Bool ZeroingMask{};
	};

	struct MvexEncodingOptions
	{
		BroadcastMode Broadcast{ BroadcastMode::Invalid };
		ConversionMode Conversion{ ConversionMode::Invalid };
		RoundingMode Rounding{ RoundingMode::Invalid };
		SwizzleMode Swizzle{ SwizzleMode::Invalid };
		Foundation::Bool SuppressAllExceptions{};
		Foundation::Bool EvictionHint{};
	};

	struct EncodingOptions
	{
		MachineMode Mode{ MachineMode::Long64 };
		EncodableEncoding AllowedEncodings{ EncodableEncoding::Default };
		InstructionPrefixes Prefixes{};
		BranchType Branch{ BranchType::Automatic };
		BranchWidth BranchSize{ BranchWidth::Automatic };
		AddressSizeHint AddressSize{ AddressSizeHint::Automatic };
		OperandSizeHint OperandSize{ OperandSizeHint::Automatic };
		EvexEncodingOptions Evex{};
		MvexEncodingOptions Mvex{};
	};
}
