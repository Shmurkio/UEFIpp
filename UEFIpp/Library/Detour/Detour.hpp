#pragma once

#include <cstdint>
#include "../Memory/Memory.hpp"
#include "../Cast/Cast.hpp"

template<typename Signature>
class Detour;

template<typename Ret, typename... Args>
class Detour<Ret(Args...)>
{
public:
    using Fn = Ret(*)(Args...);

    Detour() = default;

    Detour(Fn Target, Fn Detour, bool BypassWriteProtection = true)
    {
        Attach(Target, Detour, BypassWriteProtection);
    }

    auto Attach(Fn Target, Fn Detour, bool BypassWriteProtection = true) -> bool
    {
        if (Attached_)
        {
            return false;
        }

        if (!Target || !Detour || Target == Detour)
        {
            return false;
        }

        if (!Attach_(Target, Detour, Backup_, BypassWriteProtection))
        {
            return false;
        }

        Target_ = Target;
        Detour_ = Detour;
        BypassWriteProtection_ = BypassWriteProtection;
        Attached_ = true;

        return true;
    }

    auto CallOriginal(Args... Arguments) -> Ret
    {
        if (!Target_)
        {
            if constexpr (!Cast::Detail::IsSame<typename Cast::Detail::RemoveCv<Ret>::Type, void>::Value)
            {
                return Ret{};
            }
            else
            {
                return;
            }
        }

        const bool WasAttached = Attached_;

        if (WasAttached)
        {
            if (!Detach_(Target_, Backup_, BypassWriteProtection_))
            {
                if constexpr (!Cast::Detail::IsSame<typename Cast::Detail::RemoveCv<Ret>::Type, void>::Value)
                {
                    return Ret{};
                }
                else
                {
                    return;
                }
            }

            Attached_ = false;
        }

        if constexpr (Cast::Detail::IsSame<typename Cast::Detail::RemoveCv<Ret>::Type, void>::Value)
        {
            Target_(Arguments...);

            if (WasAttached)
            {
                if (Attach_(Target_, Detour_, Backup_, BypassWriteProtection_))
                {
                    Attached_ = true;
                }
            }

            return;
        }
        else
        {
            Ret Result = Target_(Arguments...);

            Attached_ = false;

            if (WasAttached)
            {
                if (Attach_(Target_, Detour_, Backup_, BypassWriteProtection_))
                {
                    Attached_ = true;
                }
            }

            return Result;
        }
    }

    auto Detach() -> bool
    {
        if (!Attached_ || !Target_)
        {
            return false;
        }

        if (!Detach_(Target_, Backup_, BypassWriteProtection_))
        {
            return false;
        }

        Attached_ = false;
        return true;
    }

private:
    Fn Target_ = nullptr;
    Fn Detour_ = nullptr;
    uint8_t Backup_[12] = { 0 };
    bool Attached_ = false;
    bool BypassWriteProtection_ = true;

    static auto Attach_(Fn Target, Fn Detour, uint8_t(&Backup)[12], bool BypassWriteProtection) -> bool
    {
        if (!Target || !Detour || Target == Detour)
        {
            return false;
        }

        if (!Memory::CopyMemory(Backup, Cast::To<void*>(Target), sizeof(Backup), false))
        {
            return false;
        }

        uint8_t Jump[12] =
        {
            0x48, 0xB8,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xFF, 0xE0
        };

        auto DetourAddress = Cast::To<uint64_t>(Detour);

        if (!Memory::CopyMemory(Jump + 2, &DetourAddress, sizeof(DetourAddress), false))
        {
            return false;
        }

        return Memory::CopyMemory(Cast::To<void*>(Target), Jump, sizeof(Jump), BypassWriteProtection);
    }

    static auto Detach_(Fn Target, uint8_t(&Backup)[12], bool BypassWriteProtection) -> bool
    {
        if (!Target)
        {
            return false;
        }

        return Memory::CopyMemory(Cast::To<void*>(Target), Backup, sizeof(Backup), BypassWriteProtection);
    }
};