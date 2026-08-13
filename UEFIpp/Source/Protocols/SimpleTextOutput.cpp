#include <UEFIpp/Protocols/SimpleTextOutput.hpp>

namespace UEFIpp::Protocols {
auto SimpleTextOutput::ResetDevice(Foundation::Bool ExtendedVerification)
    -> UEFI::StatusValue {
  return Reset(this, ExtendedVerification);
}

auto SimpleTextOutput::Write(const Foundation::WChar *String)
    -> UEFI::StatusValue {
  return OutputString(this, String);
}

auto SimpleTextOutput::Test(const Foundation::WChar *String)
    -> UEFI::StatusValue {
  return TestString(this, String);
}

auto SimpleTextOutput::Query(Foundation::UintN ModeNumber,
                             Foundation::UintN &Columns,
                             Foundation::UintN &Rows) -> UEFI::StatusValue {
  return QueryMode(this, ModeNumber, &Columns, &Rows);
}

auto SimpleTextOutput::SetCurrentMode(Foundation::UintN ModeNumber)
    -> UEFI::StatusValue {
  return SetMode(this, ModeNumber);
}

auto SimpleTextOutput::SetTextAttribute(Foundation::UintN Attribute)
    -> UEFI::StatusValue {
  return SetAttribute(this, Attribute);
}

auto SimpleTextOutput::Clear() -> UEFI::StatusValue {
  return ClearScreen(this);
}

auto SimpleTextOutput::SetCursor(Foundation::UintN Column,
                                 Foundation::UintN Row) -> UEFI::StatusValue {
  return SetCursorPosition(this, Column, Row);
}

auto SimpleTextOutput::SetCursorVisible(Foundation::Bool Visible)
    -> UEFI::StatusValue {
  return EnableCursor(this, Visible);
}

auto SimpleTextOutput::CurrentMode() const -> SimpleTextOutputMode * {
  return Mode;
}
} // namespace UEFIpp::Protocols
