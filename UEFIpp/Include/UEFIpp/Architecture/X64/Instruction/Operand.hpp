#pragma once

#include <UEFIpp/Architecture/X64/Instruction/Register.hpp>

namespace UEFIpp::Architecture::X64::InstructionSet
{
	enum class OperandType : Foundation::Uint8
	{
		Unused,
		Register,
		Memory,
		Pointer,
		Immediate
	};

	enum class OperandVisibility : Foundation::Uint8
	{
		Invalid,
		Explicit,
		Implicit,
		Hidden
	};

	enum class OperandAccess : Foundation::Uint8
	{
		None = 0,
		Read = 0x01,
		Write = 0x02,
		ConditionalRead = 0x04,
		ConditionalWrite = 0x08
	};

	using OperandAccessMask = Foundation::Uint8;

	enum class OperandEncoding : Foundation::Uint8
	{
		None,
		ModRmReg,
		ModRmRm,
		Opcode,
		NdsNdd,
		Is4,
		Mask,
		Disp8,
		Disp16,
		Disp32,
		Disp64,
		Disp16_32_64,
		Disp32_32_64,
		Disp16_32_32,
		UImm8,
		UImm16,
		UImm32,
		UImm64,
		UImm16_32_64,
		UImm32_32_64,
		UImm16_32_32,
		SImm8,
		SImm16,
		SImm32,
		SImm64,
		SImm16_32_64,
		SImm32_32_64,
		SImm16_32_32,
		JImm8,
		JImm16,
		JImm32,
		JImm64,
		JImm16_32_64,
		JImm32_32_64,
		JImm16_32_32
	};

	enum class ElementType : Foundation::Uint8
	{
		Invalid,
		Struct,
		UnsignedInteger,
		SignedInteger,
		Float16,
		Float32,
		Float64,
		Float80,
		BFloat16,
		LongBcd,
		ConditionCode
	};

	enum class MemoryOperandType : Foundation::Uint8
	{
		Invalid,
		Memory,
		AddressGeneration,
		Mib,
		Vsib
	};

	struct ImmediateOperand
	{
		Foundation::Uint64 UnsignedValue{};
		Foundation::Int64 SignedValue{};
		Foundation::Bool IsSigned{};
		Foundation::Bool IsRelative{};
		Foundation::Bool HasAbsoluteAddress{};
		Foundation::Uint64 AbsoluteAddress{};

		[[nodiscard]] static constexpr auto Unsigned(
			Foundation::Uint64 Value
		) noexcept -> ImmediateOperand
		{
			ImmediateOperand Result{};
			Result.UnsignedValue = Value;
			Result.SignedValue = static_cast<Foundation::Int64>(Value);
			return Result;
		}

		[[nodiscard]] static constexpr auto Signed(
			Foundation::Int64 Value
		) noexcept -> ImmediateOperand
		{
			ImmediateOperand Result{};
			Result.UnsignedValue = static_cast<Foundation::Uint64>(Value);
			Result.SignedValue = Value;
			Result.IsSigned = true;
			return Result;
		}

		[[nodiscard]] static constexpr auto Relative(
			Foundation::Uint64 Target
		) noexcept -> ImmediateOperand
		{
			auto Result = Unsigned(Target);
			Result.IsRelative = true;
			Result.HasAbsoluteAddress = true;
			Result.AbsoluteAddress = Target;
			return Result;
		}

		[[nodiscard]] static constexpr auto Absolute(
			Foundation::Uint64 Address
		) noexcept -> ImmediateOperand
		{
			auto Result = Unsigned(Address);
			Result.HasAbsoluteAddress = true;
			Result.AbsoluteAddress = Address;
			return Result;
		}
	};

	struct MemoryOperand
	{
		MemoryOperandType Type{ MemoryOperandType::Memory };
		Register Segment{};
		Register Base{};
		Register Index{};
		Foundation::Uint8 Scale{ 1 };
		Foundation::Int64 Displacement{};
		Foundation::Bool HasDisplacement{};
		Foundation::Uint16 SizeInBits{};
		Foundation::Bool HasAbsoluteAddress{};
		Foundation::Uint64 AbsoluteAddress{};

		[[nodiscard]] static constexpr auto BaseDisplacement(
			Register BaseRegister,
			Foundation::Int64 DisplacementValue = 0,
			Foundation::Uint16 WidthInBits = 0
		) noexcept -> MemoryOperand
		{
			MemoryOperand Result{};
			Result.Base = BaseRegister;
			Result.Displacement = DisplacementValue;
			Result.HasDisplacement = DisplacementValue != 0;
			Result.SizeInBits = WidthInBits;
			return Result;
		}

