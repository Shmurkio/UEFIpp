#include "Conversion.hpp"

namespace UEFIpp::Architecture::X64::ZydisBackend
{
	auto Format(
		const InstructionSet::DecodedInstruction& Instruction,
		Disassembly::FormatterStyle Style,
		Library::Span<Foundation::Char> Buffer
	) noexcept -> Library::Expected<
		Library::StringView,
		Disassembly::FormatError
	>
	{
		if (Buffer.Empty() || Instruction.Bytes().Empty())
		{
			return Library::MakeUnexpected(
				Disassembly::FormatError::InvalidArgument
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
				Disassembly::FormatError::DecodeFailure
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
				Disassembly::FormatError::DecodeFailure
			);
		}

		ZydisFormatter Formatter{};

		const auto NativeStyle =
			Style == Disassembly::FormatterStyle::Att
			? ZYDIS_FORMATTER_STYLE_ATT
			: ZYDIS_FORMATTER_STYLE_INTEL;

		if (!ZYAN_SUCCESS(ZydisFormatterInit(
			&Formatter,
			NativeStyle
		)))
		{
			return Library::MakeUnexpected(
				Disassembly::FormatError::BackendFailure
			);
		}

		const auto Status = ZydisFormatterFormatInstruction(
			&Formatter,
			&NativeInstruction,
			NativeOperands,
			NativeInstruction.operand_count_visible,
			Buffer.Data(),
			Buffer.Size(),
			Instruction.Address(),
			nullptr
		);

		if (!ZYAN_SUCCESS(Status))
		{
			return Library::MakeUnexpected(
				Status == ZYAN_STATUS_INSUFFICIENT_BUFFER_SIZE
				? Disassembly::FormatError::BufferTooSmall
				: Disassembly::FormatError::BackendFailure
			);
		}

		Foundation::Size Length{};

		while (
			Length < Buffer.Size() &&
			Buffer[Length] != '\0'
		)
		{
			++Length;
		}

		return Library::StringView{
			Buffer.Data(),
			Length
		};
	}
}
