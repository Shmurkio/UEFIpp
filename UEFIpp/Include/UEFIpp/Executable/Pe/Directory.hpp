#pragma once

#include <UEFIpp/Executable/Pe/Types.hpp>

namespace UEFIpp::Executable::Pe {
class Directory {
public:
  constexpr Directory() = default;

  constexpr Directory(DirectoryIndex Index, Foundation::Uint32 Rva,
                      Foundation::Uint32 Size)
      : Index_(Index), Rva_(Rva), Size_(Size) {}

  [[nodiscard]] constexpr auto Index() const noexcept -> DirectoryIndex {
    return Index_;
  }

  [[nodiscard]] constexpr auto Rva() const noexcept -> Foundation::Uint32 {
    return Rva_;
  }

  [[nodiscard]] constexpr auto Size() const noexcept -> Foundation::Uint32 {
    return Size_;
  }

  [[nodiscard]] constexpr auto Empty() const noexcept -> Foundation::Bool {
    return Rva_ == 0 || Size_ == 0;
  }

private:
  DirectoryIndex Index_{};
  Foundation::Uint32 Rva_{};
  Foundation::Uint32 Size_{};
};
} // namespace UEFIpp::Executable::Pe