		[[nodiscard]] static constexpr auto Address(
			Register BaseRegister,
			Register IndexRegister,
			Foundation::Uint8 ScaleValue = 1,
			Foundation::Int64 DisplacementValue = 0,
			Foundation::Uint16 WidthInBits = 0
		) noexcept -> MemoryOperand
		{
			MemoryOperand Result{};
			Result.Base = BaseRegister;
			Result.Index = IndexRegister;
			Result.Scale = ScaleValue;
			Result.Displacement = DisplacementValue;
			Result.HasDisplacement = DisplacementValue != 0;
			Result.SizeInBits = WidthInBits;
			return Result;
		}

		[[nodiscard]] static constexpr auto Absolute(
			Foundation::Uint64 Address,
			Foundation::Uint16 WidthInBits = 0
		) noexcept -> MemoryOperand
		{
			MemoryOperand Result{};
			Result.HasDisplacement = true;
			Result.Displacement = static_cast<Foundation::Int64>(Address);
			Result.SizeInBits = WidthInBits;
			Result.HasAbsoluteAddress = true;
			Result.AbsoluteAddress = Address;
			return Result;
		}

		[[nodiscard]] static constexpr auto RipRelative(
			Foundation::Uint64 Target,
			Foundation::Uint16 WidthInBits = 0
		) noexcept -> MemoryOperand
		{
			MemoryOperand Result{};
			Result.Base = Registers::Rip;
			Result.SizeInBits = WidthInBits;
			Result.HasAbsoluteAddress = true;
			Result.AbsoluteAddress = Target;
			return Result;
		}
	};

	struct PointerOperand
	{
		Foundation::Uint16 Segment{};
		Foundation::Uint32 Offset{};
	};

	class Operand
	{
	public:
		constexpr Operand() noexcept = default;

		[[nodiscard]] static constexpr auto FromRegister(
			Register Value,
			Foundation::Uint16 SizeInBits = 0
		) noexcept -> Operand
		{
			Operand Result{};
			Result.Type_ = OperandType::Register;
			Result.Register_ = Value;
			Result.SizeInBits_ = SizeInBits;
			return Result;
		}

		[[nodiscard]] static constexpr auto FromMemory(
			const MemoryOperand& Value
		) noexcept -> Operand
		{
			Operand Result{};
			Result.Type_ = OperandType::Memory;
			Result.Memory_ = Value;
			Result.SizeInBits_ = Value.SizeInBits;
			return Result;
		}

		[[nodiscard]] static constexpr auto FromImmediate(
			const ImmediateOperand& Value,
			Foundation::Uint16 SizeInBits = 0
		) noexcept -> Operand
		{
			Operand Result{};
			Result.Type_ = OperandType::Immediate;
			Result.Immediate_ = Value;
			Result.SizeInBits_ = SizeInBits;
			return Result;
		}

		[[nodiscard]] static constexpr auto FromPointer(
			const PointerOperand& Value
		) noexcept -> Operand
		{
			Operand Result{};
			Result.Type_ = OperandType::Pointer;
			Result.Pointer_ = Value;
			return Result;
		}

		[[nodiscard]] constexpr auto Type() const noexcept -> OperandType
		{
			return Type_;
		}

		[[nodiscard]] constexpr auto Visibility() const noexcept -> OperandVisibility
		{
			return Visibility_;
		}

		[[nodiscard]] constexpr auto Access() const noexcept -> OperandAccessMask
		{
			return Access_;
		}

		[[nodiscard]] constexpr auto Encoding() const noexcept -> OperandEncoding
		{
			return Encoding_;
		}

		[[nodiscard]] constexpr auto SizeInBits() const noexcept -> Foundation::Uint16
		{
			return SizeInBits_;
		}

		[[nodiscard]] constexpr auto Element() const noexcept -> ElementType
		{
			return ElementType_;
		}

		[[nodiscard]] constexpr auto ElementSizeInBits() const noexcept -> Foundation::Uint16
		{
			return ElementSizeInBits_;
		}

		[[nodiscard]] constexpr auto ElementCount() const noexcept -> Foundation::Uint16
		{
			return ElementCount_;
		}

		[[nodiscard]] constexpr auto RegisterValue() const noexcept -> Register
		{
			return Register_;
		}

		[[nodiscard]] constexpr auto MemoryValue() const noexcept -> const MemoryOperand&
		{
			return Memory_;
		}

		[[nodiscard]] constexpr auto ImmediateValue() const noexcept -> const ImmediateOperand&
		{
			return Immediate_;
		}

		[[nodiscard]] constexpr auto PointerValue() const noexcept -> const PointerOperand&
		{
			return Pointer_;
		}

		[[nodiscard]] constexpr auto Reads() const noexcept -> Foundation::Bool
		{
			return (Access_ & (
				static_cast<OperandAccessMask>(OperandAccess::Read) |
				static_cast<OperandAccessMask>(OperandAccess::ConditionalRead)
			)) != 0;
		}

