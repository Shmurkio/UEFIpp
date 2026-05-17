#include "Uefi.hpp"

#include "Console/Console.hpp"
#include "Serial/Serial.hpp"
#include "../Library/Memory/Memory.hpp"
#include "../Library/String/String.hpp"
#include "../Library/Cast/Cast.hpp"

extern "C" int atexit(void (*func)(void))
{
	return 0;
}

extern "C" void __chkstk()
{
	return;
}

extern "C" int _fltused = 1;

PEFI_SYSTEM_TABLE gST = nullptr;
EFI_HANDLE gImageHandle = nullptr;
PEFI_BOOT_SERVICES gBS = nullptr;
PEFI_RUNTIME_SERVICES gRT = nullptr;

auto Main([[maybe_unused]] const Vector<String>& Args) -> bool;

static auto IsWhitespace(wchar_t Character) -> bool
{
	return Character == L' ' || Character == L'\t' || Character == L'\r' || Character == L'\n';
}

static auto GetLoadedImage() -> PEFI_LOADED_IMAGE_PROTOCOL
{
	PEFI_LOADED_IMAGE_PROTOCOL LoadedImage = nullptr;
	auto Status = gBS->HandleProtocol(gImageHandle, Cast::To<PCEFI_GUID>(&gEfiLoadedImageProtocolGuid), reinterpret_cast<void**>(&LoadedImage));

	if (EfiError(Status))
	{
		return nullptr;
	}

	return LoadedImage;
}

static auto GetLoadOptions(String& LoadOptions) -> bool
{
	LoadOptions.Clear();

	auto LoadedImage = GetLoadedImage();

	if (!LoadedImage || !LoadedImage->LoadOptions || !LoadedImage->LoadOptionsSize)
	{
		return true;
	}

	const auto Begin = LoadedImage->LoadOptions;
	const auto Bytes = LoadedImage->LoadOptionsSize;
	const auto Length = Bytes / sizeof(wchar_t);

	for (uint64_t i = 0; i < Length; ++i)
	{
		if (Begin[i] == L'\0')
		{
			break;
		}

		if (!LoadOptions.Append(Begin[i]))
		{
			LoadOptions.Clear();
			return false;
		}
	}

	return true;
}

static auto GetArgs(Vector<String>& Args) -> bool
{
	Args.Clear();

	String LoadOptions;

	if (!GetLoadOptions(LoadOptions))
	{
		return false;
	}

	const auto Begin = LoadOptions.WcharStr();
	const auto Length = LoadOptions.Size();
	const auto End = Begin + Length;
	auto Current = Begin;

	auto SkipWhitespace = [&]()
	{
		while (Current < End && *Current && IsWhitespace(*Current))
		{
			++Current;
		}
	};

	while (Current < End && *Current)
	{
		SkipWhitespace();

		if (Current >= End || *Current == L'\0')
		{
			break;
		}

		auto Quote = L'\0';

		if (*Current == L'"' || *Current == L'\'')
		{
			Quote = *Current++;
		}

		String Arg;

		while (Current < End && *Current)
		{
			if (Quote)
			{
				if (*Current == Quote)
				{
					++Current;
					break;
				}

				if (*Current == L'\\' && Current + 1 < End)
				{
					const auto Next = *(Current + 1);

					if (Next == Quote || Next == L'\\')
					{
						if (!Arg.Append(Next))
						{
							Args.Clear();
							return false;
						}

						Current += 2;
						continue;
					}
				}

				if (!Arg.Append(*Current++))
				{
					Args.Clear();
					return false;
				}
			}
			else
			{
				if (IsWhitespace(*Current))
				{
					break;
				}

				if (!Arg.Append(*Current++))
				{
					Args.Clear();
					return false;
				}
			}
		}

		if (!Args.PushBack(Memory::Move(Arg)))
		{
			Args.Clear();
			return false;
		}
	}

	return true;
}

EFI_STATUS EfipMain(EFI_HANDLE ImageHandle, PEFI_SYSTEM_TABLE SystemTable)
{
	gST = SystemTable;
	gImageHandle = ImageHandle;
	gBS = SystemTable->BootServices;
	gRT = SystemTable->RuntimeServices;

	Console::OutInit(SystemTable->ConOut);
	Serial::OutInit(0x3F8);

	Vector<String> Args;

	if (!GetArgs(Args))
	{
		Args.Clear();
	}

	if (!Main(Args))
	{
		return EFI_LOAD_ERROR;
	}

	return EFI_SUCCESS;
}