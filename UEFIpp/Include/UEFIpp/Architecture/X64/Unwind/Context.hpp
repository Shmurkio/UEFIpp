#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Architecture::X64::Unwind
{
	struct Context
	{
		Foundation::UintPtr Rip{};
		Foundation::UintPtr Rsp{};
		Foundation::UintPtr Rbx{};
		Foundation::UintPtr Rbp{};
		Foundation::UintPtr Rsi{};
		Foundation::UintPtr Rdi{};
		Foundation::UintPtr R12{};
		Foundation::UintPtr R13{};
		Foundation::UintPtr R14{};
		Foundation::UintPtr R15{};
	};

	static_assert(sizeof(Context) == 0x50);
}