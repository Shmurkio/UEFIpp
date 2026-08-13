#pragma once

#include <UEFIpp/Architecture/X64/Common/Registers.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>
#include <intrin.h>

extern "C" auto ReadRspAsm() -> UEFIpp::Foundation::Uint64;

namespace UEFIpp::Architecture::X64 {
class CpuidResult {
public:
  Foundation::Int32 Eax{};
  Foundation::Int32 Ebx{};
  Foundation::Int32 Ecx{};
  Foundation::Int32 Edx{};
};

class Cpu {
public:
  Cpu() = delete;

  [[nodiscard]] static auto ReadCr0() -> Register;

  static auto WriteCr0(Register Value) -> Foundation::Void;

  [[nodiscard]] static auto ReadCr2() -> Register;

  [[nodiscard]] static auto ReadCr3() -> Register;

  static auto WriteCr3(Register Value) -> Foundation::Void;

  [[nodiscard]] static auto ReadCr4() -> Register;

  static auto WriteCr4(Register Value) -> Foundation::Void;

  [[nodiscard]] static auto ReadFlags() -> Register;

  static auto WriteFlags(Register Value) -> Foundation::Void;

  [[nodiscard]] static auto ReadRsp() -> Register;

  [[nodiscard]] static auto ReadTsc() -> Foundation::Uint64;

  [[nodiscard]] static auto Cpuid(Foundation::Int32 Leaf,
                                  Foundation::Int32 Subleaf = 0) -> CpuidResult;

  static auto DisableWriteProtect() -> Register;

  static auto RestoreCr0(Register Value) -> Foundation::Void;

  [[nodiscard]] static auto InterruptsEnabled() -> Foundation::Bool;

  static auto EnableInterrupts() -> Foundation::Void;

  static auto DisableInterrupts() -> Foundation::Void;

  static auto RestoreInterrupts(Register OldFlags) -> Foundation::Void;

  static auto Halt() -> Foundation::Void;

  static auto Pause() -> Foundation::Void;
};
} // namespace UEFIpp::Architecture::X64
