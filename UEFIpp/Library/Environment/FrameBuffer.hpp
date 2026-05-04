#pragma once

#include "../Vector/Vector.hpp"

class FrameBuffer
{
public:
    typedef struct _PIXEL
    {
        uint8_t Blue = 0;
        uint8_t Green = 0;
        uint8_t Red = 0;
        uint8_t Reserved = 0;
    } PIXEL, *PPIXEL;

    enum class PIXEL_FORMAT : uint8_t
    {
        Bgra,
        Rgba,
        BitMask,
        Unknown
    };

private:
    void* FrameBuffer_ = nullptr;
    uint32_t Width_ = 0;
    uint32_t Height_ = 0;
    uint32_t PixelsPerScanLine_ = 0;
    uint32_t BytesPerPixel_ = 4;
    PIXEL_FORMAT Format_ = PIXEL_FORMAT::Unknown;
    bool Valid_ = false;

    Vector<PIXEL> BackBuffer_{};
    bool BackBufferEnabled_ = false;

    auto Attach(void* FrameBuffer, uint32_t Width, uint32_t Height, uint32_t PixelsPerScanLine, PIXEL_FORMAT Format) -> bool;

public:
    FrameBuffer() = default;

    auto Initialize() -> bool;
    auto Detach() -> void;

    auto EnableBackBuffer() -> bool;
    auto DisableBackBuffer() -> void;
    auto Present() -> bool;

    auto Valid() const -> bool;
    auto Width() const -> uint32_t;
    auto Height() const -> uint32_t;
    auto PixelsPerScanLine() const -> uint32_t;
    auto BytesPerPixel() const -> uint32_t;
    auto Format() const -> PIXEL_FORMAT;

    auto Data() -> void*;
    auto Data() const -> const void*;

    auto Buffer() -> Vector<PIXEL>&;
    auto Buffer() const -> const Vector<PIXEL>&;

    auto InBounds(uint32_t X, uint32_t Y) const -> bool;
    auto Offset(uint32_t X, uint32_t Y) const -> uint64_t;
    auto Index(uint32_t X, uint32_t Y) const -> uint64_t;

    static auto Rgb(uint8_t Red, uint8_t Green, uint8_t Blue) -> PIXEL;

    auto GetPixel(uint32_t X, uint32_t Y) const -> PIXEL;
    auto PutPixel(uint32_t X, uint32_t Y, PIXEL Pixel) -> bool;

    auto Clear(PIXEL Color = {}) -> void;
    auto FillRect(uint32_t X, uint32_t Y, uint32_t W, uint32_t H, PIXEL Color) -> void;

    auto DrawHorizontalLine(uint32_t X, uint32_t Y, uint32_t W, PIXEL Color) -> void;
    auto DrawVerticalLine(uint32_t X, uint32_t Y, uint32_t H, PIXEL Color) -> void;
};