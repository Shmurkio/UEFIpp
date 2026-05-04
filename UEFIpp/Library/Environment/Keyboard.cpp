#include "Keyboard.hpp"
#include "../../UEFIpp.hpp"
#include "../Vector/Vector.hpp"

struct KEYBOARD_NOTIFY_ENTRY
{
    bool Used = false;
    Keyboard::KEY_EVENT Event{};
    Keyboard::NotifyFn Callback = nullptr;
};

static Vector<KEYBOARD_NOTIFY_ENTRY> gKeyboardNotifyEntries{};

static auto KeyEventsMatch(const Keyboard::KEY_EVENT& A, const Keyboard::KEY_EVENT& B) -> bool
{
    return A.ScanCode == B.ScanCode && A.Character == B.Character;
}

static auto AllocateNotifySlot(uint64_t& Slot) -> bool
{
    for (uint64_t i = 0; i < gKeyboardNotifyEntries.Size(); ++i)
    {
        if (!gKeyboardNotifyEntries[i].Used)
        {
            Slot = i;
            return true;
        }
    }

    KEYBOARD_NOTIFY_ENTRY Entry{};

    if (!gKeyboardNotifyEntries.PushBack(Entry))
    {
        return false;
    }

    Slot = gKeyboardNotifyEntries.Size() - 1;
    return true;
}

static auto ToNativeKeyData(const Keyboard::KEY_EVENT& Event) -> EFI_KEY_DATA
{
    EFI_KEY_DATA Data{};

    Data.Key.ScanCode = Event.ScanCode;
    Data.Key.UnicodeChar = Event.Character;
    Data.KeyState.KeyShiftState = Event.ShiftState;
    Data.KeyState.KeyToggleState = Event.ToggleState;

    return Data;
}

static auto FromNativeKeyData(const EFI_KEY_DATA& Data) -> Keyboard::KEY_EVENT
{
    Keyboard::KEY_EVENT Event{};

    Event.ScanCode = Data.Key.ScanCode;
    Event.Character = Data.Key.UnicodeChar;
    Event.ShiftState = Data.KeyState.KeyShiftState;
    Event.ToggleState = Data.KeyState.KeyToggleState;

    return Event;
}

static auto EFIAPI KeyboardNotifyThunk(PEFI_KEY_DATA KeyData) -> EFI_STATUS
{
    if (!KeyData)
    {
        return EFI_SUCCESS;
    }

    const auto Event = FromNativeKeyData(*KeyData);

    for (uint64_t i = 0; i < gKeyboardNotifyEntries.Size(); ++i)
    {
        auto& Entry = gKeyboardNotifyEntries[i];

        if (!Entry.Used || !Entry.Callback)
        {
            continue;
        }

        if (!KeyEventsMatch(Event, Entry.Event))
        {
            continue;
        }

        Entry.Callback(Event);
    }

    return EFI_SUCCESS;
}

Keyboard::~Keyboard()
{
    Detach();
}

Keyboard::Keyboard(Keyboard&& Other) noexcept : Protocol_(Other.Protocol_), Valid_(Other.Valid_)
{
    Other.Protocol_ = nullptr;
    Other.Valid_ = false;
}

auto Keyboard::operator=(Keyboard&& Other) noexcept -> Keyboard&
{
    if (this == &Other)
    {
        return *this;
    }

    Detach();

    Protocol_ = Other.Protocol_;
    Valid_ = Other.Valid_;

    Other.Protocol_ = nullptr;
    Other.Valid_ = false;

    return *this;
}

auto Keyboard::Initialize() -> bool
{
    Protocol_ = nullptr;
    Valid_ = false;

    auto Status = gBS->LocateProtocol(&gEfiSimpleTextInputExProtocolGuid, nullptr, &Protocol_);

    Valid_ = Status == EFI_SUCCESS && Protocol_;
    return Valid_;
}

auto Keyboard::Attach(void* Protocol) -> bool
{
    Protocol_ = Protocol;
    Valid_ = Protocol_ != nullptr;

    return Valid_;
}

auto Keyboard::Detach() -> void
{
    Protocol_ = nullptr;
    Valid_ = false;
}

auto Keyboard::Valid() const -> bool
{
    return Valid_;
}

auto Keyboard::Reset(bool ExtendedVerification) -> bool
{
    if (!Valid_)
    {
        return false;
    }

    auto Input = Cast::To<PEFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL>(Protocol_);

    return Input->Reset(Input, ExtendedVerification) == EFI_SUCCESS;
}

