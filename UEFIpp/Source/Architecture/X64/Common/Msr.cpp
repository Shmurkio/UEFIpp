#include <UEFIpp/Architecture/X64/Common/Msr.hpp>

namespace UEFIpp::Architecture::X64 {
auto Msr::Read(MsrIndex Index) -> Register {
  return __readmsr(Foundation::Cast::Underlying(Index));
}

auto Msr::Write(MsrIndex Index, Register Value) -> Foundation::Void {
  __writemsr(Foundation::Cast::Underlying(Index), Value);
}

auto Msr::ReadApicBase() -> Register { return Read(MsrIndex::ApicBase); }

auto Msr::WriteApicBase(Register Value) -> Foundation::Void {
  Write(MsrIndex::ApicBase, Value);
}

auto Msr::ReadTscDeadline() -> Register { return Read(MsrIndex::TscDeadline); }

auto Msr::WriteTscDeadline(Register Value) -> Foundation::Void {
  Write(MsrIndex::TscDeadline, Value);
}

auto Msr::ReadEfer() -> Register { return Read(MsrIndex::Efer); }

auto Msr::WriteEfer(Register Value) -> Foundation::Void {
  Write(MsrIndex::Efer, Value);
}

auto Msr::ReadFsBase() -> Register { return Read(MsrIndex::FsBase); }

auto Msr::WriteFsBase(Register Value) -> Foundation::Void {
  Write(MsrIndex::FsBase, Value);
}

auto Msr::ReadGsBase() -> Register { return Read(MsrIndex::GsBase); }

auto Msr::WriteGsBase(Register Value) -> Foundation::Void {
  Write(MsrIndex::GsBase, Value);
}

auto Msr::ReadKernelGsBase() -> Register {
  return Read(MsrIndex::KernelGsBase);
}

auto Msr::WriteKernelGsBase(Register Value) -> Foundation::Void {
  Write(MsrIndex::KernelGsBase, Value);
}
} // namespace UEFIpp::Architecture::X64
