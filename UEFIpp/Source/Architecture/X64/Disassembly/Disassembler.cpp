#include <UEFIpp/Architecture/X64/Disassembly/Disassembler.hpp>

namespace UEFIpp::Architecture::X64::Disassembly
{
	auto Disassembler::Disassemble(
		Library::Span<const Foundation::Byte> Code,
		Foundation::Uint64 BaseAddress
	) const -> Library::Expected<
		Disassembly,
		DisassemblyError
	>
	{
		if (Code.IsNull())
		{
			return Library::MakeUnexpected(
				DisassemblyError::InvalidArgument
			);
		}

		Disassembly Result{ Allocator_ };
		Result.BaseAddress_ = BaseAddress;
		Result.SizeInBytes_ = Code.Size();

		if (!Result.Instructions_.Reserve(
			Code.Size() / 2 + 1
		))
		{
			return Library::MakeUnexpected(
				DisassemblyError::AllocationFailure
			);
		}

		Foundation::Size Offset{};

		while (Offset < Code.Size())
		{
			auto Decoded = Decoder_.Decode(
				Code.Subspan(Offset),
				BaseAddress + Offset
			);

			if (!Decoded)
			{
				return Library::MakeUnexpected(
					DisassemblyError::DecodeFailure
				);
			}

			const auto Length = Decoded->Length();

			if (!Length)
			{
				return Library::MakeUnexpected(
					DisassemblyError::DecodeFailure
				);
			}

			if (!Result.Instructions_.PushBack(
				Foundation::Utility::Move(Decoded.Value())
			))
			{
				return Library::MakeUnexpected(
					DisassemblyError::AllocationFailure
				);
			}

			Offset += Length;
		}

		return Result;
	}
}

namespace UEFIpp::Architecture::X64::Disassembly
{

}
