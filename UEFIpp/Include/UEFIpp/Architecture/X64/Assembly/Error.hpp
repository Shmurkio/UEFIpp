#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Architecture::X64::Assembly
{
	enum class EncodeError : Foundation::Uint8
	{
		InvalidArgument,
		TooManyOperands,
		InvalidOperand,
		ImpossibleInstruction,
		BufferTooSmall,
		BackendFailure
	};

	enum class AssemblyError : Foundation::Uint8
	{
		InvalidArgument,
		AllocationFailure,
		InvalidLabel,
		LabelAlreadyBound,
		UnboundLabel,
		InvalidSection,
		EncodingFailure,
		LayoutDidNotConverge,
		InternalError
	};
}
