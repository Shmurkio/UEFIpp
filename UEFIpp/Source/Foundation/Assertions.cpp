#include <UEFIpp/Foundation/Assertions.hpp>

#include <intrin.h>

namespace UEFIpp::Foundation {
[[noreturn]] auto
Assertions::Panic([[maybe_unused]] const Char *Message,
                  [[maybe_unused]] SourceLocation Location) noexcept -> Void {
  __debugbreak();

  for (;;) {
  }
}

auto Assertions::Assert([[maybe_unused]] Bool Condition,
                        [[maybe_unused]] const Char *Expression,
                        [[maybe_unused]] SourceLocation Location) noexcept
    -> Void {
#if defined(UEFIPP_DEBUG)
  if (!Condition) {
    Panic(Expression, Location);
  }
#endif
}

auto Assertions::Verify(Bool Condition, const Char *Expression,
                        SourceLocation Location) noexcept -> Void {
  if (!Condition) {
    Panic(Expression, Location);
  }
}

[[noreturn]] auto Assertions::Unreachable(SourceLocation Location) noexcept
    -> Void {
  Panic("Unreachable code reached", Location);
}
} // namespace UEFIpp::Foundation
