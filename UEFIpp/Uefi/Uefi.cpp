#include "Uefi.hpp"

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

EFI_STATUS EfiMain(
	MAYBE_UNUSED IN EFI_HANDLE ImageHandle,
	MAYBE_UNUSED IN PEFI_SYSTEM_TABLE SystemTable
);

EFI_STATUS
EfipMain(
	IN EFI_HANDLE ImageHandle,
	IN PEFI_SYSTEM_TABLE SystemTable
)
{
	gST = SystemTable;
	gImageHandle = ImageHandle;
	gBS = SystemTable->BootServices;
	Console::OutInit(SystemTable->ConOut);
	Serial::OutInit(0x3F8);
	return EfiMain(ImageHandle, SystemTable);
}