#include <UEFIpp/Architecture/X64/Cpu/Cpu.hpp>

namespace UEFIpp::Architecture::X64 {
auto Cpu::ReadCr0() -> Register { return __readcr0(); }

auto Cpu::WriteCr0(Register Value) -> Foundation::Void { __writecr0(Value); }

auto Cpu::ReadCr2() -> Register { return __readcr2(); }

auto Cpu::ReadCr3() -> Register { return __readcr3(); }

auto Cpu::WriteCr3(Register Value) -> Foundation::Void { __writecr3(Value); }

auto Cpu::ReadCr4() -> Register { return __readcr4(); }

auto Cpu::WriteCr4(Register Value) -> Foundation::Void { __writecr4(Value); }

auto Cpu::ReadFlags() -> Register { return __readeflags(); }

auto Cpu::WriteFlags(Register Value) -> Foundation::Void {
  __writeeflags(Value);
}

auto Cpu::ReadRsp() -> Register { return ReadRspAsm(); }

auto Cpu::ReadTsc() -> Foundation::Uint64 { return __rdtsc(); }

auto Cpu::Cpuid(Foundation::Int32 Leaf, Foundation::Int32 Subleaf)
    -> CpuidResult {
  Foundation::Int32 Values[4]{};
  __cpuidex(Values, Leaf, Subleaf);
  return {Values[0], Values[1], Values[2], Values[3]};
}

auto Cpu::DisableWriteProtect() -> Register {
  const auto Old = ReadCr0();
  WriteCr0(Foundation::Bit::Clear(Old, Cr0::WriteProtect));
  return Old;
}

auto Cpu::RestoreCr0(Register Value) -> Foundation::Void { WriteCr0(Value); }

auto Cpu::InterruptsEnabled() -> Foundation::Bool {
  return Foundation::Bit::IsSet(ReadFlags(), Rflags::InterruptEnable);
}

auto Cpu::EnableInterrupts() -> Foundation::Void { _enable(); }

auto Cpu::DisableInterrupts() -> Foundation::Void { _disable(); }

auto Cpu::RestoreInterrupts(Register OldFlags) -> Foundation::Void {
  if (Foundation::Bit::IsSet(OldFlags, Rflags::InterruptEnable)) {
    EnableInterrupts();
  } else {
    DisableInterrupts();
  }
}

auto Cpu::Halt() -> Foundation::Void { __halt(); }

auto Cpu::Pause() -> Foundation::Void { _mm_pause(); }
} // namespace UEFIpp::Architecture::X64
