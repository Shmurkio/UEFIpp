#include <UEFIpp/Architecture/X64/Disassembly/Formatter.hpp>

#include "../Zydis/Conversion.hpp"

namespace UEFIpp::Architecture::X64::Disassembly
{
	auto Formatter::Format(
		const InstructionSet::DecodedInstruction& Instruction
	) const -> Library::Expected<
		Library::String,
		FormatError
	>
	{
		Foundation::Char Buffer[512]{};

		auto Formatted = ZydisBackend::Format(
			Instruction,
			Style_,
			Buffer
		);

		if (!Formatted)
		{
			return Library::MakeUnexpected(
				Formatted.Error()
			);
		}

		Library::String Result{ Allocator_ };

		if (!Result.Assign(Formatted.Value()))
		{
			return Library::MakeUnexpected(
				FormatError::AllocationFailure
			);
		}

		return Result;
	}

	auto Formatter::FormatTo(
		const InstructionSet::DecodedInstruction& Instruction,
		Library::Span<Foundation::Char> Buffer
	) const noexcept -> Library::Expected<
		Library::StringView,
		FormatError
	>
	{
		return ZydisBackend::Format(
			Instruction,
			Style_,
			Buffer
		);
	}
}

namespace UEFIpp::Architecture::X64::Disassembly
{

}
