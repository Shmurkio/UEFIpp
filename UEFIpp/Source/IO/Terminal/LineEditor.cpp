#include <UEFIpp/IO/Terminal/LineEditor.hpp>

namespace UEFIpp::IO {
auto LineEditor::ReadLine(Terminal &Terminal, LineEditorOptions Options)
    -> Result<Library::U8String> {
  if (!Terminal.Valid()) {
    return Failure(Error::Semantic(ErrorCode::Closed, Operation::Read));
  }

  auto Geometry = Terminal.Geometry();
  if (!Geometry && Options.Echo) {
    return Failure(Geometry.Error());
  }
  const auto Origin = Geometry ? Geometry.Value() : ConsoleGeometry{};
  Library::U8String Line{Allocator_};
  Foundation::Size Cursor{};
  Foundation::Size PreviousCells{};
  Foundation::Size HistoryIndex = History_.Size();

  for (;;) {
    auto Event = Terminal.WaitKey(Options.Wait);
    if (!Event) return Failure(Event.Error());
    const auto Key = Event.Value();
    Foundation::Bool Changed{};

    if (Key.Code == KeyCode::Escape) {
      return Failure(Error::Semantic(ErrorCode::Cancelled, Operation::Read));
    }
    if (Key.Code == KeyCode::Left) {
      Cursor = PreviousBoundary(Line.View(), Cursor);
      Changed = true;
    } else if (Key.Code == KeyCode::Right) {
      Cursor = NextBoundary(Line.View(), Cursor);
      Changed = true;
    } else if (Key.Code == KeyCode::Home) {
      Cursor = 0;
      Changed = true;
    } else if (Key.Code == KeyCode::End) {
      Cursor = Line.Size();
      Changed = true;
    } else if (Key.Code == KeyCode::Delete) {
      const auto End = NextBoundary(Line.View(), Cursor);
      if (End > Cursor) {
        (void)Line.Erase(Cursor, End - Cursor);
        Changed = true;
      }
    } else if (Key.Code == KeyCode::Up && !History_.Empty()) {
      if (HistoryIndex) --HistoryIndex;
      if (!Line.Assign(History_[HistoryIndex].View())) {
        return Failure(
            Error::Semantic(ErrorCode::OutOfMemory, Operation::Allocate));
      }
      Cursor = Line.Size();
      Changed = true;
    } else if (Key.Code == KeyCode::Down && !History_.Empty()) {
      if (HistoryIndex + 1 < History_.Size()) {
        ++HistoryIndex;
        if (!Line.Assign(History_[HistoryIndex].View())) {
          return Failure(
              Error::Semantic(ErrorCode::OutOfMemory, Operation::Allocate));
        }
      } else {
        HistoryIndex = History_.Size();
        Line.Clear();
      }
      Cursor = Line.Size();
      Changed = true;
    } else if (Key.HasCharacter()) {
      const auto Character = Key.Character;
      if (Character == '\r' || Character == '\n') {
        if (Options.AllowMultiline && Key.Modifiers.Shift()) {
          if (Line.Size() == Options.MaximumBytes ||
              !Line.Insert(Cursor, u8'\n')) {
            return Failure(
                Error::Semantic(ErrorCode::Overflow, Operation::Read));
          }
          ++Cursor;
          Changed = true;
        } else {
          if (Options.Echo) {
            auto Result = Terminal.Write(u8"\r\n");
            if (!Result) return Failure(Result.Error());
            Result = Terminal.Flush();
            if (!Result) return Failure(Result.Error());
          }
          if (Options.AddToHistory && !Line.Empty()) {
            if (!History_.PushBack(Line)) {
              return Failure(Error::Semantic(ErrorCode::OutOfMemory,
                                             Operation::Allocate));
            }
          }
          OnLine.Emit(Line.View());
          return Line;
        }
      } else if (Character == '\b') {
        const auto Begin = PreviousBoundary(Line.View(), Cursor);
        if (Begin < Cursor) {
          (void)Line.Erase(Begin, Cursor - Begin);
          Cursor = Begin;
          Changed = true;
        }
      } else if (Character == '\t' && Completion_) {
        auto Completed = Completion_(Line.View());
        if (!Completed) return Failure(Completed.Error());
        if (Completed.Value().Size() > Options.MaximumBytes ||
            !Line.Assign(Completed.Value().View())) {
          return Failure(
              Error::Semantic(ErrorCode::Overflow, Operation::Read));
        }
        Cursor = Line.Size();
        Changed = true;
      } else if (Character >= 0x20 ||
                 (Options.AllowMultiline && Character == '\t')) {
        Foundation::Byte Encoded[4]{};
        auto Count = EncodeUtf8(Character, Encoded);
        if (!Count) return Failure(Count.Error());
        if (Line.Size() > Options.MaximumBytes ||
            Count.Value() > Options.MaximumBytes - Line.Size()) {
          return Failure(Error::Semantic(ErrorCode::Overflow, Operation::Read));
        }
        const Library::U8StringView Text{
            Foundation::Cast::Auto<const Foundation::Char8 *>(&Encoded[0]),
            Count.Value()};
        if (!Line.Insert(Cursor, Text)) {
          return Failure(
              Error::Semantic(ErrorCode::OutOfMemory, Operation::Allocate));
        }
        Cursor += Count.Value();
        Changed = true;
      }
    }

    if (Changed && Options.Echo) {
      auto Result = Redraw(Terminal, Line.View(), Cursor, PreviousCells, Origin,
                           Options);
      if (!Result) return Failure(Result.Error());
    }
  }
}

auto LineEditor::PreviousBoundary(Library::U8StringView Text,
                                  Foundation::Size Position)
    -> Foundation::Size {
  if (!Position) return 0;
  --Position;
  while (Position &&
         (Foundation::Cast::Auto<Foundation::Uint8>(Text[Position]) & 0xC0) ==
             0x80) {
    --Position;
  }
  return Position;
}

auto LineEditor::NextBoundary(Library::U8StringView Text,
                              Foundation::Size Position) -> Foundation::Size {
  if (Position >= Text.Size()) return Text.Size();
  ++Position;
  while (Position < Text.Size() &&
         (Foundation::Cast::Auto<Foundation::Uint8>(Text[Position]) & 0xC0) ==
             0x80) {
    ++Position;
  }
  return Position;
}

auto LineEditor::Redraw(Terminal &Terminal, Library::U8StringView Line,
                        Foundation::Size Cursor,
                        Foundation::Size &PreviousCells,
                        const ConsoleGeometry &Origin,
                        const LineEditorOptions &Options) -> Result<> {
  auto Result = Terminal.SetCursor(Origin.Column, Origin.Row);
  if (!Result) return Result;

  Library::U8String Display{Allocator_};
  if (Options.MaskInput) {
    for (Foundation::Size Position{}; Position < Line.Size();
         Position = NextBoundary(Line, Position)) {
      if (!Display.PushBack(Line[Position] == u8'\n' ? u8'\n' : u8'*')) {
        return Failure(
            Error::Semantic(ErrorCode::OutOfMemory, Operation::Allocate));
      }
    }
  } else if (!Display.Assign(Line)) {
    return Failure(
        Error::Semantic(ErrorCode::OutOfMemory, Operation::Allocate));
  }

  Result = Terminal.Write(Display.View());
  if (!Result) return Result;
  const auto LayoutExtent = [&](Foundation::Size End) {
    Foundation::Size Offset{};
    for (Foundation::Size Position{}; Position < End;
         Position = NextBoundary(Line, Position)) {
      if (Line[Position] == u8'\n' && Origin.Columns) {
        const auto Absolute = Origin.Column + Offset;
        Offset += Origin.Columns - (Absolute % Origin.Columns);
      } else {
        ++Offset;
      }
    }
    return Offset;
  };
  const auto Extent = LayoutExtent(Line.Size());
  if (PreviousCells > Extent) {
    Library::U8String Spaces{Allocator_};
    if (!Spaces.Resize(PreviousCells - Extent, u8' ')) {
      return Failure(
          Error::Semantic(ErrorCode::OutOfMemory, Operation::Allocate));
    }
    Result = Terminal.Write(Spaces.View());
    if (!Result) return Result;
  }
  Result = Terminal.Flush();
  if (!Result) return Result;
  PreviousCells = Extent;

  const auto CursorCells = LayoutExtent(Cursor);
  if (!Origin.Columns) {
    return Terminal.SetCursor(Origin.Column + CursorCells, Origin.Row);
  }
  const auto Linear = Origin.Row * Origin.Columns + Origin.Column + CursorCells;
  return Terminal.SetCursor(Linear % Origin.Columns, Linear / Origin.Columns);
}
} // namespace UEFIpp::IO
