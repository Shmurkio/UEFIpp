#pragma once

#include <UEFIpp/Architecture/X64/Disassembly/Decoder.hpp>
#include <UEFIpp/Architecture/X64/Disassembly/Disassembly.hpp>

namespace UEFIpp::Architecture::X64::Disassembly
{
	class Disassembler
	{
	public:
		constexpr explicit Disassembler(
			Memory::AllocatorStub Allocator = {},
			MachineMode Mode = MachineMode::Long64,
			StackWidth Stack = StackWidth::Width64
		) noexcept :
			Allocator_(Allocator),
			Decoder_(Mode, Stack)
		{
		}

		[[nodiscard]] auto Disassemble(
			Library::Span<const Foundation::Byte> Code,
			Foundation::Uint64 BaseAddress = 0
		) const -> Library::Expected<
			Disassembly,
			DisassemblyError
		>;

		[[nodiscard]] constexpr auto Allocator() const noexcept
			-> Memory::AllocatorStub
		{
			return Allocator_;
		}

		[[nodiscard]] constexpr auto GetDecoder() const noexcept
			-> const Decoder&
		{
			return Decoder_;
		}

	private:
		Memory::AllocatorStub Allocator_{};
		Decoder Decoder_{};
	};
}
