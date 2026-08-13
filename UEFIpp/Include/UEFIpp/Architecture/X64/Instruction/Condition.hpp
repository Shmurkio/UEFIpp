#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Architecture::X64::InstructionSet
{
	enum class Condition : Foundation::Uint8
	{
		Overflow,
		NotOverflow,
		Below,
		AboveEqual,
		Equal,
		NotEqual,
		BelowEqual,
		Above,
		Sign,
		NotSign,
		Parity,
		NotParity,
		Less,
		GreaterEqual,
		LessEqual,
		Greater
	};
}
