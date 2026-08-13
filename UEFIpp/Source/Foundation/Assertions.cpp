#include <UEFIpp/Foundation/Assertions.hpp>
#include <UEFIpp/IO/Logging/Logger.hpp>

#include <intrin.h>

namespace UEFIpp::Foundation {
[[noreturn]] auto
Assertions::Panic([[maybe_unused]] const Char *Message,
                  [[maybe_unused]] SourceLocation Location) noexcept -> Void {
  IO::PanicWriter::Write(u8"PANIC: ");
  IO::PanicWriter::Write(IO::AsUtf8(Library::StringView{
      Message ? Message : "unspecified failure"}));
  if (Location.File) {
    IO::PanicWriter::Write(u8"\n  at ");
    IO::PanicWriter::Write(IO::AsUtf8(Library::StringView{Location.File}));
    IO::PanicWriter::Write(u8":");
    Char8 Digits[10]{};
    Size Count{};
    auto Line = Location.Line;
    do {
      Digits[Count++] = Cast::Auto<Char8>(u8'0' + Line % 10);
      Line /= 10;
    } while (Line);
    for (Size Index{}; Index < Count / 2; ++Index) {
      const auto Temporary = Digits[Index];
      Digits[Index] = Digits[Count - Index - 1];
      Digits[Count - Index - 1] = Temporary;
    }
    IO::PanicWriter::Write({Digits, Count});
  }
  IO::PanicWriter::Write(u8"\n");
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
