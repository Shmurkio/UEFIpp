#pragma once

#include <UEFIpp/CRT/StdDef.hpp>
#include <vcruntime_new.h>

// Throwing allocation
[[nodiscard]] void* operator new(std::size_t Size);
[[nodiscard]] void* operator new[](std::size_t Size);

// Aligned allocation
[[nodiscard]] void* operator new(std::size_t Size, std::align_val_t Alignment);
[[nodiscard]] void* operator new[](std::size_t Size, std::align_val_t Alignment);

// Nothrow allocation
[[nodiscard]] void* operator new(std::size_t Size, const std::nothrow_t&) noexcept;
[[nodiscard]] void* operator new[](std::size_t Size, const std::nothrow_t&) noexcept;

// Nothrow aligned allocation
[[nodiscard]] void* operator new(std::size_t Size, std::align_val_t Alignment, const std::nothrow_t&) noexcept;
[[nodiscard]] void* operator new[](std::size_t Size, std::align_val_t Alignment, const std::nothrow_t&) noexcept;

// Delete
void operator delete(void* Address) noexcept;
void operator delete[](void* Address) noexcept;

// Sized delete
void operator delete(void* Address, std::size_t Size) noexcept;
void operator delete[](void* Address, std::size_t Size) noexcept;

// Aligned delete
void operator delete(void* Address, std::align_val_t Alignment) noexcept;
void operator delete[](void* Address, std::align_val_t Alignment) noexcept;

// Sized aligned delete
void operator delete(void* Address, std::size_t Size, std::align_val_t Alignment) noexcept;
void operator delete[](void* Address, std::size_t Size, std::align_val_t Alignment) noexcept;

// Nothrow delete
void operator delete(void* Address, const std::nothrow_t&) noexcept;
void operator delete[](void* Address, const std::nothrow_t&) noexcept;

// Nothrow aligned delete
void operator delete(void* Address, std::align_val_t Alignment, const std::nothrow_t&) noexcept;
void operator delete[](void* Address, std::align_val_t Alignment, const std::nothrow_t&) noexcept;