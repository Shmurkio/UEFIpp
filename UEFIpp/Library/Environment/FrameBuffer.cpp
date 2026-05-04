#include "FrameBuffer.hpp"
#include "../../UEFIpp.hpp"

auto FrameBuffer::Attach(void* FrameBuffer, uint32_t Width, uint32_t Height, uint32_t PixelsPerScanLine, PIXEL_FORMAT Format) -> bool
{
    if (!FrameBuffer || !Width || !Height || !PixelsPerScanLine)
    {
        return false;
    }

    FrameBuffer_ = FrameBuffer;
    Width_ = Width;
    Height_ = Height;
    PixelsPerScanLine_ = PixelsPerScanLine;
    BytesPerPixel_ = sizeof(PIXEL);
    Format_ = Format;
    Valid_ = true;

    return true;
}

auto FrameBuffer::Initialize() -> bool
{
    PEFI_GRAPHICS_OUTPUT_PROTOCOL Gop = nullptr;

    auto Status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, nullptr, Cast::To<void**>(&Gop));
    
    if (EfiError(Status) || !Gop || !Gop->Mode || !Gop->Mode->Info)
    {
        return false;
    }

    auto Format = PIXEL_FORMAT::Unknown;

    switch (Gop->Mode->Info->PixelFormat)
    {
    case PixelBlueGreenRedReserved8BitPerColor:
    {
        Format = PIXEL_FORMAT::Bgra;
        break;
    }
    case PixelRedGreenBlueReserved8BitPerColor:
    {
        Format = PIXEL_FORMAT::Rgba;
        break;
    }
    case PixelBitMask:
    {
        Format = PIXEL_FORMAT::BitMask;
        break;
    }
    default:
    {
        Format = PIXEL_FORMAT::Unknown;
        break;
    }
    }

    return Attach(Cast::To<void*>(Gop->Mode->FrameBufferBase), Gop->Mode->Info->HorizontalResolution, Gop->Mode->Info->VerticalResolution, Gop->Mode->Info->PixelsPerScanLine, Format);
}

auto FrameBuffer::Detach() -> void
{
    FrameBuffer_ = nullptr;
    Width_ = 0;
    Height_ = 0;
    PixelsPerScanLine_ = 0;
    BytesPerPixel_ = sizeof(PIXEL);
    Format_ = PIXEL_FORMAT::Unknown;
    Valid_ = false;

    BackBuffer_.Clear();
    BackBufferEnabled_ = false;
}

auto FrameBuffer::EnableBackBuffer() -> bool
{
    if (!Valid_)
    {
        return false;
    }

    const auto Count = Cast::To<uint64_t>(PixelsPerScanLine_) * Height_;

    if (!BackBuffer_.Resize(Count))
    {
        return false;
    }

    for (uint32_t y = 0; y < Height_; ++y)
    {
        for (uint32_t x = 0; x < Width_; ++x)
        {
            BackBuffer_[Index(x, y)] = GetPixel(x, y);
        }
    }

    BackBufferEnabled_ = true;

    return true;
}

auto FrameBuffer::DisableBackBuffer() -> void
{
    BackBuffer_.Clear();
    BackBufferEnabled_ = false;
}

auto FrameBuffer::Present() -> bool
{
    if (!Valid_ || !BackBufferEnabled_ || BackBuffer_.Empty())
    {
        return false;
    }

    for (uint32_t y = 0; y < Height_; ++y)
    {
        for (uint32_t x = 0; x < Width_; ++x)
        {
            auto Pixel = BackBuffer_[Index(x, y)];

            auto PixelPtr = reinterpret_cast<uint8_t*>(FrameBuffer_) + Offset(x, y);

            switch (Format_)
            {
            case PIXEL_FORMAT::Bgra:
            {
                PixelPtr[0] = Pixel.Blue;
                PixelPtr[1] = Pixel.Green;
                PixelPtr[2] = Pixel.Red;
                PixelPtr[3] = Pixel.Reserved;
                break;
            }
            case PIXEL_FORMAT::Rgba:
            {
                PixelPtr[0] = Pixel.Red;
                PixelPtr[1] = Pixel.Green;
                PixelPtr[2] = Pixel.Blue;
                PixelPtr[3] = Pixel.Reserved;
                break;
            }
            default:
            {
                PixelPtr[0] = Pixel.Blue;
                PixelPtr[1] = Pixel.Green;
                PixelPtr[2] = Pixel.Red;
                PixelPtr[3] = Pixel.Reserved;
                break;
            }
            }
        }
    }

    return true;
}

auto FrameBuffer::Valid() const -> bool
{
    return Valid_;
}

auto FrameBuffer::Width() const -> uint32_t
{
    return Width_;
}

auto FrameBuffer::Height() const -> uint32_t
{
    return Height_;
}

auto FrameBuffer::PixelsPerScanLine() const -> uint32_t
{
    return PixelsPerScanLine_;
}

auto FrameBuffer::BytesPerPixel() const -> uint32_t
{
    return BytesPerPixel_;
}

auto FrameBuffer::Format() const -> PIXEL_FORMAT
{
    return Format_;
}

