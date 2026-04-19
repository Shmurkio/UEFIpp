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

auto EfiMain([[maybe_unused]] const char** ArgV, [[maybe_unused]] uint64_t ArgC) -> EFI_STATUS
{
    return EFI_SUCCESS;
}