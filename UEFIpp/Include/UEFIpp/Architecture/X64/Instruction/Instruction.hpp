#pragma once

#include <UEFIpp/Architecture/X64/Instruction/Mnemonic.hpp>
#include <UEFIpp/Architecture/X64/Instruction/Operand.hpp>
#include <UEFIpp/Architecture/X64/Instruction/Encoding.hpp>
#include <UEFIpp/Library/Containers/Array.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>

namespace UEFIpp::Architecture::X64::InstructionSet
{
	class Instruction
	{
	public:
		static constexpr Foundation::Size MaximumOperandCount = 5;

	public:
		constexpr Instruction() noexcept = default;

		constexpr explicit Instruction(
			Mnemonic InstructionMnemonic
		) noexcept :
			Mnemonic_(InstructionMnemonic)
		{
		}

		[[nodiscard]] constexpr auto MnemonicValue() const noexcept
			-> Mnemonic
		{
			return Mnemonic_;
		}

		constexpr auto SetMnemonic(
			Mnemonic Value
		) noexcept -> Foundation::Void
		{
			Mnemonic_ = Value;
		}

		[[nodiscard]] constexpr auto Options() noexcept
			-> EncodingOptions&
		{
			return Options_;
		}

		[[nodiscard]] constexpr auto Options() const noexcept
			-> const EncodingOptions&
		{
			return Options_;
		}

		[[nodiscard]] constexpr auto Operands() noexcept
			-> Library::Span<Operand>
		{
			return { Operands_.Data(), OperandCount_ };
		}

		[[nodiscard]] constexpr auto Operands() const noexcept
			-> Library::Span<const Operand>
		{
			return { Operands_.Data(), OperandCount_ };
		}

		[[nodiscard]] constexpr auto OperandCount() const noexcept
			-> Foundation::Size
		{
			return OperandCount_;
		}

		[[nodiscard]] constexpr auto AddOperand(
			const Operand& Value
		) noexcept -> Foundation::Bool
		{
			if (OperandCount_ >= MaximumOperandCount)
			{
				return false;
			}

			Operands_[OperandCount_++] = Value;
			return true;
		}

		[[nodiscard]] constexpr auto OperandAt(
			Foundation::Size Index
		) noexcept -> Operand*
		{
			return Index < OperandCount_
				? &Operands_[Index]
				: nullptr;
		}

		[[nodiscard]] constexpr auto OperandAt(
			Foundation::Size Index
		) const noexcept -> const Operand*
		{
			return Index < OperandCount_
				? &Operands_[Index]
				: nullptr;
		}

	private:
		Mnemonic Mnemonic_{ Mnemonic::Invalid };
		EncodingOptions Options_{};
		Library::Array<Operand, MaximumOperandCount> Operands_{};
		Foundation::Uint8 OperandCount_{};
	};
}
