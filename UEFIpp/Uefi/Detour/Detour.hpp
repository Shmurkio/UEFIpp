#pragma once

#include "../Uefi.hpp"

template<typename Signature>
class Detour;

template<typename Ret, typename... Args>
class Detour<Ret(Args...)>
{
public:
    using Fn = Ret(*)(Args...);

    Detour() = default;

    Detour(
        IN const Fn Target,
        IN const Fn Detour,
        IN CBOOLEAN BypassWriteProtection = false OPTIONAL
    )
    {
        Attach(Target, Detour, BypassWriteProtection);
    }

    EFI_STATUS
        Attach(
            IN const Fn Target,
            IN const Fn Detour,
            IN CBOOLEAN BypassWriteProtection = false OPTIONAL
        )
    {
        if (mAttached)
        {
            return EFI_ACCESS_DENIED;
        }

        if (!Target || !Detour || Target == Detour)
        {
            return EFI_INVALID_PARAMETER;
        }

        mTarget = Target;
        mDetour = Detour;
        mBypassWriteProtection = BypassWriteProtection;

        EFI_STATUS Status = mAttach(Target, Detour, mBackup, mBypassWriteProtection);

        if (EfiError(Status))
        {
            mTarget = nullptr;
            return Status;
        }

        mAttached = true;
        return EFI_SUCCESS;
    }

    Ret
        CallOriginal(
            Args... Arguments
        )
    {
        if (!mTarget)
        {
            if constexpr (!std::is_void_v<Ret>)
            {
                return Ret{};
            }
            else
            {
                return;
            }
        }

        if (mAttached)
        {
            EFI_STATUS Status = mDetach(mTarget, mBackup, mBypassWriteProtection);

            if (EfiError(Status))
            {
                if constexpr (std::is_void_v<Ret>)
                {
                    return;
                }
                else
                {
                    return Ret{};
                }
            }
        }

        if constexpr (std::is_void_v<Ret>)
        {
            mTarget(std::forward<Args>(Arguments)...);

            if (mAttached)
            {
                EFI_STATUS Status = mAttach(mTarget, mDetour, mBackup, mBypassWriteProtection);

                if (EfiError(Status))
                {
                    mAttached = false;
                }
            }

            return;
        }
        else
        {
            Ret ReturnValue = mTarget(std::forward<Args>(Arguments)...);

            if (mAttached)
            {
                EFI_STATUS Status = mAttach(mTarget, mDetour, mBackup, mBypassWriteProtection);

                if (EfiError(Status))
                {
                    mAttached = false;
                }
            }

            return ReturnValue;
        }
    }

    EFI_STATUS
        Detach(
            VOID
        )
    {
        if (!mAttached || !mTarget)
        {
            return EFI_NOT_STARTED;
        }

        EFI_STATUS Status = mDetach(mTarget, mBackup, mBypassWriteProtection);

        if (!EfiError(Status))
        {
            mAttached = false;
        }

        return Status;
    }

private:
    Fn mTarget = nullptr, mDetour = nullptr;
    UINT8 mBackup[12] = { 0 };
    BOOLEAN mAttached = false, mBypassWriteProtection = false;

    EFI_STATUS
        mAttach(
            IN const Fn Target,
            IN const Fn Detour,
            OUT UINT8(&Backup)[12],
            IN CBOOLEAN BypassWriteProtection
        )
    {
        if (!Target || !Detour || Target == Detour)
        {
            return EFI_INVALID_PARAMETER;
        }

        EFI_STATUS Status = Util::CopyMemory(Target, Backup, sizeof(Backup), false);

        if (EfiError(Status))
        {
            return Status;
        }

        UINT8 Instruction[] =
        {
            // MOV RAX, Target.
            0x48, 0xB8,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

            //
            // JMP RAX.
            //
            0xFF, 0xE0
        };

        *reinterpret_cast<PUINT64>(&Instruction[2]) = reinterpret_cast<UINT64>(Detour);

        return Util::CopyMemory(Instruction, Target, sizeof(Instruction), BypassWriteProtection);
    }

    EFI_STATUS
        mDetach(
            IN const Fn Target,
            IN CUINT8(&Backup)[12],
            IN CBOOLEAN BypassWriteProtection
        )
    {
        if (!Target)
        {
            return EFI_INVALID_PARAMETER;
        }

        return Util::CopyMemory(Backup, Target, sizeof(Backup), BypassWriteProtection);
    }
};