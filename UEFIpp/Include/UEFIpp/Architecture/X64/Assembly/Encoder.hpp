#pragma once

#include <UEFIpp/Architecture/X64/Instruction/Instruction.hpp>
#include <UEFIpp/Architecture/X64/Instruction/DecodedInstruction.hpp>
#include <UEFIpp/Architecture/X64/Assembly/Error.hpp>
#include <UEFIpp/Library/Containers/Expected.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>

namespace UEFIpp::Architecture::X64::Assembly
{
	class Encoder
	{
	public:
		[[nodiscard]] auto Encode(
			const InstructionSet::Instruction& Instruction,
			Library::Span<Foundation::Byte> Output,
			Foundation::Uint64 RuntimeAddress = 0
		) const noexcept -> Library::Expected<
			Foundation::Size,
			EncodeError
		>;

		[[nodiscard]] auto EncodeSize(
			const InstructionSet::Instruction& Instruction,
			Foundation::Uint64 RuntimeAddress = 0
		) const noexcept -> Library::Expected<
			Foundation::Size,
			EncodeError
		>;


		[[nodiscard]] auto Reencode(
			const InstructionSet::DecodedInstruction& Instruction,
			Library::Span<Foundation::Byte> Output,
			Foundation::Uint64 RuntimeAddress
		) const noexcept -> Library::Expected<
			Foundation::Size,
			EncodeError
		>;

		[[nodiscard]] auto ReencodeSize(
			const InstructionSet::DecodedInstruction& Instruction,
			Foundation::Uint64 RuntimeAddress
		) const noexcept -> Library::Expected<
			Foundation::Size,
			EncodeError
		>;
	};
}
