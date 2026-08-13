#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Expected.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Memory/AllocatorStub.hpp>
#include <UEFIpp/UEFI/Guid.hpp>

namespace UEFIpp::Reverse {
using SymbolId = Foundation::Uint32;
using ModuleId = Foundation::Uint32;
using StringId = Foundation::Uint32;
using TypeIndex = Foundation::Uint32;
using InlineSiteId = Foundation::Uint32;

inline constexpr auto InvalidSymbolId = SymbolId{0xFFFFFFFFu};
inline constexpr auto InvalidModuleId = ModuleId{0xFFFFFFFFu};
inline constexpr auto InvalidTypeIndex = TypeIndex{0xFFFFFFFFu};
inline constexpr auto IpiTypeIndexMask = TypeIndex{0x80000000u};

[[nodiscard]] constexpr auto MakeIpiTypeIndex(TypeIndex Index) noexcept
    -> TypeIndex {
  return Index == InvalidTypeIndex ? Index : (Index | IpiTypeIndexMask);
}
[[nodiscard]] constexpr auto IsIpiTypeIndex(TypeIndex Index) noexcept
    -> Foundation::Bool {
  return Index != InvalidTypeIndex && (Index & IpiTypeIndexMask) != 0;
}
[[nodiscard]] constexpr auto RawTypeIndex(TypeIndex Index) noexcept
    -> TypeIndex {
  return IsIpiTypeIndex(Index) ? (Index & ~IpiTypeIndexMask) : Index;
}
inline constexpr auto EmptyStringId = StringId{0};
inline constexpr auto InvalidInlineSiteId = InlineSiteId{0xFFFFFFFFu};

enum class SymbolKind : Foundation::Uint8 {
  Unknown,
  Function,
  PublicFunction,
  Thunk,
  GlobalVariable,
  StaticVariable,
  ThreadLocal,
  Label,
  Constant,
  UserDefinedType,
  LocalVariable,
  Parameter
};

enum class SymbolVisibility : Foundation::Uint8 {
  Unknown,
  Public,
  Global,
  Local
};

enum class ResolutionSource : Foundation::Uint8 {
  Unknown,
  ExactPdb,
  PeExport,
  Signature,
  Heuristic
};

enum class TypeKind : Foundation::Uint8 {
  Unknown,
  Primitive,
  Modifier,
  Pointer,
  Array,
  Procedure,
  MemberFunction,
  ArgumentList,
  FieldList,
  Struct,
  Class,
  Union,
  Enum,
  Alias,
  BitField,
  FunctionId,
  MemberFunctionId,
  StringId
};

enum class CallingConvention : Foundation::Uint8 {
  Unknown = 0xFF,
  NearC = 0x00,
  FarC = 0x01,
  NearPascal = 0x02,
  FarPascal = 0x03,
  NearFast = 0x04,
  FarFast = 0x05,
  NearStd = 0x07,
  FarStd = 0x08,
  NearSys = 0x09,
  FarSys = 0x0A,
  ThisCall = 0x0B,
  MipsCall = 0x0C,
  Generic = 0x0D,
  AlphaCall = 0x0E,
  PpcCall = 0x0F,
  ShCall = 0x10,
  ArmCall = 0x11,
  Am33Call = 0x12,
  TriCall = 0x13,
  Sh5Call = 0x14,
  M32RCall = 0x15,
  ClrCall = 0x16,
  Inline = 0x17,
  NearVector = 0x18,
  Swift = 0x19
};

enum class SymbolFlag : Foundation::Uint32 {
  None = 0,
  Code = 1u << 0,
  Function = 1u << 1,
  Managed = 1u << 2,
  MsvcDecorated = 1u << 3,
  CompilerGenerated = 1u << 4,
  Optimized = 1u << 5,
  HasType = 1u << 6,
  HasSource = 1u << 7,
  HasInlineSites = 1u << 8
};

[[nodiscard]] constexpr auto operator|(SymbolFlag Left,
                                       SymbolFlag Right) noexcept
    -> SymbolFlag {
  return Foundation::Cast::Auto<SymbolFlag>(
      Foundation::Cast::Underlying(Left) | Foundation::Cast::Underlying(Right));
}

[[nodiscard]] constexpr auto operator&(SymbolFlag Left,
                                       SymbolFlag Right) noexcept
    -> SymbolFlag {
  return Foundation::Cast::Auto<SymbolFlag>(
      Foundation::Cast::Underlying(Left) & Foundation::Cast::Underlying(Right));
}

constexpr auto operator|=(SymbolFlag &Left, SymbolFlag Right) noexcept
    -> SymbolFlag & {
  Left = Left | Right;
  return Left;
}

[[nodiscard]] constexpr auto HasFlag(SymbolFlag Value, SymbolFlag Flag) noexcept
    -> Foundation::Bool {
  return (Foundation::Cast::Underlying(Value) &
          Foundation::Cast::Underlying(Flag)) != 0;
}

struct PdbIdentity {
  UEFI::Guid Guid{};
  Foundation::Uint32 Age{};

