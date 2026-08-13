#pragma once

#include <UEFIpp/Architecture/X64/Instruction/Register.hpp>
#include <UEFIpp/Library/Containers/Array.hpp>

namespace UEFIpp::Architecture::X64::InstructionSet
{
	class RegisterSet
	{
	private:
		static constexpr Foundation::Size WordBits = 64;
		static constexpr Foundation::Size WordCount =
			(RegisterCount + WordBits - 1) / WordBits;

	public:
		constexpr RegisterSet() noexcept = default;

		constexpr auto Clear() noexcept -> Foundation::Void
		{
			Words_.Fill(0);
		}

		constexpr auto Add(
			Register Value
		) noexcept -> Foundation::Void
		{
			const auto Index =
				static_cast<Foundation::Size>(Value.Id());

			if (Index >= RegisterCount)
			{
				return;
			}

			Words_[Index / WordBits] |=
				Foundation::Uint64{ 1 } << (Index % WordBits);
		}

		constexpr auto Remove(
			Register Value
		) noexcept -> Foundation::Void
		{
			const auto Index =
				static_cast<Foundation::Size>(Value.Id());

			if (Index >= RegisterCount)
			{
				return;
			}

			Words_[Index / WordBits] &=
				~(Foundation::Uint64{ 1 } << (Index % WordBits));
		}

		[[nodiscard]] constexpr auto Contains(
			Register Value
		) const noexcept -> Foundation::Bool
		{
			const auto Index =
				static_cast<Foundation::Size>(Value.Id());

			if (Index >= RegisterCount)
			{
				return false;
			}

			return Foundation::Bit::HasAny(
				Words_[Index / WordBits],
				Foundation::Uint64{ 1 } << (Index % WordBits)
			);
		}

		[[nodiscard]] constexpr auto Empty() const noexcept
			-> Foundation::Bool
		{
			for (const auto Word : Words_)
			{
				if (Word)
				{
					return false;
				}
			}

			return true;
		}

	private:
		Library::Array<Foundation::Uint64, WordCount> Words_{};
	};
}
