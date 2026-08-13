#include "Conversion.hpp"

namespace UEFIpp::Architecture::X64::ZydisBackend
{
	namespace
	{
		[[nodiscard]] auto MapDecodeError(
			ZyanStatus Status
		) noexcept -> Disassembly::DecodeError
		{
			if (Status == ZYDIS_STATUS_NO_MORE_DATA)
			{
				return Disassembly::DecodeError::NoMoreData;
			}

			if (Status == ZYDIS_STATUS_INSTRUCTION_TOO_LONG)
			{
				return Disassembly::DecodeError::InstructionTooLong;
			}

			if (
				Status == ZYDIS_STATUS_DECODING_ERROR ||
				Status == ZYDIS_STATUS_BAD_REGISTER ||
				Status == ZYDIS_STATUS_ILLEGAL_LOCK ||
				Status == ZYDIS_STATUS_ILLEGAL_LEGACY_PFX ||
				Status == ZYDIS_STATUS_ILLEGAL_REX ||
				Status == ZYDIS_STATUS_INVALID_MAP ||
				Status == ZYDIS_STATUS_MALFORMED_EVEX ||
				Status == ZYDIS_STATUS_MALFORMED_MVEX ||
				Status == ZYDIS_STATUS_INVALID_MASK
			)
			{
				return Disassembly::DecodeError::InvalidInstruction;
			}

			return Disassembly::DecodeError::BackendFailure;
		}

		[[nodiscard]] constexpr auto MapControlFlow(
			InstructionSet::InstructionCategory Category
		) noexcept -> InstructionSet::ControlFlowType
		{
			using enum InstructionSet::InstructionCategory;
			using InstructionSet::ControlFlowType;

			switch (Category)
			{
			case Call:
				return ControlFlowType::Call;
			case Ret:
				return ControlFlowType::Return;
			case CondBr:
				return ControlFlowType::ConditionalBranch;
			case UncondBr:
				return ControlFlowType::UnconditionalBranch;
			case Interrupt:
				return ControlFlowType::Interrupt;
			case Syscall:
				return ControlFlowType::SystemCall;
			case Sysret:
				return ControlFlowType::SystemReturn;
			default:
				return ControlFlowType::None;
			}
		}

		[[nodiscard]] auto MapAttributes(
			ZydisInstructionAttributes Attributes
		) noexcept -> InstructionSet::InstructionAttributes
		{
			using InstructionSet::InstructionAttribute;
			using InstructionSet::AttributeMask;

			InstructionSet::InstructionAttributes Result{};

			auto Add = [&](ZydisInstructionAttributes ZydisMask,
				InstructionAttribute Attribute)
			{
				if (Attributes & ZydisMask)
				{
					Result |= AttributeMask(Attribute);
				}
			};

			Add(ZYDIS_ATTRIB_HAS_LOCK, InstructionAttribute::Lock);
			Add(ZYDIS_ATTRIB_HAS_REP, InstructionAttribute::Rep);
			Add(ZYDIS_ATTRIB_HAS_REPE, InstructionAttribute::Repe);
			Add(ZYDIS_ATTRIB_HAS_REPNE, InstructionAttribute::Repne);
			Add(ZYDIS_ATTRIB_HAS_BND, InstructionAttribute::Bound);
			Add(ZYDIS_ATTRIB_HAS_XACQUIRE, InstructionAttribute::XAcquire);
			Add(ZYDIS_ATTRIB_HAS_XRELEASE, InstructionAttribute::XRelease);
			Add(ZYDIS_ATTRIB_HAS_BRANCH_TAKEN, InstructionAttribute::BranchTaken);
			Add(ZYDIS_ATTRIB_HAS_BRANCH_NOT_TAKEN, InstructionAttribute::BranchNotTaken);
			Add(ZYDIS_ATTRIB_HAS_NOTRACK, InstructionAttribute::NoTrack);
			Add(ZYDIS_ATTRIB_HAS_OPERANDSIZE, InstructionAttribute::OperandSizeOverride);
			Add(ZYDIS_ATTRIB_HAS_ADDRESSSIZE, InstructionAttribute::AddressSizeOverride);

			if (Attributes & ZYDIS_ATTRIB_HAS_SEGMENT)
			{
				Result |= AttributeMask(InstructionAttribute::SegmentOverride);
			}

			return Result;
		}

