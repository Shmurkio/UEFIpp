#pragma once

#include <UEFIpp/FileSystem/File.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Stream {
class FileOutputSink {
public:
  constexpr FileOutputSink() = default;

  constexpr explicit FileOutputSink(FileSystem::File *File,
                                    Foundation::Bool AutoFlush = true)
      : File_(File), AutoFlush_(AutoFlush) {}

  [[nodiscard]] constexpr auto Valid() const -> Foundation::Bool {
    return File_ && File_->Valid();
  }

  [[nodiscard]] constexpr explicit operator Foundation::Bool() const {
    return Valid();
  }

  auto Reset(FileSystem::File *File, Foundation::Bool AutoFlush = true)
      -> Foundation::Void;

  auto Write(const Foundation::Char *Data, Foundation::Size Length)
      -> Foundation::Void;

  auto Write(const Foundation::WChar *Data, Foundation::Size Length)
      -> Foundation::Void;

  auto Clear() -> Foundation::Void;

private:
  FileSystem::File *File_{};
  Foundation::Bool AutoFlush_{true};
};
} // namespace UEFIpp::Stream
