#include <UEFIpp/IO/Transport/Console.hpp>

namespace UEFIpp::IO {
auto ConsoleSink::WriteSome(Library::Span<const Foundation::Byte> Bytes)
    -> Result<Foundation::Size> {
  if (!Valid()) {
    return Failure(Error::Semantic(ErrorCode::Closed, Operation::Write));
  }

  for (Foundation::Size Index{}; Index < Bytes.Size(); ++Index) {
    Foundation::Bool Consumed{};
    while (!Consumed) {
      auto Decoded = Decoder_.Feed(Bytes[Index], InvalidPolicy_);
      if (!Decoded) {
        auto Error = Decoded.Error();
        Error.Offset = Index;
        Error.Transferred = Index;
        return Failure(Error);
      }
      Consumed = Decoded.Value().Consumed;
      if (Decoded.Value().CodePoint) {
        auto Result = Append(Decoded.Value().CodePoint.Value());
        if (!Result) {
          auto Error = Result.Error();
          Error.Offset = Index;
          Error.Transferred = Index;
          return Failure(Error);
        }
      }
    }
  }

  auto Result = FlushWide();
  if (!Result) {
    auto Error = Result.Error();
    Error.Transferred = Bytes.Size();
    return Failure(Error);
  }
  return Bytes.Size();
}

auto ConsoleSink::Flush() -> Result<> {
  auto Finished = Decoder_.Finish(InvalidPolicy_);
  if (!Finished) {
    return Failure(Finished.Error());
  }
  if (Finished.Value()) {
    auto Result = Append(Finished.Value().Value());
    if (!Result) {
      return Result;
    }
  }
  return FlushWide();
}

auto ConsoleSink::ClearTerminal() -> Result<> {
  auto Result = Flush();
  if (!Result) {
    return Result;
  }
  if (!Valid()) {
    return Failure(Error::Semantic(ErrorCode::Closed, Operation::Write));
  }
  const auto Status = Output_->ClearScreen(Output_);
  if (UEFI::IsError(Foundation::Cast::Auto<UEFI::StatusCode>(Status))) {
    return Failure(Error::FromStatus(Operation::Write, Status));
  }
  return {};
}

auto ConsoleSink::SetAttribute(Foundation::UintN Attribute) -> Result<> {
  if (!Valid()) {
    return Failure(Error::Semantic(ErrorCode::Closed, Operation::Write));
  }
  const auto Status = Output_->SetAttribute(Output_, Attribute);
  if (UEFI::IsError(Foundation::Cast::Auto<UEFI::StatusCode>(Status))) {
    return Failure(Error::FromStatus(Operation::Write, Status));
  }
  return {};
}

auto ConsoleSink::SetStyle(ConsoleStyle Style) -> Result<> {
  const auto Foreground =
      Foundation::Cast::Auto<Foundation::UintN>(Style.Foreground) & 0x0F;
  const auto Background =
      Foundation::Cast::Auto<Foundation::UintN>(Style.Background) & 0x07;
  return SetAttribute(Foreground | (Background << 4));
}

auto ConsoleSink::SetCursor(Foundation::UintN Column, Foundation::UintN Row)
    -> Result<> {
  if (!Valid()) {
    return Failure(Error::Semantic(ErrorCode::Closed, Operation::Write));
  }
  const auto Status = Output_->SetCursorPosition(Output_, Column, Row);
  if (UEFI::IsError(Foundation::Cast::Auto<UEFI::StatusCode>(Status))) {
    return Failure(Error::FromStatus(Operation::Write, Status));
  }
  return {};
}

auto ConsoleSink::SetCursorVisible(Foundation::Bool Visible) -> Result<> {
  if (!Valid()) {
    return Failure(Error::Semantic(ErrorCode::Closed, Operation::Write));
  }
  const auto Status = Output_->EnableCursor(Output_, Visible);
  if (UEFI::IsError(Foundation::Cast::Auto<UEFI::StatusCode>(Status))) {
    return Failure(Error::FromStatus(Operation::Write, Status));
  }
  return {};
}

auto ConsoleSink::Geometry() const -> Result<ConsoleGeometry> {
  if (!Valid() || !Output_->Mode) {
    return Failure(Error::Semantic(ErrorCode::Closed, Operation::Read));
  }
  Foundation::UintN Columns{};
  Foundation::UintN Rows{};
  const auto Status = Output_->QueryMode(
      Output_, Foundation::Cast::Auto<Foundation::UintN>(Output_->Mode->Mode),
      &Columns, &Rows);
  if (UEFI::IsError(Foundation::Cast::Auto<UEFI::StatusCode>(Status))) {
    return Failure(Error::FromStatus(Operation::Read, Status));
  }
  return ConsoleGeometry{
      Columns, Rows,
      Foundation::Cast::Auto<Foundation::UintN>(Output_->Mode->CursorColumn),
      Foundation::Cast::Auto<Foundation::UintN>(Output_->Mode->CursorRow)};
}

auto ConsoleSink::Append(Foundation::Char32 CodePoint) -> Result<> {
  Foundation::Char16 Encoded[2]{};
  auto Count = EncodeUtf16(CodePoint, Encoded);
  if (!Count) {
    return Failure(Count.Error());
  }

  if (WideLength_ + Count.Value() > WideBufferCapacity) {
    auto Result = FlushWide();
    if (!Result) {
      return Result;
    }
  }

  for (Foundation::Size Index{}; Index < Count.Value(); ++Index) {
    WideBuffer_[WideLength_++] =
        Foundation::Cast::Auto<Foundation::WChar>(Encoded[Index]);
  }
  return {};
}

auto ConsoleSink::FlushWide() -> Result<> {
  if (!WideLength_) {
    return {};
  }
  WideBuffer_[WideLength_] = 0;
  const auto Status = Output_->OutputString(Output_, WideBuffer_.Data());
  if (UEFI::IsError(Foundation::Cast::Auto<UEFI::StatusCode>(Status))) {
    return Failure(Error::FromStatus(Operation::Write, Status, 0, 0));
  }
  WideLength_ = 0;
  return {};
}
} // namespace UEFIpp::IO
