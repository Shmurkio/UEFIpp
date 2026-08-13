#pragma once

#include <UEFIpp/External/Zydis/Zydis.h>

#include <UEFIpp/Architecture/X64/Instruction/InstructionSet.hpp>
#include <UEFIpp/Architecture/X64/Disassembly/Decoder.hpp>
#include <UEFIpp/Architecture/X64/Disassembly/Formatter.hpp>
#include <UEFIpp/Architecture/X64/Assembly/Encoder.hpp>

namespace UEFIpp::Architecture::X64::InstructionSet::Detail
{
	struct DecodedInstructionAccess
	{
		static auto Address(DecodedInstruction& Value) noexcept -> Foundation::Uint64&
		{ return Value.Address_; }

		static auto Bytes(DecodedInstruction& Value) noexcept
			-> Library::Array<Foundation::Byte, DecodedInstruction::MaximumLength>&
		{ return Value.Bytes_; }

		static auto Length(DecodedInstruction& Value) noexcept -> Foundation::Uint8&
		{ return Value.Length_; }

		static auto MnemonicValue(DecodedInstruction& Value) noexcept -> Mnemonic&
		{ return Value.Mnemonic_; }

		static auto Mode(DecodedInstruction& Value) noexcept -> MachineMode&
		{ return Value.Mode_; }

		static auto Encoding(DecodedInstruction& Value) noexcept -> InstructionEncoding&
		{ return Value.Encoding_; }

		static auto BranchKind(DecodedInstruction& Value) noexcept -> BranchType&
		{ return Value.BranchType_; }

		static auto Prefixes(DecodedInstruction& Value) noexcept -> InstructionPrefixes&
		{ return Value.Prefixes_; }

		static auto Category(DecodedInstruction& Value) noexcept -> InstructionCategory&
		{ return Value.Category_; }

		static auto Isa(DecodedInstruction& Value) noexcept -> IsaSet&
		{ return Value.Isa_; }

		static auto IsaExtensionValue(DecodedInstruction& Value) noexcept -> IsaExtension&
		{ return Value.IsaExtension_; }

		static auto ControlFlow(DecodedInstruction& Value) noexcept -> ControlFlowType&
		{ return Value.ControlFlow_; }

		static auto Attributes(DecodedInstruction& Value) noexcept -> InstructionAttributes&
		{ return Value.Attributes_; }

		static auto RawAttributes(DecodedInstruction& Value) noexcept -> Foundation::Uint64&
		{ return Value.RawAttributes_; }

		static auto StackWidth(DecodedInstruction& Value) noexcept -> Foundation::Uint8&
		{ return Value.StackWidth_; }

		static auto OperandWidth(DecodedInstruction& Value) noexcept -> Foundation::Uint8&
		{ return Value.OperandWidth_; }

		static auto AddressWidth(DecodedInstruction& Value) noexcept -> Foundation::Uint8&
		{ return Value.AddressWidth_; }

		static auto Operands(DecodedInstruction& Value) noexcept
			-> Library::Array<Operand, DecodedInstruction::MaximumOperandCount>&
		{ return Value.Operands_; }

		static auto OperandCount(DecodedInstruction& Value) noexcept -> Foundation::Uint8&
		{ return Value.OperandCount_; }

		static auto VisibleOperandCount(DecodedInstruction& Value) noexcept -> Foundation::Uint8&
		{ return Value.VisibleOperandCount_; }

		static auto RegistersRead(DecodedInstruction& Value) noexcept -> RegisterSet&
		{ return Value.RegistersRead_; }

		static auto RegistersWritten(DecodedInstruction& Value) noexcept -> RegisterSet&
		{ return Value.RegistersWritten_; }

		static auto Flags(DecodedInstruction& Value) noexcept -> CpuFlagAccess&
		{ return Value.Flags_; }

		static auto RawEncoding(DecodedInstruction& Value) noexcept -> RawEncodingInfo&
		{ return Value.RawEncoding_; }

		static auto VectorEncoding(DecodedInstruction& Value) noexcept -> VectorEncodingInfo&
		{ return Value.VectorEncoding_; }

		static auto DirectTarget(DecodedInstruction& Value) noexcept
			-> Library::Optional<Foundation::Uint64>&
		{ return Value.DirectTarget_; }

		static auto StackDelta(DecodedInstruction& Value) noexcept
			-> Library::Optional<Foundation::Int64>&
		{ return Value.StackDelta_; }
	};
}

namespace UEFIpp::Architecture::X64::ZydisBackend
{
	// The public instruction model intentionally mirrors the numeric identity of
	// the vendored Zydis database. Keep hard guards here so upgrading Zydis can
	// never silently corrupt the private conversion layer.
	static_assert(
		static_cast<Foundation::Uint16>(InstructionSet::Mnemonic::Invalid) ==
		static_cast<Foundation::Uint16>(ZYDIS_MNEMONIC_INVALID)
	);

	static_assert(
		static_cast<Foundation::Uint16>(InstructionSet::Mnemonic::Xtest) ==
		static_cast<Foundation::Uint16>(ZYDIS_MNEMONIC_XTEST)
	);

	static_assert(
		InstructionSet::MnemonicCount ==
		static_cast<Foundation::Size>(ZYDIS_MNEMONIC_MAX_VALUE) + 1
	);

