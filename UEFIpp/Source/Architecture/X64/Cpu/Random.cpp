#include <UEFIpp/Architecture/X64/Cpu/Random.hpp>

namespace UEFIpp::Architecture::X64 {
auto CpuRandom::RdRand16(Foundation::Uint16 &Value) -> Foundation::Bool {
  return UEFIpp_X64_RdRand16(&Value) != 0;
}

auto CpuRandom::RdRand32(Foundation::Uint32 &Value) -> Foundation::Bool {
  return UEFIpp_X64_RdRand32(&Value) != 0;
}

auto CpuRandom::RdRand64(Foundation::Uint64 &Value) -> Foundation::Bool {
  return UEFIpp_X64_RdRand64(&Value) != 0;
}

auto CpuRandom::RdSeed16(Foundation::Uint16 &Value) -> Foundation::Bool {
  return UEFIpp_X64_RdSeed16(&Value) != 0;
}

auto CpuRandom::RdSeed32(Foundation::Uint32 &Value) -> Foundation::Bool {
  return UEFIpp_X64_RdSeed32(&Value) != 0;
}

auto CpuRandom::RdSeed64(Foundation::Uint64 &Value) -> Foundation::Bool {
  return UEFIpp_X64_RdSeed64(&Value) != 0;
}
} // namespace UEFIpp::Architecture::X64
