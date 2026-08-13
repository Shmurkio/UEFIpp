#pragma once

#include <UEFIpp/CRT/StdDef.hpp>

extern "C"
{
	[[nodiscard]] __declspec(restrict) void* malloc(std::size_t Size);
	[[nodiscard]] __declspec(restrict) void* calloc(std::size_t Count, std::size_t Size);
	[[nodiscard]] __declspec(restrict) void* realloc(void* Address, std::size_t Size);
	void free(void* Address);
	[[nodiscard]] void* aligned_alloc(std::size_t Alignment, std::size_t Size);
}