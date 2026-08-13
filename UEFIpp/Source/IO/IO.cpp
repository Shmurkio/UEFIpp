#include <UEFIpp/IO/IO.hpp>

namespace UEFIpp::IO {
static_assert(OutputSink<NullSink>);
static_assert(OutputSink<MemorySink>);
static_assert(InputSource<MemorySource>);
static_assert(Flushable<ConsoleSink>);
static_assert(Seekable<FileSource>);
static_assert(Truncatable<FileSink>);
static_assert(TerminalOutput<ConsoleSink>);

namespace {
[[maybe_unused]] auto InstantiateModernIoTemplates() -> Foundation::Void {
  NullSink Sink{};
  Library::Vector<Foundation::Uint32> Range{};
  (void)Range.PushBack(7);
  const auto Tuple = Library::MakeTuple(Foundation::Uint32{1}, u8"two");
  const Foundation::Byte Bytes[]{0x41, 0x00, 0xFF};
  const Error IoError = Error::Semantic(ErrorCode::Timeout, Operation::Wait,
                                        12, 3);
  (void)Print(Sink, "{} {} {:?} {} {} {:#010X} {:.3f}", Range, Tuple,
              Library::U8StringView{u8"escaped\ntext"},
              HexDump{{Bytes, sizeof(Bytes)}}, IoError,
              Foundation::Uint32{0xBEEF}, Foundation::Float64{3.14159});
  const Foundation::Char8 *Utf8Pointer = u8"pointer text";
  (void)Print(Sink, "{} {} {} {:a}", Utf8Pointer, u"UTF-16 \U0001F680",
              U"UTF-32 \U0001F680", Foundation::Float64{0.0});
  const Result<Foundation::Uint32> ExpectedValue{Foundation::Uint32{5}};
  (void)Print(Sink, "{} {}", nullptr, ExpectedValue);

  const Foundation::Uint32 RuntimeValue{42};
  const FormatArgument RuntimeArguments[]{MakeFormatArgument(RuntimeValue)};
  (void)VPrint(Sink, "runtime={}", RuntimeArguments);

  AnyWriter<> Writer{NullSink{}};
  (void)WriteAll(Writer, {Bytes, sizeof(Bytes)});
  AnyReader<> Reader{MemorySource{{Bytes, sizeof(Bytes)}}};
  Foundation::Byte ReadBuffer[sizeof(Bytes)]{};
  (void)ReadExact(Reader, ReadBuffer);

  constexpr Library::U8StringView ScanText{
      u8"-42 true 3.5 12345678-1234-5678-90AB-CDEF01234567"};
  TextReader<MemorySource> TextInput{MemorySource{AsBytes(ScanText)}};
  (void)Scan<Foundation::Int32, Foundation::Bool, Foundation::Float64,
             UEFI::Guid>(TextInput);
}
} // namespace
} // namespace UEFIpp::IO
