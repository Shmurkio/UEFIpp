#pragma once

#include <cstdint>

namespace Cpu
{
#pragma pack(push, 1)
	typedef struct _IDTR
	{
		uint16_t Limit;
		uint64_t Base;
	} IDTR, *PIDTR;
#pragma pack(pop)

	auto ReadCr0() -> uint64_t;
	auto WriteCr0(uint64_t Value) -> void;

	auto ReadCr3() -> uint64_t;
	auto WriteCr3(uint64_t Value) -> void;

	auto ReadIdtr(IDTR& Idtr) -> void;
	auto WriteIdtr(IDTR& Idtr) -> void;
	auto ReadCs() -> uint16_t;
}