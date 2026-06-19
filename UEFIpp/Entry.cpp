#include <UEFIpp/UEFIpp.hpp>

auto Main([[maybe_unused]] const Vector<String>& Args) -> Foundation::Bool
{
    // Argument parsing and serial output stream
    Stream::Out::Serial << "Argument count: " << Args.Size() << Stream::Endl;

    Foundation::Size Index{};

    for (const auto& Arg : Args)
    {
        Stream::Out::Serial << "  [" << Index++ << "] " << Arg << Stream::Endl;
    }

    // Console output stream
    Stream::Out::Console << "UEFI++ Sample Application" << Stream::Endl << Stream::Endl;

    // Console input stream
    Stream::Out::Console << "Press any key to continue..." << Stream::Endl;

    Stream::In::Console.Get();

    Stream::Out::Console << "We love UEFI!" << Stream::Endl;

    String Name{};
    Stream::Out::Console << "Enter name: " << Stream::FlushNow;
    Stream::In::Console >> Name;

    Stream::Out::Console << "Hello, " << Name << "!" << Stream::Endl;
    Stream::Out::Serial << "Entered name: " << Name << Stream::Endl;

    // File output stream
    Stream::FileOutputStream Log("Sample.txt");

    Log << "UEFI++ File Stream Example" << Stream::Endl
        << "Name: " << Name << Stream::Endl
        << "Number: " << 12345 << Stream::Endl
        << "Hex: " << Stream::Hexadecimal << 0xDEADBEEF << Stream::Decimal << Stream::Endl;

    // File input stream
    Stream::FileInputStream Input("Sample.txt");

    String Line{};
    Stream::Out::Console << "Contents of Sample.txt:" << Stream::Endl;

    while (Input.ReadLine(Line))
    {
        Stream::Out::Console << Line << Stream::Endl;
    }

    // Locate a loaded DXE image by file GUID
    constexpr UEFI::Guid PcdDxeGuid{ 0x80CF7257, 0x87AB, 0x47F9, { 0xA3, 0xFE, 0xD5, 0x0B, 0x76, 0xD8, 0x95, 0x41 } };
    Protocols::Access Access{ &UEFI::Context::BootServices() };

    const auto PcdDxe = Loader::FindLoadedImageByFileGuid(Access, PcdDxeGuid);

    if (!PcdDxe)
    {
        Stream::Out::Serial << "PcdDxe not found" << Stream::Endl;
        return false;
    }

    Stream::Out::Serial << "PcdDxe found at " << PcdDxe->Base << " (size: " << PcdDxe->Size << " bytes)" << Stream::Endl;

    // Hex dump
    Stream::Out::Serial << Stream::HexDump(PcdDxe->Base, 100, PcdDxe->Base);

    return true;
}