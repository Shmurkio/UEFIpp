#include "UEFIpp.hpp"
#include "Library/Vector/Vector.hpp"
#include "Library/String/String.hpp"
#include "Library/Environment/Environment.hpp"

auto Main([[maybe_unused]] const Vector<String>& Args) -> bool
{
    String DxeCoreGuid = "D6A2CB7F-6A18-4E2F-B43B-9920A733700A";

    auto DxeCore = Image::FindByGuid(DxeCoreGuid.Guid());

    if (!DxeCore.Loaded())
    {
        Console::Out << "Failed to find DXE Core image by GUID: " << DxeCoreGuid << Console::Endl;
        return false;
    }

    Console::Out << "Found DXE Core image at 0x" << Console::Hex << DxeCore.ImageBase() << Console::Dec << " (" << DxeCore.ImageSize() << " bytes)" << Console::Endl;

    return true;
}