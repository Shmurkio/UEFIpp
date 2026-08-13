#include <UEFIpp/Architecture/X64/Assembly/Encoder.hpp>

#include "../Zydis/Conversion.hpp"

namespace UEFIpp::Architecture::X64::Assembly
{
	auto Encoder::Encode(
		const InstructionSet::Instruction& Instruction,
		Library::Span<Foundation::Byte> Output,
		Foundation::Uint64 RuntimeAddress
	) const noexcept -> Library::Expected<
		Foundation::Size,
		EncodeError
	>
	{
		return ZydisBackend::Encode(
			Instruction,
			Output,
			RuntimeAddress
		);
	}

	auto Encoder::EncodeSize(
		const InstructionSet::Instruction& Instruction,
		Foundation::Uint64 RuntimeAddress
	) const noexcept -> Library::Expected<
		Foundation::Size,
		EncodeError
	>
	{
		Foundation::Byte Buffer[
			InstructionSet::DecodedInstruction::MaximumLength
		]{};

		return Encode(
			Instruction,
			Buffer,
			RuntimeAddress
		);
	}
	auto Encoder::Reencode(
		const InstructionSet::DecodedInstruction& Instruction,
		Library::Span<Foundation::Byte> Output,
		Foundation::Uint64 RuntimeAddress
	) const noexcept -> Library::Expected<
		Foundation::Size,
		EncodeError
	>
	{
		return ZydisBackend::Reencode(
			Instruction,
			Output,
			RuntimeAddress
		);
	}

	auto Encoder::ReencodeSize(
		const InstructionSet::DecodedInstruction& Instruction,
		Foundation::Uint64 RuntimeAddress
	) const noexcept -> Library::Expected<
		Foundation::Size,
		EncodeError
	>
	{
		Foundation::Byte Buffer[
			InstructionSet::DecodedInstruction::MaximumLength
		]{};

		return Reencode(
			Instruction,
			Buffer,
			RuntimeAddress
		);
	}

}
