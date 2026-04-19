#include "Cpu.hpp"
#include <intrin.h>

extern "C" uint16_t CpuReadCs();

auto Cpu::ReadCr0() -> uint64_t
{
	return __readcr0();
}

auto Cpu::WriteCr0(uint64_t Value) -> void
{
	__writecr0(Value);
}

auto Cpu::ReadCr3() -> uint64_t
{
	return __readcr3();
}

auto Cpu::WriteCr3(uint64_t Value) -> void
{
	__writecr3(Value);
}

auto Cpu::ReadIdtr(IDTR& Idtr) -> void
{
	__sidt(&Idtr);
}

auto Cpu::WriteIdtr(IDTR& Idtr) -> void
{
	__lidt(&Idtr);
}

auto Cpu::ReadCs() -> uint16_t
{
	return CpuReadCs();
}