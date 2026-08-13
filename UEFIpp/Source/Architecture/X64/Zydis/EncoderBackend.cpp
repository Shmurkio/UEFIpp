#include "Conversion.hpp"

namespace UEFIpp::Architecture::X64::ZydisBackend
{
	namespace
	{
		[[nodiscard]] auto MapEncodeError(
			ZyanStatus Status
		) noexcept -> Assembly::EncodeError
		{
			if (Status == ZYAN_STATUS_INVALID_ARGUMENT)
			{
				return Assembly::EncodeError::InvalidArgument;
			}

			if (Status == ZYAN_STATUS_INSUFFICIENT_BUFFER_SIZE)
			{
				return Assembly::EncodeError::BufferTooSmall;
			}

			if (Status == ZYDIS_STATUS_IMPOSSIBLE_INSTRUCTION)
			{
				return Assembly::EncodeError::ImpossibleInstruction;
			}

			return Assembly::EncodeError::BackendFailure;
		}

		[[nodiscard]] constexpr auto MapBranchType(
			InstructionSet::BranchType Value
		) noexcept -> ZydisBranchType
		{
			switch (Value)
			{
			case InstructionSet::BranchType::Short:
				return ZYDIS_BRANCH_TYPE_SHORT;
			case InstructionSet::BranchType::Near:
				return ZYDIS_BRANCH_TYPE_NEAR;
			case InstructionSet::BranchType::Far:
				return ZYDIS_BRANCH_TYPE_FAR;
			default:
				return ZYDIS_BRANCH_TYPE_NONE;
			}
		}

		[[nodiscard]] constexpr auto MapBranchWidth(
			InstructionSet::BranchWidth Value
		) noexcept -> ZydisBranchWidth
		{
			switch (Value)
			{
			case InstructionSet::BranchWidth::Width8:
				return ZYDIS_BRANCH_WIDTH_8;
			case InstructionSet::BranchWidth::Width16:
				return ZYDIS_BRANCH_WIDTH_16;
			case InstructionSet::BranchWidth::Width32:
				return ZYDIS_BRANCH_WIDTH_32;
			case InstructionSet::BranchWidth::Width64:
				return ZYDIS_BRANCH_WIDTH_64;
			default:
				return ZYDIS_BRANCH_WIDTH_NONE;
			}
		}

		[[nodiscard]] constexpr auto MapAddressSize(
			InstructionSet::AddressSizeHint Value
		) noexcept -> ZydisAddressSizeHint
		{
			switch (Value)
			{
			case InstructionSet::AddressSizeHint::Width16:
				return ZYDIS_ADDRESS_SIZE_HINT_16;
			case InstructionSet::AddressSizeHint::Width32:
				return ZYDIS_ADDRESS_SIZE_HINT_32;
			case InstructionSet::AddressSizeHint::Width64:
				return ZYDIS_ADDRESS_SIZE_HINT_64;
			default:
				return ZYDIS_ADDRESS_SIZE_HINT_NONE;
			}
		}

		[[nodiscard]] constexpr auto MapOperandSize(
			InstructionSet::OperandSizeHint Value
		) noexcept -> ZydisOperandSizeHint
		{
			switch (Value)
			{
			case InstructionSet::OperandSizeHint::Width8:
				return ZYDIS_OPERAND_SIZE_HINT_8;
			case InstructionSet::OperandSizeHint::Width16:
				return ZYDIS_OPERAND_SIZE_HINT_16;
			case InstructionSet::OperandSizeHint::Width32:
				return ZYDIS_OPERAND_SIZE_HINT_32;
			case InstructionSet::OperandSizeHint::Width64:
				return ZYDIS_OPERAND_SIZE_HINT_64;
			default:
				return ZYDIS_OPERAND_SIZE_HINT_NONE;
			}
		}

