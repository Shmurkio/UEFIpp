#include <UEFIpp/IO/Context.hpp>
#include <UEFIpp/CRT/New.hpp>
#include <intrin.h>

namespace UEFIpp::IO {
namespace {
alignas(IoContext) Foundation::Byte GlobalContextStorage[sizeof(IoContext)]{};
IoContext *GlobalContext{};
constinit Foundation::SpinLock GlobalContextLock{};
[[nodiscard]] auto Timestamp(Foundation::Void *) -> Foundation::Uint64 {
  return __rdtsc();
}
}

auto IoContext::Attach(UEFI::Table::BootServices *BootServices,
                       Protocols::SimpleTextInputEx *InputEx,
                       Protocols::SimpleTextInput *Input,
                       Protocols::SimpleTextOutput *Output,
                       Foundation::Uint16 SerialPort) -> Foundation::Void {
  ConsoleTransport().Reset(Output);
  Serial_.Sink().Sink().Reset(SerialPort);
  Terminal_.Reset(BootServices, InputEx, Input, WriterRef{Console_},
                  Output ? &ConsoleTransport() : nullptr);
  Events_.Reset(BootServices);
  Logger_.ClearSinks();
  Logger_.SetModule(u8"UEFIpp");
  Logger_.SetTimestampSource(nullptr, &Timestamp);
  (void)Logger_.AddSink(WriterRef{Serial_});
  PanicWriter::Configure(WriterRef{Serial_});
}

auto IoContext::Flush() -> Result<> {
  auto ConsoleResult = Console_.Flush();
  auto SerialResult = Serial_.Flush();
  if (!ConsoleResult) return ConsoleResult;
  return SerialResult;
}

auto IoContext::PrepareExitBootServices() -> Result<> { return Flush(); }

auto IoContext::ExitBootServices() noexcept -> Foundation::Void {
  ConsoleTransport().Reset(nullptr);
  Terminal_.Reset(nullptr, nullptr, nullptr, {}, nullptr);
  Events_.Reset(nullptr);
}

auto SystemIO() noexcept -> IoContext & {
  Foundation::SpinLockGuard Guard{GlobalContextLock};
  if (!GlobalContext) {
    GlobalContext = ::new (GlobalContextStorage) IoContext{};
  }
  return *GlobalContext;
}
} // namespace UEFIpp::IO
