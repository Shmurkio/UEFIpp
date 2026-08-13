#include <UEFIpp/Reverse/SymbolDatabase.hpp>

namespace UEFIpp::Reverse {
namespace {
inline constexpr auto FnvOffset = Foundation::Uint64{0xCBF29CE484222325ull};
inline constexpr auto FnvPrime = Foundation::Uint64{0x100000001B3ull};

[[nodiscard]] auto IsFunctionKind(SymbolKind Kind) noexcept
    -> Foundation::Bool {
  return Kind == SymbolKind::Function || Kind == SymbolKind::PublicFunction ||
         Kind == SymbolKind::Thunk;
}

[[nodiscard]] auto IsGlobalKind(SymbolKind Kind) noexcept -> Foundation::Bool {
  return Kind == SymbolKind::GlobalVariable ||
         Kind == SymbolKind::StaticVariable || Kind == SymbolKind::ThreadLocal;
}
} // namespace

SymbolDatabase::SymbolDatabase(Memory::AllocatorStub Allocator) noexcept
    : Allocator_(Allocator), Strings_(Allocator), Symbols_(Allocator),
      Modules_(Allocator), Types_(Allocator), Fields_(Allocator),
      ParameterTypes_(Allocator), SourceLines_(Allocator),
      SectionMap_(Allocator), OmapToSource_(Allocator),
      OmapFromSource_(Allocator), InlineSites_(Allocator), Locals_(Allocator),
      NameIndex_(Allocator), TypeNameIndex_(Allocator),
      AddressIndex_(Allocator) {
  (void)Strings_.EmplaceBack(Allocator_);
}

auto SymbolDatabase::Reset() -> Foundation::Void {
  const auto Allocator = Allocator_;
  *this = SymbolDatabase{Allocator};
}

auto SymbolDatabase::Ready() const noexcept -> Foundation::Bool {
  return Ready_;
}

auto SymbolDatabase::SetReady(Foundation::Bool Value) noexcept
    -> Foundation::Void {
  Ready_ = Value;
}

auto SymbolDatabase::ImageBase() const noexcept -> const Foundation::Void * {
  return ImageBase_;
}

auto SymbolDatabase::ImageSize() const noexcept -> Foundation::Uint64 {
  return ImageSize_;
}

auto SymbolDatabase::SetImage(const Foundation::Void *Base,
                              Foundation::Uint64 Size) noexcept
    -> Foundation::Void {
  ImageBase_ = Base;
  ImageSize_ = Size;
}

auto SymbolDatabase::Identity() const noexcept -> const PdbIdentity & {
  return Identity_;
}

auto SymbolDatabase::SetIdentity(const PdbIdentity &Identity) noexcept
    -> Foundation::Void {
  Identity_ = Identity;
}

auto SymbolDatabase::Strings() const noexcept
    -> Library::Span<const Library::String> {
  return Strings_.View();
}

auto SymbolDatabase::Symbols() const noexcept -> Library::Span<const Symbol> {
  return Symbols_.View();
}

auto SymbolDatabase::Modules() const noexcept
    -> Library::Span<const ModuleInfo> {
  return Modules_.View();
}

auto SymbolDatabase::Types() const noexcept -> Library::Span<const TypeInfo> {
  return Types_.View();
}

auto SymbolDatabase::Fields() const noexcept -> Library::Span<const TypeField> {
  return Fields_.View();
}

auto SymbolDatabase::ParameterTypes() const noexcept
    -> Library::Span<const TypeIndex> {
  return ParameterTypes_.View();
}

auto SymbolDatabase::SourceLines() const noexcept
    -> Library::Span<const SourceLine> {
  return SourceLines_.View();
}

auto SymbolDatabase::SectionMap() const noexcept
    -> Library::Span<const SectionMapEntry> {
  return SectionMap_.View();
}

auto SymbolDatabase::OmapToSource() const noexcept
    -> Library::Span<const OmapEntry> {
  return OmapToSource_.View();
}

auto SymbolDatabase::OmapFromSource() const noexcept
    -> Library::Span<const OmapEntry> {
  return OmapFromSource_.View();
}

auto SymbolDatabase::InlineSites() const noexcept
    -> Library::Span<const InlineSite> {
  return InlineSites_.View();
}

auto SymbolDatabase::Locals() const noexcept
    -> Library::Span<const LocalVariable> {
  return Locals_.View();
}

auto SymbolDatabase::String(StringId Id) const noexcept -> Library::StringView {
  return Id < Strings_.Size() ? Strings_[Id].View() : Library::StringView{};
}

auto SymbolDatabase::SymbolById(SymbolId Id) const noexcept -> const Symbol * {
  return Id < Symbols_.Size() ? &Symbols_[Id] : nullptr;
}

auto SymbolDatabase::MutableSymbolById(SymbolId Id) noexcept -> Symbol * {
  return Id < Symbols_.Size() ? &Symbols_[Id] : nullptr;
}

auto SymbolDatabase::MutableInlineSites() noexcept
    -> Library::Span<InlineSite> {
  return InlineSites_.View();
}

auto SymbolDatabase::ModuleById(ModuleId Id) const noexcept
    -> const ModuleInfo * {
  return Id < Modules_.Size() ? &Modules_[Id] : nullptr;
}

auto SymbolDatabase::FindTypeStorageIndex(TypeIndex Index) const noexcept
    -> Foundation::Size {
  Foundation::Size Left = 0;
  Foundation::Size Right = Types_.Size();

  while (Left < Right) {
    const auto Middle = Left + (Right - Left) / 2;
    if (Types_[Middle].Index < Index) {
      Left = Middle + 1;
    } else {
      Right = Middle;
    }
  }

  return Left;
}

auto SymbolDatabase::TypeByIndex(TypeIndex Index) const noexcept
    -> const TypeInfo * {
  if (Index == InvalidTypeIndex || Types_.Empty())
    return nullptr;
  if (!Ready_) {
    for (const auto &Type : Types_)
      if (Type.Index == Index)
        return &Type;
    return nullptr;
  }
  const auto Position = FindTypeStorageIndex(Index);
  return Position < Types_.Size() && Types_[Position].Index == Index
             ? &Types_[Position]
             : nullptr;
}

auto SymbolDatabase::Intern(Library::StringView Value)
    -> Library::Expected<StringId, SymbolDatabaseError> {
  if (Value.Empty()) {
    return EmptyStringId;
  }

  for (Foundation::Size i = 1; i < Strings_.Size(); ++i) {
    if (Strings_[i].View() == Value) {
      return Foundation::Cast::Auto<StringId>(i);
    }
  }

  auto *NewString = Strings_.EmplaceBack(Value, Allocator_);
  if (!NewString) {
    return Library::MakeUnexpected(SymbolDatabaseError::AllocationFailure);
  }

  return Foundation::Cast::Auto<StringId>(Strings_.Size() - 1);
}

auto SymbolDatabase::AddModule(const ModuleInfo &Value)
    -> Library::Expected<ModuleId, SymbolDatabaseError> {
  auto Copy = Value;
  Copy.Id = Foundation::Cast::Auto<ModuleId>(Modules_.Size());
  if (!Modules_.PushBack(Copy)) {
    return Library::MakeUnexpected(SymbolDatabaseError::AllocationFailure);
  }
  return Copy.Id;
}

auto SymbolDatabase::AddSymbol(Symbol Value)
    -> Library::Expected<SymbolId, SymbolDatabaseError> {
  Value.Id = Foundation::Cast::Auto<SymbolId>(Symbols_.Size());
  if (!Symbols_.PushBack(Value)) {
    return Library::MakeUnexpected(SymbolDatabaseError::AllocationFailure);
  }
  return Value.Id;
}

auto SymbolDatabase::AddType(const TypeInfo &Value)
    -> Library::Expected<Foundation::Void, SymbolDatabaseError> {
  if (Value.Index == InvalidTypeIndex) {
    return Library::MakeUnexpected(SymbolDatabaseError::InvalidType);
  }
  if (!Types_.PushBack(Value)) {
    return Library::MakeUnexpected(SymbolDatabaseError::AllocationFailure);
  }
  return {};
}

auto SymbolDatabase::AddField(const TypeField &Value)
    -> Library::Expected<Foundation::Uint32, SymbolDatabaseError> {
  const auto Index = Foundation::Cast::Auto<Foundation::Uint32>(Fields_.Size());
  if (!Fields_.PushBack(Value)) {
    return Library::MakeUnexpected(SymbolDatabaseError::AllocationFailure);
  }
  return Index;
}

auto SymbolDatabase::AddParameterType(TypeIndex Value)
    -> Library::Expected<Foundation::Uint32, SymbolDatabaseError> {
  const auto Index =
      Foundation::Cast::Auto<Foundation::Uint32>(ParameterTypes_.Size());
  if (!ParameterTypes_.PushBack(Value)) {
    return Library::MakeUnexpected(SymbolDatabaseError::AllocationFailure);
  }
  return Index;
}

auto SymbolDatabase::AddSectionMap(const SectionMapEntry &Value)
    -> Library::Expected<Foundation::Void, SymbolDatabaseError> {
  if (!SectionMap_.PushBack(Value)) {
    return Library::MakeUnexpected(SymbolDatabaseError::AllocationFailure);
  }
  return {};
}

auto SymbolDatabase::AddOmapToSource(const OmapEntry &Value)
    -> Library::Expected<Foundation::Void, SymbolDatabaseError> {
  if (!OmapToSource_.PushBack(Value)) {
    return Library::MakeUnexpected(SymbolDatabaseError::AllocationFailure);
  }
  return {};
}

auto SymbolDatabase::AddOmapFromSource(const OmapEntry &Value)
    -> Library::Expected<Foundation::Void, SymbolDatabaseError> {
  if (!OmapFromSource_.PushBack(Value)) {
    return Library::MakeUnexpected(SymbolDatabaseError::AllocationFailure);
  }
  return {};
}

auto SymbolDatabase::AddSourceLine(const SourceLine &Value)
    -> Library::Expected<Foundation::Void, SymbolDatabaseError> {
  if (!SourceLines_.PushBack(Value)) {
    return Library::MakeUnexpected(SymbolDatabaseError::AllocationFailure);
  }
  return {};
}

auto SymbolDatabase::AddInlineSite(const InlineSite &Value)
    -> Library::Expected<InlineSiteId, SymbolDatabaseError> {
  auto Copy = Value;
  Copy.Id = Foundation::Cast::Auto<InlineSiteId>(InlineSites_.Size());
  if (!InlineSites_.PushBack(Copy)) {
    return Library::MakeUnexpected(SymbolDatabaseError::AllocationFailure);
  }
  return Copy.Id;
}

auto SymbolDatabase::AddLocal(const LocalVariable &Value)
    -> Library::Expected<Foundation::Void, SymbolDatabaseError> {
  if (!Locals_.PushBack(Value)) {
    return Library::MakeUnexpected(SymbolDatabaseError::AllocationFailure);
  }
  return {};
}

auto SymbolDatabase::AddNameIndex(StringId Name, SymbolId SymbolValue)
    -> Foundation::Bool {
  if (Name == EmptyStringId || SymbolValue == InvalidSymbolId) {
    return true;
  }
  return NameIndex_.PushBack(NameIndexEntry{
      .Hash = HashName(String(Name)), .Name = Name, .Symbol = SymbolValue});
}

auto SymbolDatabase::Finalize()
    -> Library::Expected<Foundation::Void, SymbolDatabaseError> {
  NameIndex_.Clear();
  TypeNameIndex_.Clear();
  AddressIndex_.Clear();

  for (auto &Value : Symbols_) {
    if (!Value.SemanticId) {
      const auto Canonical = Value.DecoratedName != EmptyStringId
                                 ? String(Value.DecoratedName)
                                 : (Value.QualifiedName != EmptyStringId
                                        ? String(Value.QualifiedName)
                                        : String(Value.Name));
      Value.SemanticId = HashName(Canonical);
      auto SemanticKind = Value.Kind;
      if (IsFunctionKind(Value.Kind))
        SemanticKind = SymbolKind::Function;
      Value.SemanticId ^=
          Foundation::Cast::Auto<Foundation::Uint64>(SemanticKind) *
          0x9E3779B185EBCA87ull;
      // Raw CodeView type indices are intentionally excluded: they are
      // allocation-order identifiers inside one PDB and are not stable across
      // compiler/linker builds.
      if (Value.Visibility == SymbolVisibility::Local &&
          Value.Module != InvalidModuleId) {
        if (const auto *Module = ModuleById(Value.Module))
          Value.SemanticId ^=
              HashName(String(Module->Name)) * 0xC2B2AE3D27D4EB4Full;
      }
    }
    if (!AddNameIndex(Value.Name, Value.Id) ||
        (Value.DecoratedName != Value.Name &&
         !AddNameIndex(Value.DecoratedName, Value.Id)) ||
        (Value.QualifiedName != Value.Name &&
         Value.QualifiedName != Value.DecoratedName &&
         !AddNameIndex(Value.QualifiedName, Value.Id)) ||
        (Value.BaseName != Value.Name &&
         Value.BaseName != Value.DecoratedName &&
         Value.BaseName != Value.QualifiedName &&
         !AddNameIndex(Value.BaseName, Value.Id))) {
      return Library::MakeUnexpected(SymbolDatabaseError::AllocationFailure);
    }

    if (Value.Rva && !AddressIndex_.PushBack(Value.Id)) {
      return Library::MakeUnexpected(SymbolDatabaseError::AllocationFailure);
    }
  }

  for (const auto &Type : Types_) {
    if (Type.Name == EmptyStringId) {
      continue;
    }
    if (!TypeNameIndex_.PushBack(
            TypeNameIndexEntry{.Hash = HashName(String(Type.Name)),
                               .Name = Type.Name,
                               .Type = Type.Index})) {
      return Library::MakeUnexpected(SymbolDatabaseError::AllocationFailure);
    }
  }

  NameIndex_.Sort([&](const NameIndexEntry &A, const NameIndexEntry &B) {
    if (A.Hash != B.Hash) {
      return A.Hash < B.Hash;
    }
    const auto Compare = String(A.Name).Compare(String(B.Name));
    return Compare != 0 ? Compare < 0 : A.Symbol < B.Symbol;
  });

  TypeNameIndex_.Sort(
      [&](const TypeNameIndexEntry &A, const TypeNameIndexEntry &B) {
        if (A.Hash != B.Hash) {
          return A.Hash < B.Hash;
        }
        const auto Compare = String(A.Name).Compare(String(B.Name));
        return Compare != 0 ? Compare < 0 : A.Type < B.Type;
      });

  AddressIndex_.Sort([&](SymbolId A, SymbolId B) {
    const auto &Left = Symbols_[A];
    const auto &Right = Symbols_[B];
    if (Left.Rva != Right.Rva) {
      return Left.Rva < Right.Rva;
    }
    if (Left.Size != Right.Size) {
      return Left.Size > Right.Size;
    }
    return Foundation::Cast::Underlying(Left.Kind) <
           Foundation::Cast::Underlying(Right.Kind);
  });

  Types_.Sort(
      [](const TypeInfo &A, const TypeInfo &B) { return A.Index < B.Index; });

  OmapToSource_.Sort(
      [](const OmapEntry &A, const OmapEntry &B) { return A.Rva < B.Rva; });
  OmapFromSource_.Sort(
      [](const OmapEntry &A, const OmapEntry &B) { return A.Rva < B.Rva; });

  SourceLines_.Sort(
      [](const SourceLine &A, const SourceLine &B) { return A.Rva < B.Rva; });

  for (auto &Value : Symbols_) {
    if (!Value.Rva)
      continue;
    const auto Location = SourceForRva(Value.Rva);
    if (Location.Valid()) {
      Value.Declaration = Location;
      Value.Flags |= SymbolFlag::HasSource;
    }
  }

  InlineSites_.Sort([](const InlineSite &A, const InlineSite &B) {
    if (A.StartRva != B.StartRva) {
      return A.StartRva < B.StartRva;
    }
    return A.EndRva > B.EndRva;
  });

  Ready_ = true;
  return {};
}

auto SymbolDatabase::NameMatches(const NameIndexEntry &Entry,
                                 Library::StringView Name) const
    -> Foundation::Bool {
  return Entry.Hash == HashName(Name) && String(Entry.Name) == Name;
}

auto SymbolDatabase::TypeNameMatches(const TypeNameIndexEntry &Entry,
                                     Library::StringView Name) const
    -> Foundation::Bool {
  return Entry.Hash == HashName(Name) && String(Entry.Name) == Name;
}

auto SymbolDatabase::FindExact(Library::StringView Name,
                               Library::Vector<SymbolId> &Results) const
    -> Foundation::Bool {
  Results.Clear();
  if (Name.Empty() || NameIndex_.Empty()) {
    return false;
  }

  const auto Hash = HashName(Name);
  Foundation::Size Left = 0;
  Foundation::Size Right = NameIndex_.Size();
  while (Left < Right) {
    const auto Middle = Left + (Right - Left) / 2;
    if (NameIndex_[Middle].Hash < Hash) {
      Left = Middle + 1;
    } else {
      Right = Middle;
    }
  }

  for (auto i = Left; i < NameIndex_.Size() && NameIndex_[i].Hash == Hash;
       ++i) {
    if (NameMatches(NameIndex_[i], Name) &&
        !Results.Contains(NameIndex_[i].Symbol)) {
      if (!Results.PushBack(NameIndex_[i].Symbol)) {
        return false;
      }
    }
  }
  return !Results.Empty();
}

auto SymbolDatabase::FindFirstExact(Library::StringView Name) const
    -> const Symbol * {
  Library::Vector<SymbolId> Results{Allocator_};
  if (!FindExact(Name, Results) || Results.Empty()) {
    return nullptr;
  }
  return SymbolById(Results[0]);
}

auto SymbolDatabase::FindFunction(Library::StringView Name) const
    -> const Symbol * {
  Library::Vector<SymbolId> Results{Allocator_};
  if (!FindExact(Name, Results))
    return nullptr;
  const Symbol *Best{};
  Foundation::Uint32 BestScore{};
  for (const auto Id : Results) {
    const auto *Value = SymbolById(Id);
    if (!Value || !IsFunctionKind(Value->Kind))
      continue;
    Foundation::Uint32 Score{};
    if (Value->Kind == SymbolKind::Function)
      Score += 8;
    if (Value->Size)
      Score += 4;
    if (Value->Type != InvalidTypeIndex)
      Score += 2;
    if (Value->Module != InvalidModuleId)
      Score += 1;
    if (Value->Source == ResolutionSource::ExactPdb)
      Score += 2;
    if (!Best || Score > BestScore) {
      Best = Value;
      BestScore = Score;
    }
  }
  return Best;
}

auto SymbolDatabase::FindGlobal(Library::StringView Name) const
    -> const Symbol * {
  Library::Vector<SymbolId> Results{Allocator_};
  if (!FindExact(Name, Results)) {
    return nullptr;
  }
  for (const auto Id : Results) {
    const auto *Value = SymbolById(Id);
    if (Value && IsGlobalKind(Value->Kind)) {
      return Value;
    }
  }
  return nullptr;
}

auto SymbolDatabase::FindType(Library::StringView Name) const
    -> const TypeInfo * {
  if (Name.Empty() || TypeNameIndex_.Empty()) {
    return nullptr;
  }
  const auto Hash = HashName(Name);
  Foundation::Size Left = 0;
  Foundation::Size Right = TypeNameIndex_.Size();
  while (Left < Right) {
    const auto Middle = Left + (Right - Left) / 2;
    if (TypeNameIndex_[Middle].Hash < Hash) {
      Left = Middle + 1;
    } else {
      Right = Middle;
    }
  }
  const TypeInfo *Best{};
  Foundation::Uint32 BestScore{};
  for (auto i = Left;
       i < TypeNameIndex_.Size() && TypeNameIndex_[i].Hash == Hash; ++i) {
    if (!TypeNameMatches(TypeNameIndex_[i], Name))
      continue;
    const auto *Candidate = TypeByIndex(TypeNameIndex_[i].Type);
    if (!Candidate)
      continue;
    Foundation::Uint32 Score{};
    const auto IsAggregate = Candidate->Kind == TypeKind::Struct ||
                             Candidate->Kind == TypeKind::Class ||
                             Candidate->Kind == TypeKind::Union ||
                             Candidate->Kind == TypeKind::Enum;
    if (IsAggregate) {
      if ((Candidate->Attributes & 0x0080u) == 0)
        Score += 16; // not a forward reference
      if (Candidate->FieldListType != InvalidTypeIndex &&
          Candidate->FieldListType != 0)
        Score += 8;
      if (Candidate->Size)
        Score += 4;
      if (Candidate->MemberCount)
        Score += 2;
    }
    if (Candidate->UniqueName != EmptyStringId)
      ++Score;
    if (!Best || Score > BestScore) {
      Best = Candidate;
      BestScore = Score;
    }
  }
  return Best;
}

auto SymbolDatabase::SymbolAtOrBeforeRva(Foundation::Uint64 Rva) const
    -> const Symbol * {
  if (AddressIndex_.Empty()) {
    return nullptr;
  }
  Foundation::Size Left = 0;
  Foundation::Size Right = AddressIndex_.Size();
  while (Left < Right) {
    const auto Middle = Left + (Right - Left) / 2;
    if (Symbols_[AddressIndex_[Middle]].Rva <= Rva) {
      Left = Middle + 1;
    } else {
      Right = Middle;
    }
  }
  if (!Left) {
    return nullptr;
  }
  return &Symbols_[AddressIndex_[Left - 1]];
}

auto SymbolDatabase::FunctionContainingRva(Foundation::Uint64 Rva) const
    -> const Symbol * {
  if (AddressIndex_.Empty()) {
    return nullptr;
  }
  Foundation::Size Left = 0;
  Foundation::Size Right = AddressIndex_.Size();
  while (Left < Right) {
    const auto Middle = Left + (Right - Left) / 2;
    if (Symbols_[AddressIndex_[Middle]].Rva <= Rva) {
      Left = Middle + 1;
    } else {
      Right = Middle;
    }
  }

  while (Left > 0) {
    --Left;
    const auto &Value = Symbols_[AddressIndex_[Left]];
    if (!IsFunctionKind(Value.Kind)) {
      continue;
    }
    if (Value.Rva > Rva) {
      continue;
    }
    if (Value.Size == 0) {
      if (Rva == Value.Rva)
        return &Value;
      continue;
    }
    if (Rva - Value.Rva < Value.Size) {
      return &Value;
    }
    if (Rva - Value.Rva > 0x100000) {
      break;
    }
  }
  return nullptr;
}

auto SymbolDatabase::SourceForRva(Foundation::Uint64 Rva) const
    -> SourceLocation {
  if (SourceLines_.Empty()) {
    return {};
  }
  Foundation::Size Left = 0;
  Foundation::Size Right = SourceLines_.Size();
  while (Left < Right) {
    const auto Middle = Left + (Right - Left) / 2;
    if (SourceLines_[Middle].Rva <= Rva) {
      Left = Middle + 1;
    } else {
      Right = Middle;
    }
  }
  if (!Left) {
    return {};
  }
  const auto &Line = SourceLines_[Left - 1];
  if (Line.EndRva && Rva >= Line.EndRva) {
    return {};
  }
  return Line.Location;
}

auto SymbolDatabase::MapRvaToSource(Foundation::Uint32 Rva) const noexcept
    -> Foundation::Uint32 {
  if (OmapToSource_.Empty()) {
    return Rva;
  }
  Foundation::Size Left = 0, Right = OmapToSource_.Size();
  while (Left < Right) {
    const auto Mid = Left + (Right - Left) / 2;
    if (OmapToSource_[Mid].Rva <= Rva)
      Left = Mid + 1;
    else
      Right = Mid;
  }
  if (!Left)
    return 0;
  const auto &Entry = OmapToSource_[Left - 1];
  return Entry.MappedRva ? Entry.MappedRva + (Rva - Entry.Rva) : 0;
}

auto SymbolDatabase::MapRvaFromSource(Foundation::Uint32 Rva) const noexcept
    -> Foundation::Uint32 {
  if (OmapFromSource_.Empty()) {
    return Rva;
  }
  Foundation::Size Left = 0, Right = OmapFromSource_.Size();
  while (Left < Right) {
    const auto Mid = Left + (Right - Left) / 2;
    if (OmapFromSource_[Mid].Rva <= Rva)
      Left = Mid + 1;
    else
      Right = Mid;
  }
  if (!Left)
    return 0;
  const auto &Entry = OmapFromSource_[Left - 1];
  return Entry.MappedRva ? Entry.MappedRva + (Rva - Entry.Rva) : 0;
}

auto SymbolDatabase::InlineStack(Foundation::Uint64 Rva,
                                 Library::Vector<InlineSite> &Frames) const
    -> Foundation::Bool {
  Frames.Clear();
  for (const auto &Site : InlineSites_) {
    if (Site.StartRva == 0 && Site.EndRva == 0) {
      continue;
    }
    if (Site.StartRva <= Rva && (Site.EndRva == 0 || Rva < Site.EndRva)) {
      if (!Frames.PushBack(Site)) {
        Frames.Clear();
        return false;
      }
    }
  }
  Frames.Sort([](const InlineSite &A, const InlineSite &B) {
    const auto SpanA =
        A.EndRva > A.StartRva ? A.EndRva - A.StartRva : ~Foundation::Uint64{};
    const auto SpanB =
        B.EndRva > B.StartRva ? B.EndRva - B.StartRva : ~Foundation::Uint64{};
    return SpanA > SpanB;
  });
  return !Frames.Empty();
}

auto SymbolDatabase::Symbolize(Foundation::Uint64 AddressOrRva,
                               Foundation::Bool IsRva) const
    -> SymbolizedAddress {
  SymbolizedAddress Result{Allocator_};
  Result.Address = IsRva || !ImageBase_ ? AddressOrRva : AddressOrRva;

  if (IsRva) {
    Result.Rva = AddressOrRva;
    if (ImageBase_) {
      Result.Address =
          Foundation::Cast::PointerToAddress<Foundation::Uint64>(ImageBase_) +
          AddressOrRva;
    }
  } else if (ImageBase_) {
    const auto Base =
        Foundation::Cast::PointerToAddress<Foundation::Uint64>(ImageBase_);
    if (AddressOrRva >= Base && AddressOrRva < Base + ImageSize_) {
      Result.Rva = AddressOrRva - Base;
    } else {
      Result.Rva = AddressOrRva;
    }
  } else {
    Result.Rva = AddressOrRva;
  }

  Result.SymbolValue = FunctionContainingRva(Result.Rva);
  if (!Result.SymbolValue) {
    Result.SymbolValue = SymbolAtOrBeforeRva(Result.Rva);
  }
  if (Result.SymbolValue && Result.Rva >= Result.SymbolValue->Rva) {
    Result.OffsetIntoSymbol = Result.Rva - Result.SymbolValue->Rva;
  }
  Result.Source = SourceForRva(Result.Rva);
  (void)InlineStack(Result.Rva, Result.InlineFrames);
  return Result;
}

auto SymbolDatabase::TypeFields(const TypeInfo &Type) const noexcept
    -> Library::Span<const TypeField> {
  if (Type.FieldBegin >= Fields_.Size() ||
      Foundation::Cast::Auto<Foundation::Size>(Type.FieldBegin) +
              Type.FieldCount >
          Fields_.Size()) {
    return {};
  }
  return {Fields_.Data() + Type.FieldBegin, Type.FieldCount};
}

auto SymbolDatabase::TypeParameters(const TypeInfo &Type) const noexcept
    -> Library::Span<const TypeIndex> {
  if (Type.ParameterBegin >= ParameterTypes_.Size() ||
      Foundation::Cast::Auto<Foundation::Size>(Type.ParameterBegin) +
              Type.ParameterTypeCount >
          ParameterTypes_.Size()) {
    return {};
  }
  return {ParameterTypes_.Data() + Type.ParameterBegin,
          Type.ParameterTypeCount};
}

auto SymbolDatabase::FieldOffset(Library::StringView TypeName,
                                 Library::StringView FieldName,
                                 Foundation::Uint64 &Offset) const
    -> Foundation::Bool {
  Offset = 0;
  const auto *Type = FindType(TypeName);
  if (!Type) {
    return false;
  }

  const TypeInfo *FieldOwner = Type;
  if (FieldOwner->FieldCount == 0 &&
      FieldOwner->FieldListType != InvalidTypeIndex) {
    const auto *List = TypeByIndex(FieldOwner->FieldListType);
    if (List) {
      FieldOwner = List;
    }
  }

  for (Foundation::Uint32 Depth = 0; FieldOwner && Depth < 64; ++Depth) {
    for (const auto &Field : TypeFields(*FieldOwner)) {
      if (String(Field.Name) == FieldName && !Field.IsStatic &&
          !Field.IsEnumerator) {
        Offset = Field.Offset;
        return true;
      }
    }
    FieldOwner = FieldOwner->ContinuationType != InvalidTypeIndex
                     ? TypeByIndex(FieldOwner->ContinuationType)
                     : nullptr;
  }
  return false;
}

auto SymbolDatabase::Allocator() const noexcept -> Memory::AllocatorStub {
  return Allocator_;
}

auto SymbolDatabase::HashName(Library::StringView Name) noexcept
    -> Foundation::Uint64 {
  if (Name.Empty()) {
    return 0;
  }
  auto Hash = FnvOffset;
  for (const auto Character : Name) {
    Hash ^= Foundation::Cast::Auto<Foundation::Uint8>(Character);
    Hash *= FnvPrime;
  }
  return Hash;
}

auto SymbolDatabase::NormalizeMsvcName(Library::StringView Name,
                                       Library::String &Qualified,
                                       Library::String &Base)
    -> Foundation::Bool {
  Qualified.Clear();
  Base.Clear();

  if (Name.Empty()) {
    return false;
  }

  if (Name[0] != '?') {
    if (!Qualified.Assign(Name)) {
      return false;
    }
    const auto Scope = Name.FindLast(Library::StringView{"::"});
    const auto BaseView =
        Scope == Library::StringView::NotFound ? Name : Name.Substr(Scope + 2);
    return Base.Assign(BaseView);
  }

  // This intentionally handles the common MSVC decorated form
  // ?Function@Class@Namespace@@... without pretending to be a full ABI
  // demangler.
  auto Cursor = Foundation::Size{1};
  Library::Vector<Library::StringView> Components{Qualified.Allocator()};
  while (Cursor < Name.Size()) {
    const auto Tail = Name.Substr(Cursor);
    const auto At = Tail.Find('@');
    if (At == Library::StringView::NotFound) {
      break;
    }
    if (At == 0) {
      break;
    }
    if (!Components.PushBack(Name.Substr(Cursor, At))) {
      return false;
    }
    Cursor += At + 1;
    if (Cursor < Name.Size() && Name[Cursor] == '@') {
      break;
    }
  }

  if (Components.Empty()) {
    if (!Qualified.Assign(Name)) {
      return false;
    }
    return Base.Assign(Name);
  }

  if (!Base.Assign(Components[0])) {
    return false;
  }
  for (Foundation::Size i = Components.Size(); i > 0; --i) {
    const auto Index = i - 1;
    if (!Qualified.Append(Components[Index])) {
      return false;
    }
    if (Index != 0 && !Qualified.Append(Library::StringView{"::"})) {
      return false;
    }
  }
  return true;
}
} // namespace UEFIpp::Reverse

namespace UEFIpp::Reverse {
SymbolizedAddress::SymbolizedAddress(Memory::AllocatorStub Allocator)
    : InlineFrames(Allocator) {}
} // namespace UEFIpp::Reverse
