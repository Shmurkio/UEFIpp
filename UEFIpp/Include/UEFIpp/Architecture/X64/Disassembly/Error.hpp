#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Memory/MemoryAccess.hpp>

namespace UEFIpp::Architecture::X64::Disassembly
{
	enum class DecodeError : Foundation::Uint8
	{
		InvalidArgument,
		NoMoreData,
		InvalidInstruction,
		InstructionTooLong,
		UnsupportedMode,
		BackendFailure
	};

	enum class DecodeAtError : Foundation::Uint8
	{
		InvalidView,
		MemoryReadFailure,
		DecodeFailure
	};

	struct DecodeAtErrorInfo
	{
		DecodeAtError Code{ DecodeAtError::DecodeFailure };
		Foundation::Uint64 Address{};
		Foundation::Size BytesAvailable{};
		DecodeError Decode{ DecodeError::InvalidArgument };
		Memory::MemoryViewErrorInfo Memory{};
	};

	enum class FormatError : Foundation::Uint8
	{
		InvalidArgument,
		DecodeFailure,
		BufferTooSmall,
		AllocationFailure,
		BackendFailure
	};

	enum class DisassemblyError : Foundation::Uint8
	{
		InvalidArgument,
		DecodeFailure,
		AllocationFailure
	};
}
