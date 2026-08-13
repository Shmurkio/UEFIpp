#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Types.hpp>

namespace UEFIpp::Protocols {
class GraphicsOutput;

struct GraphicsPixelBitMask final {
  Foundation::Uint32 RedMask;
  Foundation::Uint32 GreenMask;
  Foundation::Uint32 BlueMask;
  Foundation::Uint32 ReservedMask;

  [[nodiscard]] constexpr auto
  operator<=>(const GraphicsPixelBitMask &) const = default;
};

enum class GraphicsPixelFormat : Foundation::Uint32 {
  RedGreenBlueReserved8BitPerColor,
  BlueGreenRedReserved8BitPerColor,
  BitMask,
  BltOnly,
  Maximum
};

struct GraphicsOutputModeInformation final {
  Foundation::Uint32 Version;
  Foundation::Uint32 HorizontalResolution;
  Foundation::Uint32 VerticalResolution;
  GraphicsPixelFormat PixelFormat;
  GraphicsPixelBitMask PixelInformation;
  Foundation::Uint32 PixelsPerScanLine;

  [[nodiscard]] constexpr auto
  operator<=>(const GraphicsOutputModeInformation &) const = default;
};

struct GraphicsOutputProtocolMode final {
  Foundation::Uint32 MaxMode;
  Foundation::Uint32 Mode;
  GraphicsOutputModeInformation *Info;
  Foundation::UintN SizeOfInfo;
  UEFI::PhysicalAddress FrameBufferBase;
  Foundation::UintN FrameBufferSize;

  [[nodiscard]] constexpr auto
  operator<=>(const GraphicsOutputProtocolMode &) const = default;
};

struct GraphicsOutputBltPixel final {
  Foundation::Uint8 Blue;
  Foundation::Uint8 Green;
  Foundation::Uint8 Red;
  Foundation::Uint8 Reserved;

  [[nodiscard]] constexpr auto
  operator<=>(const GraphicsOutputBltPixel &) const = default;
};

enum class GraphicsOutputBltOperation : Foundation::Uint32 {
  VideoFill,
  VideoToBltBuffer,
  BltBufferToVideo,
  VideoToVideo,
  Maximum
};

using GraphicsOutputQueryModeFn = UEFI::StatusValue (*)(
    GraphicsOutput *This, Foundation::Uint32 ModeNumber,
    Foundation::UintN *SizeOfInfo, GraphicsOutputModeInformation **Info);
using GraphicsOutputSetModeFn = UEFI::StatusValue (*)(
    GraphicsOutput *This, Foundation::Uint32 ModeNumber);
using GraphicsOutputBltFn = UEFI::StatusValue (*)(
    GraphicsOutput *This, GraphicsOutputBltPixel *BltBuffer,
    GraphicsOutputBltOperation BltOperation, Foundation::UintN SourceX,
    Foundation::UintN SourceY, Foundation::UintN DestinationX,
    Foundation::UintN DestinationY, Foundation::UintN Width,
    Foundation::UintN Height, Foundation::UintN Delta);

class GraphicsOutput {
public:
  GraphicsOutputQueryModeFn QueryMode;
  GraphicsOutputSetModeFn SetMode;
  GraphicsOutputBltFn Blt;
  GraphicsOutputProtocolMode *Mode;

  // QueryMode allocates Info through firmware. The caller owns that buffer and
  // must release it with BootServices::FreePool.
  [[nodiscard]] auto Query(Foundation::Uint32 ModeNumber,
                           Foundation::UintN &SizeOfInfo,
                           GraphicsOutputModeInformation *&Info)
      -> UEFI::StatusValue;

  [[nodiscard]] auto SetCurrentMode(Foundation::Uint32 ModeNumber)
      -> UEFI::StatusValue;

  [[nodiscard]] auto
  BlockTransfer(GraphicsOutputBltPixel *BltBuffer,
                GraphicsOutputBltOperation Operation,
                Foundation::UintN SourceX, Foundation::UintN SourceY,
                Foundation::UintN DestinationX,
                Foundation::UintN DestinationY, Foundation::UintN Width,
                Foundation::UintN Height, Foundation::UintN Delta = 0)
      -> UEFI::StatusValue;

  [[nodiscard]] constexpr auto CurrentMode() noexcept
      -> GraphicsOutputProtocolMode * {
    return Mode;
  }

  [[nodiscard]] constexpr auto CurrentMode() const noexcept
      -> const GraphicsOutputProtocolMode * {
    return Mode;
  }
};

static_assert(sizeof(GraphicsPixelBitMask) == 16);
static_assert(sizeof(GraphicsPixelFormat) == 4);
static_assert(sizeof(GraphicsOutputModeInformation) == 36);
static_assert(sizeof(GraphicsOutputProtocolMode) == 40);
static_assert(sizeof(GraphicsOutputBltPixel) == 4);
static_assert(sizeof(GraphicsOutputBltOperation) == 4);
static_assert(sizeof(GraphicsOutput) == 32);

static_assert(Foundation::Traits::IsStandardLayout<GraphicsPixelBitMask>::Value);
static_assert(
    Foundation::Traits::IsTriviallyCopyable<GraphicsPixelBitMask>::Value);
static_assert(Foundation::Traits::IsStandardLayout<
              GraphicsOutputModeInformation>::Value);
static_assert(Foundation::Traits::IsTriviallyCopyable<
              GraphicsOutputModeInformation>::Value);
static_assert(Foundation::Traits::IsStandardLayout<
              GraphicsOutputProtocolMode>::Value);
static_assert(Foundation::Traits::IsTriviallyCopyable<
              GraphicsOutputProtocolMode>::Value);
static_assert(
    Foundation::Traits::IsStandardLayout<GraphicsOutputBltPixel>::Value);
static_assert(
    Foundation::Traits::IsTriviallyCopyable<GraphicsOutputBltPixel>::Value);
static_assert(Foundation::Traits::IsStandardLayout<GraphicsOutput>::Value);
static_assert(Foundation::Traits::IsTriviallyCopyable<GraphicsOutput>::Value);
} // namespace UEFIpp::Protocols
