#include <UEFIpp/Reverse/SymbolMatcher.hpp>
#include <UEFIpp/Reverse/SymbolProvider.hpp>

namespace UEFIpp::Reverse {
namespace {
[[nodiscard]] auto KindMatches(SymbolKind Requested, SymbolKind Actual) noexcept
    -> Foundation::Bool {
  if (Requested == SymbolKind::Unknown)
    return true;
  if (Requested == SymbolKind::Function) {
    return Actual == SymbolKind::Function ||
           Actual == SymbolKind::PublicFunction || Actual == SymbolKind::Thunk;
  }
  return Requested == Actual;
}

[[nodiscard]] auto QueryMatches(const SymbolQuery &Query, const Symbol &Value,
                                ResolutionSource SourceFilter) noexcept
    -> Foundation::Bool {
  if (!KindMatches(Query.Kind, Value.Kind))
    return false;
  if (Query.HasVisibility && Query.Visibility != Value.Visibility)
    return false;
  if (Query.HasModule && Query.Module != Value.Module)
    return false;
  if (Query.HasSource && Query.Source != Value.Source)
    return false;
  if (SourceFilter != ResolutionSource::Unknown && SourceFilter != Value.Source)
    return false;
  return true;
}

[[nodiscard]] auto MatchQueryMatches(const SymbolQuery &Query,
                                     const SymbolMatch &Match,
                                     ResolutionSource SourceFilter) noexcept
    -> Foundation::Bool {
  if (!Match.Original)
    return false;
  if (!KindMatches(Query.Kind, Match.Original->Kind))
    return false;
  if (Query.HasVisibility && Query.Visibility != Match.Original->Visibility)
    return false;
  if (Query.HasModule && Query.Module != Match.Original->Module)
    return false;
  if (Query.HasSource && Query.Source != Match.Source)
    return false;
  if (SourceFilter != ResolutionSource::Unknown && SourceFilter != Match.Source)
    return false;
  return true;
}
} // namespace

auto DatabaseSymbolProvider::Resolve(const SymbolQuery &Query,
                                     SymbolResolutionSet &Results) const
    -> Foundation::Bool {
  if (!Database_ || !Database_->Ready())
    return false;
  const auto Before = Results.Size();

  if (Query.HasRva) {
    const auto *Value = Database_->FunctionContainingRva(Query.Rva);
    if (!Value)
      Value = Database_->SymbolAtOrBeforeRva(Query.Rva);
    if (Value && QueryMatches(Query, *Value, SourceFilter_)) {
      if (!Results.PushBack({Value, Value->Rva, Value->Source, 1.0f}))
        return false;
    }
  }

  if (!Query.Name.Empty()) {
    Library::Vector<SymbolId> Matches{Database_->Allocator()};
    if (Database_->FindExact(Query.Name, Matches)) {
      for (const auto Id : Matches) {
        const auto *Value = Database_->SymbolById(Id);
        if (!Value || !QueryMatches(Query, *Value, SourceFilter_))
          continue;
        if (!Results.PushBack({Value, Value->Rva, Value->Source, 1.0f}))
          return false;
      }
    } else if (!Query.ExactName) {
      for (const auto &Value : Database_->Symbols()) {
        if (!QueryMatches(Query, Value, SourceFilter_))
          continue;
        const auto BaseName = Database_->String(Value.BaseName);
        const auto QualifiedName = Database_->String(Value.QualifiedName);
        if (BaseName.Find(Query.Name) == Library::StringView::NotFound &&
            QualifiedName.Find(Query.Name) == Library::StringView::NotFound)
          continue;
        if (!Results.PushBack({&Value, Value.Rva, Value.Source, 0.85f}))
          return false;
      }
    }
  }
  return Results.Size() != Before;
}

auto MatchSymbolProvider::Resolve(const SymbolQuery &Query,
                                  SymbolResolutionSet &Results) const
    -> Foundation::Bool {
  if (!Matches_ || !Count_)
    return false;
  const auto Before = Results.Size();
  for (Foundation::Size I = 0; I < Count_; ++I) {
    const auto &Match = Matches_[I];
    if (!MatchQueryMatches(Query, Match, SourceFilter_))
      continue;
    if (Query.HasRva && Query.Rva != Match.NewRva)
      continue;
    if (!Query.Name.Empty()) {
      if (!OriginalDatabase_)
        continue;
      const auto Name = OriginalDatabase_->String(Match.Original->Name);
      const auto Qualified =
          OriginalDatabase_->String(Match.Original->QualifiedName);
      const auto Base = OriginalDatabase_->String(Match.Original->BaseName);
      const auto Decorated =
          OriginalDatabase_->String(Match.Original->DecoratedName);
      if (Query.ExactName) {
        if (!(Name == Query.Name) && !(Qualified == Query.Name) &&
            !(Base == Query.Name) && !(Decorated == Query.Name))
          continue;
      } else if (Name.Find(Query.Name) == Library::StringView::NotFound &&
                 Qualified.Find(Query.Name) == Library::StringView::NotFound &&
                 Base.Find(Query.Name) == Library::StringView::NotFound &&
                 Decorated.Find(Query.Name) == Library::StringView::NotFound)
        continue;
    }
    if (!Results.PushBack(
            {Match.Original, Match.NewRva, Match.Source, Match.Confidence}))
      return false;
  }
  return Results.Size() != Before;
}

auto SymbolProviderChain::Resolve(const SymbolQuery &Query,
                                  SymbolResolutionSet &Results) const
    -> Foundation::Bool {
  Results.Clear();
  for (const auto &Provider : Providers_)
    (void)Provider.Resolve(Query, Results);
  Results.Sort([](const SymbolResolution &A, const SymbolResolution &B) {
    if (A.Confidence != B.Confidence)
      return A.Confidence > B.Confidence;
    return Foundation::Cast::Underlying(A.Source) <
           Foundation::Cast::Underlying(B.Source);
  });
  return !Results.Empty();
}

auto SymbolProviderChain::ResolveBest(const SymbolQuery &Query,
                                      SymbolResolution &Result) const
    -> Foundation::Bool {
  SymbolResolutionSet Results{Allocator_};
  if (!Resolve(Query, Results))
    return false;
  Result = Results[0];
  return true;
}
} // namespace UEFIpp::Reverse

namespace UEFIpp::Reverse {
auto SymbolProviderStub::Resolve(const SymbolQuery &Query,
                                 SymbolResolutionSet &Results) const
    -> Foundation::Bool {
  return Resolve_ ? Resolve_(Context_, Query, Results) : false;
}

SymbolProviderChain::SymbolProviderChain(
    Memory::AllocatorStub Allocator) noexcept
    : Providers_(Allocator), Allocator_(Allocator) {}

auto SymbolProviderChain::Add(SymbolProviderStub Provider) -> Foundation::Bool {
  return Provider && Providers_.PushBack(Provider);
}
} // namespace UEFIpp::Reverse
