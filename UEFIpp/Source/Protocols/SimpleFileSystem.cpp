#include <UEFIpp/Protocols/SimpleFileSystem.hpp>

namespace UEFIpp::Protocols {
auto SimpleFileSystem::OpenRoot(File *&Root) -> UEFI::StatusCode {
  return OpenVolume(this, &Root);
}
} // namespace UEFIpp::Protocols