  [[nodiscard]] constexpr auto Valid() const noexcept -> Foundation::Bool {
    return Guid.IsValid() && Age != 0;
  }

  [[nodiscard]] constexpr auto operator<=>(const PdbIdentity &) const = default;
};

struct SourceLocation {
  StringId File{};
  Foundation::Uint32 Line{};
  Foundation::Uint32 Column{};

  [[nodiscard]] constexpr auto Valid() const noexcept -> Foundation::Bool {
    return File != EmptyStringId || Line != 0;
  }
};

struct ModuleInfo {
  ModuleId Id{InvalidModuleId};
  StringId Name{};
  StringId ObjectFile{};
  Foundation::Uint16 StreamIndex{0xFFFFu};
  Foundation::Uint32 SymbolBytes{};
  Foundation::Uint32 C11Bytes{};
  Foundation::Uint32 C13Bytes{};
};

struct Symbol {
  SymbolId Id{InvalidSymbolId};
  StringId Name{};
  StringId DecoratedName{};
  StringId QualifiedName{};
  StringId BaseName{};
  SymbolKind Kind{SymbolKind::Unknown};
  SymbolVisibility Visibility{SymbolVisibility::Unknown};
  ResolutionSource Source{ResolutionSource::Unknown};
  SymbolFlag Flags{SymbolFlag::None};
  Foundation::Uint64 Rva{};
  Foundation::Uint64 Size{};
  Foundation::Uint64 SemanticId{};
  TypeIndex Type{InvalidTypeIndex};
  ModuleId Module{InvalidModuleId};
  SymbolId Parent{InvalidSymbolId};
  SourceLocation Declaration{};
};

struct LocalVariable {
  StringId Name{};
  TypeIndex Type{InvalidTypeIndex};
  SymbolId Function{InvalidSymbolId};
  Foundation::Int64 Offset{};
  Foundation::Uint16 Register{};
  Foundation::Uint16 Flags{};
  Foundation::Bool RegisterRelative{};
  Foundation::Bool FrameRelative{};
  Foundation::Bool IsParameter{};
};

struct TypeField {
  StringId Name{};
  TypeIndex Type{InvalidTypeIndex};
  Foundation::Uint64 Offset{};
  Foundation::Uint64 Value{};
  Foundation::Uint16 Attributes{};
  Foundation::Bool IsStatic{};
  Foundation::Bool IsBaseClass{};
  Foundation::Bool IsEnumerator{};
};

struct TypeInfo {
  TypeIndex Index{InvalidTypeIndex};
  TypeKind Kind{TypeKind::Unknown};
  StringId Name{};
  StringId UniqueName{};
  Foundation::Uint64 Size{};
  Foundation::Uint32 Options{};
  Foundation::Uint32 Attributes{};
  TypeIndex ReferentType{InvalidTypeIndex};
  TypeIndex ElementType{InvalidTypeIndex};
  TypeIndex IndexType{InvalidTypeIndex};
  TypeIndex ReturnType{InvalidTypeIndex};
  TypeIndex ClassType{InvalidTypeIndex};
  TypeIndex ThisType{InvalidTypeIndex};
  TypeIndex UnderlyingType{InvalidTypeIndex};
  TypeIndex FieldListType{InvalidTypeIndex};
  TypeIndex DerivationListType{InvalidTypeIndex};
  TypeIndex VTableShapeType{InvalidTypeIndex};
  TypeIndex ContinuationType{InvalidTypeIndex};
  TypeIndex ArgumentListType{InvalidTypeIndex};
  TypeIndex ParentScope{InvalidTypeIndex};
  CallingConvention Convention{CallingConvention::Unknown};
  Foundation::Uint32 MemberCount{};
  Foundation::Uint32 ParameterCount{};
  Foundation::Int32 ThisAdjustment{};
  Foundation::Uint32 FieldBegin{};
  Foundation::Uint32 FieldCount{};
  Foundation::Uint32 ParameterBegin{};
  Foundation::Uint32 ParameterTypeCount{};
};

struct OmapEntry {
  Foundation::Uint32 Rva{};
  Foundation::Uint32 MappedRva{};
};

struct SectionMapEntry {
  Foundation::Uint16 Flags{};
  Foundation::Uint16 Overlay{};
  Foundation::Uint16 Group{};
  Foundation::Uint16 Frame{};
  Foundation::Uint16 SectionName{};
  Foundation::Uint16 ClassName{};
  Foundation::Uint32 Offset{};
  Foundation::Uint32 Length{};
};

struct SourceLine {
  Foundation::Uint64 Rva{};
  Foundation::Uint64 EndRva{};
  SourceLocation Location{};
  ModuleId Module{InvalidModuleId};
  Foundation::Bool Statement{};
};

struct InlineSite {
  InlineSiteId Id{InvalidInlineSiteId};
  SymbolId ParentFunction{InvalidSymbolId};
  InlineSiteId ParentInlineSite{InvalidInlineSiteId};
  TypeIndex Inlinee{InvalidTypeIndex};
  StringId Name{};
  SourceLocation Declaration{};
  Foundation::Uint64 StartRva{};
  Foundation::Uint64 EndRva{};
};

struct SymbolizedAddress {
  const Symbol *SymbolValue{};
  Foundation::Uint64 Address{};
  Foundation::Uint64 Rva{};
  Foundation::Uint64 OffsetIntoSymbol{};
  SourceLocation Source{};
  Library::Vector<InlineSite> InlineFrames;

