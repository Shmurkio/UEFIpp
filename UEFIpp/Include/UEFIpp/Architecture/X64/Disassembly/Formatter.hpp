#pragma once

#include <UEFIpp/Architecture/X64/Instruction/DecodedInstruction.hpp>
#include <UEFIpp/Architecture/X64/Disassembly/Error.hpp>
#include <UEFIpp/Memory/AllocatorStub.hpp>
#include <UEFIpp/Library/Containers/Expected.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Library/String/StringView.hpp>

namespace UEFIpp::Architecture::X64::Disassembly
{
	enum class FormatterStyle : Foundation::Uint8
	{
		Intel,
		Att
	};

	class Formatter
	{
	public:
		constexpr explicit Formatter(
			FormatterStyle Style = FormatterStyle::Intel,
			Memory::AllocatorStub Allocator = {}
		) noexcept :
			Style_(Style),
			Allocator_(Allocator)
		{
		}

		[[nodiscard]] auto Format(
			const InstructionSet::DecodedInstruction& Instruction
		) const -> Library::Expected<
			Library::String,
			FormatError
		>;

		[[nodiscard]] auto FormatTo(
			const InstructionSet::DecodedInstruction& Instruction,
			Library::Span<Foundation::Char> Buffer
		) const noexcept -> Library::Expected<
			Library::StringView,
			FormatError
		>;

		[[nodiscard]] constexpr auto Style() const noexcept -> FormatterStyle
		{
			return Style_;
		}

		[[nodiscard]] constexpr auto Allocator() const noexcept -> Memory::AllocatorStub
		{
			return Allocator_;
		}

	private:
		FormatterStyle Style_{};
		Memory::AllocatorStub Allocator_{};
	};
}
