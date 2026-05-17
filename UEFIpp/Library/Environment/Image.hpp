#pragma once

#include "../Vector/Vector.hpp"
#include "../String/String.hpp"

class Image
{
private:
    String Path_{};
    void* Handle_ = nullptr;
    void* ParentHandle_ = nullptr;
    Vector<String> Args_{};

    void* ImageBase_ = nullptr;
    uint64_t ImageSize_ = 0;

    String ExitData_{};
    void* UnloadFunction_ = nullptr;

public:
    Image() = default;
    ~Image();

    Image(const Image&) = delete;
    auto operator=(const Image&) -> Image & = delete;

    Image(Image&& Other) noexcept;
    auto operator=(Image&& Other) noexcept -> Image&;

    auto Load(const String& Path) -> bool;
    auto Unload() -> bool;
    auto Start() -> bool;

    auto Loaded() const -> bool;
    auto Empty() const -> bool;

    auto Path() const -> const String&;
    auto Handle() const -> void*;
    auto ParentHandle() const -> void*;

    auto Args() -> Vector<String>&;
    auto Args() const -> const Vector<String>&;

    auto ImageBase() const -> void*;
    auto ImageSize() const -> uint64_t;

    auto ExitData() const -> const String&;
    auto UnloadFunction() const -> void*;

    static auto FindByGuid(CGUID& Guid) -> Image;
};