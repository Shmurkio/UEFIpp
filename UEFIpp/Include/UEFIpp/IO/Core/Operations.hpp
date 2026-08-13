#pragma once

#include <UEFIpp/IO/Core/Concepts.hpp>

namespace UEFIpp::IO {
template <OutputSink TSink>
[[nodiscard]] auto WriteAll(
    TSink &Sink, Library::Span<const Foundation::Byte> Bytes) -> Result<> {
  Foundation::Size Written{};

  while (Written < Bytes.Size()) {
    auto Result = Sink.WriteSome(Bytes.Subspan(Written));

    if (!Result) {
      auto Error = Result.Error();
      Error.Transferred += Written;
      return Failure(Error);
    }

    if (!Result.Value()) {
      return Failure(Error::Semantic(ErrorCode::NoProgress, Operation::Write,
                                     Written, Written));
    }

    Written += Result.Value();
  }

  return {};
}

template <InputSource TSource>
[[nodiscard]] auto ReadExact(
    TSource &Source, Library::Span<Foundation::Byte> Bytes) -> Result<> {
  Foundation::Size Read{};

  while (Read < Bytes.Size()) {
    auto Result = Source.ReadSome(Bytes.Subspan(Read));

    if (!Result) {
      auto Error = Result.Error();
      Error.Transferred += Read;
      return Failure(Error);
    }

    Read += Result.Value().Count;

    if (Result.Value().End && Read < Bytes.Size()) {
      return Failure(Error::Semantic(ErrorCode::UnexpectedEndOfFile,
                                     Operation::Read, Read, Read));
    }

    if (!Result.Value().Count) {
      return Failure(Error::Semantic(ErrorCode::NoProgress, Operation::Read,
                                     Read, Read));
    }
  }

  return {};
}

struct FlushType final {
  template <typename T>
  [[nodiscard]] auto operator()(T &Value) const -> Result<> {
    if constexpr (Flushable<T>) {
      return Value.Flush();
    } else {
      return {};
    }
  }

  template <OutputSink TWriter>
  [[nodiscard]] auto WriteTo(TWriter &Writer) const -> Result<> {
    return (*this)(Writer);
  }
};

inline constexpr FlushType Flush{};

template <typename T>
[[nodiscard]] auto Close(T &Value) -> Result<> {
  if constexpr (Closable<T>) {
    return Value.Close();
  } else {
    return {};
  }
}
} // namespace UEFIpp::IO
