#include <UEFIpp/Protocols/LoadedImage.hpp>

namespace UEFIpp::Protocols {
auto LoadedImage::HasLoadOptions() const -> Foundation::Bool {
  return LoadOptions && LoadOptionsSize;
}

auto LoadedImage::LoadOptionsText() const -> const Foundation::Char16 * {
  return Foundation::Cast::Auto<const Foundation::Char16 *>(LoadOptions);
}
} // namespace UEFIpp::Protocols