auto FrameBuffer::Data() -> void*
{
    return FrameBuffer_;
}

auto FrameBuffer::Data() const -> const void*
{
    return FrameBuffer_;
}

auto FrameBuffer::Buffer() -> Vector<PIXEL>&
{
    return BackBuffer_;
}

auto FrameBuffer::Buffer() const -> const Vector<PIXEL>&
{
    return BackBuffer_;
}

auto FrameBuffer::InBounds(uint32_t X, uint32_t Y) const -> bool
{
    return Valid_ && X < Width_ && Y < Height_;
}

auto FrameBuffer::Offset(uint32_t X, uint32_t Y) const -> uint64_t
{
    return ((Cast::To<uint64_t>(Y) * PixelsPerScanLine_) + X) * BytesPerPixel_;
}

auto FrameBuffer::Index(uint32_t X, uint32_t Y) const -> uint64_t
{
    return (Cast::To<uint64_t>(Y) * PixelsPerScanLine_) + X;
}

auto FrameBuffer::Rgb(uint8_t Red, uint8_t Green, uint8_t Blue) -> PIXEL
{
    return PIXEL
    {
        Blue,
        Green,
        Red,
        0
    };
}

auto FrameBuffer::GetPixel(uint32_t X, uint32_t Y) const -> PIXEL
{
    if (!InBounds(X, Y))
    {
        return {};
    }

    if (BackBufferEnabled_ && !BackBuffer_.Empty())
    {
        return BackBuffer_[Index(X, Y)];
    }

    auto PixelPtr = reinterpret_cast<const uint8_t*>(FrameBuffer_) + Offset(X, Y);
    PIXEL Pixel{};

    switch (Format_)
    {
    case PIXEL_FORMAT::Bgra:
    {
        Pixel.Blue = PixelPtr[0];
        Pixel.Green = PixelPtr[1];
        Pixel.Red = PixelPtr[2];
        Pixel.Reserved = PixelPtr[3];
        break;
    }
    case PIXEL_FORMAT::Rgba:
    {
        Pixel.Red = PixelPtr[0];
        Pixel.Green = PixelPtr[1];
        Pixel.Blue = PixelPtr[2];
        Pixel.Reserved = PixelPtr[3];
        break;
    }
    default:
    {
        Pixel.Blue = PixelPtr[0];
        Pixel.Green = PixelPtr[1];
        Pixel.Red = PixelPtr[2];
        Pixel.Reserved = PixelPtr[3];
        break;
    }
    }

    return Pixel;
}

auto FrameBuffer::PutPixel(uint32_t X, uint32_t Y, PIXEL Pixel) -> bool
{
    if (!InBounds(X, Y))
    {
        return false;
    }

    if (BackBufferEnabled_)
    {
        BackBuffer_[Index(X, Y)] = Pixel;
        return true;
    }

    auto PixelPtr = reinterpret_cast<uint8_t*>(FrameBuffer_) + Offset(X, Y);

    switch (Format_)
    {
    case PIXEL_FORMAT::Bgra:
    {
        PixelPtr[0] = Pixel.Blue;
        PixelPtr[1] = Pixel.Green;
        PixelPtr[2] = Pixel.Red;
        PixelPtr[3] = Pixel.Reserved;
        break;
    }
    case PIXEL_FORMAT::Rgba:
    {
        PixelPtr[0] = Pixel.Red;
        PixelPtr[1] = Pixel.Green;
        PixelPtr[2] = Pixel.Blue;
        PixelPtr[3] = Pixel.Reserved;
        break;
    }
    default:
    {
        PixelPtr[0] = Pixel.Blue;
        PixelPtr[1] = Pixel.Green;
        PixelPtr[2] = Pixel.Red;
        PixelPtr[3] = Pixel.Reserved;
        break;
    }
    }

    return true;
}

auto FrameBuffer::Clear(PIXEL Color) -> void
{
    FillRect(0, 0, Width_, Height_, Color);
}

auto FrameBuffer::FillRect(uint32_t X, uint32_t Y, uint32_t W, uint32_t H, PIXEL Color) -> void
{
    if (!Valid_)
    {
        return;
    }

    for (uint32_t yy = 0; yy < H; ++yy)
    {
        auto Py = Y + yy;

        if (Py >= Height_)
        {
            break;
        }

        for (uint32_t xx = 0; xx < W; ++xx)
        {
            auto Px = X + xx;

            if (Px >= Width_)
            {
                break;
            }

            PutPixel(Px, Py, Color);
        }
    }
}

auto FrameBuffer::DrawHorizontalLine(uint32_t X, uint32_t Y, uint32_t W, PIXEL Color) -> void
{
    if (!InBounds(X, Y))
    {
        return;
    }

    for (uint32_t i = 0; i < W && X + i < Width_; ++i)
    {
        PutPixel(X + i, Y, Color);
    }
}

auto FrameBuffer::DrawVerticalLine(uint32_t X, uint32_t Y, uint32_t H, PIXEL Color) -> void
{
    if (!InBounds(X, Y))
    {
        return;
    }

    for (uint32_t i = 0; i < H && Y + i < Height_; ++i)
    {
        PutPixel(X, Y + i, Color);
    }
}