auto Keyboard::ReadKey(KEY_EVENT& Event) -> bool
{
    if (!Valid_)
    {
        return false;
    }

    auto Input = Cast::To<PEFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL>(Protocol_);

    EFI_KEY_DATA Data{};

    auto Status = Input->ReadKeyStrokeEx(Input, &Data);

    if (Status != EFI_SUCCESS)
    {
        return false;
    }

    Event = FromNativeKeyData(Data);
    return true;
}

auto Keyboard::WaitForKey() -> bool
{
    if (!Valid_)
    {
        return false;
    }

    auto Input = Cast::To<PEFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL>(Protocol_);

    return gBS->WaitForEvent(1, &Input->WaitForKeyEx, nullptr) == EFI_SUCCESS;
}

auto Keyboard::ReadKeyBlocking(KEY_EVENT& Event) -> bool
{
    if (!WaitForKey())
    {
        return false;
    }

    return ReadKey(Event);
}

auto Keyboard::RegisterKey(const KEY_EVENT& Event, NotifyFn Callback, NOTIFY_HANDLE& Handle) -> bool
{
    if (!Valid_ || !Callback)
    {
        return false;
    }

    Handle.NativeHandle = nullptr;
    Handle.Slot = 0;
    Handle.Valid = false;

    uint64_t Slot = 0;

    if (!AllocateNotifySlot(Slot))
    {
        return false;
    }

    auto Input = Cast::To<PEFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL>(Protocol_);
    auto Data = ToNativeKeyData(Event);

    void* NativeHandle = nullptr;

    auto Status = Input->RegisterKeyNotify(Input, &Data, KeyboardNotifyThunk, &NativeHandle);

    if (Status != EFI_SUCCESS || !NativeHandle)
    {
        return false;
    }

    gKeyboardNotifyEntries[Slot].Used = true;
    gKeyboardNotifyEntries[Slot].Event = Event;
    gKeyboardNotifyEntries[Slot].Callback = Callback;

    Handle.NativeHandle = NativeHandle;
    Handle.Slot = Slot;
    Handle.Valid = true;

    return true;
}

auto Keyboard::RegisterScanCode(uint16_t ScanCode, NotifyFn Callback, NOTIFY_HANDLE& Handle) -> bool
{
    KEY_EVENT Event{};

    Event.ScanCode = ScanCode;
    Event.Character = 0;

    return RegisterKey(Event, Callback, Handle);
}

auto Keyboard::RegisterKey(KEY Key, NotifyFn Callback, NOTIFY_HANDLE& Handle) -> bool
{
    return RegisterScanCode(Cast::To<uint16_t>(Key), Callback, Handle);
}

auto Keyboard::RegisterChar(char16_t Character, NotifyFn Callback, NOTIFY_HANDLE& Handle) -> bool
{
    KEY_EVENT Event{};

    Event.ScanCode = 0;
    Event.Character = Character;

    return RegisterKey(Event, Callback, Handle);
}

auto Keyboard::Unregister(NOTIFY_HANDLE& Handle) -> bool
{
    if (!Valid_ || !Handle.Valid || !Handle.NativeHandle)
    {
        return false;
    }

    auto Input = Cast::To<PEFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL>(Protocol_);

    auto Status = Input->UnregisterKeyNotify(Input, Handle.NativeHandle);

    if (Status != EFI_SUCCESS)
    {
        return false;
    }

    if (Handle.Slot < gKeyboardNotifyEntries.Size())
    {
        gKeyboardNotifyEntries[Handle.Slot].Used = false;
        gKeyboardNotifyEntries[Handle.Slot].Event = {};
        gKeyboardNotifyEntries[Handle.Slot].Callback = nullptr;
    }

    Handle.NativeHandle = nullptr;
    Handle.Slot = 0;
    Handle.Valid = false;

    return true;
}

auto Keyboard::NativeProtocol() const -> void*
{
    return Protocol_;
}

auto Keyboard::IsScanCode(const KEY_EVENT& Event, uint16_t ScanCode) -> bool
{
    return Event.ScanCode == ScanCode && Event.Character == 0;
}

auto Keyboard::IsKey(
    const KEY_EVENT& Event,
    KEY Key
) -> bool
{
    return IsScanCode(Event, Cast::To<uint16_t>(Key));
}

auto Keyboard::IsChar(const KEY_EVENT& Event, char16_t Character) -> bool
{
    return Event.Character == Character;
}