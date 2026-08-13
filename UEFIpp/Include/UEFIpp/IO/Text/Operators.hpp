#pragma once

#include <UEFIpp/IO/Text/Format.hpp>
#include <UEFIpp/IO/Text/Scan.hpp>

namespace UEFIpp::IO {
namespace Detail {
template <typename TValue>
class OutputCapture final {
public:
  using ValueType = Foundation::Traits::RemoveCvReferenceType<TValue>;

  constexpr explicit OutputCapture(TValue &&Value)
      : Value_(Foundation::Utility::Forward<TValue>(Value)) {}

  [[nodiscard]] constexpr auto Get() const noexcept -> const ValueType & {
    return Value_;
  }

private:
  ValueType Value_;
};

template <typename TValue>
class OutputCapture<TValue &> final {
public:
  using ValueType =
      Foundation::Traits::RemoveCvReferenceType<TValue>;

  constexpr explicit OutputCapture(TValue &Value) noexcept : Value_(&Value) {}

  [[nodiscard]] constexpr auto Get() const noexcept -> TValue & {
    return *Value_;
  }

private:
  TValue *Value_{};
};
} // namespace Detail

template <typename TValue>
class FormattedValue final {
public:
  using ValueType = Foundation::Traits::RemoveCvReferenceType<TValue>;

  constexpr FormattedValue(FormatString<ValueType> Format, TValue &&Value)
      : Format_(Format), Value_(Foundation::Utility::Forward<TValue>(Value)) {}

  [[nodiscard]] auto WriteTo(WriterRef Writer) const -> Result<> {
    return Print(Writer, Format_, Value_.Get());
  }

private:
  FormatString<ValueType> Format_;
  Detail::OutputCapture<TValue> Value_;
};

template <typename TValue>
[[nodiscard]] constexpr auto
Formatted(FormatString<Foundation::Traits::RemoveCvReferenceType<TValue>> Format,
          TValue &&Value) -> FormattedValue<TValue> {
  return {Format, Foundation::Utility::Forward<TValue>(Value)};
}

template <typename TValue>
[[nodiscard]] constexpr auto Quoted(TValue &&Value) -> FormattedValue<TValue> {
  using ValueType = Foundation::Traits::RemoveCvReferenceType<TValue>;
  return {FormatString<ValueType>{"{:?}"},
          Foundation::Utility::Forward<TValue>(Value)};
}

struct NewlineType final {
  [[nodiscard]] auto WriteTo(WriterRef Writer) const -> Result<> {
    const Foundation::Byte LineFeed{'\n'};
    return WriteAll(Writer, {&LineFeed, 1});
  }
};

inline constexpr NewlineType Newline{};

class [[nodiscard]] OutputChain final {
public:
  constexpr explicit OutputChain(WriterRef Writer) noexcept
      : Writer_(Foundation::Utility::Move(Writer)) {}

  OutputChain(const OutputChain &) = delete;
  auto operator=(const OutputChain &) -> OutputChain & = delete;
  OutputChain(OutputChain &&) = default;
  auto operator=(OutputChain &&) -> OutputChain & = default;

  template <typename TValue>
  [[nodiscard]] auto operator<<(TValue &&Value) & -> OutputChain & {
    Append(Foundation::Utility::Forward<TValue>(Value));
    return *this;
  }

  template <typename TValue>
  [[nodiscard]] auto operator<<(TValue &&Value) && -> OutputChain && {
    Append(Foundation::Utility::Forward<TValue>(Value));
    return Foundation::Utility::Move(*this);
  }

  [[nodiscard]] auto HasValue() const -> Foundation::Bool {
    return Status_.HasValue();
  }

  [[nodiscard]] explicit operator Foundation::Bool() const {
    return HasValue();
  }

  auto Value() const -> Foundation::Void { Status_.Value(); }

  [[nodiscard]] auto Error() & -> UEFIpp::IO::Error & {
    return Status_.Error();
  }

  [[nodiscard]] auto Error() const & -> const UEFIpp::IO::Error & {
    return Status_.Error();
  }

  [[nodiscard]] auto Error() && -> UEFIpp::IO::Error && {
    return Foundation::Utility::Move(Status_).Error();
  }

  [[nodiscard]] auto Status() const & -> const Result<> & { return Status_; }

  [[nodiscard]] auto TakeResult() && -> Result<> {
    return Foundation::Utility::Move(Status_);
  }

private:
  template <typename TValue>
  auto Append(TValue &&Value) -> Foundation::Void {
    if (!Status_) return;

    using ValueType = Foundation::Traits::RemoveCvReferenceType<TValue>;
    if constexpr (requires(const ValueType &Fragment, WriterRef Writer) {
                    { Fragment.WriteTo(Writer) }
                        -> Foundation::Concepts::Same<Result<>>;
                  }) {
      Status_ = Value.WriteTo(Writer_);
    } else {
      Status_ = Formatter<ValueType>{}.Format(
          Writer_, Foundation::Utility::Forward<TValue>(Value), {});
    }
  }

