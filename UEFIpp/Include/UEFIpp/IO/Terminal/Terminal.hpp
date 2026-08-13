#pragma once

#include <UEFIpp/IO/Core/WriterRef.hpp>
#include <UEFIpp/IO/Terminal/EventLoop.hpp>
#include <UEFIpp/IO/Transport/Console.hpp>
#include <UEFIpp/Library/Functional/Event.hpp>
#include <UEFIpp/Protocols/SimpleTextInputEx.hpp>
#include <UEFIpp/Protocols/SimpleTextInput.hpp>

namespace UEFIpp::IO {
enum class KeyCode : Foundation::Uint16 {
  None,
  Up,
  Down,
  Right,
  Left,
  Home,
  End,
  Insert,
  Delete,
  PageUp,
  PageDown,
  Function1,
  Function2,
  Function3,
  Function4,
  Function5,
  Function6,
  Function7,
  Function8,
  Function9,
  Function10,
  Escape,
  Unknown = 0xFFFF
};

struct KeyModifiers final {
  Foundation::Uint32 ShiftState{};
  Foundation::Uint8 ToggleState{};

  [[nodiscard]] constexpr auto Shift() const noexcept -> Foundation::Bool {
    return (ShiftState & (Protocols::KeyShift::LeftShiftPressed |
                          Protocols::KeyShift::RightShiftPressed)) != 0;
  }
  [[nodiscard]] constexpr auto Control() const noexcept -> Foundation::Bool {
    return (ShiftState & (Protocols::KeyShift::LeftControlPressed |
                          Protocols::KeyShift::RightControlPressed)) != 0;
  }
  [[nodiscard]] constexpr auto Alt() const noexcept -> Foundation::Bool {
    return (ShiftState & (Protocols::KeyShift::LeftAltPressed |
                          Protocols::KeyShift::RightAltPressed)) != 0;
  }
};

struct KeyEvent final {
  KeyCode Code{KeyCode::None};
  Foundation::Char32 Character{};
  KeyModifiers Modifiers{};
  Protocols::KeyData Native{};

  [[nodiscard]] constexpr auto HasCharacter() const noexcept
      -> Foundation::Bool {
    return Character != 0;
  }
};

class Terminal final {
public:
  Library::Event<const KeyEvent &> OnKey{};

  constexpr Terminal() = default;
  constexpr Terminal(UEFI::Table::BootServices *BootServices,
                     Protocols::SimpleTextInputEx *InputEx,
                     Protocols::SimpleTextInput *Input, WriterRef Output,
                     ConsoleSink *Console = nullptr,
                     Memory::AllocatorStub Allocator = {})
      : OnKey(Allocator), BootServices_(BootServices), InputEx_(InputEx),
        Input_(Input),
        Output_(Output), Console_(Console) {}

  auto Reset(UEFI::Table::BootServices *BootServices,
             Protocols::SimpleTextInputEx *InputEx,
             Protocols::SimpleTextInput *Input, WriterRef Output,
             ConsoleSink *Console = nullptr) -> Foundation::Void;

  [[nodiscard]] constexpr auto Valid() const noexcept -> Foundation::Bool {
    return BootServices_ && (InputEx_ || Input_);
  }
  [[nodiscard]] auto PollKey() -> Result<KeyEvent>;
  [[nodiscard]] auto WaitKey(WaitOptions Options = {}) -> Result<KeyEvent>;
  [[nodiscard]] auto WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size>;
  [[nodiscard]] auto Write(Library::U8StringView Text) -> Result<>;
  [[nodiscard]] auto Flush() -> Result<>;
  [[nodiscard]] auto Clear() -> Result<>;
  [[nodiscard]] auto SetStyle(ConsoleStyle Style) -> Result<>;
  [[nodiscard]] auto ResetStyle() -> Result<>;
  [[nodiscard]] auto SetCursorVisible(Foundation::Bool Visible) -> Result<>;
  [[nodiscard]] auto Geometry() const -> Result<ConsoleGeometry>;
  [[nodiscard]] auto SetCursor(Foundation::UintN Column,
                               Foundation::UintN Row) -> Result<>;

  class KeyAwaitable final {
  public:
    constexpr KeyAwaitable(Terminal &Owner, EventLoop &Loop) noexcept
        : Owner_(&Owner), Loop_(&Loop) {}
    [[nodiscard]] constexpr auto await_ready() const noexcept
        -> Foundation::Bool {
      return Owner_ && Owner_->QueuedEvent_.HasValue();
    }
    auto await_suspend(std::coroutine_handle<> Coroutine) -> Foundation::Bool;
    [[nodiscard]] auto await_resume() -> Result<KeyEvent>;

  private:
    Terminal *Owner_{};
    EventLoop *Loop_{};
    Result<> Registration_{};
  };

  [[nodiscard]] constexpr auto NextEventAsync(EventLoop &Loop) noexcept
      -> KeyAwaitable {
    return KeyAwaitable{*this, Loop};
  }

private:
  [[nodiscard]] auto Convert(const Protocols::KeyData &Native)
      -> Result<KeyEvent>;

  UEFI::Table::BootServices *BootServices_{};
  Protocols::SimpleTextInputEx *InputEx_{};
  Protocols::SimpleTextInput *Input_{};
  WriterRef Output_{};
  ConsoleSink *Console_{};
  Foundation::Char16 PendingHighSurrogate_{};
  Library::Optional<KeyEvent> QueuedEvent_{};
};
} // namespace UEFIpp::IO
