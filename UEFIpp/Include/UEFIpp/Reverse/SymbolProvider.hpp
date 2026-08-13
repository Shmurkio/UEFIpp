#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/String/StringView.hpp>
#include <UEFIpp/Memory/AllocatorStub.hpp>
#include <UEFIpp/Reverse/SymbolDatabase.hpp>

namespace UEFIpp::Reverse {
struct SymbolMatch;

struct SymbolQuery {
  Library::StringView Name{};
  SymbolKind Kind{SymbolKind::Unknown};
  SymbolVisibility Visibility{SymbolVisibility::Unknown};
  ResolutionSource Source{ResolutionSource::Unknown};
  ModuleId Module{InvalidModuleId};
  Foundation::Uint64 Rva{};
  Foundation::Bool HasRva{};
  Foundation::Bool ExactName{true};
  Foundation::Bool HasVisibility{};
  Foundation::Bool HasSource{};
  Foundation::Bool HasModule{};
};

struct SymbolResolution {
  const Symbol *SymbolValue{};
  Foundation::Uint64 Rva{};
  ResolutionSource Source{ResolutionSource::Unknown};
  Foundation::Float32 Confidence{};
};

using SymbolResolutionSet = Library::Vector<SymbolResolution>;

class SymbolProviderStub {
public:
  using ResolveFunction = auto (*)(const Foundation::Void *,
                                   const SymbolQuery &, SymbolResolutionSet &)
      -> Foundation::Bool;

  constexpr SymbolProviderStub() = default;
  constexpr SymbolProviderStub(const Foundation::Void *Context,
                               ResolveFunction Resolve) noexcept
      : Context_(Context), Resolve_(Resolve) {}

  template <typename TProvider>
  [[nodiscard]] static constexpr auto From(const TProvider &Provider) noexcept
      -> SymbolProviderStub {
    return {
        &Provider, [](const Foundation::Void *Context, const SymbolQuery &Query,
                      SymbolResolutionSet &Results) {
          return Foundation::Cast::Auto<const TProvider *>(Context)->Resolve(
              Query, Results);
        }};
  }

  [[nodiscard]] constexpr explicit operator Foundation::Bool() const noexcept {
    return Context_ && Resolve_;
  }
  [[nodiscard]] auto Resolve(const SymbolQuery &Query,
                             SymbolResolutionSet &Results) const
      -> Foundation::Bool;

private:
  const Foundation::Void *Context_{};
  ResolveFunction Resolve_{};
};

class DatabaseSymbolProvider {
public:
  constexpr DatabaseSymbolProvider() = default;
  constexpr explicit DatabaseSymbolProvider(
      const SymbolDatabase *Database,
      ResolutionSource SourceFilter = ResolutionSource::Unknown) noexcept
      : Database_(Database), SourceFilter_(SourceFilter) {}

  [[nodiscard]] auto Resolve(const SymbolQuery &Query,
                             SymbolResolutionSet &Results) const
      -> Foundation::Bool;

private:
  const SymbolDatabase *Database_{};
  ResolutionSource SourceFilter_{ResolutionSource::Unknown};
};

class PdbSymbolProvider final : public DatabaseSymbolProvider {
public:
  constexpr PdbSymbolProvider() = default;
  constexpr explicit PdbSymbolProvider(const SymbolDatabase *Database) noexcept
      : DatabaseSymbolProvider(Database, ResolutionSource::ExactPdb) {}
};

class PeExportSymbolProvider final : public DatabaseSymbolProvider {
public:
  constexpr PeExportSymbolProvider() = default;
  constexpr explicit PeExportSymbolProvider(
      const SymbolDatabase *Database) noexcept
      : DatabaseSymbolProvider(Database, ResolutionSource::PeExport) {}
};

class MatchSymbolProvider {
public:
  constexpr MatchSymbolProvider() = default;
  constexpr MatchSymbolProvider(
      const SymbolMatch *Matches, Foundation::Size Count,
      const SymbolDatabase *OriginalDatabase = nullptr,
      ResolutionSource SourceFilter = ResolutionSource::Unknown) noexcept
      : Matches_(Matches), Count_(Count), OriginalDatabase_(OriginalDatabase),
        SourceFilter_(SourceFilter) {}

  [[nodiscard]] auto Resolve(const SymbolQuery &Query,
                             SymbolResolutionSet &Results) const
      -> Foundation::Bool;

private:
  const SymbolMatch *Matches_{};
  Foundation::Size Count_{};
  const SymbolDatabase *OriginalDatabase_{};
  ResolutionSource SourceFilter_{ResolutionSource::Unknown};
};

class SignatureSymbolProvider final : public MatchSymbolProvider {
public:
  constexpr SignatureSymbolProvider() = default;
  constexpr SignatureSymbolProvider(
      const SymbolMatch *Matches, Foundation::Size Count,
      const SymbolDatabase *OriginalDatabase = nullptr) noexcept
      : MatchSymbolProvider(Matches, Count, OriginalDatabase,
                            ResolutionSource::Signature) {}
};

class HeuristicSymbolProvider final : public MatchSymbolProvider {
public:
  constexpr HeuristicSymbolProvider() = default;
  constexpr HeuristicSymbolProvider(
      const SymbolMatch *Matches, Foundation::Size Count,
      const SymbolDatabase *OriginalDatabase = nullptr) noexcept
      : MatchSymbolProvider(Matches, Count, OriginalDatabase,
                            ResolutionSource::Heuristic) {}
};

class SymbolProviderChain {
public:
  constexpr SymbolProviderChain() = default;
  explicit SymbolProviderChain(Memory::AllocatorStub Allocator) noexcept;

  [[nodiscard]] auto Add(SymbolProviderStub Provider) -> Foundation::Bool;
  [[nodiscard]] auto Resolve(const SymbolQuery &Query,
                             SymbolResolutionSet &Results) const
      -> Foundation::Bool;
  [[nodiscard]] auto ResolveBest(const SymbolQuery &Query,
                                 SymbolResolution &Result) const
      -> Foundation::Bool;

private:
  Library::Vector<SymbolProviderStub> Providers_{};
  Memory::AllocatorStub Allocator_{};
};
} // namespace UEFIpp::Reverse