		[[nodiscard]] auto MapPrefixes(
			InstructionSet::InstructionPrefixes Prefixes
		) noexcept -> ZydisInstructionAttributes
		{
			using InstructionSet::InstructionPrefix;

			ZydisInstructionAttributes Result{};

			auto Has = [&](InstructionPrefix Prefix)
			{
				return (
					Prefixes &
					static_cast<InstructionSet::InstructionPrefixes>(Prefix)
				) != 0;
			};

			if (Has(InstructionPrefix::Lock)) Result |= ZYDIS_ATTRIB_HAS_LOCK;
			if (Has(InstructionPrefix::Rep)) Result |= ZYDIS_ATTRIB_HAS_REP;
			if (Has(InstructionPrefix::Repe)) Result |= ZYDIS_ATTRIB_HAS_REPE;
			if (Has(InstructionPrefix::Repne)) Result |= ZYDIS_ATTRIB_HAS_REPNE;
			if (Has(InstructionPrefix::Bound)) Result |= ZYDIS_ATTRIB_HAS_BND;
			if (Has(InstructionPrefix::XAcquire)) Result |= ZYDIS_ATTRIB_HAS_XACQUIRE;
			if (Has(InstructionPrefix::XRelease)) Result |= ZYDIS_ATTRIB_HAS_XRELEASE;
			if (Has(InstructionPrefix::BranchTaken)) Result |= ZYDIS_ATTRIB_HAS_BRANCH_TAKEN;
			if (Has(InstructionPrefix::BranchNotTaken)) Result |= ZYDIS_ATTRIB_HAS_BRANCH_NOT_TAKEN;
			if (Has(InstructionPrefix::NoTrack)) Result |= ZYDIS_ATTRIB_HAS_NOTRACK;
			if (Has(InstructionPrefix::SegmentCs)) Result |= ZYDIS_ATTRIB_HAS_SEGMENT_CS;
			if (Has(InstructionPrefix::SegmentSs)) Result |= ZYDIS_ATTRIB_HAS_SEGMENT_SS;
			if (Has(InstructionPrefix::SegmentDs)) Result |= ZYDIS_ATTRIB_HAS_SEGMENT_DS;
			if (Has(InstructionPrefix::SegmentEs)) Result |= ZYDIS_ATTRIB_HAS_SEGMENT_ES;
			if (Has(InstructionPrefix::SegmentFs)) Result |= ZYDIS_ATTRIB_HAS_SEGMENT_FS;
			if (Has(InstructionPrefix::SegmentGs)) Result |= ZYDIS_ATTRIB_HAS_SEGMENT_GS;

			return Result;
		}

