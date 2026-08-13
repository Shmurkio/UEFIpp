#include <UEFIpp/Memory/MemoryView.hpp>

namespace UEFIpp::Memory {
auto MemoryView::ReadPartial(Foundation::Uint64 Address,
                             Library::Span<Foundation::Byte> Destination) const
    -> MemoryTransferResult {
  if (!Read_) {
    return FailureTransfer(MemoryViewError::ReadUnsupported,
                           MemoryViewOperation::Read, Address,
                           Destination.Size());
  }

  if (Destination.Empty() || !Destination.Data()) {
    return FailureTransfer(MemoryViewError::InvalidRange,
                           MemoryViewOperation::Read, Address,
                           Destination.Size());
  }

  if (!IsAddressRangeValid(Address, Destination.Size())) {
    return FailureTransfer(MemoryViewError::InvalidRange,
                           MemoryViewOperation::Read, Address,
                           Destination.Size());
  }

  return Read_(Context_, Address, Destination);
}

auto MemoryView::Read(Foundation::Uint64 Address,
                      Library::Span<Foundation::Byte> Destination) const
    -> MemoryViewStatus {
  auto Result = ReadPartial(Address, Destination);

  if (!Result) {
    return MemoryViewStatus{Library::Unexpect, Result.Error()};
  }

  if (Result.Value() != Destination.Size()) {
    return FailureStatus(MemoryViewError::PartialAccess,
                         MemoryViewOperation::Read, Address, Destination.Size(),
                         Result.Value());
  }

  return {};
}

auto MemoryView::WritePartial(Foundation::Uint64 Address,
                              Library::Span<const Foundation::Byte> Source)
    const -> MemoryTransferResult {
  if (!Write_) {
    return FailureTransfer(MemoryViewError::WriteUnsupported,
                           MemoryViewOperation::Write, Address, Source.Size());
  }

  if (Source.Empty() || !Source.Data()) {
    return FailureTransfer(MemoryViewError::InvalidRange,
                           MemoryViewOperation::Write, Address, Source.Size());
  }

  if (!IsAddressRangeValid(Address, Source.Size())) {
    return FailureTransfer(MemoryViewError::InvalidRange,
                           MemoryViewOperation::Write, Address, Source.Size());
  }

  return Write_(Context_, Address, Source);
}

auto MemoryView::Write(Foundation::Uint64 Address,
                       Library::Span<const Foundation::Byte> Source) const
    -> MemoryViewStatus {
  auto Result = WritePartial(Address, Source);

  if (!Result) {
    return MemoryViewStatus{Library::Unexpect, Result.Error()};
  }

  if (Result.Value() != Source.Size()) {
    return FailureStatus(MemoryViewError::PartialAccess,
                         MemoryViewOperation::Write, Address, Source.Size(),
                         Result.Value());
  }

  return {};
}

auto MemoryView::Probe(Foundation::Uint64 Address, Foundation::Size Size,
                       MemoryAccessMask Access) const -> MemoryViewStatus {
  if (!Probe_) {
    return FailureStatus(MemoryViewError::ProbeUnsupported,
                         MemoryViewOperation::Probe, Address, Size);
  }

  if (!IsAddressRangeValid(Address, Size)) {
    return FailureStatus(MemoryViewError::InvalidRange,
                         MemoryViewOperation::Probe, Address, Size);
  }

  return Probe_(Context_, Address, Size, Access);
}

auto MemoryView::Probe(Foundation::Uint64 Address, Foundation::Size Size,
                       MemoryAccess Access) const -> MemoryViewStatus {
  return Probe(Address, Size, static_cast<MemoryAccessMask>(Access));
}

auto MemoryView::ReadAddress(Foundation::Uint64 Address) const
    -> Library::Expected<Foundation::Uint64, MemoryViewErrorInfo> {
  return ReadObject<Foundation::Uint64>(Address);
}

auto MemoryView::FailureStatus(MemoryViewError Code,
                               MemoryViewOperation Operation,
                               Foundation::Uint64 Address,
                               Foundation::Size Size,
                               Foundation::Size BytesTransferred)
    -> MemoryViewStatus {
  return MemoryViewStatus{Library::Unexpect, MakeError(Code, Operation, Address,
                                                       Size, BytesTransferred)};
}

auto MemoryView::FailureTransfer(MemoryViewError Code,
                                 MemoryViewOperation Operation,
                                 Foundation::Uint64 Address,
                                 Foundation::Size Size,
                                 Foundation::Size BytesTransferred)
    -> MemoryTransferResult {
  return MemoryTransferResult{
      Library::Unexpect,
      MakeError(Code, Operation, Address, Size, BytesTransferred)};
}
} // namespace UEFIpp::Memory