  explicit SymbolizedAddress(Memory::AllocatorStub Allocator = {});
};

enum class SymbolDatabaseError : Foundation::Uint8 {
  None,
  AllocationFailure,
  InvalidArgument,
  InvalidSymbol,
  InvalidType
};

class SymbolDatabase {
public:
  constexpr SymbolDatabase() = default;
  explicit SymbolDatabase(Memory::AllocatorStub Allocator) noexcept;

  SymbolDatabase(const SymbolDatabase &) = delete;
  auto operator=(const SymbolDatabase &) -> SymbolDatabase & = delete;
  SymbolDatabase(SymbolDatabase &&) noexcept = default;
  auto operator=(SymbolDatabase &&) noexcept -> SymbolDatabase & = default;

  auto Reset() -> Foundation::Void;

  [[nodiscard]] auto Ready() const noexcept -> Foundation::Bool;
  auto SetReady(Foundation::Bool Value) noexcept -> Foundation::Void;

  [[nodiscard]] auto ImageBase() const noexcept -> const Foundation::Void *;
  [[nodiscard]] auto ImageSize() const noexcept -> Foundation::Uint64;
  auto SetImage(const Foundation::Void *Base, Foundation::Uint64 Size) noexcept
      -> Foundation::Void;

  [[nodiscard]] auto Identity() const noexcept -> const PdbIdentity &;
  auto SetIdentity(const PdbIdentity &Identity) noexcept -> Foundation::Void;

