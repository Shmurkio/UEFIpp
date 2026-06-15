#pragma once

#include <UEFIpp/Memory/Allocator.hpp>

namespace UEFIpp::Memory
{
	auto Copy(Foundation::Void* Destination, const Foundation::Void* Source, Foundation::Size NumberOfBytes) -> Foundation::Void*;

	auto Move(Foundation::Void* Destination, const Foundation::Void* Source, Foundation::Size NumberOfBytes) -> Foundation::Void*;

	auto Set(Foundation::Void* Destination, Foundation::Uint8 Value, Foundation::Size NumberOfBytes) -> Foundation::Void*;

	auto Zero(Foundation::Void* Destination, Foundation::Size NumberOfBytes) -> Foundation::Void*;

	[[nodiscard]] auto Compare(const Foundation::Void* Left, const Foundation::Void* Right, Foundation::Size NumberOfBytes) -> Foundation::Int32;
}