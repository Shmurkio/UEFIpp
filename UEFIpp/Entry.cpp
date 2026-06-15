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

    Stream::Out::Console << "Hello, " << Name << "!"  << Stream::Endl;
    Stream::Out::Serial << "Entered name: " << Name << Stream::Endl;

    return true;
}