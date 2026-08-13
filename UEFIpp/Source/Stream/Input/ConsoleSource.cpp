#include <UEFIpp/Stream/Input/ConsoleSource.hpp>

namespace UEFIpp::Stream {
namespace In {
Input<ConsoleInputSource> Console{};
}

auto ConsoleInputSource::Reset(UEFI::Table::BootServices *BootServices,
                               Protocols::SimpleTextInputEx *Input)
    -> Foundation::Void {
  BootServices_ = BootServices;
  Input_ = Input;
  LineLength_ = 0;
}

auto ConsoleInputSource::ReadKey() -> Protocols::KeyData {
  return ReadKey(false);
}

auto ConsoleInputSource::ReadKeyEcho() -> Protocols::KeyData {
  return ReadKey(true);
}

auto ConsoleInputSource::ReadKey(Foundation::Bool Echo) -> Protocols::KeyData {
  if (!Valid()) {
    return {};
  }

  for (;;) {
    Foundation::UintN Index{};
    BootServices_->WaitForEvent(1, &Input_->WaitForKeyEx, &Index);

    Protocols::KeyData Key{};
    const auto Status = Input_->ReadKey(Key);

    if (UEFI::Status(Status).Succeeded()) {
      if (Echo) {
        EchoKey(Key);
      }

      return Key;
    }
  }
}

auto ConsoleInputSource::EchoKey(const Protocols::KeyData &Key)
    -> Foundation::Void {
  if (!Key.HasUnicode()) {
    return;
  }

  const auto Character = Key.Key.UnicodeChar;

  if (Character == L'\b') {
    if (LineLength_) {
      --LineLength_;
      Stream::Out::Console << "\b \b" << Stream::FlushNow;
    }

    return;
  }

  if (Character == L'\r' || Character == L'\n') {
    LineLength_ = 0;
    Stream::Out::Console << Stream::Endl;
    return;
  }

  ++LineLength_;
  Stream::Out::Console << Character << Stream::FlushNow;
}
} // namespace UEFIpp::Stream
