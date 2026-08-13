#include <UEFIpp/Architecture/X64/Disassembly/Decoder.hpp>

#include "../Zydis/Conversion.hpp"

namespace UEFIpp::Architecture::X64::Disassembly
{
	auto Decoder::Decode(
		Library::Span<const Foundation::Byte> Code,
		Foundation::Uint64 RuntimeAddress
	) const noexcept -> Library::Expected<
		DecodedInstruction,
		DecodeError
	>
	{
		return ZydisBackend::Decode(
			Code,
			RuntimeAddress,
			Mode_,
			Stack_
		);
	}

	auto Decoder::DecodeAt(
		Memory::MemoryView Memory,
		Foundation::Uint64 Address
	) const noexcept -> Library::Expected<
		DecodedInstruction,
		DecodeAtErrorInfo
	>
	{
		if (!Memory)
		{
			return Library::Expected<
				DecodedInstruction,
				DecodeAtErrorInfo
			>{
				Library::Unexpect,
				DecodeAtErrorInfo{
					.Code = DecodeAtError::InvalidView,
					.Address = Address
				}
			};
		}

		Foundation::Byte Buffer[
			DecodedInstruction::MaximumLength
		]{};

		Memory::MemoryViewErrorInfo LastMemoryError{};
		Foundation::Bool HadMemoryError{};
		Foundation::Size BytesAvailable{};

		for (
			Foundation::Size Requested =
				DecodedInstruction::MaximumLength;
			Requested != 0;
			--Requested
		)
		{
			auto Read = Memory.ReadPartial(
				Address,
				Library::Span<Foundation::Byte>{
					Buffer,
					Requested
				}
			);

			if (!Read)
			{
				LastMemoryError = Read.Error();
				HadMemoryError = true;
				continue;
			}

			if (!Read.Value())
			{
				continue;
			}

			BytesAvailable = Read.Value();
			break;
		}

		if (!BytesAvailable)
		{
			return Library::Expected<
				DecodedInstruction,
				DecodeAtErrorInfo
			>{
				Library::Unexpect,
				DecodeAtErrorInfo{
					.Code = DecodeAtError::MemoryReadFailure,
					.Address = Address,
					.Memory = HadMemoryError
						? LastMemoryError
						: Memory::MemoryViewErrorInfo{
							.Code = Memory::MemoryViewError::PartialAccess,
							.Operation = Memory::MemoryViewOperation::Read,
							.Address = Address,
							.Size = DecodedInstruction::MaximumLength
						}
				}
			};
		}

		auto Result = Decode(
			Library::Span<const Foundation::Byte>{
				Buffer,
				BytesAvailable
			},
			Address
		);

		if (!Result)
		{
			return Library::Expected<
				DecodedInstruction,
				DecodeAtErrorInfo
			>{
				Library::Unexpect,
				DecodeAtErrorInfo{
					.Code = DecodeAtError::DecodeFailure,
					.Address = Address,
					.BytesAvailable = BytesAvailable,
					.Decode = Result.Error(),
					.Memory = LastMemoryError
				}
			};
		}

		return Result.Value();
	}
}
