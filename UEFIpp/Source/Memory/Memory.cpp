#include <UEFIpp/Memory/Memory.hpp>

extern "C" auto FastCopy(UEFIpp::Foundation::Void* Destination, const UEFIpp::Foundation::Void* Source, UEFIpp::Foundation::Uint64 NumberOfBytes) -> UEFIpp::Foundation::Void*;
extern "C" auto FastMove(UEFIpp::Foundation::Void* Destination, const UEFIpp::Foundation::Void* Source, UEFIpp::Foundation::Uint64 NumberOfBytes) -> UEFIpp::Foundation::Void*;
extern "C" auto FastSet(UEFIpp::Foundation::Void* Destination, UEFIpp::Foundation::Uint8 Value, UEFIpp::Foundation::Uint64 NumberOfBytes) -> UEFIpp::Foundation::Void*;
extern "C" auto FastCompare(const UEFIpp::Foundation::Void* Left, const UEFIpp::Foundation::Void* Right, UEFIpp::Foundation::Size NumberOfBytes) -> UEFIpp::Foundation::Int32;

namespace UEFIpp::Memory
{
	auto Copy(Foundation::Void* Destination, const Foundation::Void* Source, Foundation::Size NumberOfBytes) -> Foundation::Void*
	{
		return FastCopy(Destination, Source, NumberOfBytes);
	}

	auto Move(Foundation::Void* Destination, const Foundation::Void* Source, Foundation::Size NumberOfBytes) -> Foundation::Void*
	{
		return FastMove(Destination, Source, NumberOfBytes);
	}

	auto Set(Foundation::Void* Destination, Foundation::Uint8 Value, Foundation::Size NumberOfBytes) -> Foundation::Void*
	{
		return FastSet(Destination, Value, NumberOfBytes);
	}

	auto Zero(Foundation::Void* Destination, Foundation::Size NumberOfBytes) -> Foundation::Void*
	{
		return Set(Destination, 0, NumberOfBytes);
	}

	auto Compare(const Foundation::Void* Left, const Foundation::Void* Right, Foundation::Size NumberOfBytes) -> Foundation::Int32
	{
		return FastCompare(Left, Right, NumberOfBytes);
	}
}