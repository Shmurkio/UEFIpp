#include "Uefi.hpp"

#include "Console/Console.hpp"
#include "Serial/Serial.hpp"
#include "Memory/Memory.hpp"

//
// Required for compiler.
//
extern "C"
VOID
__chkstk(
	VOID
)
{
	return;
}

extern "C" int _fltused = 1;

//
// Entry & initialization.
//
PEFI_SYSTEM_TABLE gST = nullptr;
EFI_HANDLE gImageHandle = nullptr;
PEFI_BOOT_SERVICES gBS = nullptr;
PEFI_RUNTIME_SERVICES gRT = nullptr;

EFI_STATUS
EfiMain(
	MAYBE_UNUSED IN PCSTR* ArgV,
	MAYBE_UNUSED IN UINT64 ArgC
);

static
EFI_STATUS
GetLoadOptions(
	OUT PWSTR& LoadOptions,
	OUT UINT32& LoadOptionsSize
)
{
	LoadOptions = nullptr;
	LoadOptionsSize = 0;

	PEFI_LOADED_IMAGE_PROTOCOL LoadedImage = nullptr;
	EFI_STATUS Status = gBS->HandleProtocol(gImageHandle, const_cast<PCEFI_GUID>(&gEfiLoadedImageProtocolGuid), reinterpret_cast<PVOID*>(&LoadedImage));

	if (EfiError(Status))
	{
		return Status;
	}

	LoadOptions = LoadedImage->LoadOptions;
	LoadOptionsSize = LoadedImage->LoadOptionsSize;

	return EFI_SUCCESS;
}

static
inline
BOOLEAN
IsWs(
	IN CWCHAR C
)
{
	return (C == L' ' || C == L'\t' || C == L'\r' || C == L'\n');
}

static
inline
UINT64
Utf8LenFromCodepoint(
	IN CUINT32 Cp
)
{
	if (Cp <= 0x7F) return 1;
	if (Cp <= 0x7FF) return 2;
	if (Cp <= 0xFFFF) return 3;
	return 4;
}

static
EFI_STATUS
AppendUtf8FromCodepoint(
	IN CUINT32 Cp,
	IN OUT PSTR& Out
)
{
	if (!Out)
	{
		return EFI_INVALID_PARAMETER;
	}

	switch (Utf8LenFromCodepoint(Cp))
	{
	case 1:
	{
		*Out++ = Cp;
		break;
	}
	case 2:
	{
		*Out++ = (0xC0 | ((Cp >> 6) & 0x1F));
		*Out++ = (0x80 | (Cp & 0x3F));
		break;
	}
	case 3:
	{
		*Out++ = (0xE0 | ((Cp >> 12) & 0x0F));
		*Out++ = (0x80 | ((Cp >> 6) & 0x3F));
		*Out++ = (0x80 | (Cp & 0x3F));
		break;
	}
	default:
	{
		*Out++ = (0xF0 | ((Cp >> 18) & 0x07));
		*Out++ = (0x80 | ((Cp >> 12) & 0x3F));
		*Out++ = (0x80 | ((Cp >> 6) & 0x3F));
		*Out++ = (0x80 | (Cp & 0x3F));
		break;
	}
	}

	return EFI_SUCCESS;
}

static
inline
BOOLEAN
ReadCodePoint(
	IN OUT PWSTR& P,
	IN PCWSTR End,
	OUT UINT32& Cp
)
{
	if (P >= End || *P == 0)
	{
		return false;
	}

	UINT16 W1 = static_cast<UINT16>(*P++);

	if (W1 >= 0xD800 && W1 <= 0xDBFF)
	{
		if (P < End && *P)
		{
			UINT16 W2 = static_cast<UINT16>(*P);

			if (W2 >= 0xDC00 && W2 <= 0xDFFF)
			{
				++P;
				Cp = 0x10000 + (((W1 - 0xD800) << 10) | (W2 - 0xDC00));
				return true;
			}
		}

		Cp = 0xFFFD;
		return true;
	}

	if (W1 >= 0xDC00 && W1 <= 0xDFFF)
	{
		Cp = 0xFFFD;
		return true;
	}

	Cp = W1;
	return true;
}