		[[nodiscard]] auto BuildRequest(
			const InstructionSet::Instruction& Instruction,
			ZydisEncoderRequest& Request,
			Foundation::Bool& UsesAbsoluteOperands
		) noexcept -> Assembly::EncodeError
		{
			using namespace InstructionSet;

			Request = {};
			Request.machine_mode = ToZydis(Instruction.Options().Mode);
			Request.allowed_encodings = static_cast<ZydisEncodableEncoding>(
				static_cast<Foundation::Uint32>(
					Instruction.Options().AllowedEncodings
				)
			);
			Request.mnemonic = ToZydis(Instruction.MnemonicValue());
			Request.prefixes = MapPrefixes(Instruction.Options().Prefixes);
			Request.branch_type = MapBranchType(Instruction.Options().Branch);
			Request.branch_width = MapBranchWidth(Instruction.Options().BranchSize);
			Request.address_size_hint = MapAddressSize(Instruction.Options().AddressSize);
			Request.operand_size_hint = MapOperandSize(Instruction.Options().OperandSize);

			Request.evex.broadcast = static_cast<ZydisBroadcastMode>(
				Instruction.Options().Evex.Broadcast
			);
			Request.evex.rounding = static_cast<ZydisRoundingMode>(
				Instruction.Options().Evex.Rounding
			);
			Request.evex.sae = Instruction.Options().Evex.SuppressAllExceptions
				? ZYAN_TRUE
				: ZYAN_FALSE;
			Request.evex.zeroing_mask = Instruction.Options().Evex.ZeroingMask
				? ZYAN_TRUE
				: ZYAN_FALSE;

			Request.mvex.broadcast = static_cast<ZydisBroadcastMode>(
				Instruction.Options().Mvex.Broadcast
			);
			Request.mvex.conversion = static_cast<ZydisConversionMode>(
				Instruction.Options().Mvex.Conversion
			);
			Request.mvex.rounding = static_cast<ZydisRoundingMode>(
				Instruction.Options().Mvex.Rounding
			);
			Request.mvex.swizzle = static_cast<ZydisSwizzleMode>(
				Instruction.Options().Mvex.Swizzle
			);
			Request.mvex.sae = Instruction.Options().Mvex.SuppressAllExceptions
				? ZYAN_TRUE
				: ZYAN_FALSE;
			Request.mvex.eviction_hint = Instruction.Options().Mvex.EvictionHint
				? ZYAN_TRUE
				: ZYAN_FALSE;

			if (Instruction.OperandCount() > ZYDIS_ENCODER_MAX_OPERANDS)
			{
				return Assembly::EncodeError::TooManyOperands;
			}

			Request.operand_count =
				static_cast<ZyanU8>(Instruction.OperandCount());

			UsesAbsoluteOperands = false;

			for (Foundation::Size Index = 0;
				Index < Instruction.OperandCount();
				++Index)
			{
				const auto& Source = Instruction.Operands()[Index];
				auto& Destination = Request.operands[Index];

				switch (Source.Type())
				{
				case OperandType::Register:
					Destination.type = ZYDIS_OPERAND_TYPE_REGISTER;
					Destination.reg.value =
						ToZydis(Source.RegisterValue());
					Destination.reg.is4 =
						Source.Encoding() == OperandEncoding::Is4
						? ZYAN_TRUE
						: ZYAN_FALSE;
					break;

				case OperandType::Memory:
				{
					const auto& Memory = Source.MemoryValue();

					Destination.type = ZYDIS_OPERAND_TYPE_MEMORY;
					Destination.mem.base = ToZydis(Memory.Base);
					Destination.mem.index = ToZydis(Memory.Index);
					Destination.mem.scale = Memory.Index.IsValid() ? Memory.Scale : 0;
					Destination.mem.size =
						static_cast<ZyanU16>(
							(Source.SizeInBits()
								? Source.SizeInBits()
								: Memory.SizeInBits) / 8
						);

					switch (Memory.Segment.Id())
					{
					case RegisterId::Cs:
						Request.prefixes |= ZYDIS_ATTRIB_HAS_SEGMENT_CS;
						break;
					case RegisterId::Ss:
						Request.prefixes |= ZYDIS_ATTRIB_HAS_SEGMENT_SS;
						break;
					case RegisterId::Ds:
						Request.prefixes |= ZYDIS_ATTRIB_HAS_SEGMENT_DS;
						break;
					case RegisterId::Es:
						Request.prefixes |= ZYDIS_ATTRIB_HAS_SEGMENT_ES;
						break;
					case RegisterId::Fs:
						Request.prefixes |= ZYDIS_ATTRIB_HAS_SEGMENT_FS;
						break;
					case RegisterId::Gs:
						Request.prefixes |= ZYDIS_ATTRIB_HAS_SEGMENT_GS;
						break;
					default:
						break;
					}

					if (Memory.HasAbsoluteAddress)
					{
						Destination.mem.displacement =
							static_cast<ZyanI64>(
								Memory.AbsoluteAddress
							);
						UsesAbsoluteOperands = true;
					}
					else
					{
						Destination.mem.displacement =
							Memory.Displacement;
					}

					break;
				}

				case OperandType::Pointer:
					Destination.type = ZYDIS_OPERAND_TYPE_POINTER;
					Destination.ptr.segment =
						Source.PointerValue().Segment;
					Destination.ptr.offset =
						Source.PointerValue().Offset;
					break;

				case OperandType::Immediate:
				{
					const auto& Immediate =
						Source.ImmediateValue();

					Destination.type = ZYDIS_OPERAND_TYPE_IMMEDIATE;

					if (Immediate.HasAbsoluteAddress)
					{
						Destination.imm.u =
							Immediate.AbsoluteAddress;
						UsesAbsoluteOperands = Immediate.IsRelative;
					}
					else if (Immediate.IsSigned)
					{
						Destination.imm.s =
							Immediate.SignedValue;
					}
					else
					{
						Destination.imm.u =
							Immediate.UnsignedValue;
					}

					break;
				}

				default:
					return Assembly::EncodeError::InvalidOperand;
				}
			}

			return Assembly::EncodeError::BackendFailure;
		}
	}