		[[nodiscard]] auto MapPrefixes(
			ZydisInstructionAttributes Attributes
		) noexcept -> InstructionSet::InstructionPrefixes
		{
			using InstructionSet::InstructionPrefix;
			using InstructionSet::InstructionPrefixes;

			InstructionPrefixes Result{};

			auto Add = [&](ZydisInstructionAttributes Native,
				InstructionPrefix Prefix)
			{
				if (Attributes & Native)
				{
					Result |= static_cast<InstructionPrefixes>(Prefix);
				}
			};

			Add(ZYDIS_ATTRIB_HAS_LOCK, InstructionPrefix::Lock);
			Add(ZYDIS_ATTRIB_HAS_REP, InstructionPrefix::Rep);
			Add(ZYDIS_ATTRIB_HAS_REPE, InstructionPrefix::Repe);
			Add(ZYDIS_ATTRIB_HAS_REPNE, InstructionPrefix::Repne);
			Add(ZYDIS_ATTRIB_HAS_BND, InstructionPrefix::Bound);
			Add(ZYDIS_ATTRIB_HAS_XACQUIRE, InstructionPrefix::XAcquire);
			Add(ZYDIS_ATTRIB_HAS_XRELEASE, InstructionPrefix::XRelease);
			Add(ZYDIS_ATTRIB_HAS_BRANCH_TAKEN, InstructionPrefix::BranchTaken);
			Add(ZYDIS_ATTRIB_HAS_BRANCH_NOT_TAKEN, InstructionPrefix::BranchNotTaken);
			Add(ZYDIS_ATTRIB_HAS_NOTRACK, InstructionPrefix::NoTrack);
			Add(ZYDIS_ATTRIB_HAS_SEGMENT_CS, InstructionPrefix::SegmentCs);
			Add(ZYDIS_ATTRIB_HAS_SEGMENT_SS, InstructionPrefix::SegmentSs);
			Add(ZYDIS_ATTRIB_HAS_SEGMENT_DS, InstructionPrefix::SegmentDs);
			Add(ZYDIS_ATTRIB_HAS_SEGMENT_ES, InstructionPrefix::SegmentEs);
			Add(ZYDIS_ATTRIB_HAS_SEGMENT_FS, InstructionPrefix::SegmentFs);
			Add(ZYDIS_ATTRIB_HAS_SEGMENT_GS, InstructionPrefix::SegmentGs);

			return Result;
		}

		auto AddRegisterAccess(
			InstructionSet::RegisterSet& Set,
			InstructionSet::Register Value,
			InstructionSet::MachineMode Mode
		) noexcept -> Foundation::Void
		{
			if (!Value.IsValid())
			{
				return;
			}

			const auto Root = Value.Root(Mode);
			Set.Add(Root.IsValid() ? Root : Value);
		}

