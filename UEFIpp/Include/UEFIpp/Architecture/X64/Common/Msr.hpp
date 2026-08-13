#pragma once

#include <UEFIpp/Architecture/X64/Common/Registers.hpp>
#include <intrin.h>

namespace UEFIpp::Architecture::X64 {
enum class MsrIndex : Foundation::Uint32 {
  ApicBase = 0x0000001B,
  FeatureControl = 0x0000003A,

  SysenterCs = 0x00000174,
  SysenterEsp = 0x00000175,
  SysenterEip = 0x00000176,

  DebugControl = 0x000001D9,
  Pat = 0x00000277,
  TscDeadline = 0x000006E0,

  Efer = 0xC0000080,
  Star = 0xC0000081,
  LStar = 0xC0000082,
  CStar = 0xC0000083,
  SfMask = 0xC0000084,

  FsBase = 0xC0000100,
  GsBase = 0xC0000101,
  KernelGsBase = 0xC0000102,
};

class FeatureControlMsr {
public:
  FeatureControlMsr() = delete;

  static constexpr auto Lock = Foundation::Bit::Mask<Register>(0);
  static constexpr auto EnableVmxInsideSmx = Foundation::Bit::Mask<Register>(1);
  static constexpr auto EnableVmxOutsideSmx =
      Foundation::Bit::Mask<Register>(2);

  [[nodiscard]] static constexpr auto IsLocked(Register Value)
      -> Foundation::Bool {
    return Foundation::Bit::IsSet(Value, Lock);
  }

  [[nodiscard]] static constexpr auto IsVmxEnabledOutsideSmx(Register Value)
      -> Foundation::Bool {
    return Foundation::Bit::IsSet(Value, EnableVmxOutsideSmx);
  }
};

class PatMsr {
public:
  PatMsr() = delete;

  static constexpr auto EntryWidth = Register{8};

  [[nodiscard]] static constexpr auto Entry(Register Value,
                                            Foundation::Uint8 Index)
      -> Foundation::Uint8 {
    return Foundation::Cast::Auto<Foundation::Uint8>(Foundation::Bit::Extract(
        Value, Foundation::Cast::Auto<Register>(Index) * EntryWidth,
        EntryWidth));
  }
};

class Msr {
public:
  Msr() = delete;

  [[nodiscard]] static auto Read(MsrIndex Index) -> Register;

  static auto Write(MsrIndex Index, Register Value) -> Foundation::Void;

  [[nodiscard]] static auto ReadApicBase() -> Register;

  static auto WriteApicBase(Register Value) -> Foundation::Void;

  [[nodiscard]] static auto ReadTscDeadline() -> Register;

  static auto WriteTscDeadline(Register Value) -> Foundation::Void;

  [[nodiscard]] static auto ReadEfer() -> Register;

  static auto WriteEfer(Register Value) -> Foundation::Void;

  [[nodiscard]] static auto ReadFsBase() -> Register;

  static auto WriteFsBase(Register Value) -> Foundation::Void;

  [[nodiscard]] static auto ReadGsBase() -> Register;

  static auto WriteGsBase(Register Value) -> Foundation::Void;

  [[nodiscard]] static auto ReadKernelGsBase() -> Register;

  static auto WriteKernelGsBase(Register Value) -> Foundation::Void;
};
} // namespace UEFIpp::Architecture::X64
