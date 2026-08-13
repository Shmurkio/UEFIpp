#pragma once

#include <UEFIpp/IO/Terminal/Terminal.hpp>
#include <UEFIpp/Library/Functional/Function.hpp>

namespace UEFIpp::IO {
struct LineEditorOptions final {
  Foundation::Size MaximumBytes{4096};
  Foundation::Bool Echo{true};
  Foundation::Bool MaskInput{};
  Foundation::Bool AllowMultiline{};
  Foundation::Bool AddToHistory{true};
  WaitOptions Wait{};
};

class LineEditor final {
public:
  using CompletionFunction =
      Library::Function<Result<Library::U8String>(Library::U8StringView)>;

  Library::Event<Library::U8StringView> OnLine{};

  constexpr explicit LineEditor(Memory::AllocatorStub Allocator = {}) noexcept
      : OnLine(Allocator), History_(Allocator), Allocator_(Allocator) {}

  auto SetCompletion(CompletionFunction Completion) -> Foundation::Void {
    Completion_ = Foundation::Utility::Move(Completion);
  }

  auto ClearHistory() -> Foundation::Void { History_.Clear(); }
  [[nodiscard]] constexpr auto History() const noexcept
      -> const Library::Vector<Library::U8String> & {
    return History_;
  }

  [[nodiscard]] auto ReadLine(Terminal &Terminal,
                              LineEditorOptions Options = {})
      -> Result<Library::U8String>;

private:
  [[nodiscard]] static auto PreviousBoundary(Library::U8StringView Text,
                                              Foundation::Size Position)
      -> Foundation::Size;
  [[nodiscard]] static auto NextBoundary(Library::U8StringView Text,
                                          Foundation::Size Position)
      -> Foundation::Size;
  [[nodiscard]] auto Redraw(Terminal &Terminal, Library::U8StringView Line,
                            Foundation::Size Cursor,
                            Foundation::Size &PreviousCells,
                            const ConsoleGeometry &Origin,
                            const LineEditorOptions &Options) -> Result<>;

  Library::Vector<Library::U8String> History_{};
  CompletionFunction Completion_{};
  Memory::AllocatorStub Allocator_{};
};
} // namespace UEFIpp::IO
