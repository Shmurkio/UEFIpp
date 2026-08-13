#include <UEFIpp/Protocols/Access.hpp>

namespace UEFIpp::Protocols {
auto Access::Reset(BootServicesType *BootServices) -> void {
  BootServices_ = BootServices;
}

auto Access::Raw() const -> BootServicesType * { return BootServices_; }
} // namespace UEFIpp::Protocols
