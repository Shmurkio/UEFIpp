#pragma once

#include <UEFIpp/Architecture/X64/Disassembly/Decoder.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Expected.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Memory/AllocatorStub.hpp>
#include <UEFIpp/Reverse/SymbolDatabase.hpp>

namespace UEFIpp::Reverse {
enum class MatchClass : Foundation::Uint8 {
  Rejected,
  Ambiguous,
  Probable,
  Certain,
  Exact
};

enum class MatchError : Foundation::Uint8 {
  None,
  InvalidArgument,
  InvalidImage,
  UnsupportedImage,
  NoFunctionBoundaries,
  DecodeFailure,
  AllocationFailure,
  SymbolNotFound,
  NoCandidate
};

struct FunctionFingerprint {
  SymbolId Symbol{InvalidSymbolId};
  Foundation::Uint64 SemanticId{};
  Foundation::Uint64 Rva{};
  Foundation::Uint64 Size{};
  Foundation::Uint32 InstructionCount{};
  Foundation::Uint32 BasicBlockCount{};
  Foundation::Uint32 CallCount{};
  Foundation::Uint32 ConditionalBranchCount{};
  Foundation::Uint32 ReturnCount{};
  Foundation::Uint64 InstructionHash{};
  Foundation::Uint64 CfgHash{};
  Library::Vector<Foundation::Uint64> InstructionFeatures;
  Library::Vector<Foundation::Uint64> CfgFeatures;
  Library::Vector<Foundation::Uint64> StringHashes;
  Library::Vector<Foundation::Uint64> ConstantHashes;
  Library::Vector<Foundation::Uint64> CallTargets;
  Library::Vector<Foundation::Uint64> CallSemanticIds;

  explicit FunctionFingerprint(Memory::AllocatorStub Allocator = {});
};

struct ResolutionEvidence {
  Foundation::Float32 InstructionSimilarity{};
  Foundation::Float32 CfgSimilarity{};
  Foundation::Float32 StringSimilarity{};
  Foundation::Float32 CallGraphSimilarity{};
  Foundation::Float32 ConstantSimilarity{};
  Foundation::Float32 StructuralSimilarity{};
  Foundation::Float32 NeighborhoodSimilarity{};
};

struct SymbolMatch {
  const Symbol *Original{};
  Foundation::Uint64 NewRva{};
  Foundation::Float32 Confidence{};
  MatchClass Classification{MatchClass::Rejected};
  ResolutionSource Source{ResolutionSource::Heuristic};
  ResolutionEvidence Evidence{};
};

struct MatcherOptions {
  Foundation::Float32 CertainThreshold{0.90f};
  Foundation::Float32 ProbableThreshold{0.74f};
  Foundation::Float32 AmbiguityMargin{0.055f};
  Foundation::Uint32 MaximumFunctionBytes{0x10000};
  Foundation::Uint32 MaximumStringBytes{160};
};

class FunctionFingerprintBuilder {
public:
  constexpr FunctionFingerprintBuilder() = default;
  explicit FunctionFingerprintBuilder(Memory::AllocatorStub Allocator) noexcept;

  [[nodiscard]] auto Build(const Foundation::Void *ImageBase,
                           Foundation::Uint64 Rva, Foundation::Uint64 Size,
                           const SymbolDatabase *KnownSymbols = nullptr,
                           SymbolId SymbolValue = InvalidSymbolId,
                           Foundation::Uint64 SemanticId = 0,
                           const MatcherOptions &Options = {}) const
      -> Library::Expected<FunctionFingerprint, MatchError>;

private:
  Memory::AllocatorStub Allocator_{};
};

class CrossVersionSymbolMatcher {
public:
  constexpr CrossVersionSymbolMatcher() = default;
  explicit CrossVersionSymbolMatcher(Memory::AllocatorStub Allocator) noexcept;

  [[nodiscard]] auto Recover(const SymbolDatabase &OldDatabase,
                             const Foundation::Void *NewImageBase,
                             Library::StringView SymbolName,
                             const MatcherOptions &Options = {}) const
      -> Library::Expected<SymbolMatch, MatchError>;

  [[nodiscard]] auto RecoverAll(const SymbolDatabase &OldDatabase,
                                const Foundation::Void *NewImageBase,
                                Library::Vector<SymbolMatch> &Matches,
                                const MatcherOptions &Options = {}) const
      -> Library::Expected<Foundation::Void, MatchError>;

private:
  Memory::AllocatorStub Allocator_{};
};
} // namespace UEFIpp::Reverse
