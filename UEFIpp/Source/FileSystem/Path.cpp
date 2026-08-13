#include <UEFIpp/FileSystem/Path.hpp>

namespace UEFIpp::FileSystem {
auto Path::String() const -> const Library::String & { return Path_; }

auto Path::View() const -> Library::StringView { return Path_.View(); }

auto Path::Empty() const -> Foundation::Bool { return Path_.Empty(); }

auto Path::IsAbsolute() const -> Foundation::Bool {
  return !Path_.Empty() && Path_.Front() == '\\';
}

auto Path::IsRoot() const -> Foundation::Bool { return Path_ == "\\"; }

auto Path::Components() const -> Library::Vector<Library::StringView> {
  Library::Vector<Library::StringView> Components{Allocator()};
  auto Text = Path_.View();
  Foundation::Size Start = 0;

  while (Start < Text.Size()) {
    while (Start < Text.Size() && Text[Start] == '\\') {
      ++Start;
    }

    if (Start >= Text.Size()) {
      break;
    }

    auto End = Start;

    while (End < Text.Size() && Text[End] != '\\') {
      ++End;
    }

    if (!Components.PushBack(Text.Substr(Start, End - Start))) {
      Components.Clear();
      return Components;
    }

    Start = End;
  }

  return Components;
}

auto Path::Normalized() const -> Path {
  auto Text = Path_.View();
  Path Result{Allocator()};

  const auto Absolute = IsAbsolute();

  if (Absolute) {
    if (!Result.Path_.Append('\\')) {
      return Path{Allocator()};
    }
  }

  Library::Vector<Library::StringView> Components{Allocator()};
  Foundation::Size Start = 0;

  while (Start < Text.Size()) {
    while (Start < Text.Size() && (Text[Start] == '\\' || Text[Start] == '/')) {
      ++Start;
    }

    if (Start >= Text.Size()) {
      break;
    }

    auto End = Start;

    while (End < Text.Size() && Text[End] != '\\' && Text[End] != '/') {
      ++End;
    }

    auto Part = Text.Substr(Start, End - Start);

    if (Part == ".") {
    } else if (Part == "..") {
      if (!Components.Empty()) {
        (void)Components.PopBack();
      } else if (!Absolute) {
        if (!Components.PushBack(Part)) {
          return Path{Allocator()};
        }
      }
    } else {
      if (!Components.PushBack(Part)) {
        return Path{Allocator()};
      }
    }

    Start = End;
  }

  for (auto Component : Components) {
    if (!Result.Path_.Empty() && Result.Path_.Back() != '\\') {
      if (!Result.Path_.Append('\\')) {
        return Path{Allocator()};
      }
    }

    if (!Result.Path_.Append(Component)) {
      return Path{Allocator()};
    }
  }

  if (Result.Path_.Empty() && Absolute) {
    if (!Result.Path_.Append('\\')) {
      return Path{Allocator()};
    }
  }

  return Result;
}

auto Path::Normalize() -> Foundation::Void { *this = Normalized(); }

auto Path::Join(const Path &Left, const Path &Right,
                Memory::AllocatorStub Allocator) -> Path {
  if (!Allocator) {
    Allocator = Left.Allocator() ? Left.Allocator() : Right.Allocator();
  }

  if (Left.Empty()) {
    return Path{Right, Allocator};
  }

  if (Right.Empty()) {
    return Path{Left, Allocator};
  }

  Path Result{Left, Allocator};

  if (!Result.Path_.EndsWith("\\")) {
    if (!Result.Path_.Append('\\')) {
      return Path{Allocator};
    }
  }

  if (!Result.Path_.Append(Right.View())) {
    return Path{Allocator};
  }

  return Result.Normalized();
}

auto Path::Join(const Path &Other) const -> Path {
  if (Empty()) {
    return Path{Other, Allocator()};
  }

  if (Other.Empty()) {
    return *this;
  }

  auto Result = *this;

  if (!Result.Path_.EndsWith("\\")) {
    if (!Result.Path_.Append('\\')) {
      return Path{Allocator()};
    }
  }

  if (!Result.Path_.Append(Other.View())) {
    return Path{Allocator()};
  }

  return Result.Normalized();
}

auto Path::Parent() const -> Path {
  auto Normal = Normalized();

  if (Normal.Empty() || Normal.IsRoot()) {
    return Normal;
  }

  auto Text = Normal.View();
  auto LastSeparator = Text.FindLast('\\');

  if (LastSeparator == Library::String::NotFound) {
    return Path{Allocator()};
  }

  if (LastSeparator == 0) {
    return Path{"\\", Allocator()};
  }

  return Path{Text.Substr(0, LastSeparator), Allocator()};
}

auto Path::FileName() const -> Library::StringView {
  auto Text = View();

  if (Text.Empty() || IsRoot()) {
    return {};
  }

  auto LastSeparator = Text.FindLast('\\');

  if (LastSeparator == Library::StringView::NotFound) {
    return Text;
  }

  return Text.Substr(LastSeparator + 1);
}

auto Path::Extension() const -> Library::StringView {
  auto Name = FileName();
  auto Position = Name.FindLast('.');

  if (Position == Library::StringView::NotFound) {
    return {};
  }

  return Name.Substr(Position + 1);
}

auto Path::Stem() const -> Library::StringView {
  auto Name = FileName();
  auto Position = Name.FindLast('.');

  if (Position == Library::StringView::NotFound) {
    return Name;
  }

  return Name.Substr(0, Position);
}

auto Path::NameCount() const -> Foundation::Size {
  auto Text = View();

  Foundation::Size Count = 0;
  Foundation::Bool InName = false;

  for (auto Character : Text) {
    if (Character == '\\' || Character == '/') {
      InName = false;
    } else if (!InName) {
      InName = true;
      ++Count;
    }
  }

  return Count;
}

auto Path::HasExtension() const -> Foundation::Bool {
  return !Extension().Empty();
}

[[nodiscard]] auto Path::ReplaceExtension(Library::StringView Extension) const
    -> Path {
  auto Result = *this;

  auto Name = FileName();
  auto Dot = Name.FindLast('.');

  if (Dot == Library::StringView::NotFound) {
    if (!Extension.Empty()) {
      if (!Result.Path_.Append('.') || !Result.Path_.Append(Extension)) {
        return Path{Allocator()};
      }
    }

    return Result;
  }

  const auto NameOffset =
      Foundation::Cast::Auto<Foundation::Size>(Name.Data() - View().Data());
  Result.Path_ = Result.Path_.Substr(0, NameOffset + Dot);

  if (!Extension.Empty()) {
    if (!Result.Path_.Append('.') || !Result.Path_.Append(Extension)) {
      return Path{Allocator()};
    }
  }

  return Result;
}

auto Path::RemoveExtension() const -> Path { return ReplaceExtension({}); }

auto Path::RelativeTo(const Path &Base) const -> Path {
  const auto TargetComponents = Normalized().Components();
  const auto BaseComponents = Path{Base, Allocator()}.Normalized().Components();

  Foundation::Size Common = 0;

  while (Common < TargetComponents.Size() && Common < BaseComponents.Size() &&
         TargetComponents[Common] == BaseComponents[Common]) {
    ++Common;
  }

  Path Result{Allocator()};

  for (Foundation::Size i = Common; i < BaseComponents.Size(); ++i) {
    Result = Result.Join("..");
  }

  for (Foundation::Size i = Common; i < TargetComponents.Size(); ++i) {
    Result = Result.Join(TargetComponents[i]);
  }

  return Result;
}

auto Path::operator=(Library::StringView Path) -> FileSystem::Path & {
  Path_ = Path;
  return *this;
}

auto Path::operator=(const Foundation::Char *Path) -> FileSystem::Path & {
  Path_ = Path;
  return *this;
}
} // namespace UEFIpp::FileSystem

namespace UEFIpp::FileSystem {
// Constructs a path from a string
Path::Path(const Library::String &Path, Memory::AllocatorStub Allocator)
    : Path_(Path, Allocator) {}

// Constructs a path from a C-style string
Path::Path(const Foundation::Char *Path, Memory::AllocatorStub Allocator)
    : Path_(Path, Allocator) {}

// Constructs a path from a string view
Path::Path(Library::StringView Path, Memory::AllocatorStub Allocator)
    : Path_(Path, Allocator) {}

// Copies another path into a specific allocator.
Path::Path(const Path &Other, Memory::AllocatorStub Allocator)
    : Path_(Other.Path_, Allocator) {}
} // namespace UEFIpp::FileSystem
