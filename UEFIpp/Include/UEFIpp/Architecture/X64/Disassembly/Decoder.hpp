#pragma once

#include <UEFIpp/Architecture/X64/Instruction/DecodedInstruction.hpp>
#include <UEFIpp/Architecture/X64/Disassembly/Error.hpp>
#include <UEFIpp/Library/Containers/Expected.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Memory/MemoryView.hpp>

namespace UEFIpp::Architecture::X64::Disassembly
{
	using InstructionSet::DecodedInstruction;
	using InstructionSet::MachineMode;

	enum class StackWidth : Foundation::Uint8
	{
		Width16,
		Width32,
		Width64
	};

	class Decoder
	{
	public:
		constexpr Decoder(
			MachineMode Mode = MachineMode::Long64,
			StackWidth Stack = StackWidth::Width64
		) noexcept :
			Mode_(Mode),
			Stack_(Stack)
		{
		}

		[[nodiscard]] auto Decode(
			Library::Span<const Foundation::Byte> Code,
			Foundation::Uint64 RuntimeAddress = 0
		) const noexcept -> Library::Expected<
			DecodedInstruction,
			DecodeError
		>;

		[[nodiscard]] auto DecodeAt(
			Memory::MemoryView Memory,
			Foundation::Uint64 Address
		) const noexcept -> Library::Expected<
			DecodedInstruction,
			DecodeAtErrorInfo
		>;

		[[nodiscard]] constexpr auto Mode() const noexcept
			-> MachineMode
		{
			return Mode_;
		}

		[[nodiscard]] constexpr auto Stack() const noexcept
			-> StackWidth
		{
			return Stack_;
		}

	private:
		MachineMode Mode_{};
		StackWidth Stack_{};
	};
}
