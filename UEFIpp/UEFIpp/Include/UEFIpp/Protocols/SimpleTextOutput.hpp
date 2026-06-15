#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Types.hpp>

namespace UEFIpp::Protocols
{
	class SimpleTextOutputMode
	{
	public:
		Foundation::Int32 MaxMode;
		Foundation::Int32 Mode;
		Foundation::Int32 Attribute;
		Foundation::Int32 CursorColumn;
		Foundation::Int32 CursorRow;
		Foundation::Bool CursorVisible;

		[[nodiscard]] constexpr auto operator<=>(const SimpleTextOutputMode&) const = default;
	};

	using SimpleTextOutputResetFn = UEFI::StatusValue(*)(class SimpleTextOutput* This, Foundation::Bool ExtendedVerification);
	using SimpleTextOutputStringFn = UEFI::StatusValue(*)(class SimpleTextOutput* This, const Foundation::WChar* String);
	using SimpleTextTestStringFn = UEFI::StatusValue(*)(class SimpleTextOutput* This, const Foundation::WChar* String);
	using SimpleTextQueryModeFn = UEFI::StatusValue(*)(class SimpleTextOutput* This, Foundation::UintN ModeNumber, Foundation::UintN* Columns, Foundation::UintN* Rows);
	using SimpleTextSetModeFn = UEFI::StatusValue(*)(class SimpleTextOutput* This, Foundation::UintN ModeNumber);
	using SimpleTextSetAttributeFn = UEFI::StatusValue(*)(class SimpleTextOutput* This, Foundation::UintN Attribute);
	using SimpleTextClearScreenFn = UEFI::StatusValue(*)(class SimpleTextOutput* This);
	using SimpleTextSetCursorPositionFn = UEFI::StatusValue(*)(class SimpleTextOutput* This, Foundation::UintN Column, Foundation::UintN Row);
	using SimpleTextEnableCursorFn = UEFI::StatusValue(*)(class SimpleTextOutput* This, Foundation::Bool Visible);

	class SimpleTextOutput
	{
	public:
		SimpleTextOutputResetFn Reset;
		SimpleTextOutputStringFn OutputString;
		SimpleTextTestStringFn TestString;
		SimpleTextQueryModeFn QueryMode;
		SimpleTextSetModeFn SetMode;
		SimpleTextSetAttributeFn SetAttribute;
		SimpleTextClearScreenFn ClearScreen;
		SimpleTextSetCursorPositionFn SetCursorPosition;
		SimpleTextEnableCursorFn EnableCursor;

		SimpleTextOutputMode* Mode;

		[[nodiscard]] auto ResetDevice(Foundation::Bool ExtendedVerification = false) -> UEFI::StatusValue
		{
			return Reset(this, ExtendedVerification);
		}

		[[nodiscard]] auto Write(const Foundation::WChar* String) -> UEFI::StatusValue
		{
			return OutputString(this, String);
		}

		[[nodiscard]] auto Test(const Foundation::WChar* String) -> UEFI::StatusValue
		{
			return TestString(this, String);
		}

		[[nodiscard]] auto Query(Foundation::UintN ModeNumber, Foundation::UintN& Columns, Foundation::UintN& Rows) -> UEFI::StatusValue
		{
			return QueryMode(this, ModeNumber, &Columns, &Rows);
		}

		[[nodiscard]] auto SetCurrentMode(Foundation::UintN ModeNumber) -> UEFI::StatusValue
		{
			return SetMode(this, ModeNumber);
		}

		[[nodiscard]] auto SetTextAttribute(Foundation::UintN Attribute) -> UEFI::StatusValue
		{
			return SetAttribute(this, Attribute);
		}

		[[nodiscard]] auto Clear() -> UEFI::StatusValue
		{
			return ClearScreen(this);
		}

		[[nodiscard]] auto SetCursor(Foundation::UintN Column, Foundation::UintN Row) -> UEFI::StatusValue
		{
			return SetCursorPosition(this, Column, Row);
		}

		[[nodiscard]] auto SetCursorVisible(Foundation::Bool Visible) -> UEFI::StatusValue
		{
			return EnableCursor(this, Visible);
		}

		[[nodiscard]] auto CurrentMode() const -> SimpleTextOutputMode*
		{
			return Mode;
		}

		[[nodiscard]] constexpr auto operator<=>(const SimpleTextOutput&) const = default;
	};

	static_assert(sizeof(SimpleTextOutputMode) == 24);
	static_assert(sizeof(SimpleTextOutput) == 80);

	static_assert(Foundation::Traits::IsStandardLayout<SimpleTextOutputMode>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<SimpleTextOutputMode>::Value);
	static_assert(Foundation::Traits::IsStandardLayout<SimpleTextOutput>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<SimpleTextOutput>::Value);
}