	static_assert(
		static_cast<Foundation::Uint16>(InstructionSet::RegisterId::Uif) ==
		static_cast<Foundation::Uint16>(ZYDIS_REGISTER_UIF)
	);

	static_assert(
		InstructionSet::RegisterCount ==
		static_cast<Foundation::Size>(ZYDIS_REGISTER_MAX_VALUE) + 1
	);

	static_assert(
		static_cast<Foundation::Uint16>(InstructionSet::InstructionCategory::Xsaveopt) ==
		static_cast<Foundation::Uint16>(ZYDIS_CATEGORY_XSAVEOPT)
	);

	static_assert(
		static_cast<Foundation::Uint16>(InstructionSet::IsaSet::Xsaves) ==
		static_cast<Foundation::Uint16>(ZYDIS_ISA_SET_XSAVES)
	);

	static_assert(
		static_cast<Foundation::Uint16>(InstructionSet::IsaExtension::Xsaves) ==
		static_cast<Foundation::Uint16>(ZYDIS_ISA_EXT_XSAVES)
	);

	static_assert(
		static_cast<Foundation::Uint8>(InstructionSet::MaskMode::ControlZeroing) ==
		static_cast<Foundation::Uint8>(ZYDIS_MASK_MODE_CONTROL_ZEROING)
	);

	static_assert(
		static_cast<Foundation::Uint8>(InstructionSet::BroadcastMode::EightTo16) ==
		static_cast<Foundation::Uint8>(ZYDIS_BROADCAST_MODE_8_TO_16)
	);

	static_assert(
		static_cast<Foundation::Uint8>(InstructionSet::RoundingMode::TowardZero) ==
		static_cast<Foundation::Uint8>(ZYDIS_ROUNDING_MODE_RZ)
	);

	static_assert(
		static_cast<Foundation::Uint8>(InstructionSet::SwizzleMode::Dddd) ==
		static_cast<Foundation::Uint8>(ZYDIS_SWIZZLE_MODE_DDDD)
	);

	static_assert(
		static_cast<Foundation::Uint8>(InstructionSet::ConversionMode::UnsignedInt16) ==
		static_cast<Foundation::Uint8>(ZYDIS_CONVERSION_MODE_UINT16)
	);
	[[nodiscard]] constexpr auto ToZydis(
		InstructionSet::MachineMode Mode
	) noexcept -> ZydisMachineMode
	{
		return static_cast<ZydisMachineMode>(
			static_cast<Foundation::Uint8>(Mode)
		);
	}

	[[nodiscard]] constexpr auto FromZydis(
		ZydisMachineMode Mode
	) noexcept -> InstructionSet::MachineMode
	{
		return static_cast<InstructionSet::MachineMode>(
			static_cast<Foundation::Uint8>(Mode)
		);
	}

	[[nodiscard]] constexpr auto ToZydis(
		Disassembly::StackWidth Width
	) noexcept -> ZydisStackWidth
	{
		return static_cast<ZydisStackWidth>(
			static_cast<Foundation::Uint8>(Width)
		);
	}

	[[nodiscard]] constexpr auto ToZydis(
		InstructionSet::Mnemonic Value
	) noexcept -> ZydisMnemonic
	{
		return static_cast<ZydisMnemonic>(
			static_cast<Foundation::Uint16>(Value)
		);
	}

	[[nodiscard]] constexpr auto FromZydis(
		ZydisMnemonic Value
	) noexcept -> InstructionSet::Mnemonic
	{
		return static_cast<InstructionSet::Mnemonic>(
			static_cast<Foundation::Uint16>(Value)
		);
	}

	[[nodiscard]] constexpr auto ToZydis(
		InstructionSet::Register Value
	) noexcept -> ZydisRegister
	{
		return static_cast<ZydisRegister>(
			static_cast<Foundation::Uint16>(Value.Id())
		);
	}

	[[nodiscard]] constexpr auto FromZydis(
		ZydisRegister Value
	) noexcept -> InstructionSet::Register
	{
		return InstructionSet::Register{
			static_cast<InstructionSet::RegisterId>(
				static_cast<Foundation::Uint16>(Value)
			)
		};
	}

	[[nodiscard]] auto Decode(
		Library::Span<const Foundation::Byte> Code,
		Foundation::Uint64 RuntimeAddress,
		InstructionSet::MachineMode Mode,
		Disassembly::StackWidth Stack
	) noexcept -> Library::Expected<
		InstructionSet::DecodedInstruction,
		Disassembly::DecodeError
	>;

	[[nodiscard]] auto Encode(
		const InstructionSet::Instruction& Instruction,
		Library::Span<Foundation::Byte> Output,
		Foundation::Uint64 RuntimeAddress
	) noexcept -> Library::Expected<
		Foundation::Size,
		Assembly::EncodeError
	>;

	[[nodiscard]] auto Reencode(
		const InstructionSet::DecodedInstruction& Instruction,
		Library::Span<Foundation::Byte> Output,
		Foundation::Uint64 RuntimeAddress
	) noexcept -> Library::Expected<
		Foundation::Size,
		Assembly::EncodeError
	>;

	[[nodiscard]] auto Format(
		const InstructionSet::DecodedInstruction& Instruction,
		Disassembly::FormatterStyle Style,
		Library::Span<Foundation::Char> Buffer
	) noexcept -> Library::Expected<
		Library::StringView,
		Disassembly::FormatError
	>;
}
