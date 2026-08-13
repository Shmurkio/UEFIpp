#pragma once

#include <UEFIpp/Library/Containers/Expected.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Memory/MemoryAccess.hpp>

namespace UEFIpp::Memory {
using MemoryViewStatus =
    Library::Expected<Foundation::Void, MemoryViewErrorInfo>;
using MemoryTransferResult =
    Library::Expected<Foundation::Size, MemoryViewErrorInfo>;

template <typename T>
concept MemoryReadableBackend =
    requires(T &Backend, Foundation::Uint64 Address,
             Library::Span<Foundation::Byte> Destination) {
      {
        Backend.ReadPartial(Address, Destination)
      } -> Foundation::Concepts::Same<MemoryTransferResult>;
    };

template <typename T>
concept MemoryWritableBackend =
    requires(T &Backend, Foundation::Uint64 Address,
             Library::Span<const Foundation::Byte> Source) {
      {
        Backend.WritePartial(Address, Source)
      } -> Foundation::Concepts::Same<MemoryTransferResult>;
    };

template <typename T>
concept MemoryProbeBackend =
    requires(T &Backend, Foundation::Uint64 Address, Foundation::Size Size,
             MemoryAccessMask Access) {
      {
        Backend.Probe(Address, Size, Access)
      } -> Foundation::Concepts::Same<MemoryViewStatus>;
    };

// Non-owning, type-erased memory-access capability. The bound backend must
// outlive every copy of the view. Read support is mandatory; write/probe
// support is discovered at compile time and exposed as optional capabilities.
class MemoryView {
public:
  using ReadFunction = auto (*)(Foundation::Void *Context,
                                Foundation::Uint64 Address,
                                Library::Span<Foundation::Byte> Destination)
      -> MemoryTransferResult;

  using WriteFunction = auto (*)(Foundation::Void *Context,
                                 Foundation::Uint64 Address,
                                 Library::Span<const Foundation::Byte> Source)
      -> MemoryTransferResult;

  using ProbeFunction = auto (*)(Foundation::Void *Context,
                                 Foundation::Uint64 Address,
                                 Foundation::Size Size, MemoryAccessMask Access)
      -> MemoryViewStatus;

  constexpr MemoryView() noexcept = default;

  constexpr MemoryView(Foundation::Void *Context, ReadFunction Read,
                       WriteFunction Write = nullptr,
                       ProbeFunction Probe = nullptr) noexcept
      : Context_(Context), Read_(Read), Write_(Write), Probe_(Probe) {}

  template <MemoryReadableBackend Backend>
    requires(!Foundation::Concepts::Same<
                Foundation::Traits::RemoveCvReferenceType<Backend>, MemoryView>)
  constexpr explicit MemoryView(Backend &Instance) noexcept
      : Context_(Foundation::Cast::Auto<Foundation::Void *>(&Instance)),
        Read_(&ReadThunk<Backend>) {
    if constexpr (MemoryWritableBackend<Backend>) {
      Write_ = &WriteThunk<Backend>;
    }

    if constexpr (MemoryProbeBackend<Backend>) {
      Probe_ = &ProbeThunk<Backend>;
    }
  }

  template <MemoryReadableBackend Backend>
  [[nodiscard]] static constexpr auto From(Backend &Instance) noexcept
      -> MemoryView {
    return MemoryView{Instance};
  }

  [[nodiscard]] constexpr auto IsValid() const noexcept -> Foundation::Bool {
    return Read_ != nullptr;
  }

  [[nodiscard]] constexpr explicit operator Foundation::Bool() const noexcept {
    return IsValid();
  }

  [[nodiscard]] constexpr auto CanRead() const noexcept -> Foundation::Bool {
    return Read_ != nullptr;
  }

  [[nodiscard]] constexpr auto CanWrite() const noexcept -> Foundation::Bool {
    return Write_ != nullptr;
  }

  [[nodiscard]] constexpr auto CanProbe() const noexcept -> Foundation::Bool {
    return Probe_ != nullptr;
  }

  [[nodiscard]] constexpr auto Context() const noexcept -> Foundation::Void * {
    return Context_;
  }

  [[nodiscard]] auto
  ReadPartial(Foundation::Uint64 Address,
              Library::Span<Foundation::Byte> Destination) const
      -> MemoryTransferResult;

  [[nodiscard]] auto Read(Foundation::Uint64 Address,
                          Library::Span<Foundation::Byte> Destination) const
      -> MemoryViewStatus;

  [[nodiscard]] auto
  WritePartial(Foundation::Uint64 Address,
               Library::Span<const Foundation::Byte> Source) const
      -> MemoryTransferResult;

  [[nodiscard]] auto Write(Foundation::Uint64 Address,
                           Library::Span<const Foundation::Byte> Source) const
      -> MemoryViewStatus;

  [[nodiscard]] auto Probe(Foundation::Uint64 Address, Foundation::Size Size,
                           MemoryAccessMask Access) const -> MemoryViewStatus;

  [[nodiscard]] auto Probe(Foundation::Uint64 Address, Foundation::Size Size,
                           MemoryAccess Access) const -> MemoryViewStatus;