		[[nodiscard]] auto ConvertOperand(
			const ZydisDecodedInstruction& Instruction,
			const ZydisDecodedOperand& NativeOperand,
			Foundation::Uint64 RuntimeAddress
		) noexcept -> InstructionSet::Operand
		{
			using namespace InstructionSet;

			Operand Result{};

			switch (NativeOperand.type)
			{
			case ZYDIS_OPERAND_TYPE_REGISTER:
				Result = Operand::FromRegister(
					FromZydis(NativeOperand.reg.value),
					NativeOperand.size
				);
				break;

			case ZYDIS_OPERAND_TYPE_MEMORY:
			{
				MemoryOperand Memory{};
				Memory.Type = static_cast<MemoryOperandType>(NativeOperand.mem.type);
				Memory.Segment = FromZydis(NativeOperand.mem.segment);
				Memory.Base = FromZydis(NativeOperand.mem.base);
				Memory.Index = FromZydis(NativeOperand.mem.index);
				Memory.Scale = NativeOperand.mem.scale ? NativeOperand.mem.scale : 1;
				Memory.HasDisplacement = NativeOperand.mem.disp.has_displacement != 0;
				Memory.Displacement = NativeOperand.mem.disp.value;
				Memory.SizeInBits = NativeOperand.size;

				ZyanU64 Absolute{};
				if (ZYAN_SUCCESS(ZydisCalcAbsoluteAddress(
					&Instruction,
					&NativeOperand,
					RuntimeAddress,
					&Absolute
				)))
				{
					Memory.HasAbsoluteAddress = true;
					Memory.AbsoluteAddress = Absolute;
				}

				Result = Operand::FromMemory(Memory);
				break;
			}

			case ZYDIS_OPERAND_TYPE_POINTER:
				Result = Operand::FromPointer(PointerOperand{
					NativeOperand.ptr.segment,
					NativeOperand.ptr.offset
				});
				break;

			case ZYDIS_OPERAND_TYPE_IMMEDIATE:
			{
				ImmediateOperand Immediate{};
				Immediate.IsSigned = NativeOperand.imm.is_signed != 0;
				Immediate.IsRelative = NativeOperand.imm.is_relative != 0;
				Immediate.UnsignedValue = NativeOperand.imm.value.u;
				Immediate.SignedValue = NativeOperand.imm.value.s;

				if (Immediate.IsRelative)
				{
					ZyanU64 Absolute{};
					if (ZYAN_SUCCESS(ZydisCalcAbsoluteAddress(
						&Instruction,
						&NativeOperand,
						RuntimeAddress,
						&Absolute
					)))
					{
						Immediate.HasAbsoluteAddress = true;
						Immediate.AbsoluteAddress = Absolute;
					}
				}

				Result = Operand::FromImmediate(
					Immediate,
					NativeOperand.size
				);
				break;
			}

			default:
				break;
			}

			Result.SetDecodedMetadata(
				static_cast<OperandVisibility>(NativeOperand.visibility),
				static_cast<OperandAccessMask>(NativeOperand.actions),
				static_cast<OperandEncoding>(NativeOperand.encoding),
				NativeOperand.size,
				static_cast<ElementType>(NativeOperand.element_type),
				static_cast<Foundation::Uint16>(NativeOperand.element_size),
				NativeOperand.element_count
			);

			return Result;
		}

		[[nodiscard]] auto CalculateStackDelta(
			const InstructionSet::DecodedInstruction& Value
		) noexcept -> Library::Optional<Foundation::Int64>
		{
			using InstructionSet::Mnemonic;

			const auto Slot =
				static_cast<Foundation::Int64>(Value.StackWidth() / 8);

			switch (Value.MnemonicValue())
			{
			case Mnemonic::Push:
			case Mnemonic::Pushf:
			case Mnemonic::Pushfd:
			case Mnemonic::Pushfq:
			case Mnemonic::Call:
				return -Slot;

			case Mnemonic::Pop:
			case Mnemonic::Popf:
			case Mnemonic::Popfd:
			case Mnemonic::Popfq:
				return Slot;

			case Mnemonic::Ret:
			{
				Foundation::Int64 Delta = Slot;

				for (const auto& Operand : Value.VisibleOperands())
				{
					if (Operand.Type() == InstructionSet::OperandType::Immediate)
					{
						Delta += static_cast<Foundation::Int64>(
							Operand.ImmediateValue().UnsignedValue
						);
						break;
					}
				}

				return Delta;
			}

			default:
				return {};
			}
		}
	}

