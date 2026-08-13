#include <UEFIpp/Stream/Output/FileSink.hpp>

namespace UEFIpp::Stream {
auto FileOutputSink::Reset(FileSystem::File *File, Foundation::Bool AutoFlush)
    -> Foundation::Void {
  File_ = File;
  AutoFlush_ = AutoFlush;
}

auto FileOutputSink::Write(const Foundation::Char *Data,
                           Foundation::Size Length) -> Foundation::Void {
  if (!Valid() || !Data || !Length) {
    return;
  }

  if (!File_->Append(Foundation::Cast::Auto<const Foundation::Uint8 *>(Data),
                     Foundation::Cast::Auto<Foundation::Uint64>(Length))) {
    return;
  }

  if (AutoFlush_) {
    (void)File_->Flush();
  }
}

auto FileOutputSink::Write(const Foundation::WChar *Data,
                           Foundation::Size Length) -> Foundation::Void {
  if (!Valid() || !Data || !Length) {
    return;
  }

  for (Foundation::Size Index = 0; Index < Length; ++Index) {
    const auto Character = Data[Index];
    const auto Narrow =
        Character <= 0x7F ? Foundation::Cast::Auto<Foundation::Char>(Character)
                          : Foundation::Char{'?'};
    Write(&Narrow, 1);
  }
}

auto FileOutputSink::Clear() -> Foundation::Void {
  if (!Valid()) {
    return;
  }

  (void)File_->Handle().SetCurrentPosition(0);
}
} // namespace UEFIpp::Stream