  template <Foundation::Concepts::TriviallyCopyable T>
  [[nodiscard]] auto ReadObject(Foundation::Uint64 Address) const
      -> Library::Expected<T, MemoryViewErrorInfo> {
    T Result{};
    auto Status =
        Read(Address, Library::Span<Foundation::Byte>{
                          Foundation::Cast::Pointer<Foundation::Byte>(&Result),
                          sizeof(T)});

    if (!Status) {
      return Library::Expected<T, MemoryViewErrorInfo>{Library::Unexpect,
                                                       Status.Error()};
    }

    return Result;
  }

  template <Foundation::Concepts::TriviallyCopyable T>
  [[nodiscard]] auto WriteObject(Foundation::Uint64 Address,
                                 const T &Value) const -> MemoryViewStatus {
    return Write(Address,
                 Library::Span<const Foundation::Byte>{
                     Foundation::Cast::Pointer<const Foundation::Byte>(&Value),
                     sizeof(T)});
  }

  template <Foundation::Concepts::TriviallyCopyable T>
  [[nodiscard]] auto ReadArray(Foundation::Uint64 Address,
                               Library::Span<T> Destination) const
      -> MemoryViewStatus {
    if (Destination.Empty()) {
      return FailureStatus(MemoryViewError::InvalidRange,
                           MemoryViewOperation::Read, Address, 0);
    }

    return Read(Address, Library::Span<Foundation::Byte>{
                             Foundation::Cast::Pointer<Foundation::Byte>(
                                 Destination.Data()),
                             Destination.SizeInBytes()});
  }

  template <Foundation::Concepts::TriviallyCopyable T>
  [[nodiscard]] auto WriteArray(Foundation::Uint64 Address,
                                Library::Span<const T> Source) const
      -> MemoryViewStatus {
    if (Source.Empty()) {
      return FailureStatus(MemoryViewError::InvalidRange,
                           MemoryViewOperation::Write, Address, 0);
    }

    return Write(Address, Library::Span<const Foundation::Byte>{
                              Foundation::Cast::Pointer<const Foundation::Byte>(
                                  Source.Data()),
                              Source.SizeInBytes()});
  }

  [[nodiscard]] auto ReadAddress(Foundation::Uint64 Address) const
      -> Library::Expected<Foundation::Uint64, MemoryViewErrorInfo>;

  constexpr auto Reset() noexcept -> Foundation::Void {
    Context_ = nullptr;
    Read_ = nullptr;
    Write_ = nullptr;
    Probe_ = nullptr;
  }

private:
  [[nodiscard]] static constexpr auto
  MakeError(MemoryViewError Code, MemoryViewOperation Operation,
            Foundation::Uint64 Address, Foundation::Size Size,
            Foundation::Size BytesTransferred = 0) noexcept
      -> MemoryViewErrorInfo {
    return MemoryViewErrorInfo{.Code = Code,
                               .Operation = Operation,
                               .Address = Address,
                               .Size = Size,
                               .BytesTransferred = BytesTransferred};
  }

  [[nodiscard]] static auto
  FailureStatus(MemoryViewError Code, MemoryViewOperation Operation,
                Foundation::Uint64 Address, Foundation::Size Size,
                Foundation::Size BytesTransferred = 0) -> MemoryViewStatus;

  [[nodiscard]] static auto
  FailureTransfer(MemoryViewError Code, MemoryViewOperation Operation,
                  Foundation::Uint64 Address, Foundation::Size Size,
                  Foundation::Size BytesTransferred = 0)
      -> MemoryTransferResult;

  template <typename Backend>
  [[nodiscard]] static auto
  ReadThunk(Foundation::Void *Context, Foundation::Uint64 Address,
            Library::Span<Foundation::Byte> Destination)
      -> MemoryTransferResult {
    auto *Instance = Foundation::Cast::Auto<Backend *>(Context);
    return Instance->ReadPartial(Address, Destination);
  }

  template <typename Backend>
  [[nodiscard]] static auto
  WriteThunk(Foundation::Void *Context, Foundation::Uint64 Address,
             Library::Span<const Foundation::Byte> Source)
      -> MemoryTransferResult {
    auto *Instance = Foundation::Cast::Auto<Backend *>(Context);
    return Instance->WritePartial(Address, Source);
  }

  template <typename Backend>
  [[nodiscard]] static auto
  ProbeThunk(Foundation::Void *Context, Foundation::Uint64 Address,
             Foundation::Size Size, MemoryAccessMask Access)
      -> MemoryViewStatus {
    auto *Instance = Foundation::Cast::Auto<Backend *>(Context);
    return Instance->Probe(Address, Size, Access);
  }

  Foundation::Void *Context_{};
  ReadFunction Read_{};
  WriteFunction Write_{};
  ProbeFunction Probe_{};
};

static_assert(sizeof(MemoryView) == sizeof(Foundation::Void *) * 4);
static_assert(Foundation::Traits::IsStandardLayout<MemoryView>::Value);
static_assert(Foundation::Traits::IsTriviallyCopyable<MemoryView>::Value);
} // namespace UEFIpp::Memory