	auto Decode(
		Library::Span<const Foundation::Byte> Code,
		Foundation::Uint64 RuntimeAddress,
		InstructionSet::MachineMode Mode,
		Disassembly::StackWidth Stack
	) noexcept -> Library::Expected<
		InstructionSet::DecodedInstruction,
		Disassembly::DecodeError
	>
	{
		using namespace InstructionSet;
		using Access = Detail::DecodedInstructionAccess;

		if (Code.Empty())
		{
			return Library::MakeUnexpected(
				Disassembly::DecodeError::InvalidArgument
			);
		}

		ZydisDecoder Decoder{};

		const auto InitStatus = ZydisDecoderInit(
			&Decoder,
			ToZydis(Mode),
			ToZydis(Stack)
		);

		if (!ZYAN_SUCCESS(InitStatus))
		{
			return Library::MakeUnexpected(
				Disassembly::DecodeError::UnsupportedMode
			);
		}

		ZydisDecodedInstruction NativeInstruction{};
		ZydisDecodedOperand NativeOperands[
			ZYDIS_MAX_OPERAND_COUNT
		]{};

		const auto Status = ZydisDecoderDecodeFull(
			&Decoder,
			Code.Data(),
			Code.Size(),
			&NativeInstruction,
			NativeOperands
		);

		if (!ZYAN_SUCCESS(Status))
		{
			return Library::MakeUnexpected(
				MapDecodeError(Status)
			);
		}

		DecodedInstruction Result{};

		Access::Address(Result) = RuntimeAddress;
		Access::Length(Result) = NativeInstruction.length;
		Access::MnemonicValue(Result) =
			FromZydis(NativeInstruction.mnemonic);
		Access::Mode(Result) =
			FromZydis(NativeInstruction.machine_mode);

		const auto NativeEncoding =
			static_cast<Foundation::Uint8>(NativeInstruction.encoding);

		Access::Encoding(Result) =
			NativeEncoding <= static_cast<Foundation::Uint8>(ZYDIS_INSTRUCTION_ENCODING_MVEX)
			? static_cast<InstructionEncoding>(NativeEncoding)
			: InstructionEncoding::Unknown;
		Access::BranchKind(Result) =
			static_cast<BranchType>(
				NativeInstruction.meta.branch_type
			);
		Access::Prefixes(Result) =
			MapPrefixes(NativeInstruction.attributes);

		Access::Category(Result) =
			static_cast<InstructionCategory>(
				NativeInstruction.meta.category
			);
		Access::Isa(Result) =
			static_cast<IsaSet>(
				NativeInstruction.meta.isa_set
			);
		Access::IsaExtensionValue(Result) =
			static_cast<IsaExtension>(
				NativeInstruction.meta.isa_ext
			);
		Access::ControlFlow(Result) =
			MapControlFlow(Access::Category(Result));

		Access::RawAttributes(Result) =
			static_cast<Foundation::Uint64>(
				NativeInstruction.attributes
			);
		Access::Attributes(Result) =
			MapAttributes(NativeInstruction.attributes);

		Access::StackWidth(Result) =
			NativeInstruction.stack_width;
		Access::OperandWidth(Result) =
			NativeInstruction.operand_width;
		Access::AddressWidth(Result) =
			NativeInstruction.address_width;

		for (Foundation::Size Index = 0;
			Index < NativeInstruction.length;
			++Index)
		{
			Access::Bytes(Result)[Index] = Code[Index];
		}

		Access::OperandCount(Result) =
			NativeInstruction.operand_count;
		Access::VisibleOperandCount(Result) =
			NativeInstruction.operand_count_visible;

		for (Foundation::Size Index = 0;
			Index < NativeInstruction.operand_count;
			++Index)
		{
			const auto& NativeOperand = NativeOperands[Index];
			auto Converted = ConvertOperand(
				NativeInstruction,
				NativeOperand,
				RuntimeAddress
			);

			Access::Operands(Result)[Index] = Converted;

			const auto Reads =
				(NativeOperand.actions & ZYDIS_OPERAND_ACTION_MASK_READ) != 0;
			const auto Writes =
				(NativeOperand.actions & ZYDIS_OPERAND_ACTION_MASK_WRITE) != 0;

			if (NativeOperand.type == ZYDIS_OPERAND_TYPE_REGISTER)
			{
				const auto RegisterValue =
					FromZydis(NativeOperand.reg.value);

				if (Reads)
				{
					AddRegisterAccess(
						Access::RegistersRead(Result),
						RegisterValue,
						Mode
					);
				}

				if (Writes)
				{
					AddRegisterAccess(
						Access::RegistersWritten(Result),
						RegisterValue,
						Mode
					);
				}
			}
			else if (NativeOperand.type == ZYDIS_OPERAND_TYPE_MEMORY)
			{
				AddRegisterAccess(
					Access::RegistersRead(Result),
					FromZydis(NativeOperand.mem.segment),
					Mode
				);
				AddRegisterAccess(
					Access::RegistersRead(Result),
					FromZydis(NativeOperand.mem.base),
					Mode
				);
				AddRegisterAccess(
					Access::RegistersRead(Result),
					FromZydis(NativeOperand.mem.index),
					Mode
				);
			}
		}

		if (NativeInstruction.cpu_flags)
		{
			auto& Flags = Access::Flags(Result);
			Flags.Read = NativeInstruction.cpu_flags->tested;
			Flags.Modified = NativeInstruction.cpu_flags->modified;
			Flags.SetToZero = NativeInstruction.cpu_flags->set_0;
			Flags.SetToOne = NativeInstruction.cpu_flags->set_1;
			Flags.Undefined = NativeInstruction.cpu_flags->undefined;
		}

		auto& Vector = Access::VectorEncoding(Result);
		Vector.VectorLength = NativeInstruction.avx.vector_length;
		Vector.Mask = static_cast<MaskMode>(NativeInstruction.avx.mask.mode);
		Vector.MaskRegister = FromZydis(NativeInstruction.avx.mask.reg);
		Vector.StaticBroadcast = NativeInstruction.avx.broadcast.is_static != 0;
		Vector.Broadcast = static_cast<BroadcastMode>(
			NativeInstruction.avx.broadcast.mode
		);
		Vector.Rounding = static_cast<RoundingMode>(
			NativeInstruction.avx.rounding.mode
		);
		Vector.Swizzle = static_cast<SwizzleMode>(
			NativeInstruction.avx.swizzle.mode
		);
		Vector.Conversion = static_cast<ConversionMode>(
			NativeInstruction.avx.conversion.mode
		);
		Vector.SuppressAllExceptions = NativeInstruction.avx.has_sae != 0;
		Vector.EvictionHint = NativeInstruction.avx.has_eviction_hint != 0;

		auto& Raw = Access::RawEncoding(Result);
		Raw.PrefixCount = NativeInstruction.raw.prefix_count;

		for (Foundation::Size Index = 0;
			Index < NativeInstruction.raw.prefix_count && Index < Raw.Prefixes.Size();
			++Index)
		{
			Raw.Prefixes[Index].Type = static_cast<PrefixType>(
				NativeInstruction.raw.prefixes[Index].type
			);
			Raw.Prefixes[Index].Value =
				NativeInstruction.raw.prefixes[Index].value;
		}

		Raw.Rex = {
			NativeInstruction.raw.rex.W,
			NativeInstruction.raw.rex.R,
			NativeInstruction.raw.rex.X,
			NativeInstruction.raw.rex.B,
			NativeInstruction.raw.rex.offset
		};

		Raw.Xop = {
			NativeInstruction.raw.xop.R,
			NativeInstruction.raw.xop.X,
			NativeInstruction.raw.xop.B,
			NativeInstruction.raw.xop.m_mmmm,
			NativeInstruction.raw.xop.W,
			NativeInstruction.raw.xop.vvvv,
			NativeInstruction.raw.xop.L,
			NativeInstruction.raw.xop.pp,
			NativeInstruction.raw.xop.offset
		};

		Raw.Vex.R = NativeInstruction.raw.vex.R;
		Raw.Vex.X = NativeInstruction.raw.vex.X;
		Raw.Vex.B = NativeInstruction.raw.vex.B;
		Raw.Vex.Map = NativeInstruction.raw.vex.m_mmmm;
		Raw.Vex.W = NativeInstruction.raw.vex.W;
		Raw.Vex.Vvvv = NativeInstruction.raw.vex.vvvv;
		Raw.Vex.L = NativeInstruction.raw.vex.L;
		Raw.Vex.Pp = NativeInstruction.raw.vex.pp;
		Raw.Vex.Offset = NativeInstruction.raw.vex.offset;
		Raw.Vex.Size = NativeInstruction.raw.vex.size;

		Raw.Evex = {
			NativeInstruction.raw.evex.R,
			NativeInstruction.raw.evex.X,
			NativeInstruction.raw.evex.B,
			NativeInstruction.raw.evex.R2,
			NativeInstruction.raw.evex.mmm,
			NativeInstruction.raw.evex.W,
			NativeInstruction.raw.evex.vvvv,
			NativeInstruction.raw.evex.pp,
			NativeInstruction.raw.evex.z,
			NativeInstruction.raw.evex.L2,
			NativeInstruction.raw.evex.L,
			NativeInstruction.raw.evex.b,
			NativeInstruction.raw.evex.V2,
			NativeInstruction.raw.evex.aaa,
			NativeInstruction.raw.evex.offset
		};

		Raw.Mvex = {
			NativeInstruction.raw.mvex.R,
			NativeInstruction.raw.mvex.X,
			NativeInstruction.raw.mvex.B,
			NativeInstruction.raw.mvex.R2,
			NativeInstruction.raw.mvex.mmmm,
			NativeInstruction.raw.mvex.W,
			NativeInstruction.raw.mvex.vvvv,
			NativeInstruction.raw.mvex.pp,
			NativeInstruction.raw.mvex.E,
			NativeInstruction.raw.mvex.SSS,
			NativeInstruction.raw.mvex.V2,
			NativeInstruction.raw.mvex.kkk,
			NativeInstruction.raw.mvex.offset
		};

		Raw.ModRm = {
			NativeInstruction.raw.modrm.mod,
			NativeInstruction.raw.modrm.reg,
			NativeInstruction.raw.modrm.rm,
			NativeInstruction.raw.modrm.offset
		};

		Raw.Sib = {
			NativeInstruction.raw.sib.scale,
			NativeInstruction.raw.sib.index,
			NativeInstruction.raw.sib.base,
			NativeInstruction.raw.sib.offset
		};

		Raw.Displacement = {
			NativeInstruction.raw.disp.value,
			NativeInstruction.raw.disp.offset,
			NativeInstruction.raw.disp.size
		};

		Raw.ModRmOffset = NativeInstruction.raw.modrm.offset;
		Raw.SibOffset = NativeInstruction.raw.sib.offset;
		Raw.DisplacementOffset = NativeInstruction.raw.disp.offset;
		Raw.DisplacementSizeInBits = NativeInstruction.raw.disp.size;

		for (Foundation::Size Index = 0; Index < 2; ++Index)
		{
			Raw.Immediates[Index].IsSigned =
				NativeInstruction.raw.imm[Index].is_signed != 0;
			Raw.Immediates[Index].IsRelative =
				NativeInstruction.raw.imm[Index].is_relative != 0;
			Raw.Immediates[Index].UnsignedValue =
				NativeInstruction.raw.imm[Index].value.u;
			Raw.Immediates[Index].SignedValue =
				NativeInstruction.raw.imm[Index].value.s;
			Raw.Immediates[Index].Offset =
				NativeInstruction.raw.imm[Index].offset;
			Raw.Immediates[Index].SizeInBits =
				NativeInstruction.raw.imm[Index].size;
		}

		if (Result.IsBranch() || Result.IsCall())
		{
			for (const auto& Operand : Result.VisibleOperands())
			{
				if (
					Operand.Type() == OperandType::Immediate &&
					Operand.ImmediateValue().IsRelative &&
					Operand.ImmediateValue().HasAbsoluteAddress
				)
				{
					Access::DirectTarget(Result) =
						Operand.ImmediateValue().AbsoluteAddress;
					break;
				}
			}
		}

		Access::StackDelta(Result) =
			CalculateStackDelta(Result);

		return Result;
	}
}
