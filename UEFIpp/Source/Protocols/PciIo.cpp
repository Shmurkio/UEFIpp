#include <UEFIpp/Protocols/PciIo.hpp>

namespace UEFIpp::Protocols {
auto PciIo::ReadConfig8(Foundation::Uint32 Offset, Foundation::Uint8 &Value)
    -> UEFI::StatusCode {
  return Pci.Read(this, PciIoWidth::Uint8, Offset, 1, &Value);
}

auto PciIo::ReadConfig16(Foundation::Uint32 Offset, Foundation::Uint16 &Value)
    -> UEFI::StatusCode {
  return Pci.Read(this, PciIoWidth::Uint16, Offset, 1, &Value);
}

auto PciIo::ReadConfig32(Foundation::Uint32 Offset, Foundation::Uint32 &Value)
    -> UEFI::StatusCode {
  return Pci.Read(this, PciIoWidth::Uint32, Offset, 1, &Value);
}

auto PciIo::WriteConfig16(Foundation::Uint32 Offset, Foundation::Uint16 Value)
    -> UEFI::StatusCode {
  return Pci.Write(this, PciIoWidth::Uint16, Offset, 1, &Value);
}

auto PciIo::ReadBar32(Foundation::Uint8 BarIndex, Foundation::Uint64 Offset,
                      Foundation::Uint32 &Value) -> UEFI::StatusCode {
  return Mem.Read(this, PciIoWidth::Uint32, BarIndex, Offset, 1, &Value);
}

auto PciIo::WriteBar32(Foundation::Uint8 BarIndex, Foundation::Uint64 Offset,
                       Foundation::Uint32 Value) -> UEFI::StatusCode {
  return Mem.Write(this, PciIoWidth::Uint32, BarIndex, Offset, 1, &Value);
}
} // namespace UEFIpp::Protocols
