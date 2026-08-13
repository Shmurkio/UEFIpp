#include <UEFIpp/Reverse/SymbolMatcher.hpp>

#include <UEFIpp/Architecture/X64/Instruction/Operand.hpp>
#include <UEFIpp/Executable/Pe/Image.hpp>

namespace UEFIpp::Reverse {
namespace {
inline constexpr auto FnvOffset = Foundation::Uint64{0xCBF29CE484222325ull};
inline constexpr auto FnvPrime = Foundation::Uint64{0x100000001B3ull};

#pragma pack(push, 1)
struct RuntimeFunction {
  Foundation::Uint32 BeginAddress;
  Foundation::Uint32 EndAddress;
  Foundation::Uint32 UnwindInfoAddress;
};
#pragma pack(pop)

struct CandidateBoundary {
  Foundation::Uint64 Rva{};
  Foundation::Uint64 Size{};
};

struct Anchor {
  Foundation::Uint64 SemanticId{};
  Foundation::Uint64 NewRva{};
};

[[nodiscard]] constexpr auto Mix(Foundation::Uint64 H,
                                 Foundation::Uint64 V) noexcept
    -> Foundation::Uint64 {
  H ^= V + 0x9E3779B97F4A7C15ull + (H << 6) + (H >> 2);
  return H * FnvPrime;
}

[[nodiscard]] auto HashBytes(const Foundation::Uint8 *Data,
                             Foundation::Uint64 Size) noexcept
    -> Foundation::Uint64 {
  auto H = FnvOffset;
  for (Foundation::Uint64 i = 0; i < Size; ++i) {
    H ^= Data[i];
    H *= FnvPrime;
  }
  return H;
}

[[nodiscard]] auto PopCount(Foundation::Uint64 V) noexcept
    -> Foundation::Uint32 {
  Foundation::Uint32 C{};
  while (V) {
    V &= V - 1;
    ++C;
  }
  return C;
}

[[nodiscard]] auto HashSimilarity(Foundation::Uint64 A,
                                  Foundation::Uint64 B) noexcept
    -> Foundation::Float32 {
  if (!A && !B)
    return 1.0f;
  if (!A || !B)
    return 0.0f;
  return 1.0f -
         Foundation::Cast::Auto<Foundation::Float32>(PopCount(A ^ B)) / 64.0f;
}

template <typename TInstruction>
[[nodiscard]] auto InstructionShape(const TInstruction &Instruction) noexcept
    -> Foundation::Uint64 {
  auto Hash = FnvOffset;
  Hash = Mix(Hash, Foundation::Cast::Auto<Foundation::Uint64>(
                       Instruction.MnemonicValue()));
  Hash = Mix(Hash, Instruction.OperandWidth());
  Hash = Mix(Hash, Instruction.AddressWidth());

  for (const auto &Operand : Instruction.VisibleOperands()) {
    Hash =
        Mix(Hash, Foundation::Cast::Auto<Foundation::Uint64>(Operand.Type()));
    Hash = Mix(Hash, Operand.SizeInBits());
    Hash = Mix(Hash, Operand.Access());

    if (Operand.Type() ==
        Architecture::X64::InstructionSet::OperandType::Memory) {
      const auto &Memory = Operand.MemoryValue();
      Hash = Mix(Hash, Foundation::Cast::Auto<Foundation::Uint64>(Memory.Type));
      Hash = Mix(Hash, Memory.Base ==
                               Architecture::X64::InstructionSet::Registers::Rip
                           ? 2
                           : (Memory.Base.IsValid() ? 1 : 0));
      Hash = Mix(Hash, Memory.Index.IsValid() ? 1 : 0);
      Hash = Mix(Hash, Memory.Scale);
      Hash = Mix(Hash, Memory.HasDisplacement ? 1 : 0);
    } else if (Operand.Type() ==
               Architecture::X64::InstructionSet::OperandType::Immediate) {
      const auto &Immediate = Operand.ImmediateValue();
      Hash = Mix(Hash, Immediate.IsRelative ? 1 : 0);
      Hash = Mix(Hash, Immediate.IsSigned ? 1 : 0);
    }
  }
  return Hash;
}

[[nodiscard]] auto RatioSimilarity(Foundation::Uint64 A,
                                   Foundation::Uint64 B) noexcept
    -> Foundation::Float32 {
  if (!A && !B)
    return 1.0f;
  if (!A || !B)
    return 0.0f;
  const auto Lo = A < B ? A : B, Hi = A > B ? A : B;
  return Foundation::Cast::Auto<Foundation::Float32>(Lo) /
         Foundation::Cast::Auto<Foundation::Float32>(Hi);
}

[[nodiscard]] auto
SetSimilarity(const Library::Vector<Foundation::Uint64> &A,
              const Library::Vector<Foundation::Uint64> &B) noexcept
    -> Foundation::Float32 {
  if (A.Empty() && B.Empty())
    return 1.0f;
  if (A.Empty() || B.Empty())
    return 0.0f;
  Foundation::Uint32 Common{};
  for (const auto X : A)
    if (B.Contains(X))
      ++Common;
  const auto Union =
      Foundation::Cast::Auto<Foundation::Uint32>(A.Size() + B.Size()) - Common;
  return Union ? Foundation::Cast::Auto<Foundation::Float32>(Common) / Union
               : 1.0f;
}

[[nodiscard]] auto IsFunction(SymbolKind K) noexcept -> Foundation::Bool {
  return K == SymbolKind::Function || K == SymbolKind::PublicFunction ||
         K == SymbolKind::Thunk;
}

[[nodiscard]] auto LooksLikeAscii(const Foundation::Uint8 *P,
                                  Foundation::Uint64 Max,
                                  Foundation::Uint64 &Len) noexcept
    -> Foundation::Bool {
  Len = 0;
  if (!P || Max < 4)
    return false;
  while (Len < Max && P[Len]) {
    const auto C = P[Len];
    if (C < 0x20 || C > 0x7E)
      return false;
    ++Len;
  }
  return Len >= 4 && Len < Max;
}

[[nodiscard]] auto LooksLikeUtf16(const Foundation::Uint8 *P,
                                  Foundation::Uint64 Max,
                                  Foundation::Uint64 &LenBytes) noexcept
    -> Foundation::Bool {
  LenBytes = 0;
  if (!P || Max < 10)
    return false;
  while (LenBytes + 1 < Max && (P[LenBytes] || P[LenBytes + 1])) {
    if (P[LenBytes + 1] != 0 || P[LenBytes] < 0x20 || P[LenBytes] > 0x7E)
      return false;
    LenBytes += 2;
  }
  return LenBytes >= 8 && LenBytes + 1 < Max;
}

[[nodiscard]] auto IsExecutableRva(const Executable::Pe::Image &Image,
                                   Foundation::Uint64 Rva) noexcept
    -> Foundation::Bool {
  constexpr auto Execute = Foundation::Uint32{0x20000000u};
  const auto *Sections = Image.SectionHeaders();
  for (Foundation::Uint16 i = 0; Sections && i < Image.SectionCount(); ++i) {
    if (!(Sections[i].Characteristics & Execute))
      continue;
    const auto Begin =
        Foundation::Cast::Auto<Foundation::Uint64>(Sections[i].VirtualAddress);
    const auto VirtualSize = Foundation::Cast::Auto<Foundation::Uint64>(
        Sections[i].Misc.VirtualSize);
    const auto RawSize =
        Foundation::Cast::Auto<Foundation::Uint64>(Sections[i].SizeOfRawData);
    const auto Size = VirtualSize > RawSize ? VirtualSize : RawSize;
    if (Rva >= Begin && Rva < Begin + Size)
      return true;
  }
  return false;
}

[[nodiscard]] auto DiscoverBoundaries(const Executable::Pe::Image &Image,
                                      Library::Vector<CandidateBoundary> &Out)
    -> Foundation::Bool {
  Out.Clear();
  auto Add = [&](Foundation::Uint64 Rva,
                 Foundation::Uint64 Size) -> Foundation::Bool {
    if (!Rva || Rva >= Image.ImageSize() || !IsExecutableRva(Image, Rva))
      return true;
    for (auto &Existing : Out) {
      if (Existing.Rva != Rva)
        continue;
      if (!Existing.Size && Size)
        Existing.Size = Size;
      return true;
    }
    return Out.PushBack({Rva, Size});
  };

  const auto Exception =
      Image.GetDirectory(Executable::Pe::DirectoryIndex::Exception);
  if (!Exception.Empty() && Exception.Size() >= sizeof(RuntimeFunction)) {
    const auto *Functions =
        Image.RvaToPointer<RuntimeFunction>(Exception.Rva());
    if (Functions) {
      const auto Count = Exception.Size() / sizeof(RuntimeFunction);
      for (Foundation::Uint32 i = 0; i < Count; ++i) {
        if (!Functions[i].BeginAddress ||
            Functions[i].EndAddress <= Functions[i].BeginAddress ||
            Functions[i].EndAddress > Image.ImageSize())
          continue;
        if (!Add(Functions[i].BeginAddress,
                 Functions[i].EndAddress - Functions[i].BeginAddress))
          return false;
      }
    }
  }

  if (Image.EntryPointRva() && !Add(Image.EntryPointRva(), 0))
    return false;
  for (const auto &Export : Image.Exports())
    if (!Export.Forwarded && !Add(Export.Rva, 0))
      return false;

  // Leaf functions often have no .pdata entry. Discover direct call targets by
  // linearly decoding executable sections and use those targets as additional
  // starts.
  Architecture::X64::Disassembly::Decoder Decoder{};
  const auto *Base =
      Foundation::Cast::Auto<const Foundation::Uint8 *>(Image.Base());
  const auto *Sections = Image.SectionHeaders();
  for (Foundation::Uint16 SectionIndex = 0;
       Sections && SectionIndex < Image.SectionCount(); ++SectionIndex) {
    constexpr auto Execute = Foundation::Uint32{0x20000000u};
    const auto &Section = Sections[SectionIndex];
    if (!(Section.Characteristics & Execute))
      continue;
    const auto Begin =
        Foundation::Cast::Auto<Foundation::Uint64>(Section.VirtualAddress);
    const auto VirtualSize =
        Foundation::Cast::Auto<Foundation::Uint64>(Section.Misc.VirtualSize);
    const auto RawSize =
        Foundation::Cast::Auto<Foundation::Uint64>(Section.SizeOfRawData);
    auto End = Begin + (VirtualSize > RawSize ? VirtualSize : RawSize);
    if (End > Image.ImageSize())
      End = Image.ImageSize();
    for (auto Cursor = Begin; Cursor < End;) {
      Library::Span<const Foundation::Byte> Code{
          Foundation::Cast::Auto<const Foundation::Byte *>(Base + Cursor),
          Foundation::Cast::Auto<Foundation::Size>(End - Cursor)};
      auto Decoded = Decoder.Decode(Code, Image.BaseAddress() + Cursor);
      if (!Decoded || !Decoded.Value().Length()) {
        ++Cursor;
        continue;
      }
      const auto &Instruction = Decoded.Value();
      if (Instruction.IsCall() && Instruction.DirectTarget()) {
        const auto Target = Instruction.DirectTarget().Value();
        if (Target >= Image.BaseAddress() &&
            Target < Image.BaseAddress() + Image.ImageSize())
          if (!Add(Target - Image.BaseAddress(), 0))
            return false;
      }
      Cursor += Instruction.Length();
    }
  }

  if (Out.Empty())
    return false;
  Out.Sort([](const CandidateBoundary &A, const CandidateBoundary &B) {
    return A.Rva < B.Rva;
  });
  for (Foundation::Size i = 0; i < Out.Size(); ++i) {
    if (Out[i].Size)
      continue;
    Foundation::Uint64 End = Image.ImageSize();
    if (i + 1 < Out.Size())
      End = Out[i + 1].Rva;
    if (End <= Out[i].Rva)
      continue;
    Out[i].Size = End - Out[i].Rva;
  }
  return true;
}

[[nodiscard]] auto FindAnchor(const Library::Vector<Anchor> &Anchors,
                              Foundation::Uint64 SemanticId,
                              Foundation::Uint64 &NewRva) noexcept
    -> Foundation::Bool {
  for (const auto &A : Anchors)
    if (A.SemanticId == SemanticId) {
      NewRva = A.NewRva;
      return true;
    }
  return false;
}

[[nodiscard]] auto CallGraphSimilarity(
    const FunctionFingerprint &Old, const FunctionFingerprint &New,
    const Library::Vector<Anchor> &Anchors) noexcept -> Foundation::Float32 {
  if (Old.CallSemanticIds.Empty())
    return RatioSimilarity(Old.CallCount, New.CallCount);
  Foundation::Uint32 Known{}, Matched{};
  for (const auto Id : Old.CallSemanticIds) {
    Foundation::Uint64 Target{};
    if (!FindAnchor(Anchors, Id, Target))
      continue;
    ++Known;
    if (New.CallTargets.Contains(Target))
      ++Matched;
  }
  return Known ? Foundation::Cast::Auto<Foundation::Float32>(Matched) / Known
               : RatioSimilarity(Old.CallCount, New.CallCount);
}

[[nodiscard]] auto StructuralSimilarity(const FunctionFingerprint &A,
                                        const FunctionFingerprint &B) noexcept
    -> Foundation::Float32 {
  return (RatioSimilarity(A.Size, B.Size) +
          RatioSimilarity(A.InstructionCount, B.InstructionCount) +
          RatioSimilarity(A.BasicBlockCount, B.BasicBlockCount) +
          RatioSimilarity(A.CallCount, B.CallCount) +
          RatioSimilarity(A.ConditionalBranchCount, B.ConditionalBranchCount) +
          RatioSimilarity(A.ReturnCount, B.ReturnCount)) /
         6.0f;
}

[[nodiscard]] auto NeighborhoodSimilarity(
    const Library::Vector<FunctionFingerprint> &OldFunctions,
    const FunctionFingerprint &Old, const FunctionFingerprint &Candidate,
    const Library::Vector<Anchor> &Anchors) noexcept -> Foundation::Float32 {
  Foundation::Bool HasPrevious{};
  Foundation::Bool HasNext{};
  Foundation::Uint64 PreviousOld{};
  Foundation::Uint64 PreviousNew{};
  Foundation::Uint64 NextOld{};
  Foundation::Uint64 NextNew{};

  for (const auto &Function : OldFunctions) {
    if (!Function.SemanticId || Function.SemanticId == Old.SemanticId)
      continue;
    Foundation::Uint64 AnchoredRva{};
    if (!FindAnchor(Anchors, Function.SemanticId, AnchoredRva))
      continue;

    if (Function.Rva < Old.Rva &&
        (!HasPrevious || Function.Rva > PreviousOld)) {
      HasPrevious = true;
      PreviousOld = Function.Rva;
      PreviousNew = AnchoredRva;
    } else if (Function.Rva > Old.Rva && (!HasNext || Function.Rva < NextOld)) {
      HasNext = true;
      NextOld = Function.Rva;
      NextNew = AnchoredRva;
    }
  }

  if (HasPrevious && HasNext) {
    if (PreviousNew >= NextNew || Candidate.Rva <= PreviousNew ||
        Candidate.Rva >= NextNew)
      return 0.0f;
    const auto OldSpan = NextOld - PreviousOld;
    const auto NewSpan = NextNew - PreviousNew;
    if (!OldSpan || !NewSpan)
      return 0.0f;
    const auto OldPosition =
        Foundation::Cast::Auto<Foundation::Float32>(Old.Rva - PreviousOld) /
        Foundation::Cast::Auto<Foundation::Float32>(OldSpan);
    const auto NewPosition =
        Foundation::Cast::Auto<Foundation::Float32>(Candidate.Rva -
                                                    PreviousNew) /
        Foundation::Cast::Auto<Foundation::Float32>(NewSpan);
    const auto Delta = OldPosition > NewPosition ? OldPosition - NewPosition
                                                 : NewPosition - OldPosition;
    return Delta >= 1.0f ? 0.0f : 1.0f - Delta;
  }

  if (HasPrevious) {
    if (Candidate.Rva <= PreviousNew)
      return 0.0f;
    return RatioSimilarity(Old.Rva - PreviousOld, Candidate.Rva - PreviousNew);
  }

  if (HasNext) {
    if (Candidate.Rva >= NextNew)
      return 0.0f;
    return RatioSimilarity(NextOld - Old.Rva, NextNew - Candidate.Rva);
  }

  return 0.0f;
}

[[nodiscard]] auto
Score(const FunctionFingerprint &A, const FunctionFingerprint &B,
      const Library::Vector<Anchor> &Anchors, Foundation::Float32 Neighborhood,
      ResolutionEvidence &E) noexcept -> Foundation::Float32 {
  E.InstructionSimilarity =
      A.InstructionHash == B.InstructionHash
          ? 1.0f
          : SetSimilarity(A.InstructionFeatures, B.InstructionFeatures);
  E.CfgSimilarity = A.CfgHash == B.CfgHash
                        ? 1.0f
                        : SetSimilarity(A.CfgFeatures, B.CfgFeatures);
  E.StringSimilarity = SetSimilarity(A.StringHashes, B.StringHashes);
  E.ConstantSimilarity = SetSimilarity(A.ConstantHashes, B.ConstantHashes);
  E.CallGraphSimilarity = CallGraphSimilarity(A, B, Anchors);
  E.StructuralSimilarity = StructuralSimilarity(A, B);
  E.NeighborhoodSimilarity = Neighborhood;
  const auto StringWeight =
      (A.StringHashes.Empty() && B.StringHashes.Empty()) ? 0.05f : 0.18f;
  const auto CallWeight = Anchors.Empty() ? 0.07f : 0.14f;
  const auto Other =
      1.0f - (0.34f + 0.18f + StringWeight + CallWeight + 0.07f + 0.05f);
  return E.InstructionSimilarity * 0.34f + E.CfgSimilarity * 0.18f +
         E.StringSimilarity * StringWeight +
         E.CallGraphSimilarity * CallWeight + E.ConstantSimilarity * 0.07f +
         E.StructuralSimilarity * Other + E.NeighborhoodSimilarity * 0.05f;
}

[[nodiscard]] auto Classification(Foundation::Float32 ScoreValue,
                                  Foundation::Float32 Margin,
                                  const MatcherOptions &O) noexcept
    -> MatchClass {
  if (ScoreValue >= 0.999f)
    return MatchClass::Exact;
  if (Margin < O.AmbiguityMargin && ScoreValue >= O.ProbableThreshold)
    return MatchClass::Ambiguous;
  if (ScoreValue >= O.CertainThreshold)
    return MatchClass::Certain;
  if (ScoreValue >= O.ProbableThreshold)
    return MatchClass::Probable;
  if (ScoreValue >= 0.60f)
    return MatchClass::Ambiguous;
  return MatchClass::Rejected;
}
} // namespace

auto FunctionFingerprintBuilder::Build(
    const Foundation::Void *ImageBase, Foundation::Uint64 Rva,
    Foundation::Uint64 Size, const SymbolDatabase *Known, SymbolId SymbolValue,
    Foundation::Uint64 SemanticId, const MatcherOptions &Options) const
    -> Library::Expected<FunctionFingerprint, MatchError> {
  Executable::Pe::Image Image{ImageBase, Allocator_};
  if (!Image.IsValid() || Rva >= Image.ImageSize() || !Size)
    return Library::MakeUnexpected(MatchError::InvalidArgument);

  if (Rva + Size < Rva)
    return Library::MakeUnexpected(MatchError::InvalidArgument);
  if (Rva + Size > Image.ImageSize())
    Size = Image.ImageSize() - Rva;
  if (Size > Options.MaximumFunctionBytes)
    Size = Options.MaximumFunctionBytes;

  FunctionFingerprint Fingerprint{Allocator_};
  Fingerprint.Symbol = SymbolValue;
  Fingerprint.SemanticId = SemanticId;
  Fingerprint.Rva = Rva;
  Fingerprint.Size = Size;

  auto InstructionHash = FnvOffset;
  auto CfgHash = FnvOffset;
  auto PreviousInstructionShape = FnvOffset;
  Architecture::X64::Disassembly::Decoder Decoder{};
  const auto *Base =
      Foundation::Cast::Auto<const Foundation::Uint8 *>(ImageBase);
  Foundation::Uint64 Offset{};

  while (Offset < Size) {
    const auto Remaining = Size - Offset;
    Library::Span<const Foundation::Byte> Code{
        Foundation::Cast::Auto<const Foundation::Byte *>(Base + Rva + Offset),
        Foundation::Cast::Auto<Foundation::Size>(Remaining)};
    auto Decoded = Decoder.Decode(Code, Image.BaseAddress() + Rva + Offset);
    if (!Decoded)
      break;

    const auto &Instruction = Decoded.Value();
    if (!Instruction.Length() || Instruction.Length() > Remaining)
      break;

    ++Fingerprint.InstructionCount;
    const auto Shape = InstructionShape(Instruction);
    InstructionHash = Mix(InstructionHash, Shape);

    // A normalized two-instruction shingle preserves local ordering while
    // remaining insensitive to relocated displacements and register allocation
    // details.
    const auto Shingle = Mix(PreviousInstructionShape, Shape);
    if (!Fingerprint.InstructionFeatures.Contains(Shingle) &&
        !Fingerprint.InstructionFeatures.PushBack(Shingle))
      return Library::MakeUnexpected(MatchError::AllocationFailure);
    PreviousInstructionShape = Shape;

    for (const auto &Operand : Instruction.VisibleOperands()) {
      if (Operand.Type() ==
          Architecture::X64::InstructionSet::OperandType::Memory) {
        const auto &Memory = Operand.MemoryValue();
        if (Memory.HasAbsoluteAddress &&
            Memory.AbsoluteAddress >= Image.BaseAddress() &&
            Memory.AbsoluteAddress < Image.BaseAddress() + Image.ImageSize()) {
          const auto Target = Memory.AbsoluteAddress - Image.BaseAddress();
          Foundation::Uint64 Length{};
          const auto Available = Image.ImageSize() - Target;
          const auto Maximum = Available < Options.MaximumStringBytes
                                   ? Available
                                   : Options.MaximumStringBytes;
          if (LooksLikeAscii(Base + Target, Maximum, Length) ||
              LooksLikeUtf16(Base + Target, Maximum, Length)) {
            const auto Hash = HashBytes(Base + Target, Length);
            if (!Fingerprint.StringHashes.Contains(Hash) &&
                !Fingerprint.StringHashes.PushBack(Hash))
              return Library::MakeUnexpected(MatchError::AllocationFailure);
          }
        }
      } else if (Operand.Type() ==
                 Architecture::X64::InstructionSet::OperandType::Immediate) {
        const auto &Immediate = Operand.ImmediateValue();
        if (!Immediate.IsRelative) {
          const auto Value = Immediate.UnsignedValue;
          if (Value > 0xFF &&
              !(Value >= Image.BaseAddress() &&
                Value < Image.BaseAddress() + Image.ImageSize())) {
            const auto Hash = Mix(FnvOffset, Value);
            if (!Fingerprint.ConstantHashes.Contains(Hash) &&
                !Fingerprint.ConstantHashes.PushBack(Hash))
              return Library::MakeUnexpected(MatchError::AllocationFailure);
          }
        }
      }
    }

    auto CfgFeature = FnvOffset;
    Foundation::Bool HasCfgFeature{};
    if (Instruction.IsConditionalBranch()) {
      ++Fingerprint.ConditionalBranchCount;
      ++Fingerprint.BasicBlockCount;
      CfgFeature = Mix(CfgFeature, 1);
      HasCfgFeature = true;
    } else if (Instruction.IsUnconditionalBranch()) {
      ++Fingerprint.BasicBlockCount;
      CfgFeature = Mix(CfgFeature, 2);
      HasCfgFeature = true;
    } else if (Instruction.IsReturn()) {
      ++Fingerprint.ReturnCount;
      ++Fingerprint.BasicBlockCount;
      CfgFeature = Mix(CfgFeature, 3);
      HasCfgFeature = true;
    }

    if (Instruction.DirectTarget()) {
      const auto Target = Instruction.DirectTarget().Value();
      const auto FunctionBase = Image.BaseAddress() + Rva;
      const auto Internal =
          Target >= FunctionBase && Target < FunctionBase + Size;
      CfgFeature = Mix(CfgFeature, Internal ? 4 : 5);
      if (Internal)
        CfgFeature = Mix(CfgFeature, (Target - FunctionBase) / 16);
      HasCfgFeature = true;

      if (Instruction.IsCall()) {
        ++Fingerprint.CallCount;
        if (Target >= Image.BaseAddress() &&
            Target < Image.BaseAddress() + Image.ImageSize()) {
          const auto TargetRva = Target - Image.BaseAddress();
          if (!Fingerprint.CallTargets.Contains(TargetRva) &&
              !Fingerprint.CallTargets.PushBack(TargetRva))
            return Library::MakeUnexpected(MatchError::AllocationFailure);

          if (Known) {
            const auto *TargetSymbol = Known->FunctionContainingRva(TargetRva);
            if (TargetSymbol && TargetSymbol->SemanticId &&
                !Fingerprint.CallSemanticIds.Contains(
                    TargetSymbol->SemanticId) &&
                !Fingerprint.CallSemanticIds.PushBack(TargetSymbol->SemanticId))
              return Library::MakeUnexpected(MatchError::AllocationFailure);
          }
        }
      }
    }

    if (HasCfgFeature) {
      CfgHash = Mix(CfgHash, CfgFeature);
      if (!Fingerprint.CfgFeatures.Contains(CfgFeature) &&
          !Fingerprint.CfgFeatures.PushBack(CfgFeature))
        return Library::MakeUnexpected(MatchError::AllocationFailure);
    }

    Offset += Instruction.Length();
  }

  if (!Fingerprint.InstructionCount)
    return Library::MakeUnexpected(MatchError::DecodeFailure);
  if (!Fingerprint.BasicBlockCount)
    Fingerprint.BasicBlockCount = 1;
  Fingerprint.InstructionHash = InstructionHash;
  Fingerprint.CfgHash = CfgHash;
  return Fingerprint;
}

auto CrossVersionSymbolMatcher::Recover(const SymbolDatabase &Old,
                                        const Foundation::Void *NewImageBase,
                                        Library::StringView Name,
                                        const MatcherOptions &Options) const
    -> Library::Expected<SymbolMatch, MatchError> {
  const auto *Target = Old.FindFunction(Name);
  if (!Target)
    return Library::MakeUnexpected(MatchError::SymbolNotFound);
  Library::Vector<SymbolMatch> All{Allocator_};
  auto R = RecoverAll(Old, NewImageBase, All, Options);
  if (!R)
    return Library::MakeUnexpected(R.Error());
  for (const auto &M : All)
    if (M.Original && M.Original->Id == Target->Id)
      return M;
  return Library::MakeUnexpected(MatchError::NoCandidate);
}

auto CrossVersionSymbolMatcher::RecoverAll(
    const SymbolDatabase &Old, const Foundation::Void *NewImageBase,
    Library::Vector<SymbolMatch> &Matches, const MatcherOptions &Options) const
    -> Library::Expected<Foundation::Void, MatchError> {
  Matches.Clear();
  if (!Old.Ready() || !Old.ImageBase() || !NewImageBase)
    return Library::MakeUnexpected(MatchError::InvalidArgument);
  Executable::Pe::Image NewImage{NewImageBase, Allocator_};
  if (!NewImage.IsValid() || !NewImage.Is64())
    return Library::MakeUnexpected(MatchError::UnsupportedImage);
  Library::Vector<CandidateBoundary> Bounds{Allocator_};
  if (!DiscoverBoundaries(NewImage, Bounds))
    return Library::MakeUnexpected(MatchError::NoFunctionBoundaries);
  FunctionFingerprintBuilder Builder{Allocator_};
  Library::Vector<FunctionFingerprint> OldF{Allocator_}, NewF{Allocator_};
  for (const auto &S : Old.Symbols()) {
    if (!IsFunction(S.Kind) || !S.Rva || !S.Size)
      continue;
    Foundation::Bool Duplicate{};
    for (const auto &Existing : OldF)
      if (Existing.Rva == S.Rva) {
        Duplicate = true;
        break;
      }
    if (Duplicate)
      continue;
    auto F = Builder.Build(Old.ImageBase(), S.Rva, S.Size, &Old, S.Id,
                           S.SemanticId, Options);
    if (F && !OldF.PushBack(Foundation::Utility::Move(F.Value())))
      return Library::MakeUnexpected(MatchError::AllocationFailure);
  }
  for (const auto &B : Bounds) {
    auto F = Builder.Build(NewImageBase, B.Rva, B.Size, nullptr,
                           InvalidSymbolId, 0, Options);
    if (F && !NewF.PushBack(Foundation::Utility::Move(F.Value())))
      return Library::MakeUnexpected(MatchError::AllocationFailure);
  }
  if (OldF.Empty() || NewF.Empty())
    return Library::MakeUnexpected(MatchError::NoCandidate);
  OldF.Sort([](const FunctionFingerprint &A, const FunctionFingerprint &B) {
    return A.Rva < B.Rva;
  });

  Library::Vector<Anchor> Anchors{Allocator_};
  Library::Vector<Foundation::Bool> Used{NewF.Size(), false, Allocator_};
  for (const auto &E : NewImage.Exports()) {
    if (E.Name.Empty() || E.Forwarded)
      continue;
    const auto *S = Old.FindFunction(E.Name.View());
    if (!S || !S->SemanticId)
      continue;

    Foundation::Bool HasAnchor{};
    for (const auto &Existing : Anchors)
      if (Existing.SemanticId == S->SemanticId) {
        HasAnchor = true;
        break;
      }
    if (!HasAnchor && !Anchors.PushBack({S->SemanticId, E.Rva}))
      return Library::MakeUnexpected(MatchError::AllocationFailure);

    for (Foundation::Size I = 0; I < NewF.Size(); ++I) {
      if (NewF[I].Rva != E.Rva)
        continue;
      Used[I] = true;
      ResolutionEvidence Evidence{};
      Evidence.InstructionSimilarity = 1.0f;
      Evidence.CfgSimilarity = 1.0f;
      Evidence.StringSimilarity = 1.0f;
      Evidence.CallGraphSimilarity = 1.0f;
      Evidence.ConstantSimilarity = 1.0f;
      Evidence.StructuralSimilarity = 1.0f;
      Evidence.NeighborhoodSimilarity = 1.0f;
      if (!Matches.PushBack({S, E.Rva, 1.0f, MatchClass::Exact,
                             ResolutionSource::PeExport, Evidence}))
        return Library::MakeUnexpected(MatchError::AllocationFailure);
      break;
    }
  }
  for (Foundation::Uint32 Pass = 0; Pass < 3; ++Pass) {
    for (const auto &OF : OldF) {
      Foundation::Bool Already{};
      for (const auto &M : Matches)
        if (M.Original && M.Original->Id == OF.Symbol) {
          Already = true;
          break;
        }
      if (Already)
        continue;
      Foundation::Float32 Best = -1.0f, Second = -1.0f;
      Foundation::Size BestIndex = 0;
      ResolutionEvidence BestE{};
      for (Foundation::Size j = 0; j < NewF.Size(); ++j) {
        if (Used[j])
          continue;
        const auto Neighborhood =
            NeighborhoodSimilarity(OldF, OF, NewF[j], Anchors);
        ResolutionEvidence E{};
        const auto S = Score(OF, NewF[j], Anchors, Neighborhood, E);
        if (S > Best) {
          Second = Best;
          Best = S;
          BestIndex = j;
          BestE = E;
        } else if (S > Second)
          Second = S;
      }
      if (Best < 0)
        continue;
      const auto Margin = Best - Second;
      const auto ExactSignature =
          OF.InstructionHash == NewF[BestIndex].InstructionHash &&
          OF.CfgHash == NewF[BestIndex].CfgHash &&
          OF.InstructionCount == NewF[BestIndex].InstructionCount;
      const auto Class = ExactSignature ? MatchClass::Exact
                                        : Classification(Best, Margin, Options);
      const auto *Original = Old.SymbolById(OF.Symbol);
      if (!Original)
        continue;
      if (Class == MatchClass::Certain || Class == MatchClass::Exact ||
          (Pass == 2 && Class == MatchClass::Probable)) {
        SymbolMatch M{Original,
                      NewF[BestIndex].Rva,
                      ExactSignature ? 1.0f : Best,
                      Class,
                      ExactSignature ? ResolutionSource::Signature
                                     : ResolutionSource::Heuristic,
                      BestE};
        if (!Matches.PushBack(M))
          return Library::MakeUnexpected(MatchError::AllocationFailure);
        Used[BestIndex] = true;
        if (Original->SemanticId) {
          Foundation::Bool HasAnchor{};
          for (const auto &Existing : Anchors)
            if (Existing.SemanticId == Original->SemanticId) {
              HasAnchor = true;
              break;
            }
          if (!HasAnchor &&
              !Anchors.PushBack({Original->SemanticId, NewF[BestIndex].Rva}))
            return Library::MakeUnexpected(MatchError::AllocationFailure);
        }
      }
    }
  }
  Matches.Sort([](const SymbolMatch &A, const SymbolMatch &B) {
    return A.Confidence > B.Confidence;
  });
  return {};
}
} // namespace UEFIpp::Reverse

namespace UEFIpp::Reverse {
FunctionFingerprint::FunctionFingerprint(Memory::AllocatorStub Allocator)
    : InstructionFeatures(Allocator), CfgFeatures(Allocator),
      StringHashes(Allocator), ConstantHashes(Allocator),
      CallTargets(Allocator), CallSemanticIds(Allocator) {}

FunctionFingerprintBuilder::FunctionFingerprintBuilder(
    Memory::AllocatorStub Allocator) noexcept
    : Allocator_(Allocator) {}

CrossVersionSymbolMatcher::CrossVersionSymbolMatcher(
    Memory::AllocatorStub Allocator) noexcept
    : Allocator_(Allocator) {}
} // namespace UEFIpp::Reverse
