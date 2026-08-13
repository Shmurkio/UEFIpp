#pragma once

#include <UEFIpp/IO/Core/Operations.hpp>

namespace UEFIpp::IO {
class WriterRef final {
public:
  constexpr WriterRef() noexcept = default;

  constexpr WriterRef(const WriterRef &) noexcept = default;
  constexpr WriterRef(WriterRef &&) noexcept = default;
  constexpr auto operator=(const WriterRef &) noexcept -> WriterRef & = default;
  constexpr auto operator=(WriterRef &&) noexcept -> WriterRef & = default;

  template <OutputSink TSink>
    requires(!Foundation::Concepts::Same<
             Foundation::Traits::RemoveCvReferenceType<TSink>, WriterRef>)
  constexpr WriterRef(TSink &Sink) noexcept
      : Object_(&Sink), Write_(&WriteThunk<TSink>), Flush_(&FlushThunk<TSink>) {}

  [[nodiscard]] constexpr auto Valid() const noexcept -> Foundation::Bool {
    return Object_ && Write_;
  }

  [[nodiscard]] constexpr explicit operator Foundation::Bool() const noexcept {
    return Valid();
  }

  [[nodiscard]] auto WriteSome(Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size> {
    if (!Valid()) {
      return Failure(Error::Semantic(ErrorCode::Closed, Operation::Write));
    }

    return Write_(Object_, Bytes);
  }

  [[nodiscard]] auto Flush() -> Result<> {
    if (!Valid()) {
      return Failure(Error::Semantic(ErrorCode::Closed, Operation::Flush));
    }

    return Flush_(Object_);
  }

private:
  using WriteFunction = auto (*)(Foundation::Void *,
                                 Library::Span<const Foundation::Byte>)
      -> Result<Foundation::Size>;
  using FlushFunction = auto (*)(Foundation::Void *) -> Result<>;

  template <typename TSink>
  [[nodiscard]] static auto
  WriteThunk(Foundation::Void *Object,
             Library::Span<const Foundation::Byte> Bytes)
      -> Result<Foundation::Size> {
    return Foundation::Cast::Auto<TSink *>(Object)->WriteSome(Bytes);
  }

  template <typename TSink>
  [[nodiscard]] static auto FlushThunk(Foundation::Void *Object) -> Result<> {
    return IO::Flush(*Foundation::Cast::Auto<TSink *>(Object));
  }

  Foundation::Void *Object_{};
  WriteFunction Write_{};
  FlushFunction Flush_{};
};
} // namespace UEFIpp::IO
