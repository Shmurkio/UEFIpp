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
}