static
EFI_STATUS
LoadOptionsToArgs(
	IN PCWSTR LoadOptions,
	IN CUINT32 LoadOptionsSize,
	OUT PSTR*& ArgV,
	OUT UINT64& ArgC
)
{
	ArgV = nullptr;
	ArgC = 0;

	if (!LoadOptions || !LoadOptionsSize)
	{
		return EFI_INVALID_PARAMETER;
	}

	PWSTR Begin = const_cast<PWSTR>(LoadOptions);
	PWSTR End = reinterpret_cast<PWSTR>(reinterpret_cast<PUINT8>(Begin) + LoadOptionsSize);

	auto
	SkipWs = [&](
		IN OUT PWSTR& P
	)
	{
		while (P < End && *P && IsWs(*P))
		{
			++P;
		}
	};

	{
		PWSTR P = Begin;

		while (P < End && *P)
		{
			SkipWs(P);

			if (P >= End || *P == 0)
			{
				break;
			}

			++ArgC;
			WCHAR Quote = 0;

			if (*P == L'"' || *P == L'\'')
			{
				Quote = *P++;
			}

			while (P < End && *P)
			{
				if (Quote)
				{
					if (*P == Quote)
					{
						++P;
						break;
					}

					if (*P == L'\\')
					{
						PWSTR T = P + 1;

						if (T < End && *T && (*T == Quote || *T == L'\\'))
						{
							P += 2;
							continue;
						}
					}

					++P;
				}
				else
				{
					if (IsWs(*P))
					{
						break;
					}

					++P;
				}
			}
		}
	}

	ArgV = new PSTR[ArgC + 1];

	if (!ArgV)
	{
		ArgC = 0;
		return EFI_OUT_OF_RESOURCES;
	}

	for (UINT64 i = 0; i < ArgC + 1; ++i)
	{
		ArgV[i] = nullptr;
	}

	auto
	Cleanup = [&](
		VOID
	)
	{
		if (!ArgV)
		{
			return;
		}

		for (UINT64 i = 0; i < ArgC; ++i)
		{
			delete[] ArgV[i];
			ArgV[i] = nullptr;
		}

		delete[] ArgV;
		ArgV = nullptr;
		ArgC = 0;
	};

	UINT64 OutIndex = 0;
	PWSTR P = Begin;

	while (P < End && *P)
	{
		SkipWs(P);
		if (P >= End || *P == 0)
		{
			break;
		}

		PWSTR TokBegin = P;
		WCHAR Quote = 0;

		if (*P == L'"' || *P == L'\'')
		{
			Quote = *P++;
		}

		PWSTR TokEnd = P;

		while (P < End && *P)
		{
			if (Quote)
			{
				if (*P == Quote)
				{
					TokEnd = P;
					++P;
					break;
				}

				if (*P == L'\\')
				{
					PWSTR T = P + 1;

					if (T < End && *T && (*T == Quote || *T == L'\\'))
					{
						P += 2;
						TokEnd = P;
						continue;
					}
				}

				++P;
				TokEnd = P;
			}
			else
			{
				if (IsWs(*P))
				{
					TokEnd = P;
					break;
				}

				++P;
				TokEnd = P;
			}
		}

		UINT64 Need = 0;
		{
			PWSTR Q = TokBegin;

			if (Quote)
			{
				Q = TokBegin + 1;
			}

			PWSTR QEnd = TokEnd;

			while (Q < QEnd)
			{
				if (Quote && *Q == L'\\' && (Q + 1) < QEnd)
				{
					WCHAR N = *(Q + 1);
					if (N == Quote || N == L'\\')
					{
						Q += 2;
						Need += 1;
						continue;
					}
				}

				UINT32 Cp = 0;
				PWSTR Before = Q;

				if (!ReadCodePoint(Q, QEnd, Cp))
				{
					break;
				}

				if (Q == Before)
				{
					++Q;
					Cp = 0xFFFD;
				}

				Need += Utf8LenFromCodepoint(Cp);
			}
		}

		PSTR Out = new CHAR[Need + 1];

		if (!Out)
		{
			Cleanup();
			return EFI_OUT_OF_RESOURCES;
		}

		PSTR Cur = Out;

		{
			PWSTR Q = TokBegin;

			if (Quote)
			{
				Q = TokBegin + 1;
			}

			PWSTR QEnd = TokEnd;

			while (Q < QEnd)
			{
				if (Quote && *Q == L'\\' && (Q + 1) < QEnd)
				{
					WCHAR N = *(Q + 1);

					if (N == Quote || N == L'\\')
					{
						*Cur++ = static_cast<CHAR8>(N);
						Q += 2;
						continue;
					}
				}

				UINT32 Cp = 0;

				if (!ReadCodePoint(Q, QEnd, Cp))
				{
					break;
				}

				EFI_STATUS Status = AppendUtf8FromCodepoint(Cp, Cur);

				if (EfiError(Status))
				{
					delete[] Out;
					Cleanup();
					return Status;
				}
			}
		}

		*Cur = '\0';
		ArgV[OutIndex++] = Out;
	}

	ArgV[OutIndex] = nullptr;
	ArgC = OutIndex;

	return EFI_SUCCESS;
}

EFI_STATUS
EfipMain(
	IN EFI_HANDLE ImageHandle,
	IN PEFI_SYSTEM_TABLE SystemTable
)
{
	//
	// Set globals.
	//
	gST = SystemTable;
	gImageHandle = ImageHandle;
	gBS = SystemTable->BootServices;
	gRT = SystemTable->RuntimeServices;

	//
	// Set up streams.
	//
	Console::OutInit(SystemTable->ConOut);
	Serial::OutInit(0x3F8);

	//
	// Get load options (command line args) if possible.
	//
	PWSTR LoadOptions = nullptr;
	UINT32 LoadOptionsSize = 0;

	EFI_STATUS Status = GetLoadOptions(LoadOptions, LoadOptionsSize);

	if (EfiError(Status))
	{
		return EfiMain(nullptr, 0);
	}

	PSTR* ArgV = nullptr;
	UINT64 ArgC = 0;

	Status = LoadOptionsToArgs(LoadOptions, LoadOptionsSize, ArgV, ArgC);

	if (EfiError(Status))
	{
		return EfiMain(nullptr, 0);
	}

	//
	// Call real entry point.
	//
	Status = EfiMain(const_cast<PCSTR*>(ArgV), ArgC);

	//
	// Clean up.
	//
	if (ArgV)
	{
		for (UINT64 i = 0; i < ArgC; ++i)
		{
			delete[] ArgV[i];
		}

		delete[] ArgV;
	}

	return Status;
}