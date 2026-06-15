#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Types.hpp>
#include <UEFIpp/Protocols/InputKey.hpp>

namespace UEFIpp::Protocols
{
	class SimpleTextInputEx;

	using KeyShiftState = Foundation::Uint32;
	using KeyToggleState = Foundation::Uint8;

	class KeyShift
	{
	public:
		inline static constexpr KeyShiftState RightShiftPressed = Foundation::Bit::Mask<KeyShiftState>(0);
		inline static constexpr KeyShiftState LeftShiftPressed = Foundation::Bit::Mask<KeyShiftState>(1);
		inline static constexpr KeyShiftState RightControlPressed = Foundation::Bit::Mask<KeyShiftState>(2);
		inline static constexpr KeyShiftState LeftControlPressed = Foundation::Bit::Mask<KeyShiftState>(3);
		inline static constexpr KeyShiftState RightAltPressed = Foundation::Bit::Mask<KeyShiftState>(4);
		inline static constexpr KeyShiftState LeftAltPressed = Foundation::Bit::Mask<KeyShiftState>(5);
		inline static constexpr KeyShiftState RightLogoPressed = Foundation::Bit::Mask<KeyShiftState>(6);
		inline static constexpr KeyShiftState LeftLogoPressed = Foundation::Bit::Mask<KeyShiftState>(7);
		inline static constexpr KeyShiftState MenuKeyPressed = Foundation::Bit::Mask<KeyShiftState>(8);
		inline static constexpr KeyShiftState SysReqPressed = Foundation::Bit::Mask<KeyShiftState>(9);

		inline static constexpr KeyShiftState Valid = Foundation::Bit::Mask<KeyShiftState>(31);
	};

	class KeyToggle
	{
	public:
		inline static constexpr KeyToggleState ScrollLockActive = Foundation::Bit::Mask<KeyToggleState>(0);
		inline static constexpr KeyToggleState NumLockActive = Foundation::Bit::Mask<KeyToggleState>(1);
		inline static constexpr KeyToggleState CapsLockActive = Foundation::Bit::Mask<KeyToggleState>(2);

		inline static constexpr KeyToggleState KeyStateExposed = Foundation::Bit::Mask<KeyToggleState>(6);
		inline static constexpr KeyToggleState Valid = Foundation::Bit::Mask<KeyToggleState>(7);
	};

	class KeyState
	{
	public:
		KeyShiftState KeyShiftState;
		KeyToggleState KeyToggleState;

		[[nodiscard]] constexpr auto ShiftStateValid() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(KeyShiftState, KeyShift::Valid);
		}

		[[nodiscard]] constexpr auto ToggleStateValid() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(KeyToggleState, KeyToggle::Valid);
		}

		[[nodiscard]] constexpr auto ShiftPressed() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(KeyShiftState, KeyShift::LeftShiftPressed | KeyShift::RightShiftPressed);
		}

		[[nodiscard]] constexpr auto ControlPressed() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(KeyShiftState, KeyShift::LeftControlPressed | KeyShift::RightControlPressed);
		}

		[[nodiscard]] constexpr auto AltPressed() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(KeyShiftState, KeyShift::LeftAltPressed | KeyShift::RightAltPressed);
		}

		[[nodiscard]] constexpr auto CapsLockActive() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(KeyToggleState, KeyToggle::CapsLockActive);
		}

		[[nodiscard]] constexpr auto NumLockActive() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(KeyToggleState, KeyToggle::NumLockActive);
		}

		[[nodiscard]] constexpr auto ScrollLockActive() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(KeyToggleState, KeyToggle::ScrollLockActive);
		}

		[[nodiscard]] constexpr auto LeftShiftPressed() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(KeyShiftState, KeyShift::LeftShiftPressed);
		}

		[[nodiscard]] constexpr auto RightShiftPressed() const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(KeyShiftState, KeyShift::RightShiftPressed);
		}

		[[nodiscard]] constexpr auto operator<=>(const KeyState&) const = default;
	};

	class KeyData
	{
	public:
		InputKey Key;
		KeyState KeyState;

		[[nodiscard]] constexpr auto HasUnicode() const -> Foundation::Bool
		{
			return Key.HasUnicode();
		}

		[[nodiscard]] constexpr auto HasScanCode() const -> Foundation::Bool
		{
			return Key.HasScanCode();
		}

		[[nodiscard]] constexpr auto Empty() const -> Foundation::Bool
		{
			return Key.Empty();
		}

		[[nodiscard]] constexpr auto operator<=>(const KeyData&) const = default;
	};

	using InputResetExFn = UEFI::StatusValue(*)(SimpleTextInputEx* This, Foundation::Bool ExtendedVerification);
	using ReadKeyStrokeExFn = UEFI::StatusValue(*)(SimpleTextInputEx* This, KeyData* KeyData);
	using SetStateFn = UEFI::StatusValue(*)(SimpleTextInputEx* This, KeyToggleState* KeyToggleState);
	using KeyNotifyFn = UEFI::StatusValue(*)(KeyData* KeyData);
	using RegisterKeyNotifyFn = UEFI::StatusValue(*)(SimpleTextInputEx* This, KeyData* KeyData, KeyNotifyFn KeyNotificationFunction, Foundation::Void** NotifyHandle);
	using UnregisterKeyNotifyFn = UEFI::StatusValue(*)(SimpleTextInputEx* This, Foundation::Void* NotificationHandle);

	class SimpleTextInputEx
	{
	public:
		InputResetExFn Reset;
		ReadKeyStrokeExFn ReadKeyStrokeEx;
		UEFI::Event WaitForKeyEx;
		SetStateFn SetState;
		RegisterKeyNotifyFn RegisterKeyNotify;
		UnregisterKeyNotifyFn UnregisterKeyNotify;

		[[nodiscard]] auto ResetDevice(Foundation::Bool ExtendedVerification = false) -> UEFI::StatusValue
		{
			return Reset(this, ExtendedVerification);
		}

		[[nodiscard]] auto ReadKey(KeyData& Data) -> UEFI::StatusValue
		{
			return ReadKeyStrokeEx(this, &Data);
		}

		[[nodiscard]] auto SetToggleState(KeyToggleState& State) -> UEFI::StatusValue
		{
			return SetState(this, &State);
		}

		[[nodiscard]] auto RegisterNotify(KeyData& Data, KeyNotifyFn Function, Foundation::Void*& NotifyHandle) -> UEFI::StatusValue
		{
			return RegisterKeyNotify(this, &Data, Function, &NotifyHandle);
		}

		[[nodiscard]] auto UnregisterNotify(Foundation::Void* NotifyHandle) -> UEFI::StatusValue
		{
			return UnregisterKeyNotify(this, NotifyHandle);
		}

		[[nodiscard]] constexpr auto operator<=>(const SimpleTextInputEx&) const = default;
	};

	static_assert(sizeof(KeyState) == 8);
	static_assert(sizeof(KeyData) == 12);
	static_assert(sizeof(SimpleTextInputEx) == 48);

	static_assert(Foundation::Traits::IsStandardLayout<KeyState>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<KeyState>::Value);
	static_assert(Foundation::Traits::IsStandardLayout<KeyData>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<KeyData>::Value);
	static_assert(Foundation::Traits::IsStandardLayout<SimpleTextInputEx>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<SimpleTextInputEx>::Value);
}