		[[nodiscard]] constexpr auto Writes() const noexcept -> Foundation::Bool
		{
			return (Access_ & (
				static_cast<OperandAccessMask>(OperandAccess::Write) |
				static_cast<OperandAccessMask>(OperandAccess::ConditionalWrite)
			)) != 0;
		}

		constexpr auto SetDecodedMetadata(
			OperandVisibility Visibility,
			OperandAccessMask Access,
			OperandEncoding Encoding,
			Foundation::Uint16 SizeInBits,
			ElementType Element,
			Foundation::Uint16 ElementSizeInBits,
			Foundation::Uint16 ElementCount
		) noexcept -> Foundation::Void
		{
			Visibility_ = Visibility;
			Access_ = Access;
			Encoding_ = Encoding;
			SizeInBits_ = SizeInBits;
			ElementType_ = Element;
			ElementSizeInBits_ = ElementSizeInBits;
			ElementCount_ = ElementCount;
		}

	private:
		OperandType Type_{ OperandType::Unused };
		OperandVisibility Visibility_{ OperandVisibility::Explicit };
		OperandAccessMask Access_{};
		OperandEncoding Encoding_{ OperandEncoding::None };
		Foundation::Uint16 SizeInBits_{};
		ElementType ElementType_{ ElementType::Invalid };
		Foundation::Uint16 ElementSizeInBits_{};
		Foundation::Uint16 ElementCount_{};

		Register Register_{};
		MemoryOperand Memory_{};
		ImmediateOperand Immediate_{};
		PointerOperand Pointer_{};
	};

	[[nodiscard]] constexpr auto Reg(Register Value) noexcept -> Operand
	{
		return Operand::FromRegister(Value);
	}

	[[nodiscard]] constexpr auto Mem(const MemoryOperand& Value) noexcept -> Operand
	{
		return Operand::FromMemory(Value);
	}

	[[nodiscard]] constexpr auto Imm(Foundation::Uint64 Value) noexcept -> Operand
	{
		return Operand::FromImmediate(ImmediateOperand::Unsigned(Value));
	}

	[[nodiscard]] constexpr auto SImm(Foundation::Int64 Value) noexcept -> Operand
	{
		return Operand::FromImmediate(ImmediateOperand::Signed(Value));
	}

	[[nodiscard]] constexpr auto Relative(Foundation::Uint64 Target) noexcept -> Operand
	{
		return Operand::FromImmediate(ImmediateOperand::Relative(Target));
	}

	[[nodiscard]] constexpr auto Absolute(Foundation::Uint64 Address) noexcept -> Operand
	{
		return Operand::FromImmediate(ImmediateOperand::Absolute(Address));
	}

	[[nodiscard]] constexpr auto Ptr(
		Foundation::Uint16 Segment,
		Foundation::Uint32 Offset
	) noexcept -> Operand
	{
		return Operand::FromPointer(PointerOperand{ Segment, Offset });
	}

	[[nodiscard]] constexpr auto BytePtr(
		Register Base,
		Foundation::Int64 Displacement = 0
	) noexcept -> Operand
	{
		return Mem(MemoryOperand::BaseDisplacement(Base, Displacement, 8));
	}

	[[nodiscard]] constexpr auto WordPtr(
		Register Base,
		Foundation::Int64 Displacement = 0
	) noexcept -> Operand
	{
		return Mem(MemoryOperand::BaseDisplacement(Base, Displacement, 16));
	}

	[[nodiscard]] constexpr auto DwordPtr(
		Register Base,
		Foundation::Int64 Displacement = 0
	) noexcept -> Operand
	{
		return Mem(MemoryOperand::BaseDisplacement(Base, Displacement, 32));
	}

	[[nodiscard]] constexpr auto QwordPtr(
		Register Base,
		Foundation::Int64 Displacement = 0
	) noexcept -> Operand
	{
		return Mem(MemoryOperand::BaseDisplacement(Base, Displacement, 64));
	}

	[[nodiscard]] constexpr auto Address(
		Register Base,
		Register Index,
		Foundation::Uint8 Scale = 1,
		Foundation::Int64 Displacement = 0,
		Foundation::Uint16 WidthInBits = 0
	) noexcept -> Operand
	{
		return Mem(MemoryOperand::Address(
			Base, Index, Scale, Displacement, WidthInBits
		));
	}

	[[nodiscard]] constexpr auto AbsolutePtr(
		Foundation::Uint64 Target,
		Foundation::Uint16 WidthInBits = 0
	) noexcept -> Operand
	{
		return Mem(MemoryOperand::Absolute(Target, WidthInBits));
	}

	[[nodiscard]] constexpr auto RipPtr(
		Foundation::Uint64 Target,
		Foundation::Uint16 WidthInBits = 0
	) noexcept -> Operand
	{
		return Mem(MemoryOperand::RipRelative(Target, WidthInBits));
	}
}
