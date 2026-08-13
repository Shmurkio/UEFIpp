#pragma once

#include <UEFIpp/IO/Text/Format.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>

namespace UEFIpp::IO {
enum class Severity : Foundation::Uint8 {
  Trace,
  Debug,
  Info,
  Warning,
  Error,
  Critical,
  Off
};

struct LogRecord final {
  Severity Level{Severity::Info};
  Library::U8StringView Module{};
  Foundation::SourceLocation Source{nullptr, nullptr, 0};
  Foundation::Uint64 Timestamp{};
  Library::U8StringView Message{};
};

template <typename T>
struct NamedField final {
  Library::U8StringView Name;
  const T &Value;
};

template <typename T>
[[nodiscard]] constexpr auto Field(Library::U8StringView Name,
                                   const T &Value) noexcept -> NamedField<T> {
  return {Name, Value};
}

class Logger final {
public:
  constexpr explicit Logger(Memory::AllocatorStub Allocator = {}) noexcept
      : Sinks_(Allocator), Allocator_(Allocator) {}

  [[nodiscard]] auto AddSink(WriterRef Sink) -> Foundation::Bool {
    return Sink && Sinks_.PushBack(Sink);
  }
  auto ClearSinks() -> Foundation::Void { Sinks_.Clear(); }
  constexpr auto SetMinimumSeverity(Severity Value) noexcept
      -> Foundation::Void {
    Minimum_ = Value;
  }
  constexpr auto SetModule(Library::U8StringView Value) noexcept
      -> Foundation::Void {
    Module_ = Value;
  }
  constexpr auto SetTimestampSource(Foundation::Void *Context,
                                    auto (*Function)(Foundation::Void *)
                                        -> Foundation::Uint64) noexcept
      -> Foundation::Void {
    TimestampContext_ = Context;
    Timestamp_ = Function;
  }

  template <typename... TArguments>
  [[nodiscard]] auto Log(
      Severity Level, Foundation::SourceLocation Source,
      FormatString<Foundation::Traits::RemoveCvReferenceType<TArguments>...>
          FormatText,
      TArguments &&...Arguments) -> Result<> {
    if (Level < Minimum_ || Minimum_ == Severity::Off) return {};

    MemorySink Message{Allocator_};
    auto Result = Print(
        Message, FormatText,
        Foundation::Utility::Forward<TArguments>(Arguments)...);
    if (!Result) return Result;
    return WriteRecord(
        {Level, Module_, Source, Timestamp_ ? Timestamp_(TimestampContext_) : 0,
         {Foundation::Cast::Auto<const Foundation::Char8 *>(
              Message.Buffer().Data()),
          Message.Buffer().Size()}},
        {});
  }

  template <typename... TFields>
  [[nodiscard]] auto LogFields(Severity Level,
                               Foundation::SourceLocation Source,
                               Library::U8StringView Message,
                               const TFields &...Fields) -> Result<> {
    if (Level < Minimum_ || Minimum_ == Severity::Off) return {};
    MemorySink Encoded{Allocator_};
    WriterRef Writer{Encoded};
    auto Result = AppendFields(Writer, Fields...);
    if (!Result) return Result;
    return WriteRecord(
        {Level, Module_, Source, Timestamp_ ? Timestamp_(TimestampContext_) : 0,
         Message},
        {Foundation::Cast::Auto<const Foundation::Char8 *>(
             Encoded.Buffer().Data()),
         Encoded.Buffer().Size()});
  }

  template <Severity Level, typename... TArguments>
  [[nodiscard]] auto Log(
      Foundation::SourceLocation Source,
      FormatString<Foundation::Traits::RemoveCvReferenceType<TArguments>...>
          FormatText,
      TArguments &&...Arguments) -> Result<> {
#if defined(UEFIPP_LOG_COMPILETIME_MINIMUM)
    if constexpr (Level < UEFIPP_LOG_COMPILETIME_MINIMUM) return {};
#endif
    return Log(Level, Source, FormatText,
               Foundation::Utility::Forward<TArguments>(Arguments)...);
  }

private:
  [[nodiscard]] auto WriteRecord(const LogRecord &Record,
                                 Library::U8StringView EncodedFields)
      -> Result<>;

  [[nodiscard]] static constexpr auto SeverityName(Severity Value)
      -> Library::U8StringView {
    switch (Value) {
    case Severity::Trace: return u8"trace";
    case Severity::Debug: return u8"debug";
    case Severity::Info: return u8"info";
    case Severity::Warning: return u8"warning";
    case Severity::Error: return u8"error";
    case Severity::Critical: return u8"critical";
    case Severity::Off: return u8"off";
    }
    return u8"unknown";
  }

  [[nodiscard]] static auto AppendFields(WriterRef) -> Result<> { return {}; }

  template <typename TFirst, typename... TRest>
  [[nodiscard]] static auto AppendFields(WriterRef Writer,
                                         const TFirst &First,
                                         const TRest &...Rest) -> Result<> {
    auto Result = Detail::WriteBytes(Writer, u8" ", 1);
    if (!Result) return Result;
    Result = WriteText(Writer, First.Name);
    if (!Result) return Result;
    Result = Detail::WriteBytes(Writer, u8"=", 1);
    if (!Result) return Result;
    FormatSpec Spec{};
    Spec.Debug = true;
    using Type = Foundation::Traits::RemoveCvReferenceType<decltype(First.Value)>;
    Result = Formatter<Type>{}.Format(Writer, First.Value, Spec);
    if (!Result) return Result;
    return AppendFields(Writer, Rest...);
  }

  Library::Vector<WriterRef> Sinks_{};
  Memory::AllocatorStub Allocator_{};
  Library::U8StringView Module_{u8"UEFIpp"};
  Severity Minimum_{Severity::Info};
  Foundation::Void *TimestampContext_{};
  auto (*Timestamp_)(Foundation::Void *) -> Foundation::Uint64{};
  Foundation::SpinLock Lock_{};
};

class PanicWriter final {
public:
  static auto Configure(WriterRef Writer) noexcept -> Foundation::Void;
  static auto Write(Library::U8StringView Message) noexcept -> Foundation::Void;

private:
  inline static WriterRef Writer_{};
};
} // namespace UEFIpp::IO

#define UEFIPP_LOG(Logger, Level, ...)                                         \
  (void)(Logger).template Log<Level>(UEFIPP_SOURCE_LOCATION(), __VA_ARGS__)