  [[nodiscard]] auto Strings() const noexcept
      -> Library::Span<const Library::String>;
  [[nodiscard]] auto Symbols() const noexcept -> Library::Span<const Symbol>;
  [[nodiscard]] auto Modules() const noexcept
      -> Library::Span<const ModuleInfo>;
  [[nodiscard]] auto Types() const noexcept -> Library::Span<const TypeInfo>;
  [[nodiscard]] auto Fields() const noexcept -> Library::Span<const TypeField>;
  [[nodiscard]] auto ParameterTypes() const noexcept
      -> Library::Span<const TypeIndex>;
  [[nodiscard]] auto SourceLines() const noexcept
      -> Library::Span<const SourceLine>;
  [[nodiscard]] auto SectionMap() const noexcept
      -> Library::Span<const SectionMapEntry>;
  [[nodiscard]] auto OmapToSource() const noexcept
      -> Library::Span<const OmapEntry>;
  [[nodiscard]] auto OmapFromSource() const noexcept
      -> Library::Span<const OmapEntry>;
  [[nodiscard]] auto InlineSites() const noexcept
      -> Library::Span<const InlineSite>;
  [[nodiscard]] auto Locals() const noexcept
      -> Library::Span<const LocalVariable>;

  [[nodiscard]] auto String(StringId Id) const noexcept -> Library::StringView;
  [[nodiscard]] auto SymbolById(SymbolId Id) const noexcept -> const Symbol *;
  [[nodiscard]] auto MutableSymbolById(SymbolId Id) noexcept -> Symbol *;
  [[nodiscard]] auto MutableInlineSites() noexcept -> Library::Span<InlineSite>;
  [[nodiscard]] auto ModuleById(ModuleId Id) const noexcept
      -> const ModuleInfo *;
  [[nodiscard]] auto TypeByIndex(TypeIndex Index) const noexcept
      -> const TypeInfo *;

  [[nodiscard]] auto Intern(Library::StringView Value)
      -> Library::Expected<StringId, SymbolDatabaseError>;

  [[nodiscard]] auto AddModule(const ModuleInfo &Value)
      -> Library::Expected<ModuleId, SymbolDatabaseError>;
  [[nodiscard]] auto AddSymbol(Symbol Value)
      -> Library::Expected<SymbolId, SymbolDatabaseError>;
  [[nodiscard]] auto AddType(const TypeInfo &Value)
      -> Library::Expected<Foundation::Void, SymbolDatabaseError>;
  [[nodiscard]] auto AddField(const TypeField &Value)
      -> Library::Expected<Foundation::Uint32, SymbolDatabaseError>;
  [[nodiscard]] auto AddParameterType(TypeIndex Value)
      -> Library::Expected<Foundation::Uint32, SymbolDatabaseError>;
  [[nodiscard]] auto AddSectionMap(const SectionMapEntry &Value)
      -> Library::Expected<Foundation::Void, SymbolDatabaseError>;
  [[nodiscard]] auto AddOmapToSource(const OmapEntry &Value)
      -> Library::Expected<Foundation::Void, SymbolDatabaseError>;
  [[nodiscard]] auto AddOmapFromSource(const OmapEntry &Value)
      -> Library::Expected<Foundation::Void, SymbolDatabaseError>;
  [[nodiscard]] auto AddSourceLine(const SourceLine &Value)
      -> Library::Expected<Foundation::Void, SymbolDatabaseError>;
  [[nodiscard]] auto AddInlineSite(const InlineSite &Value)
      -> Library::Expected<InlineSiteId, SymbolDatabaseError>;
  [[nodiscard]] auto AddLocal(const LocalVariable &Value)
      -> Library::Expected<Foundation::Void, SymbolDatabaseError>;

  [[nodiscard]] auto Finalize()
      -> Library::Expected<Foundation::Void, SymbolDatabaseError>;

  [[nodiscard]] auto FindExact(Library::StringView Name,
                               Library::Vector<SymbolId> &Results) const
      -> Foundation::Bool;

