#include <UEFIpp/IO/Logging/Logger.hpp>

namespace UEFIpp::IO {
auto Logger::WriteRecord(const LogRecord &Record,
                         Library::U8StringView EncodedFields) -> Result<> {
  MemorySink Buffer{Allocator_};
  WriterRef Writer{Buffer};
  auto Result = Detail::WriteBytes(Writer, u8"[", 1);
  if (!Result) return Result;
  Result = WriteText(Writer, SeverityName(Record.Level));
  if (!Result) return Result;
  Result = Detail::WriteBytes(Writer, u8"][", 2);
  if (!Result) return Result;
  Result = WriteText(Writer, Record.Module);
  if (!Result) return Result;
  Result = Detail::WriteBytes(Writer, u8"]", 1);
  if (!Result) return Result;
  if (Record.Timestamp) {
    Result = Detail::WriteBytes(Writer, u8"[", 1);
    if (!Result) return Result;
    Result = Detail::FormatUnsigned(Writer, Record.Timestamp, false, {});
    if (!Result) return Result;
    Result = Detail::WriteBytes(Writer, u8"]", 1);
    if (!Result) return Result;
  }
  Result = Detail::WriteBytes(Writer, u8"[", 1);
  if (!Result) return Result;
  Result = Detail::WriteAscii(Writer, Record.Source.File,
                              Record.Source.File
                                  ? Library::StringView{Record.Source.File}.Size()
                                  : 0);
  if (!Result) return Result;
  Result = Detail::WriteBytes(Writer, u8":", 1);
  if (!Result) return Result;
  Result = Detail::FormatUnsigned(Writer, Record.Source.Line, false, {});
  if (!Result) return Result;
  Result = Detail::WriteBytes(Writer, u8"] ", 2);
  if (!Result) return Result;
  Result = WriteText(Writer, Record.Message);
  if (!Result) return Result;
  Result = WriteText(Writer, EncodedFields);
  if (!Result) return Result;
  Result = Detail::WriteBytes(Writer, u8"\n", 1);
  if (!Result) return Result;

  Foundation::SpinLockGuard Guard{Lock_};
  const Library::Span<const Foundation::Byte> Bytes{Buffer.Buffer().Data(),
                                                    Buffer.Buffer().Size()};
  Foundation::Bool Failed{};
  Error FirstError{};
  for (auto &Sink : Sinks_) {
    Result = WriteAll(Sink, Bytes);
    if (!Result) {
      if (!Failed) FirstError = Result.Error();
      Failed = true;
      continue;
    }
    Result = Sink.Flush();
    if (!Result && !Failed) {
      FirstError = Result.Error();
      Failed = true;
    }
  }
  if (Failed) return Failure(FirstError);
  return {};
}

auto PanicWriter::Configure(WriterRef Writer) noexcept -> Foundation::Void {
  Writer_ = Writer;
}

auto PanicWriter::Write(Library::U8StringView Message) noexcept
    -> Foundation::Void {
  if (!Writer_) return;
  (void)WriteText(Writer_, Message);
  (void)Writer_.Flush();
}
} // namespace UEFIpp::IO
