#pragma once

#include "../Uefi.hpp"

#define IN_RANGE(x, a, b) (x >= a && x <= b)
#define GET_BITS(x) (IN_RANGE((x&(~0x20)),'A','F')?((x&(~0x20))-'A'+0xA):(IN_RANGE(x,'0','9')?x-'0':0))
#define GET_BYTE(a, b) (GET_BITS(a) << 4 | GET_BITS(b))
#define P2ALIGNUP(x, Align) (((x) + ((Align) - 1)) & ~((Align) - 1))

namespace Util
{
	static inline constexpr CUINT64 gStrWToStrStaticCap = 512;
	static inline constexpr CUINT64 gStrToStrWStaticCap = 512;

	EFI_STATUS
	StrWToStr(
		IN OUT PWSTR& StrW,
		OUT PSTR& Str,
		IN CBOOLEAN UseHeap,
		IN CBOOLEAN FreeStrW
	);

	EFI_STATUS
	StrToStrW(
		IN OUT PSTR& Str,
		OUT PWSTR& StrW,
		IN CBOOLEAN UseHeap,
		IN CBOOLEAN FreeStr
	);

    template<typename T, typename U>
    EFI_STATUS
    CopyMemory(
        IN const T* Source,
        OUT U* Destination,
        IN CUINT64 Size,
        IN CBOOLEAN BypassWriteProtection
    )
    {
        if (!Source || !Destination || !Size)
        {
            return EFI_INVALID_PARAMETER;
        }

        UINT64 OrigCr0 = __readcr0();

        if (BypassWriteProtection)
        {
            __writecr0(OrigCr0 & ~(1ULL << 16));
        }

        PCUINT8 Src = reinterpret_cast<PCUINT8>(Source);
        PUINT8 Dst = reinterpret_cast<PUINT8>(Destination);

        for (UINT64 i = 0; i < Size; ++i)
        {
            Dst[i] = Src[i];
        }

        if (BypassWriteProtection)
        {
            __writecr0(OrigCr0);
        }

        return EFI_SUCCESS;
    }

    template<typename T>
    EFI_STATUS
    FindPattern(
        IN UINT64 BaseAddress,
        IN UINT64 Size,
        IN PCSTR Pattern,
        OUT T& Out
    )
    {
        PUINT8 FirstMatch = nullptr;
        PCSTR CurrentPattern = Pattern;
        PUINT8 Start = reinterpret_cast<PUINT8>(BaseAddress);
        PUINT8 End = Start + Size;

        for (PUINT8 Current = Start; Current < End; ++Current)
        {
            UINT8 Byte = CurrentPattern[0];

            if (!Byte)
            {
                Out = reinterpret_cast<T>(FirstMatch);
                return EFI_SUCCESS;
            }

            if (Byte == '\?' || *Current == GET_BYTE(Byte, CurrentPattern[1]))
            {
                if (!FirstMatch)
                {
                    FirstMatch = Current;
                }

                if (!CurrentPattern[2])
                {
                    Out = reinterpret_cast<T>(FirstMatch);
                    return EFI_SUCCESS;
                }

                if (Byte == '\?')
                {
                    CurrentPattern += 2;
                }
                else
                {
                    CurrentPattern += 3;
                }
            }
            else
            {
                CurrentPattern = Pattern;
                FirstMatch = nullptr;
            }
        }

        Out = 0ULL;
        return EFI_NOT_FOUND;
    }
}