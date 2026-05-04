#pragma once

#include <cstdint>

class Keyboard
{
private:
    void* Protocol_ = nullptr;
    bool Valid_ = false;

public:
    typedef enum class _KEY : uint16_t
    {
        None = 0x0000,

        Up = 0x0001,
        Down = 0x0002,
        Right = 0x0003,
        Left = 0x0004,

        Home = 0x0005,
        End = 0x0006,
        Insert = 0x0007,
        Delete = 0x0008,
        PageUp = 0x0009,
        PageDown = 0x000A,

        F1 = 0x000B,
        F2 = 0x000C,
        F3 = 0x000D,
        F4 = 0x000E,
        F5 = 0x000F,
        F6 = 0x0010,
        F7 = 0x0011,
        F8 = 0x0012,
        F9 = 0x0013,
        F10 = 0x0014,
        F11 = 0x0015,
        F12 = 0x0016,

        Escape = 0x0017
    } KEY, *PKEY;

    typedef struct _KEY_EVENT
    {
        uint16_t ScanCode = 0;
        char16_t Character = 0;
        uint32_t ShiftState = 0;
        uint8_t ToggleState = 0;
    } KEY_EVENT, *PKEY_EVENT;

    typedef struct _NOTIFY_HANDLE
    {
        void* NativeHandle = nullptr;
        uint64_t Slot = 0;
        bool Valid = false;
    } NOTIFY_HANDLE, *PNOTIFY_HANDLE;

    using NotifyFn = bool (*)(const KEY_EVENT& Event);

    Keyboard() = default;
    ~Keyboard();
    Keyboard(const Keyboard&) = delete;
    auto operator=(const Keyboard&) -> Keyboard & = delete;
    Keyboard(Keyboard&& Other) noexcept;
    auto operator=(Keyboard&& Other) noexcept -> Keyboard&;

    auto Initialize() -> bool;
    auto Attach(void* Protocol) -> bool;
    auto Detach() -> void;
    auto Valid() const -> bool;
    auto Reset(bool ExtendedVerification = false) -> bool;
    auto ReadKey(KEY_EVENT& Event) -> bool;
    auto WaitForKey() -> bool;
    auto ReadKeyBlocking(KEY_EVENT& Event) -> bool;
    auto RegisterKey(const KEY_EVENT& Event, NotifyFn Callback, NOTIFY_HANDLE& Handle) -> bool;
    auto RegisterScanCode(uint16_t ScanCode, NotifyFn Callback, NOTIFY_HANDLE& Handle) -> bool;
    auto RegisterKey(KEY Key, NotifyFn Callback, NOTIFY_HANDLE& Handle) -> bool;
    auto RegisterChar(char16_t Character, NotifyFn Callback, NOTIFY_HANDLE& Handle) -> bool;
    auto Unregister(NOTIFY_HANDLE& Handle) -> bool;
    auto NativeProtocol() const -> void*;

    static auto IsScanCode(const KEY_EVENT& Event, uint16_t ScanCode) -> bool;
    static auto IsKey(const KEY_EVENT& Event, KEY Key) -> bool;
    static auto IsChar(const KEY_EVENT& Event, char16_t Character) -> bool;
};