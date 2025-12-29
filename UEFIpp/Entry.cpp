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
	MAYBE_UNUSED IN PCSTR* ArgV,
	MAYBE_UNUSED IN UINT64 ArgC
)
{
	//
	// Console stream.
	//
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

	//
	// Serial stream.
	//
	Serial::Out
		<< "Hello, UEFI++"
		<< Serial::Endl;

	Serial::Out
		<< "ArgC: "
		<< ArgC
		<< Serial::Endl;

	if (ArgC)
	{
		Serial::Out
			<< "Command line: \"";
	}

	for (UINT64 i = 0; i < ArgC; ++i)
	{
		Serial::Out
			<< ArgV[i];

		if (i + 1 < ArgC)
		{
			Serial::Out
				<< " ";
		}
	}

	if (ArgC)
	{
		Serial::Out
			<< "\""
			<< Serial::Endl;
	}

	Serial::Out
		<< "UEFI++ serial stream now supports UTF-8!"
		<< Serial::Endl
		<< "For example the Greek alphabet: "
		<< UTF8::Alpha << UTF8::Slash << UTF8::alpha << UTF8::Comma << " "
		<< UTF8::Beta << UTF8::Slash << UTF8::beta << UTF8::Comma << " "
		<< UTF8::Gamma << UTF8::Slash << UTF8::gamma << UTF8::Comma << " "
		<< UTF8::Delta << UTF8::Slash << UTF8::delta << UTF8::Comma << " "
		<< UTF8::Epsilon << UTF8::Slash << UTF8::epsilon << UTF8::Comma << " "
		<< UTF8::Zeta << UTF8::Slash << UTF8::zeta << UTF8::Comma << " "
		<< UTF8::Eta << UTF8::Slash << UTF8::eta << UTF8::Comma << " "
		<< UTF8::Theta << UTF8::Slash << UTF8::theta << UTF8::Comma << " "
		<< UTF8::Iota << UTF8::Slash << UTF8::iota << UTF8::Comma << " "
		<< UTF8::Kappa << UTF8::Slash << UTF8::kappa << UTF8::Comma << " "
		<< UTF8::Lambda << UTF8::Slash << UTF8::lambda << UTF8::Comma << " "
		<< UTF8::Mu << UTF8::Slash << UTF8::mu << UTF8::Comma << " "
		<< UTF8::Nu << UTF8::Slash << UTF8::nu << UTF8::Comma << " "
		<< UTF8::Xi << UTF8::Slash << UTF8::xi << UTF8::Comma << " "
		<< UTF8::Omicron << UTF8::Slash << UTF8::omicron << UTF8::Comma << " "
		<< UTF8::Pi << UTF8::Slash << UTF8::pi << UTF8::Comma << " "
		<< UTF8::Rho << UTF8::Slash << UTF8::rho << UTF8::Comma << " "
		<< UTF8::Sigma << UTF8::Slash << UTF8::sigma << UTF8::Comma << " "
		<< UTF8::Tau << UTF8::Slash << UTF8::tau << UTF8::Comma << " "
		<< UTF8::Upsilon << UTF8::Slash << UTF8::upsilon << UTF8::Comma << " "
		<< UTF8::Phi << UTF8::Slash << UTF8::phi << UTF8::Comma << " "
		<< UTF8::Chi << UTF8::Slash << UTF8::chi << UTF8::Comma << " "
		<< UTF8::Psi << UTF8::Slash << UTF8::psi << UTF8::Comma << " "
		<< UTF8::Omega << UTF8::Slash << UTF8::omega
		<< Serial::Endl;

	Serial::Out
		<< "Note: Use a font such as \"Julia Mono\" (https://github.com/cormullion/juliamono) for proper UTF-8 rendering."
		<< Serial::Endl;

	return EFI_SUCCESS;
}