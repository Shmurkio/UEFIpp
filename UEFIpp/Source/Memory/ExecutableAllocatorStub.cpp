#include <UEFIpp/Memory/ExecutableAllocatorStub.hpp>

namespace UEFIpp::Memory {
auto ExecutableAllocatorStub::Allocate(Foundation::Size NumberOfBytes,
                                       Foundation::Size Alignment) const
    -> Foundation::Void * {
  if (!IsValid() || !NumberOfBytes || !Alignment ||
      !Foundation::Bit::IsPowerOfTwo(Alignment)) {
    return nullptr;
  }

  return Allocate_(Context_, NumberOfBytes, Alignment);
}

auto ExecutableAllocatorStub::Free(Foundation::Void *Address) const
    -> Foundation::Bool {
  if (!Address) {
    return true;
  }

  return IsValid() && Free_(Context_, Address);
}
} // namespace UEFIpp::Memory
