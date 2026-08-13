#pragma once

#include <UEFIpp/IO/Core/Concepts.hpp>

namespace UEFIpp::IO {
class ReaderRef final {
public:
  constexpr ReaderRef() noexcept = default;

  constexpr ReaderRef(const ReaderRef &) noexcept = default;
  constexpr ReaderRef(ReaderRef &&) noexcept = default;
  constexpr auto operator=(const ReaderRef &) noexcept -> ReaderRef & = default;
  constexpr auto operator=(ReaderRef &&) noexcept -> ReaderRef & = default;

  template <InputSource TSource>
    requires(!Foundation::Concepts::Same<
             Foundation::Traits::RemoveCvReferenceType<TSource>, ReaderRef>)
  constexpr ReaderRef(TSource &Source) noexcept
      : Object_(&Source), Read_(&ReadThunk<TSource>) {}

  [[nodiscard]] constexpr auto Valid() const noexcept -> Foundation::Bool {
    return Object_ && Read_;
  }

  [[nodiscard]] constexpr explicit operator Foundation::Bool() const noexcept {
    return Valid();
  }

  [[nodiscard]] auto ReadSome(Library::Span<Foundation::Byte> Bytes)
      -> Result<ReadCount> {
    if (!Valid()) {
      return Failure(Error::Semantic(ErrorCode::Closed, Operation::Read));
    }

    return Read_(Object_, Bytes);
  }

private:
  using ReadFunction = auto (*)(Foundation::Void *,
                                Library::Span<Foundation::Byte>)
      -> Result<ReadCount>;

  template <typename TSource>
  [[nodiscard]] static auto
  ReadThunk(Foundation::Void *Object, Library::Span<Foundation::Byte> Bytes)
      -> Result<ReadCount> {
    return Foundation::Cast::Auto<TSource *>(Object)->ReadSome(Bytes);
  }

  Foundation::Void *Object_{};
  ReadFunction Read_{};
};
} // namespace UEFIpp::IO
