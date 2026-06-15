#include <UEFIpp/UEFIpp.hpp>

auto Main([[maybe_unused]] const Vector<String>& Args) -> Foundation::Bool
{
    Stream::Out::Console.Clear();

    Stream::Out::Console << "Press any key to continue.." << Stream::Endl;
    Stream::In::Console.Get();

    Stream::Out::Console << "Hello, UEFI!" << Stream::Endl;

    String Name{};
    Stream::Out::Console << "Enter name: " << Stream::FlushNow;
    Stream::In::Console >> Name;

    Stream::Out::Console << "Hello, " << Name << Stream::Endl;

    return true;
}