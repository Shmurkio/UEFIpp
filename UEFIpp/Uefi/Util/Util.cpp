#include "Util.hpp"

EFI_STATUS
Util::StrWToStr(
	IN OUT PWSTR& StrW,
	OUT PSTR& Str,
	IN CBOOLEAN UseHeap,
	IN CBOOLEAN FreeStrW
)
{
	Str = nullptr;

	if (!StrW)
	{
		return EFI_INVALID_PARAMETER;
	}

	UINT64 Length = 0;

	while (StrW[Length])
	{
		++Length;
	}

	PSTR Out = nullptr;

	if (UseHeap)
	{
		Out = new CHAR[Length + 1];

		if (!Out)
		{
			return EFI_OUT_OF_RESOURCES;
		}
	}
	else
	{
		if (Length + 1 > gStrWToStrStaticCap)
		{
			return EFI_BUFFER_TOO_SMALL;
		}

		static CHAR Buffer[gStrWToStrStaticCap] = { 0 };

		Out = Buffer;
	}

	for (UINT64 i = 0; i < Length; ++i)
	{
		CWCHAR CharW = StrW[i];
		Out[i] = (CharW <= 0x7F) ? static_cast<CHAR>(CharW) : '?';
	}

	Out[Length] = '\0';
	Str = Out;

	if (FreeStrW)
	{
		delete[] StrW;
		StrW = nullptr;
	}

	return EFI_SUCCESS;
}

EFI_STATUS
Util::StrToStrW(
	IN OUT PSTR& Str,
	OUT PWSTR& StrW,
	IN CBOOLEAN UseHeap,
	IN CBOOLEAN FreeStr
)
{
	StrW = nullptr;

	if (!Str)
	{
		return EFI_INVALID_PARAMETER;
	}

	UINT64 Length = 0;

	while (Str[Length])
	{
		++Length;
	}

	PWSTR Out = nullptr;

	if (UseHeap)
	{
		Out = new WCHAR[Length + 1];

		if (!Out)
		{
			return EFI_OUT_OF_RESOURCES;
		}
	}
	else
	{
		if (Length + 1 > gStrToStrWStaticCap)
		{
			return EFI_BUFFER_TOO_SMALL;
		}

		static WCHAR Buffer[gStrToStrWStaticCap] = { 0 };

		Out = Buffer;
	}

	for (UINT64 i = 0; i < Length; ++i)
	{
		Out[i] = static_cast<WCHAR>(Str[i]);
	}

	Out[Length] = L'\0';
	StrW = Out;

	if (FreeStr)
	{
		delete[] Str;
		Str = nullptr;
	}

	return EFI_SUCCESS;
}