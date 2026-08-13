#include <UEFIpp/Stream/Output/ConsoleSink.hpp>

namespace UEFIpp::Stream {
namespace Out {
Output<ConsoleOutputSink> Console{};
}

auto ConsoleOutputSink::Put(Foundation::WChar Character) -> void {
  if (Character == L'\b') {
    Backspace();
    return;
  }

  Foundation::WChar Buffer[] = {Character, 0};
  Output_->OutputString(Output_, Buffer);
}

auto ConsoleOutputSink::Backspace() -> void {
  if (!Output_ || !Output_->Mode) {
    return;
  }

  const auto Column = Output_->Mode->CursorColumn;
  const auto Row = Output_->Mode->CursorRow;

  if (!Column) {
    return;
  }

  Output_->SetCursorPosition(Output_, Column - 1, Row);

  Foundation::WChar Space[] = {L' ', 0};
  Output_->OutputString(Output_, Space);

  Output_->SetCursorPosition(Output_, Column - 1, Row);
}

auto ConsoleOutputSink::Write(const Foundation::Char *Data,
                              Foundation::Size Length) -> void {
  if (!Output_ || !Data) {
    return;
  }

  for (Foundation::Size Index = 0; Index < Length; ++Index) {
    Put(Foundation::Cast::Auto<Foundation::WChar>(Data[Index]));
  }
}

auto ConsoleOutputSink::Write(const Foundation::WChar *Data,
                              Foundation::Size Length) -> void {
  if (!Output_ || !Data) {
    return;
  }

  for (Foundation::Size Index = 0; Index < Length; ++Index) {
    Put(Data[Index]);
  }
}

auto ConsoleOutputSink::Clear() -> void {
  if (!Output_) {
    return;
  }

  Output_->ClearScreen(Output_);
}
} // namespace UEFIpp::Stream
