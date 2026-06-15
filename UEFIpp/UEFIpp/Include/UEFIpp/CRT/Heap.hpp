#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::CRT
{
	class Heap
	{
	public:
		Heap() = delete;

		[[nodiscard]] static auto Allocate(Foundation::Size NumberOfBytes) -> Foundation::Void*;

		[[nodiscard]] static auto AllocateZeroed(Foundation::Size Count, Foundation::Size Size) -> Foundation::Void*;

		[[nodiscard]] static auto Reallocate(Foundation::Void* Address, Foundation::Size NumberOfBytes) -> Foundation::Void*;

		static auto Free(Foundation::Void* Address) -> void;

		[[nodiscard]] static auto AllocationSize(const Foundation::Void* Address) -> Foundation::Size;

		[[nodiscard]] static auto AllocateAligned(Foundation::Size NumberOfBytes, Foundation::Size Alignment) -> Foundation::Void*;
	};
}