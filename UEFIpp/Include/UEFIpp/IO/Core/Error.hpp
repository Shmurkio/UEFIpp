#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Expected.hpp>
#include <UEFIpp/UEFI/Status.hpp>

namespace UEFIpp::IO {
enum class Operation : Foundation::Uint8 {
  None,
  Open,
  Read,
  Write,
  Flush,
  Seek,
  Truncate,
  Close,
  Wait,
  Encode,
  Decode,
  Format,
  Parse,
  Allocate
};

enum class ErrorCode : Foundation::Uint8 {
  Firmware,
  InvalidArgument,
  Closed,
  EndOfFile,
  UnexpectedEndOfFile,
  WouldBlock,
  NoProgress,
  Overflow,
  InvalidFormat,
  InvalidEncoding,
  OutOfMemory,
  Unsupported,
  Cancelled,
  Timeout
};

struct Error final {
  ErrorCode Code{ErrorCode::Firmware};
  Operation During{Operation::None};
  UEFI::Status Status{};
  Foundation::Uint64 Offset{};
  Foundation::Size Transferred{};

  [[nodiscard]] static constexpr auto
  FromStatus(Operation During, UEFI::StatusValue Status,
             Foundation::Uint64 Offset = 0,
             Foundation::Size Transferred = 0) noexcept -> Error {
    return {ErrorCode::Firmware, During, UEFI::Status{Status}, Offset,
            Transferred};
  }

  [[nodiscard]] static constexpr auto
  FromStatus(Operation During, UEFI::StatusCode Status,
             Foundation::Uint64 Offset = 0,
             Foundation::Size Transferred = 0) noexcept -> Error {
    return {ErrorCode::Firmware, During, UEFI::Status{Status}, Offset,
            Transferred};
  }

  [[nodiscard]] static constexpr auto
  Semantic(ErrorCode Code, Operation During,
           Foundation::Uint64 Offset = 0,
           Foundation::Size Transferred = 0) noexcept -> Error {
    return {Code, During, UEFI::Status{}, Offset, Transferred};
  }

  [[nodiscard]] constexpr auto operator<=>(const Error &) const = default;
};

template <typename T = Foundation::Void>
using Result = Library::Expected<T, Error>;

template <typename T>
[[nodiscard]] constexpr auto Failure(T &&Value) {
  return Library::MakeUnexpected(Foundation::Utility::Forward<T>(Value));
}

struct ReadCount final {
  Foundation::Size Count{};
  Foundation::Bool End{};

  [[nodiscard]] constexpr auto operator<=>(const ReadCount &) const = default;
};
} // namespace UEFIpp::IO
