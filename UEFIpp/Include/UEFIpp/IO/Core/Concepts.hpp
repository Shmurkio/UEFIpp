#pragma once

#include <UEFIpp/IO/Core/Error.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>

namespace UEFIpp::IO {
template <typename T>
concept OutputSink = requires(
    T &Sink, Library::Span<const Foundation::Byte> Bytes) {
  { Sink.WriteSome(Bytes) } -> Foundation::Concepts::Same<Result<Foundation::Size>>;
};

template <typename T>
concept InputSource =
    requires(T &Source, Library::Span<Foundation::Byte> Bytes) {
      { Source.ReadSome(Bytes) } -> Foundation::Concepts::Same<Result<ReadCount>>;
    };

template <typename T>
concept Flushable = requires(T &Value) {
  { Value.Flush() } -> Foundation::Concepts::Same<Result<>>;
};

template <typename T>
concept Closable = requires(T &Value) {
  { Value.Close() } -> Foundation::Concepts::Same<Result<>>;
};

template <typename T>
concept Seekable = requires(T &Value, Foundation::Uint64 Position) {
  { Value.Seek(Position) } -> Foundation::Concepts::Same<Result<Foundation::Uint64>>;
  { Value.Tell() } -> Foundation::Concepts::Same<Result<Foundation::Uint64>>;
};

template <typename T>
concept Truncatable = requires(T &Value, Foundation::Uint64 Size) {
  { Value.Truncate(Size) } -> Foundation::Concepts::Same<Result<>>;
};

template <typename T>
concept TerminalOutput = requires(T &Value) {
  { Value.ClearTerminal() } -> Foundation::Concepts::Same<Result<>>;
};
} // namespace UEFIpp::IO
