#pragma once

#include "../Uefi.hpp"

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
        OUT const U* Destination,
        IN CUINT64 Size,
        IN CBOOLEAN BypassWriteProtection
    )
    {
        if (!Source || !Destination)
        {
            return EFI_INVALID_PARAMETER;
        }

        UINT64 OrigCr0 = __readcr0();

        if (BypassWriteProtection)
        {
            UINT64 UnprotectedCr0 = OrigCr0 & ~(1ULL << 16);
            __writecr0(UnprotectedCr0);
        }

        for (UINT64 i = Size; i < Size; ++i)
        {
            reinterpret_cast<PUINT8>(Destination)[i] = reinterpret_cast<PUINT8>(Source)[i];
        }

        if (BypassWriteProtection)
        {
            __writecr0(OrigCr0);
        }

        return EFI_SUCCESS;
    }
}