  WriterRef Writer_{};
  Result<> Status_{};
};

template <OutputSink TSink>
[[nodiscard]] constexpr auto Out(TSink &Sink) noexcept -> OutputChain {
  return OutputChain{WriterRef{Sink}};
}

template <typename... TValues>
struct ReadRequest final {
  ParseOptions Options{};

  [[nodiscard]] constexpr auto With(ParseOptions Value) const noexcept
      -> ReadRequest {
    return {Value};
  }
};

template <typename TFirst, typename... TRest>
inline constexpr ReadRequest<TFirst, TRest...> Read{};

template <InputSource TSource, Foundation::Size Capacity, typename TValue>
[[nodiscard]] auto operator>>(TextReader<TSource, Capacity> &Reader,
                              ReadRequest<TValue> Request) -> Result<TValue> {
  if constexpr (Seekable<TSource>) {
    auto Checkpoint = Reader.Save();
    if (!Checkpoint) return Failure(Checkpoint.Error());
    auto Value = Scan<TValue>(Reader, Request.Options);
    if (!Value) {
      auto Restored = Reader.Restore(Checkpoint.Value());
      if (!Restored) return Failure(Restored.Error());
      return Failure(Value.Error());
    }
    return Value;
  } else {
    return Scan<TValue>(Reader, Request.Options);
  }
}

template <InputSource TSource, Foundation::Size Capacity, typename TFirst,
          typename TSecond, typename... TRest>
[[nodiscard]] auto
operator>>(TextReader<TSource, Capacity> &Reader,
           ReadRequest<TFirst, TSecond, TRest...> Request)
    -> Result<Library::Tuple<TFirst, TSecond, TRest...>> {
  return Scan<TFirst, TSecond, TRest...>(Reader, Request.Options);
}

template <typename... TValues>
class IntoRequest final {
public:
  constexpr explicit IntoRequest(TValues &...Values) noexcept
      : Destinations_(&Values...) {}

  [[nodiscard]] constexpr auto With(ParseOptions Value) const noexcept
      -> IntoRequest {
    auto Copy = *this;
    Copy.Options_ = Value;
    return Copy;
  }

  [[nodiscard]] constexpr auto Destinations() const noexcept
      -> const Library::Tuple<TValues *...> & {
    return Destinations_;
  }

  [[nodiscard]] constexpr auto Options() const noexcept -> ParseOptions {
    return Options_;
  }

private:
  Library::Tuple<TValues *...> Destinations_;
  ParseOptions Options_{};
};

template <typename... TValues>
  requires(sizeof...(TValues) > 0 &&
           (!Foundation::Traits::IsConst<TValues>::Value && ...))
[[nodiscard]] constexpr auto Into(TValues &...Values) noexcept
    -> IntoRequest<TValues...> {
  return IntoRequest<TValues...>{Values...};
}

namespace Detail {
template <Foundation::Size Index = 0, typename... TDestinations,
          typename... TValues>
auto CommitInto(const IntoRequest<TDestinations...> &Request,
                Library::Tuple<TValues...> &Values) -> Foundation::Void {
  *Library::Get<Index>(Request.Destinations()) =
      Foundation::Utility::Move(Library::Get<Index>(Values));
  if constexpr (Index + 1 < sizeof...(TValues)) {
    CommitInto<Index + 1>(Request, Values);
  }
}
} // namespace Detail

template <InputSource TSource, Foundation::Size Capacity, typename TValue>
[[nodiscard]] auto operator>>(TextReader<TSource, Capacity> &Reader,
                              const IntoRequest<TValue> &Request) -> Result<> {
  auto Value = Reader >> Read<TValue>.With(Request.Options());
  if (!Value) return Failure(Value.Error());
  *Library::Get<0>(Request.Destinations()) =
      Foundation::Utility::Move(Value.Value());
  return {};
}

template <InputSource TSource, Foundation::Size Capacity, typename TFirst,
          typename TSecond, typename... TRest>
[[nodiscard]] auto
operator>>(TextReader<TSource, Capacity> &Reader,
           const IntoRequest<TFirst, TSecond, TRest...> &Request) -> Result<> {
  auto Values =
      Reader >> Read<TFirst, TSecond, TRest...>.With(Request.Options());
  if (!Values) return Failure(Values.Error());
  Detail::CommitInto(Request, Values.Value());
  return {};
}
} // namespace UEFIpp::IO
