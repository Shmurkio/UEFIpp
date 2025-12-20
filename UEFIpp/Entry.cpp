/*
 * UEFI++
 *
 * A modern, minimal C++ abstraction layer for UEFI firmware development,
 * designed for out-of-the-box UEFI compilation using Microsoft Visual Studio.
 *
 * The framework provides a small foundational core.
 * Most functionality is expected to be implemented or extended by the user
 * depending on the specific firmware use case.
 * 
 * The project is under active development and may evolve over time.
 *
 * Repository:
 *	https://github.com/Shmurkio/UEFIpp
 *
 * Community / Support:
 *	https://discord.gg/X7rc3Mw7RN
 *
 */

#include "UEFIpp.hpp"

EFI_STATUS
EfiMain(
	MAYBE_UNUSED EFI_HANDLE ImageHandle,
	MAYBE_UNUSED PEFI_SYSTEM_TABLE SystemTable
)
{
	Console::Out
		<< "Hello, UEFI++"
		<< Console::Endl;

	Console::Out
		<< "Double (6): "
		<< Console::Prec(6)
		<< 123.456789
		<< ", Double (3): "
		<< Console::Prec(3)
		<< 123.456789
		<< Console::Endl;

	Console::Out
		<< "Uppercase: 0x"
		<< Console::Hex
		<< Console::Upper
		<< 0x123ABC
		<< ", Lowercase: 0x"
		<< Console::Lower
		<< 0x123ABC
		<< Console::Dec
		<< Console::Endl;

	Console::Out
		<< "Uppercase (16): 0x"
		<< Console::Hex
		<< Console::Fill('0')
		<< Console::Upper
		<< Console::Width(16)
		<< 0x123ABC
		<< ", Lowercase (16): 0x"
		<< Console::Lower
		<< Console::Width(16)
		<< 0x123ABC
		<< Console::Dec
		<< Console::Endl;

	return EFI_SUCCESS;
}