	auto Encode(
		const InstructionSet::Instruction& Instruction,
		Library::Span<Foundation::Byte> Output,
		Foundation::Uint64 RuntimeAddress
	) noexcept -> Library::Expected<
		Foundation::Size,
		Assembly::EncodeError
	>
	{
		if (Output.Empty())
		{
			return Library::MakeUnexpected(
				Assembly::EncodeError::InvalidArgument
			);
		}

		ZydisEncoderRequest Request{};
		Foundation::Bool UsesAbsoluteOperands{};

		const auto BuildError = BuildRequest(
			Instruction,
			Request,
			UsesAbsoluteOperands
		);

		if (BuildError != Assembly::EncodeError::BackendFailure)
		{
			return Library::MakeUnexpected(BuildError);
		}

		ZyanUSize Length = Output.Size();

		const auto Status = UsesAbsoluteOperands
			? ZydisEncoderEncodeInstructionAbsolute(
				&Request,
				Output.Data(),
				&Length,
				RuntimeAddress
			)
			: ZydisEncoderEncodeInstruction(
				&Request,
				Output.Data(),
				&Length
			);

		if (!ZYAN_SUCCESS(Status))
		{
			return Library::MakeUnexpected(
				MapEncodeError(Status)
			);
		}

		return static_cast<Foundation::Size>(Length);
	}
	auto Reencode(
		const InstructionSet::DecodedInstruction& Instruction,
		Library::Span<Foundation::Byte> Output,
		Foundation::Uint64 RuntimeAddress
	) noexcept -> Library::Expected<
		Foundation::Size,
		Assembly::EncodeError
	>
	{
		if (Output.Empty() || Instruction.Bytes().Empty())
		{
			return Library::MakeUnexpected(
				Assembly::EncodeError::InvalidArgument
			);
		}

		ZydisDecoder Decoder{};

		ZydisStackWidth Stack = ZYDIS_STACK_WIDTH_64;

		switch (Instruction.StackWidth())
		{
		case 16:
			Stack = ZYDIS_STACK_WIDTH_16;
			break;
		case 32:
			Stack = ZYDIS_STACK_WIDTH_32;
			break;
		default:
			Stack = ZYDIS_STACK_WIDTH_64;
			break;
		}

		if (!ZYAN_SUCCESS(ZydisDecoderInit(
			&Decoder,
			ToZydis(Instruction.Mode()),
			Stack
		)))
		{
			return Library::MakeUnexpected(
				Assembly::EncodeError::BackendFailure
			);
		}

		ZydisDecodedInstruction NativeInstruction{};
		ZydisDecodedOperand NativeOperands[
			ZYDIS_MAX_OPERAND_COUNT
		]{};

		if (!ZYAN_SUCCESS(ZydisDecoderDecodeFull(
			&Decoder,
			Instruction.Bytes().Data(),
			Instruction.Bytes().Size(),
			&NativeInstruction,
			NativeOperands
		)))
		{
			return Library::MakeUnexpected(
				Assembly::EncodeError::BackendFailure
			);
		}

		ZydisEncoderRequest Request{};

		const auto ConvertStatus =
			ZydisEncoderDecodedInstructionToEncoderRequest(
				&NativeInstruction,
				NativeOperands,
				NativeInstruction.operand_count_visible,
				&Request
			);

		if (!ZYAN_SUCCESS(ConvertStatus))
		{
			return Library::MakeUnexpected(
				MapEncodeError(ConvertStatus)
			);
		}

		Foundation::Bool UsesAbsoluteOperands{};

		for (Foundation::Size Index = 0;
			Index < NativeInstruction.operand_count_visible;
			++Index)
		{
			const auto& NativeOperand =
				NativeOperands[Index];

			if (
				NativeOperand.type ==
					ZYDIS_OPERAND_TYPE_IMMEDIATE &&
				NativeOperand.imm.is_relative
			)
			{
				ZyanU64 Absolute{};

				if (ZYAN_SUCCESS(
					ZydisCalcAbsoluteAddress(
						&NativeInstruction,
						&NativeOperand,
						Instruction.Address(),
						&Absolute
					)
				))
				{
					Request.operands[Index].imm.u = Absolute;
					UsesAbsoluteOperands = true;
				}
			}
			else if (
				NativeOperand.type ==
					ZYDIS_OPERAND_TYPE_MEMORY &&
				(
					NativeOperand.mem.base ==
						ZYDIS_REGISTER_RIP ||
					NativeOperand.mem.base ==
						ZYDIS_REGISTER_EIP ||
					NativeOperand.mem.base ==
						ZYDIS_REGISTER_NONE
				)
			)
			{
				ZyanU64 Absolute{};

				if (ZYAN_SUCCESS(
					ZydisCalcAbsoluteAddress(
						&NativeInstruction,
						&NativeOperand,
						Instruction.Address(),
						&Absolute
					)
				))
				{
					Request.operands[Index].mem.displacement =
						static_cast<ZyanI64>(Absolute);
					UsesAbsoluteOperands = true;
				}
			}
		}

		ZyanUSize Length = Output.Size();

		const auto Status = UsesAbsoluteOperands
			? ZydisEncoderEncodeInstructionAbsolute(
				&Request,
				Output.Data(),
				&Length,
				RuntimeAddress
			)
			: ZydisEncoderEncodeInstruction(
				&Request,
				Output.Data(),
				&Length
			);

		if (!ZYAN_SUCCESS(Status))
		{
			return Library::MakeUnexpected(
				MapEncodeError(Status)
			);
		}

		return static_cast<Foundation::Size>(Length);
	}

}
