#include <UEFIpp/Protocols/GraphicsOutput.hpp>

namespace UEFIpp::Protocols {
auto GraphicsOutput::Query(Foundation::Uint32 ModeNumber,
                           Foundation::UintN &SizeOfInfo,
                           GraphicsOutputModeInformation *&Info)
    -> UEFI::StatusValue {
  return QueryMode(this, ModeNumber, &SizeOfInfo, &Info);
}

auto GraphicsOutput::SetCurrentMode(Foundation::Uint32 ModeNumber)
    -> UEFI::StatusValue {
  return SetMode(this, ModeNumber);
}

auto GraphicsOutput::BlockTransfer(
    GraphicsOutputBltPixel *BltBuffer, GraphicsOutputBltOperation Operation,
    Foundation::UintN SourceX, Foundation::UintN SourceY,
    Foundation::UintN DestinationX, Foundation::UintN DestinationY,
    Foundation::UintN Width, Foundation::UintN Height,
    Foundation::UintN Delta) -> UEFI::StatusValue {
  return Blt(this, BltBuffer, Operation, SourceX, SourceY, DestinationX,
             DestinationY, Width, Height, Delta);
}
} // namespace UEFIpp::Protocols