  [[nodiscard]] auto FindFirstExact(Library::StringView Name) const
      -> const Symbol *;
  [[nodiscard]] auto FindFunction(Library::StringView Name) const
      -> const Symbol *;
  [[nodiscard]] auto FindGlobal(Library::StringView Name) const
      -> const Symbol *;
  [[nodiscard]] auto FindType(Library::StringView Name) const
      -> const TypeInfo *;
  [[nodiscard]] auto FunctionContainingRva(Foundation::Uint64 Rva) const
      -> const Symbol *;
  [[nodiscard]] auto SymbolAtOrBeforeRva(Foundation::Uint64 Rva) const
      -> const Symbol *;
  [[nodiscard]] auto SourceForRva(Foundation::Uint64 Rva) const
      -> SourceLocation;
  [[nodiscard]] auto MapRvaToSource(Foundation::Uint32 Rva) const noexcept
      -> Foundation::Uint32;
  [[nodiscard]] auto MapRvaFromSource(Foundation::Uint32 Rva) const noexcept
      -> Foundation::Uint32;
  [[nodiscard]] auto InlineStack(Foundation::Uint64 Rva,
                                 Library::Vector<InlineSite> &Frames) const
      -> Foundation::Bool;
  [[nodiscard]] auto Symbolize(Foundation::Uint64 AddressOrRva,
                               Foundation::Bool IsRva = false) const
      -> SymbolizedAddress;

  [[nodiscard]] auto TypeFields(const TypeInfo &Type) const noexcept
      -> Library::Span<const TypeField>;
  [[nodiscard]] auto TypeParameters(const TypeInfo &Type) const noexcept
      -> Library::Span<const TypeIndex>;
  [[nodiscard]] auto FieldOffset(Library::StringView TypeName,
                                 Library::StringView FieldName,
                                 Foundation::Uint64 &Offset) const
      -> Foundation::Bool;

  [[nodiscard]] auto Allocator() const noexcept -> Memory::AllocatorStub;

  [[nodiscard]] static auto HashName(Library::StringView Name) noexcept
      -> Foundation::Uint64;
  [[nodiscard]] static auto NormalizeMsvcName(Library::StringView Name,
                                              Library::String &Qualified,
                                              Library::String &Base)
      -> Foundation::Bool;

private:
  struct NameIndexEntry {
    Foundation::Uint64 Hash{};
    StringId Name{};
    SymbolId Symbol{InvalidSymbolId};
  };

  struct TypeNameIndexEntry {
    Foundation::Uint64 Hash{};
    StringId Name{};
    TypeIndex Type{InvalidTypeIndex};
  };

  [[nodiscard]] auto AddNameIndex(StringId Name, SymbolId Symbol)
      -> Foundation::Bool;
  [[nodiscard]] auto NameMatches(const NameIndexEntry &Entry,
                                 Library::StringView Name) const
      -> Foundation::Bool;
  [[nodiscard]] auto TypeNameMatches(const TypeNameIndexEntry &Entry,
                                     Library::StringView Name) const
      -> Foundation::Bool;
  [[nodiscard]] auto FindTypeStorageIndex(TypeIndex Index) const noexcept
      -> Foundation::Size;

private:
  Memory::AllocatorStub Allocator_{};
  const Foundation::Void *ImageBase_{};
  Foundation::Uint64 ImageSize_{};
  PdbIdentity Identity_{};
  Foundation::Bool Ready_{};

  Library::Vector<Library::String> Strings_{};
  Library::Vector<Symbol> Symbols_{};
  Library::Vector<ModuleInfo> Modules_{};
  Library::Vector<TypeInfo> Types_{};
  Library::Vector<TypeField> Fields_{};
  Library::Vector<TypeIndex> ParameterTypes_{};
  Library::Vector<SourceLine> SourceLines_{};
  Library::Vector<SectionMapEntry> SectionMap_{};
  Library::Vector<OmapEntry> OmapToSource_{};
  Library::Vector<OmapEntry> OmapFromSource_{};
  Library::Vector<InlineSite> InlineSites_{};
  Library::Vector<LocalVariable> Locals_{};
  Library::Vector<NameIndexEntry> NameIndex_{};
  Library::Vector<TypeNameIndexEntry> TypeNameIndex_{};
  Library::Vector<SymbolId> AddressIndex_{};
};

} // namespace UEFIpp::Reverse
