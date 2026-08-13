#include <UEFIpp/Reverse/Pdb.hpp>

#include <UEFIpp/Executable/Pe/Image.hpp>
#include <UEFIpp/Memory/Memory.hpp>

namespace UEFIpp::Reverse::Pdb
{
	namespace
	{
		inline constexpr Foundation::Char Msf7Magic[32] = {
			'M','i','c','r','o','s','o','f','t',' ','C','/','C','+','+',' ','M','S','F',' ','7','.','0','0','\r','\n',
			Foundation::Char{0x1A}, 'D','S',0,0,0
		};
		inline constexpr auto DeletedStream = Foundation::Uint32{0xFFFFFFFFu};
		inline constexpr auto InvalidStream = Foundation::Uint16{0xFFFFu};

		inline constexpr auto S_END = Foundation::Uint16{0x0006};
		inline constexpr auto S_THUNK32 = Foundation::Uint16{0x1102};
		inline constexpr auto S_LABEL32 = Foundation::Uint16{0x1105};
		inline constexpr auto S_CONSTANT = Foundation::Uint16{0x1107};
		inline constexpr auto S_UDT = Foundation::Uint16{0x1108};
		inline constexpr auto S_BPREL32 = Foundation::Uint16{0x110B};
		inline constexpr auto S_LDATA32 = Foundation::Uint16{0x110C};
		inline constexpr auto S_GDATA32 = Foundation::Uint16{0x110D};
		inline constexpr auto S_PUB32 = Foundation::Uint16{0x110E};
		inline constexpr auto S_LPROC32 = Foundation::Uint16{0x110F};
		inline constexpr auto S_GPROC32 = Foundation::Uint16{0x1110};
		inline constexpr auto S_REGREL32 = Foundation::Uint16{0x1111};
		inline constexpr auto S_LTHREAD32 = Foundation::Uint16{0x1112};
		inline constexpr auto S_GTHREAD32 = Foundation::Uint16{0x1113};
		inline constexpr auto S_LOCAL = Foundation::Uint16{0x113E};
		inline constexpr auto S_LPROC32_ID = Foundation::Uint16{0x1146};
		inline constexpr auto S_GPROC32_ID = Foundation::Uint16{0x1147};
		inline constexpr auto S_INLINESITE = Foundation::Uint16{0x114D};
		inline constexpr auto S_INLINESITE_END = Foundation::Uint16{0x114E};

		inline constexpr auto LF_MODIFIER = Foundation::Uint16{0x1001};
		inline constexpr auto LF_POINTER = Foundation::Uint16{0x1002};
		inline constexpr auto LF_PROCEDURE = Foundation::Uint16{0x1008};
		inline constexpr auto LF_MFUNCTION = Foundation::Uint16{0x1009};
		inline constexpr auto LF_BCLASS = Foundation::Uint16{0x1400};
		inline constexpr auto LF_INDEX = Foundation::Uint16{0x1404};
		inline constexpr auto LF_ARGLIST = Foundation::Uint16{0x1201};
		inline constexpr auto LF_FIELDLIST = Foundation::Uint16{0x1203};
		inline constexpr auto LF_BITFIELD = Foundation::Uint16{0x1205};
		inline constexpr auto LF_ENUMERATE = Foundation::Uint16{0x1502};
		inline constexpr auto LF_ARRAY = Foundation::Uint16{0x1503};
		inline constexpr auto LF_CLASS = Foundation::Uint16{0x1504};
		inline constexpr auto LF_STRUCTURE = Foundation::Uint16{0x1505};
		inline constexpr auto LF_UNION = Foundation::Uint16{0x1506};
		inline constexpr auto LF_ENUM = Foundation::Uint16{0x1507};
		inline constexpr auto LF_ALIAS = Foundation::Uint16{0x150A};
		inline constexpr auto LF_MEMBER = Foundation::Uint16{0x150D};
		inline constexpr auto LF_STMEMBER = Foundation::Uint16{0x150E};
		inline constexpr auto LF_METHOD = Foundation::Uint16{0x150F};
		inline constexpr auto LF_NESTTYPE = Foundation::Uint16{0x1510};
		inline constexpr auto LF_ONEMETHOD = Foundation::Uint16{0x1511};
		inline constexpr auto LF_FUNC_ID = Foundation::Uint16{0x1601};
		inline constexpr auto LF_MFUNC_ID = Foundation::Uint16{0x1602};
		inline constexpr auto LF_STRING_ID = Foundation::Uint16{0x1605};

		inline constexpr auto DEBUG_S_LINES = Foundation::Uint32{0xF2};
		inline constexpr auto DEBUG_S_STRINGTABLE = Foundation::Uint32{0xF3};
		inline constexpr auto DEBUG_S_FILECHKSMS = Foundation::Uint32{0xF4};
		inline constexpr auto DEBUG_S_INLINEELINES = Foundation::Uint32{0xF6};

		inline constexpr auto IMAGE_DEBUG_TYPE_CODEVIEW = Foundation::Uint32{2};
		inline constexpr auto CV_SIGNATURE_RSDS = Foundation::Uint32{0x53445352u};
		inline constexpr auto IMAGE_SCN_MEM_EXECUTE = Foundation::Uint32{0x20000000u};

#pragma pack(push, 1)
		struct MsfSuperBlock7
		{
			Foundation::Char Magic[32];
			Foundation::Uint32 BlockSize;
			Foundation::Uint32 FreeBlockMapBlock;
			Foundation::Uint32 NumBlocks;
			Foundation::Uint32 NumDirectoryBytes;
			Foundation::Uint32 Unknown;
			Foundation::Uint32 BlockMapAddr;
		};

		struct InfoStreamHeader
		{
			Foundation::Uint32 Version;
			Foundation::Uint32 Signature;
			Foundation::Uint32 Age;
			UEFI::Guid Guid;
		};

		struct DbiStreamHeader
		{
			Foundation::Int32 VersionSignature;
			Foundation::Uint32 VersionHeader;
			Foundation::Uint32 Age;
			Foundation::Uint16 GlobalSymbolStreamIndex;
			Foundation::Uint16 BuildNumber;
			Foundation::Uint16 PublicSymbolStreamIndex;
			Foundation::Uint16 PdbDllVersion;
			Foundation::Uint16 SymRecordStreamIndex;
			Foundation::Uint16 PdbDllRbld;
			Foundation::Int32 ModInfoSize;
			Foundation::Int32 SectionContributionSize;
			Foundation::Int32 SectionMapSize;
			Foundation::Int32 FileInfoSize;
			Foundation::Int32 TypeServerMapSize;
			Foundation::Uint32 MFCTypeServerIndex;
			Foundation::Int32 OptionalDbgHeaderSize;
			Foundation::Int32 ECSubstreamSize;
			Foundation::Uint16 Flags;
			Foundation::Uint16 Machine;
			Foundation::Uint32 Padding;
		};

		struct SectionContrib
		{
			Foundation::Uint16 Section;
			Foundation::Uint16 Padding1;
			Foundation::Int32 Offset;
			Foundation::Int32 Size;
			Foundation::Uint32 Characteristics;
			Foundation::Uint16 ModuleIndex;
			Foundation::Uint16 Padding2;
			Foundation::Uint32 DataCrc;
			Foundation::Uint32 RelocCrc;
		};

		struct ModuleInfoHeader
		{
			Foundation::Uint32 ModulePointer;
			SectionContrib Contribution;
			Foundation::Uint16 Flags;
			Foundation::Uint16 ModuleDebugStream;
			Foundation::Uint32 SymbolBytes;
			Foundation::Uint32 C11Bytes;
			Foundation::Uint32 C13Bytes;
			Foundation::Uint16 SourceFileCount;
			Foundation::Uint16 Padding;
			Foundation::Uint32 FileNameOffsets;
			Foundation::Uint32 SourceFileNameIndex;
			Foundation::Uint32 PdbFilePathNameIndex;
		};

		struct TpiStreamHeader
		{
			Foundation::Uint32 Version;
			Foundation::Uint32 HeaderSize;
			Foundation::Uint32 TypeIndexBegin;
			Foundation::Uint32 TypeIndexEnd;
			Foundation::Uint32 TypeRecordBytes;
			Foundation::Uint16 HashStreamIndex;
			Foundation::Uint16 HashAuxStreamIndex;
			Foundation::Uint32 HashKeySize;
			Foundation::Uint32 NumHashBuckets;
			Foundation::Int32 HashValueBufferOffset;
			Foundation::Uint32 HashValueBufferLength;
			Foundation::Int32 IndexOffsetBufferOffset;
			Foundation::Uint32 IndexOffsetBufferLength;
			Foundation::Int32 HashAdjBufferOffset;
			Foundation::Uint32 HashAdjBufferLength;
		};

		struct ImageDebugDirectory
		{
			Foundation::Uint32 Characteristics;
			Foundation::Uint32 TimeDateStamp;
			Foundation::Uint16 MajorVersion;
			Foundation::Uint16 MinorVersion;
			Foundation::Uint32 Type;
			Foundation::Uint32 SizeOfData;
			Foundation::Uint32 AddressOfRawData;
			Foundation::Uint32 PointerToRawData;
		};

		struct RsdsHeader
		{
			Foundation::Uint32 Signature;
			UEFI::Guid Guid;
			Foundation::Uint32 Age;
		};
#pragma pack(pop)

		static_assert(sizeof(DbiStreamHeader) == 64);
		static_assert(sizeof(ModuleInfoHeader) == 64);
		static_assert(sizeof(InfoStreamHeader) == 28);

		[[nodiscard]] constexpr auto ErrorAt(
			Error Code,
			Foundation::Uint32 Stream = 0xFFFFFFFFu,
			Foundation::Uint64 Offset = 0,
			Foundation::Uint16 RecordKind = 0
		) noexcept -> ErrorInfo
		{
			return { Code, Stream, Offset, RecordKind };
		}

		[[nodiscard]] constexpr auto IsPowerOfTwo(Foundation::Uint64 Value) noexcept -> Foundation::Bool
		{
			return Value && !(Value & (Value - 1));
		}

		[[nodiscard]] constexpr auto AlignUp(Foundation::Uint64 Value, Foundation::Uint64 Alignment) noexcept -> Foundation::Uint64
		{
			return Alignment ? (Value + Alignment - 1) / Alignment * Alignment : Value;
		}

		[[nodiscard]] constexpr auto CheckedAdd(Foundation::Uint64 A, Foundation::Uint64 B, Foundation::Uint64& Out) noexcept -> Foundation::Bool
		{
			Out = A + B;
			return Out >= A;
		}

		[[nodiscard]] constexpr auto CheckedMul(Foundation::Uint64 A, Foundation::Uint64 B, Foundation::Uint64& Out) noexcept -> Foundation::Bool
		{
			if (A && B > (~Foundation::Uint64{}) / A) return false;
			Out = A * B;
			return true;
		}

		[[nodiscard]] auto ReadU16(const Foundation::Uint8* P) noexcept -> Foundation::Uint16
		{
			return Foundation::Cast::Auto<Foundation::Uint16>(P[0]) |
				(Foundation::Cast::Auto<Foundation::Uint16>(P[1]) << 8);
		}

		[[nodiscard]] auto ReadU32(const Foundation::Uint8* P) noexcept -> Foundation::Uint32
		{
			return Foundation::Cast::Auto<Foundation::Uint32>(P[0]) |
				(Foundation::Cast::Auto<Foundation::Uint32>(P[1]) << 8) |
				(Foundation::Cast::Auto<Foundation::Uint32>(P[2]) << 16) |
				(Foundation::Cast::Auto<Foundation::Uint32>(P[3]) << 24);
		}

		[[nodiscard]] auto ReadU64(const Foundation::Uint8* P) noexcept -> Foundation::Uint64
		{
			return Foundation::Cast::Auto<Foundation::Uint64>(ReadU32(P)) |
				(Foundation::Cast::Auto<Foundation::Uint64>(ReadU32(P + 4)) << 32);
		}

		[[nodiscard]] auto CStr(const Foundation::Uint8* Begin, Foundation::Uint64 Available) noexcept -> Library::StringView
		{
			if (!Begin) return {};
			for (Foundation::Uint64 i = 0; i < Available; ++i)
			{
				if (Begin[i] == 0)
				{
					return { Foundation::Cast::Auto<const Foundation::Char*>(Begin), Foundation::Cast::Auto<Foundation::Size>(i) };
				}
			}
			return {};
		}

		[[nodiscard]] auto FindNamedStream(
			const MsfFile& Msf,
			Library::StringView Name,
			Foundation::Uint32& StreamIndex
		) -> Library::Expected<Foundation::Bool, ErrorInfo>
		{
			StreamIndex = DeletedStream;
			auto Info = Msf.Stream(1);
			if (!Info.Valid() || Info.Size() < sizeof(InfoStreamHeader) + sizeof(Foundation::Uint32))
				return Library::MakeUnexpected(ErrorAt(Error::InvalidPdbInfo, 1));

			Library::Vector<Foundation::Uint8> Bytes{ Msf.Allocator() };
			auto Read = Info.ReadAll(Bytes);
			if (!Read) return Library::MakeUnexpected(Read.Error());

			Foundation::Uint64 Offset = sizeof(InfoStreamHeader);
			const auto StringBytes = ReadU32(Bytes.Data() + Offset);
			Offset += sizeof(Foundation::Uint32);
			if (Offset + StringBytes > Bytes.Size())
				return Library::MakeUnexpected(ErrorAt(Error::InvalidPdbInfo, 1, Offset));
			const auto* Strings = Bytes.Data() + Offset;
			Offset += StringBytes;

			if (Offset + 8 > Bytes.Size())
				return Library::MakeUnexpected(ErrorAt(Error::InvalidPdbInfo, 1, Offset));
			const auto EntryCount = ReadU32(Bytes.Data() + Offset);
			const auto Capacity = ReadU32(Bytes.Data() + Offset + 4);
			Offset += 8;
			if (EntryCount > Capacity)
				return Library::MakeUnexpected(ErrorAt(Error::InvalidPdbInfo, 1, Offset));

			auto SkipBitVector = [&]() -> Foundation::Bool
			{
				if (Offset + 4 > Bytes.Size()) return false;
				const auto WordCount = ReadU32(Bytes.Data() + Offset);
				Offset += 4;
				Foundation::Uint64 VectorBytes{};
				if (!CheckedMul(WordCount, sizeof(Foundation::Uint32), VectorBytes) || Offset + VectorBytes > Bytes.Size())
					return false;
				Offset += VectorBytes;
				return true;
			};
			if (!SkipBitVector() || !SkipBitVector())
				return Library::MakeUnexpected(ErrorAt(Error::InvalidPdbInfo, 1, Offset));

			Foundation::Uint64 PairBytes{};
			if (!CheckedMul(EntryCount, 8, PairBytes) || Offset + PairBytes > Bytes.Size())
				return Library::MakeUnexpected(ErrorAt(Error::InvalidPdbInfo, 1, Offset));
			for (Foundation::Uint32 i = 0; i < EntryCount; ++i)
			{
				const auto NameOffset = ReadU32(Bytes.Data() + Offset);
				const auto Value = ReadU32(Bytes.Data() + Offset + 4);
				Offset += 8;
				if (NameOffset >= StringBytes) continue;
				auto Candidate = CStr(Strings + NameOffset, StringBytes - NameOffset);
				if (Candidate.Data() && Candidate == Name)
				{
					if (Value >= Msf.StreamCount())
						return Library::MakeUnexpected(ErrorAt(Error::InvalidPdbInfo, 1, Offset - 4));
					StreamIndex = Value;
					return true;
				}
			}
			return false;
		}

		[[nodiscard]] auto OpenGlobalStringTable(
			const MsfFile& Msf,
			Library::Vector<Foundation::Uint8>& Storage,
			const Foundation::Uint8*& Strings,
			Foundation::Uint64& StringBytes
		) -> Library::Expected<Foundation::Void, ErrorInfo>
		{
			constexpr auto StringTableSignature = Foundation::Uint32{ 0xEFFEEFFEu };
			Strings = nullptr;
			StringBytes = 0;
			Foundation::Uint32 StreamIndex{};
			auto Found = FindNamedStream(Msf, Library::StringView{ "/names" }, StreamIndex);
			if (!Found) return Library::MakeUnexpected(Found.Error());
			if (!Found.Value()) return {};

			auto Stream = Msf.Stream(StreamIndex);
			if (!Stream.Valid() || Stream.Size() < 12)
				return Library::MakeUnexpected(ErrorAt(Error::InvalidPdbInfo, StreamIndex));
			auto Read = Stream.ReadAll(Storage);
			if (!Read) return Read;
			if (ReadU32(Storage.Data()) != StringTableSignature)
				return Library::MakeUnexpected(ErrorAt(Error::InvalidPdbInfo, StreamIndex));
			StringBytes = ReadU32(Storage.Data() + 8);
			if (StringBytes > Storage.Size() - 12)
				return Library::MakeUnexpected(ErrorAt(Error::InvalidPdbInfo, StreamIndex, 8));
			Strings = Storage.Data() + 12;
			return {};
		}

		[[nodiscard]] auto DbError(SymbolDatabaseError Value) noexcept -> ErrorInfo
		{
			return ErrorAt(Value == SymbolDatabaseError::AllocationFailure ? Error::AllocationFailure : Error::InvalidArgument);
		}

		[[nodiscard]] auto SegmentOffsetToRva(
			const Executable::Pe::Image& Image,
			Foundation::Uint16 Segment,
			Foundation::Uint64 Offset,
			Foundation::Uint64& Rva
		) -> Foundation::Bool
		{
			Rva = 0;
			if (!Segment || !Image.IsValid()) return false;
			const auto* Headers = Image.SectionHeaders();
			const auto Index = Foundation::Cast::Auto<Foundation::Uint32>(Segment - 1);
			if (!Headers || Index >= Image.SectionCount()) return false;
			const auto Candidate = Foundation::Cast::Auto<Foundation::Uint64>(Headers[Index].VirtualAddress) + Offset;
			if (Candidate >= Image.ImageSize()) return false;
			Rva = Candidate;
			return true;
		}

		[[nodiscard]] auto Intern(SymbolDatabase& Database, Library::StringView Value, StringId& Id) -> Library::Expected<Foundation::Void, ErrorInfo>
		{
			auto Result = Database.Intern(Value);
			if (!Result) return Library::MakeUnexpected(DbError(Result.Error()));
			Id = Result.Value();
			return {};
		}

		[[nodiscard]] auto FillSymbolNames(SymbolDatabase& Database, Library::StringView Raw, Symbol& Value)
			-> Library::Expected<Foundation::Void, ErrorInfo>
		{
			StringId RawId{};
			auto Status = Intern(Database, Raw, RawId);
			if (!Status) return Status;

			Library::String Qualified{ Database.Allocator() };
			Library::String Base{ Database.Allocator() };
			if (!SymbolDatabase::NormalizeMsvcName(Raw, Qualified, Base))
			{
				return Library::MakeUnexpected(ErrorAt(Error::AllocationFailure));
			}
			StringId QualifiedId{};
			StringId BaseId{};
			Status = Intern(Database, Qualified.View(), QualifiedId); if (!Status) return Status;
			Status = Intern(Database, Base.View(), BaseId); if (!Status) return Status;

			Value.Name = Raw[0] == '?' && QualifiedId ? QualifiedId : RawId;
			Value.DecoratedName = Raw[0] == '?' ? RawId : EmptyStringId;
			Value.QualifiedName = QualifiedId ? QualifiedId : RawId;
			Value.BaseName = BaseId ? BaseId : RawId;
			if (Raw[0] == '?') Value.Flags |= SymbolFlag::MsvcDecorated;
			return {};
		}

		[[nodiscard]] auto ReadNumeric(
			const Foundation::Uint8*& Cursor,
			const Foundation::Uint8* End,
			Foundation::Uint64& Value
		) noexcept -> Foundation::Bool
		{
			if (Cursor + 2 > End) return false;
			const auto Leaf = ReadU16(Cursor); Cursor += 2;
			if (Leaf < 0x8000) { Value = Leaf; return true; }
			switch (Leaf)
			{
			case 0x8000: if (Cursor + 1 > End) return false; Value = Cursor[0]; Cursor += 1; return true;
			case 0x8001: if (Cursor + 2 > End) return false; Value = Foundation::Cast::Auto<Foundation::Uint64>(Foundation::Cast::Auto<Foundation::Int16>(ReadU16(Cursor))); Cursor += 2; return true;
			case 0x8002: if (Cursor + 2 > End) return false; Value = ReadU16(Cursor); Cursor += 2; return true;
			case 0x8003: if (Cursor + 4 > End) return false; Value = Foundation::Cast::Auto<Foundation::Uint64>(Foundation::Cast::Auto<Foundation::Int32>(ReadU32(Cursor))); Cursor += 4; return true;
			case 0x8004: if (Cursor + 4 > End) return false; Value = ReadU32(Cursor); Cursor += 4; return true;
			case 0x8009: if (Cursor + 8 > End) return false; Value = ReadU64(Cursor); Cursor += 8; return true;
			case 0x800A: if (Cursor + 8 > End) return false; Value = ReadU64(Cursor); Cursor += 8; return true;
			default: return false;
			}
		}

		[[nodiscard]] auto ParseFieldList(
			SymbolDatabase& Database,
			TypeInfo& Info,
			const Foundation::Uint8* Payload,
			Foundation::Uint64 Size
		) -> Library::Expected<Foundation::Void, ErrorInfo>
		{
			Info.Kind = TypeKind::FieldList;
			Info.FieldBegin = Foundation::Cast::Auto<Foundation::Uint32>(Database.Fields().Size());
			const auto* Cursor = Payload;
			const auto* End = Payload + Size;
			while (Cursor < End)
			{
				if (*Cursor >= 0xF0)
				{
					auto Skip = Foundation::Cast::Auto<Foundation::Uint32>(*Cursor & 0x0F);
					if (!Skip) Skip = 1;
					if (Cursor + Skip > End) break;
					Cursor += Skip;
					continue;
				}
				if (Cursor + 2 > End) break;
				const auto Kind = ReadU16(Cursor); Cursor += 2;
				TypeField Field{};
				switch (Kind)
				{
				case LF_MEMBER:
				{
					if (Cursor + 6 > End) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, 2));
					Field.Attributes = ReadU16(Cursor); Cursor += 2;
					Field.Type = ReadU32(Cursor); Cursor += 4;
					if (!ReadNumeric(Cursor, End, Field.Offset)) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, 2));
					const auto Name = CStr(Cursor, End - Cursor); if (Name.Data() == nullptr) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, 2));
					auto S = Intern(Database, Name, Field.Name); if (!S) return S;
					Cursor += Name.Size() + 1;
					break;
				}
				case LF_STMEMBER:
				{
					if (Cursor + 6 > End) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, 2));
					Field.Attributes = ReadU16(Cursor); Cursor += 2;
					Field.Type = ReadU32(Cursor); Cursor += 4;
					Field.IsStatic = true;
					const auto Name = CStr(Cursor, End - Cursor); if (!Name.Data()) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, 2));
					auto S = Intern(Database, Name, Field.Name); if (!S) return S;
					Cursor += Name.Size() + 1;
					break;
				}
				case LF_ENUMERATE:
				{
					if (Cursor + 2 > End) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, 2));
					Field.Attributes = ReadU16(Cursor); Cursor += 2;
					if (!ReadNumeric(Cursor, End, Field.Value)) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, 2));
					Field.IsEnumerator = true;
					const auto Name = CStr(Cursor, End - Cursor); if (!Name.Data()) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, 2));
					auto S = Intern(Database, Name, Field.Name); if (!S) return S;
					Cursor += Name.Size() + 1;
					break;
				}
				case LF_INDEX:
				{
					if (Cursor + 4 > End) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, 2));
					Info.ContinuationType = ReadU32(Cursor); Cursor += 4;
					continue;
				}
				case LF_BCLASS:
				{
					if (Cursor + 6 > End) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, 2));
					Field.Attributes = ReadU16(Cursor); Cursor += 2;
					Field.Type = ReadU32(Cursor); Cursor += 4;
					if (!ReadNumeric(Cursor, End, Field.Offset)) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, 2));
					Field.IsBaseClass = true;
					break;
				}
				case LF_NESTTYPE:
				{
					if (Cursor + 6 > End) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, 2));
					Cursor += 2; Field.Type = ReadU32(Cursor); Cursor += 4;
					const auto Name = CStr(Cursor, End - Cursor); if (!Name.Data()) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, 2));
					auto S = Intern(Database, Name, Field.Name); if (!S) return S;
					Cursor += Name.Size() + 1;
					break;
				}
				case LF_METHOD:
				{
					if (Cursor + 6 > End) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, 2));
					Cursor += 2; Field.Type = ReadU32(Cursor); Cursor += 4;
					const auto Name = CStr(Cursor, End - Cursor); if (!Name.Data()) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, 2));
					auto S = Intern(Database, Name, Field.Name); if (!S) return S; Cursor += Name.Size() + 1; break;
				}
				case LF_ONEMETHOD:
				{
					if (Cursor + 6 > End) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, 2));
					const auto Attr = ReadU16(Cursor); Cursor += 2; Field.Attributes = Attr; Field.Type = ReadU32(Cursor); Cursor += 4;
					const auto MethodProperty = Foundation::Cast::Auto<Foundation::Uint16>((Attr >> 2) & 7);
					if ((MethodProperty == 4 || MethodProperty == 6) && Cursor + 4 <= End) Cursor += 4;
					const auto Name = CStr(Cursor, End - Cursor); if (!Name.Data()) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, 2));
					auto S = Intern(Database, Name, Field.Name); if (!S) return S; Cursor += Name.Size() + 1; break;
				}
				default:
					// Unknown embedded records cannot be skipped safely without their layout.
					// Preserve everything parsed so far and stop this field list rather than desynchronizing.
					Cursor = End;
					continue;
				}

				auto Add = Database.AddField(Field);
				if (!Add) return Library::MakeUnexpected(DbError(Add.Error()));
				++Info.FieldCount;
			}
			return {};
		}

		[[nodiscard]] auto ParseTypeRecord(
			SymbolDatabase& Database,
			TypeIndex Index,
			Foundation::Uint16 Kind,
			const Foundation::Uint8* Payload,
			Foundation::Uint64 Size,
			Foundation::Uint32 StreamIndex
		) -> Library::Expected<Foundation::Void, ErrorInfo>
		{
			TypeInfo Info{}; Info.Index = Index;
			const auto* C = Payload; const auto* E = Payload + Size;
			auto Need = [&](Foundation::Uint64 N) { return C + N <= E; };
			auto AddName = [&](Library::StringView Name, StringId& Out) -> Library::Expected<Foundation::Void, ErrorInfo> { return Intern(Database, Name, Out); };

			switch (Kind)
			{
			case LF_MODIFIER:
				if (!Need(6)) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, StreamIndex, 0, Kind));
				Info.Kind = TypeKind::Modifier; Info.ReferentType = ReadU32(C); Info.Attributes = ReadU16(C + 4); break;
			case LF_POINTER:
				if (!Need(8)) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, StreamIndex, 0, Kind));
				Info.Kind = TypeKind::Pointer; Info.ReferentType = ReadU32(C); Info.Attributes = ReadU32(C + 4); break;
			case LF_PROCEDURE:
				if (!Need(12)) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, StreamIndex, 0, Kind));
				Info.Kind = TypeKind::Procedure; Info.ReturnType = ReadU32(C); Info.Convention = Foundation::Cast::Auto<CallingConvention>(C[4]); Info.Options = C[5]; Info.ParameterCount = ReadU16(C + 6); Info.ArgumentListType = ReadU32(C + 8); break;
			case LF_MFUNCTION:
				if (!Need(24)) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, StreamIndex, 0, Kind));
				Info.Kind = TypeKind::MemberFunction; Info.ReturnType = ReadU32(C); Info.ClassType = ReadU32(C + 4); Info.ThisType = ReadU32(C + 8); Info.Convention = Foundation::Cast::Auto<CallingConvention>(C[12]); Info.Options = C[13]; Info.ParameterCount = ReadU16(C + 14); Info.ArgumentListType = ReadU32(C + 16); Info.ThisAdjustment = Foundation::Cast::Auto<Foundation::Int32>(ReadU32(C + 20)); break;
			case LF_ARGLIST:
			{
				if (!Need(4)) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, StreamIndex, 0, Kind));
				Info.Kind = TypeKind::ArgumentList; const auto Count = ReadU32(C); C += 4;
				if (Foundation::Cast::Auto<Foundation::Uint64>(Count) * 4 > Foundation::Cast::Auto<Foundation::Uint64>(E - C)) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, StreamIndex, 0, Kind));
				Info.ParameterBegin = Foundation::Cast::Auto<Foundation::Uint32>(Database.ParameterTypes().Size());
				for (Foundation::Uint32 i = 0; i < Count; ++i) { auto A = Database.AddParameterType(ReadU32(C)); if (!A) return Library::MakeUnexpected(DbError(A.Error())); C += 4; ++Info.ParameterTypeCount; }
				break;
			}
			case LF_FIELDLIST:
			{
				auto S = ParseFieldList(Database, Info, C, E - C); if (!S) { auto Err=S.Error(); Err.Stream=StreamIndex; Err.RecordKind=Kind; return Library::MakeUnexpected(Err); } break;
			}
			case LF_BITFIELD:
				if (!Need(6)) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, StreamIndex, 0, Kind));
				Info.Kind=TypeKind::BitField; Info.UnderlyingType=ReadU32(C); Info.Size=C[4]; Info.Attributes=C[5]; break;
			case LF_ARRAY:
			{
				if (!Need(8)) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, StreamIndex, 0, Kind)); Info.Kind=TypeKind::Array; Info.ElementType=ReadU32(C); Info.IndexType=ReadU32(C+4); C+=8;
				if (!ReadNumeric(C,E,Info.Size)) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, StreamIndex, 0, Kind)); const auto N=CStr(C,E-C); if (N.Data()) { auto S=AddName(N,Info.Name); if(!S)return S; } break;
			}
			case LF_CLASS: case LF_STRUCTURE:
			{
				if (!Need(16)) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, StreamIndex, 0, Kind)); Info.Kind=Kind==LF_CLASS?TypeKind::Class:TypeKind::Struct; Info.MemberCount=ReadU16(C); Info.Attributes=ReadU16(C+2); Info.FieldListType=ReadU32(C+4); Info.DerivationListType=ReadU32(C+8); Info.VTableShapeType=ReadU32(C+12); C+=16;
				if (!ReadNumeric(C,E,Info.Size)) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, StreamIndex, 0, Kind)); auto N=CStr(C,E-C); if(!N.Data())return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord,StreamIndex,0,Kind)); auto S=AddName(N,Info.Name); if(!S)return S; C+=N.Size()+1;
				if ((Info.Attributes & 0x0200) && C<E) { auto U=CStr(C,E-C); if(U.Data()){ S=AddName(U,Info.UniqueName); if(!S)return S; }} break;
			}
			case LF_UNION:
			{
				if (!Need(8)) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, StreamIndex, 0, Kind)); Info.Kind=TypeKind::Union; Info.MemberCount=ReadU16(C); Info.Attributes=ReadU16(C+2); Info.FieldListType=ReadU32(C+4); C+=8;
				if(!ReadNumeric(C,E,Info.Size))return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord,StreamIndex,0,Kind)); auto N=CStr(C,E-C); if(!N.Data())return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord,StreamIndex,0,Kind)); auto S=AddName(N,Info.Name); if(!S)return S; C+=N.Size()+1; if((Info.Attributes&0x0200)&&C<E){auto U=CStr(C,E-C);if(U.Data()){S=AddName(U,Info.UniqueName);if(!S)return S;}} break;
			}
			case LF_ENUM:
			{
				if (!Need(12)) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, StreamIndex, 0, Kind)); Info.Kind=TypeKind::Enum; Info.MemberCount=ReadU16(C); Info.Attributes=ReadU16(C+2); Info.UnderlyingType=ReadU32(C+4); Info.FieldListType=ReadU32(C+8); C+=12; auto N=CStr(C,E-C); if(!N.Data())return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord,StreamIndex,0,Kind)); auto S=AddName(N,Info.Name); if(!S)return S; break;
			}
			case LF_ALIAS:
			{
				if (!Need(4)) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord, StreamIndex, 0, Kind)); Info.Kind=TypeKind::Alias; Info.UnderlyingType=ReadU32(C); C+=4; auto N=CStr(C,E-C); if(!N.Data())return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord,StreamIndex,0,Kind)); auto S=AddName(N,Info.Name); if(!S)return S; break;
			}
			case LF_FUNC_ID:
			{
				if(!Need(8))return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord,StreamIndex,0,Kind)); Info.Kind=TypeKind::FunctionId; Info.ParentScope=ReadU32(C) ? MakeIpiTypeIndex(ReadU32(C)) : 0; Info.ReferentType=ReadU32(C+4); C+=8; auto N=CStr(C,E-C); if(N.Data()){auto S=AddName(N,Info.Name);if(!S)return S;} break;
			}
			case LF_MFUNC_ID:
			{
				if(!Need(8))return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord,StreamIndex,0,Kind)); Info.Kind=TypeKind::MemberFunctionId; Info.ClassType=ReadU32(C); Info.ReferentType=ReadU32(C+4); C+=8; auto N=CStr(C,E-C); if(N.Data()){auto S=AddName(N,Info.Name);if(!S)return S;} break;
			}
			case LF_STRING_ID:
			{
				if(!Need(4))return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord,StreamIndex,0,Kind)); Info.Kind=TypeKind::StringId; Info.ParentScope=ReadU32(C) ? MakeIpiTypeIndex(ReadU32(C)) : 0; C+=4; auto N=CStr(C,E-C); if(N.Data()){auto S=AddName(N,Info.Name);if(!S)return S;} break;
			}
			default: Info.Kind=TypeKind::Unknown; break;
			}

			auto Add = Database.AddType(Info);
			if (!Add) return Library::MakeUnexpected(DbError(Add.Error()));
			return {};
		}


		[[nodiscard]] auto AddPrimitiveTypes(SymbolDatabase& Database) -> Library::Expected<Foundation::Void, ErrorInfo>
		{
			struct Primitive { TypeIndex Index; const Foundation::Char* Name; Foundation::Uint64 Size; };
			static constexpr Primitive Values[] = {
				{0x0003,"void",0},{0x0008,"HRESULT",4},
				{0x0010,"char",1},{0x0011,"short",2},{0x0012,"long",4},{0x0013,"__int64",8},
				{0x0020,"unsigned char",1},{0x0021,"unsigned short",2},{0x0022,"unsigned long",4},{0x0023,"unsigned __int64",8},
				{0x0030,"bool8",1},{0x0031,"bool16",2},{0x0032,"bool32",4},{0x0033,"bool64",8},
				{0x0040,"float",4},{0x0041,"double",8},{0x0042,"long double",10},{0x0043,"float128",16},
				{0x0068,"int8_t",1},{0x0069,"uint8_t",1},{0x0072,"int16_t",2},{0x0073,"uint16_t",2},
				{0x0074,"int32_t",4},{0x0075,"uint32_t",4},{0x0076,"int64_t",8},{0x0077,"uint64_t",8},
				{0x0078,"int128_t",16},{0x0079,"uint128_t",16},{0x007A,"char16_t",2},{0x007B,"char32_t",4},{0x007C,"char8_t",1}
			};
			for (const auto& P : Values)
			{
				TypeInfo T{}; T.Index=P.Index;T.Kind=TypeKind::Primitive;T.Size=P.Size;
				auto I=Database.Intern(Library::StringView{P.Name});if(!I)return Library::MakeUnexpected(DbError(I.Error()));T.Name=I.Value();
				auto A=Database.AddType(T);if(!A)return Library::MakeUnexpected(DbError(A.Error()));
				for (Foundation::Uint32 Mode=1;Mode<=7;++Mode)
				{
					TypeInfo Ptr{};Ptr.Index=P.Index|(Mode<<8);Ptr.Kind=TypeKind::Pointer;Ptr.ReferentType=P.Index;
					Ptr.Size=(Mode==6?8:(Mode==7?16:(Mode>=4?4:2)));
					A=Database.AddType(Ptr);if(!A)return Library::MakeUnexpected(DbError(A.Error()));
				}
			}
			return {};
		}

		[[nodiscard]] auto ParseTypeStream(const MsfFile& Msf, Foundation::Uint32 Index, SymbolDatabase& Database, Foundation::Bool Ipi)
			-> Library::Expected<Foundation::Void, ErrorInfo>
		{
			const auto Stream = Msf.Stream(Index); if (!Stream.Valid() || Stream.Size() < sizeof(TpiStreamHeader)) return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeStream,Index));
			auto H=Stream.Read<TpiStreamHeader>(); if(!H)return Library::MakeUnexpected(H.Error()); const auto Header=H.Value();
			if(Header.HeaderSize<sizeof(TpiStreamHeader)||Header.HeaderSize>Stream.Size())return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeStream,Index));
			Foundation::Uint64 End{}; if(!CheckedAdd(Header.HeaderSize,Header.TypeRecordBytes,End)||End>Stream.Size())return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeStream,Index));
			Library::Vector<Foundation::Uint8> Bytes{Msf.Allocator()}; if(!Bytes.Resize(Header.TypeRecordBytes))return Library::MakeUnexpected(ErrorAt(Error::AllocationFailure,Index));
			auto RS=Stream.Read(Header.HeaderSize,Bytes.Data(),Bytes.Size());if(!RS)return RS;
			Foundation::Uint64 Off=0; TypeIndex RawType=Header.TypeIndexBegin;
			while(Off<Bytes.Size()&&RawType<Header.TypeIndexEnd)
			{
				if(Off+4>Bytes.Size())return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord,Index,Header.HeaderSize+Off));
				const auto Len=ReadU16(Bytes.Data()+Off); const auto Kind=ReadU16(Bytes.Data()+Off+2); if(Len<2||Off+2+Len>Bytes.Size())return Library::MakeUnexpected(ErrorAt(Error::InvalidTypeRecord,Index,Header.HeaderSize+Off,Kind));
				auto P=ParseTypeRecord(Database,Ipi ? MakeIpiTypeIndex(RawType) : RawType,Kind,Bytes.Data()+Off+4,Len-2,Index); if(!P)return P;
				Off=AlignUp(Off+2+Len,4); ++RawType;
			}
			return {};
		}


		struct BinaryAnnotation
		{
			Foundation::Uint32 Op{};
			Foundation::Uint32 U1{};
			Foundation::Uint32 U2{};
			Foundation::Int32 S1{};
		};

		[[nodiscard]] auto DecodeCompressedAnnotation(
			const Foundation::Uint8*& Cursor,
			const Foundation::Uint8* End,
			Foundation::Uint32& Value
		) noexcept -> Foundation::Bool
		{
			if (Cursor >= End) return false;
			const auto First = *Cursor++;
			if ((First & 0x80) == 0) { Value = First; return true; }
			if ((First & 0xC0) == 0x80)
			{
				if (Cursor >= End) return false;
				Value = (Foundation::Cast::Auto<Foundation::Uint32>(First & 0x3F) << 8) | *Cursor++;
				return true;
			}
			if ((First & 0xE0) == 0xC0)
			{
				if (Cursor + 3 > End) return false;
				Value = (Foundation::Cast::Auto<Foundation::Uint32>(First & 0x1F) << 24) |
					(Foundation::Cast::Auto<Foundation::Uint32>(Cursor[0]) << 16) |
					(Foundation::Cast::Auto<Foundation::Uint32>(Cursor[1]) << 8) |
					Cursor[2];
				Cursor += 3;
				return true;
			}
			return false;
		}

		[[nodiscard]] constexpr auto DecodeSignedAnnotation(Foundation::Uint32 Value) noexcept -> Foundation::Int32
		{
			const auto Magnitude = Foundation::Cast::Auto<Foundation::Int32>(Value >> 1);
			return (Value & 1) ? -Magnitude : Magnitude;
		}

		[[nodiscard]] auto ReadAnnotation(
			const Foundation::Uint8*& Cursor,
			const Foundation::Uint8* End,
			BinaryAnnotation& Result
		) noexcept -> Foundation::Bool
		{
			Result = {};
			if (!DecodeCompressedAnnotation(Cursor, End, Result.Op)) return false;
			switch (Result.Op)
			{
			case 0: return false;
			case 1: case 2: case 3: case 4: case 5: case 7: case 8: case 9: case 13:
				return DecodeCompressedAnnotation(Cursor, End, Result.U1);
			case 6: case 10:
			{
				Foundation::Uint32 V{}; if (!DecodeCompressedAnnotation(Cursor, End, V)) return false;
				Result.S1 = DecodeSignedAnnotation(V); return true;
			}
			case 11:
			{
				Foundation::Uint32 V{}; if (!DecodeCompressedAnnotation(Cursor, End, V)) return false;
				Result.U1 = V >> 4; Result.S1 = DecodeSignedAnnotation(V & 0xF); return true;
			}
			case 12:
				return DecodeCompressedAnnotation(Cursor, End, Result.U1) && DecodeCompressedAnnotation(Cursor, End, Result.U2);
			default: return false;
			}
		}

		[[nodiscard]] auto InlineAnnotationRange(
			const Foundation::Uint8* Cursor,
			const Foundation::Uint8* End,
			Foundation::Uint64 FunctionRva,
			Foundation::Uint64& StartRva,
			Foundation::Uint64& EndRva
		) noexcept -> Foundation::Bool
		{
			Foundation::Uint64 CodeOffset{};
			Foundation::Uint64 Minimum = ~Foundation::Uint64{};
			Foundation::Uint64 Maximum{};
			Foundation::Bool Open{};
			Foundation::Uint64 OpenStart{};
			while (Cursor < End)
			{
				BinaryAnnotation A{}; if (!ReadAnnotation(Cursor, End, A)) break;
				switch (A.Op)
				{
				case 1: case 3: case 11:
					CodeOffset += A.U1;
					if (!Open) { Open = true; OpenStart = CodeOffset; }
					break;
				case 4:
				{
					const auto RangeStart = Open ? OpenStart : CodeOffset;
					CodeOffset += A.U1;
					if (RangeStart < Minimum) Minimum = RangeStart;
					if (CodeOffset > Maximum) Maximum = CodeOffset;
					Open = false;
					break;
				}
				case 12:
					CodeOffset += A.U2;
					if (CodeOffset < Minimum) Minimum = CodeOffset;
					if (CodeOffset + A.U1 > Maximum) Maximum = CodeOffset + A.U1;
					Open = false;
					break;
				default: break;
				}
			}
			if (Minimum == ~Foundation::Uint64{} || Maximum <= Minimum) return false;
			StartRva = FunctionRva + Minimum;
			EndRva = FunctionRva + Maximum;
			return true;
		}

		struct ScopeEntry
		{
			SymbolId Function{ InvalidSymbolId };
			InlineSiteId InlineSite{ InvalidInlineSiteId };
		};

		[[nodiscard]] auto ParseSymbolRecords(
			const Foundation::Uint8* Data,
			Foundation::Uint64 Size,
			SymbolDatabase& Database,
			const Executable::Pe::Image& Image,
			ModuleId Module,
			ResolutionSource Source,
			Foundation::Uint32 StreamIndex
		) -> Library::Expected<Foundation::Void, ErrorInfo>
		{
			Library::Vector<ScopeEntry> Scopes{Database.Allocator()};
			Foundation::Uint64 Off=0;
			while(Off+4<=Size)
			{
				const auto Len=ReadU16(Data+Off); const auto Kind=ReadU16(Data+Off+2); if(!Len){Off+=2;continue;} if(Len<2||Off+2+Len>Size)return Library::MakeUnexpected(ErrorAt(Error::InvalidCodeViewRecord,StreamIndex,Off,Kind));
				const auto* P=Data+Off+4; const auto PSize=Foundation::Cast::Auto<Foundation::Uint64>(Len-2); const auto* E=P+PSize;
				auto CurrentFunction=[&](){for(auto i=Scopes.Size();i>0;--i)if(Scopes[i-1].Function!=InvalidSymbolId)return Scopes[i-1].Function;return InvalidSymbolId;};
				auto AddSym=[&](Symbol V, Library::StringView N)->Library::Expected<SymbolId,ErrorInfo>{auto S=FillSymbolNames(Database,N,V);if(!S)return Library::MakeUnexpected(S.Error());auto A=Database.AddSymbol(V);if(!A)return Library::MakeUnexpected(DbError(A.Error()));return A.Value();};

				switch(Kind)
				{
				case S_GPROC32: case S_LPROC32: case S_GPROC32_ID: case S_LPROC32_ID:
				{
					if(PSize<36)return Library::MakeUnexpected(ErrorAt(Error::InvalidCodeViewRecord,StreamIndex,Off,Kind)); Foundation::Uint64 Rva{}; const auto Seg=ReadU16(P+32); const auto O=ReadU32(P+28); const auto N=CStr(P+35,E-(P+35)); if(!N.Data()||!SegmentOffsetToRva(Image,Seg,O,Rva))break;
					Symbol V{}; V.Kind=SymbolKind::Function; V.Visibility=(Kind==S_GPROC32||Kind==S_GPROC32_ID)?SymbolVisibility::Global:SymbolVisibility::Local; V.Source=Source; V.Flags=SymbolFlag::Code|SymbolFlag::Function; V.Rva=Rva; V.Size=ReadU32(P+12); V.Type=(Kind==S_GPROC32_ID||Kind==S_LPROC32_ID)?MakeIpiTypeIndex(ReadU32(P+24)):ReadU32(P+24); V.Module=Module; V.Parent=CurrentFunction(); if(V.Type!=InvalidTypeIndex)V.Flags|=SymbolFlag::HasType;
					auto A=AddSym(V,N);if(!A)return Library::MakeUnexpected(A.Error()); if(!Scopes.PushBack({A.Value(),InvalidInlineSiteId}))return Library::MakeUnexpected(ErrorAt(Error::AllocationFailure)); break;
				}
				case S_PUB32:
				{
					if(PSize<11)return Library::MakeUnexpected(ErrorAt(Error::InvalidCodeViewRecord,StreamIndex,Off,Kind)); Foundation::Uint64 Rva{}; auto N=CStr(P+10,E-(P+10)); if(!N.Data()||!SegmentOffsetToRva(Image,ReadU16(P+8),ReadU32(P+4),Rva))break; const auto F=ReadU32(P);
					Symbol V{}; V.Kind=(F&2)?SymbolKind::PublicFunction:SymbolKind::GlobalVariable; V.Visibility=SymbolVisibility::Public;V.Source=Source;V.Rva=Rva;V.Module=Module;if(F&1)V.Flags|=SymbolFlag::Code;if(F&2)V.Flags|=SymbolFlag::Function;if(F&4)V.Flags|=SymbolFlag::Managed;auto A=AddSym(V,N);if(!A)return Library::MakeUnexpected(A.Error());break;
				}
				case S_GDATA32: case S_LDATA32: case S_GTHREAD32: case S_LTHREAD32:
				{
					if(PSize<11)return Library::MakeUnexpected(ErrorAt(Error::InvalidCodeViewRecord,StreamIndex,Off,Kind)); Foundation::Uint64 Rva{}; auto N=CStr(P+10,E-(P+10));if(!N.Data()||!SegmentOffsetToRva(Image,ReadU16(P+8),ReadU32(P+4),Rva))break; Symbol V{}; V.Kind=(Kind==S_GTHREAD32||Kind==S_LTHREAD32)?SymbolKind::ThreadLocal:(Kind==S_GDATA32?SymbolKind::GlobalVariable:SymbolKind::StaticVariable);V.Visibility=(Kind==S_GDATA32||Kind==S_GTHREAD32)?SymbolVisibility::Global:SymbolVisibility::Local;V.Source=Source;V.Rva=Rva;V.Type=ReadU32(P);V.Module=Module;V.Parent=CurrentFunction();V.Flags|=SymbolFlag::HasType;auto A=AddSym(V,N);if(!A)return Library::MakeUnexpected(A.Error());break;
				}
				case S_THUNK32:
				{
					if(PSize<22)return Library::MakeUnexpected(ErrorAt(Error::InvalidCodeViewRecord,StreamIndex,Off,Kind));Foundation::Uint64 Rva{};auto N=CStr(P+21,E-(P+21));if(!N.Data()||!SegmentOffsetToRva(Image,ReadU16(P+16),ReadU32(P+12),Rva))break;Symbol V{};V.Kind=SymbolKind::Thunk;V.Visibility=SymbolVisibility::Local;V.Source=Source;V.Flags=SymbolFlag::Code|SymbolFlag::Function;V.Rva=Rva;V.Size=ReadU16(P+18);V.Module=Module;V.Parent=CurrentFunction();auto A=AddSym(V,N);if(!A)return Library::MakeUnexpected(A.Error());break;
				}
				case S_LABEL32:
				{
					if(PSize<8)return Library::MakeUnexpected(ErrorAt(Error::InvalidCodeViewRecord,StreamIndex,Off,Kind));Foundation::Uint64 Rva{};auto N=CStr(P+7,E-(P+7));if(!N.Data()||!SegmentOffsetToRva(Image,ReadU16(P+4),ReadU32(P),Rva))break;Symbol V{};V.Kind=SymbolKind::Label;V.Visibility=SymbolVisibility::Local;V.Source=Source;V.Rva=Rva;V.Module=Module;V.Parent=CurrentFunction();auto A=AddSym(V,N);if(!A)return Library::MakeUnexpected(A.Error());break;
				}
				case S_LOCAL:
				{
					if(PSize<7)break;auto N=CStr(P+6,E-(P+6));if(!N.Data())break;LocalVariable L{};L.Type=ReadU32(P);L.Flags=ReadU16(P+4);L.Function=CurrentFunction();L.IsParameter=(L.Flags&1)!=0;auto S=Intern(Database,N,L.Name);if(!S)return S;auto A=Database.AddLocal(L);if(!A)return Library::MakeUnexpected(DbError(A.Error()));break;
				}
				case S_BPREL32:
				{
					if(PSize<9)break;auto N=CStr(P+8,E-(P+8));if(!N.Data())break;LocalVariable L{};L.Offset=Foundation::Cast::Auto<Foundation::Int32>(ReadU32(P));L.Type=ReadU32(P+4);L.Function=CurrentFunction();L.FrameRelative=true;auto S=Intern(Database,N,L.Name);if(!S)return S;auto A=Database.AddLocal(L);if(!A)return Library::MakeUnexpected(DbError(A.Error()));break;
				}
				case S_REGREL32:
				{
					if(PSize<11)break;auto N=CStr(P+10,E-(P+10));if(!N.Data())break;LocalVariable L{};L.Offset=Foundation::Cast::Auto<Foundation::Int32>(ReadU32(P));L.Type=ReadU32(P+4);L.Register=ReadU16(P+8);L.Function=CurrentFunction();L.RegisterRelative=true;auto S=Intern(Database,N,L.Name);if(!S)return S;auto A=Database.AddLocal(L);if(!A)return Library::MakeUnexpected(DbError(A.Error()));break;
				}
				case S_UDT:
				{
					if(PSize<5)break;auto N=CStr(P+4,E-(P+4));if(!N.Data())break;Symbol V{};V.Kind=SymbolKind::UserDefinedType;V.Visibility=SymbolVisibility::Local;V.Source=Source;V.Type=ReadU32(P);V.Module=Module;V.Parent=CurrentFunction();V.Flags|=SymbolFlag::HasType;auto A=AddSym(V,N);if(!A)return Library::MakeUnexpected(A.Error());break;
				}
				case S_CONSTANT:
				{
					if(PSize<7)break;const auto* Q=P+4;Foundation::Uint64 Value{};if(!ReadNumeric(Q,E,Value))break;auto N=CStr(Q,E-Q);if(!N.Data())break;Symbol V{};V.Kind=SymbolKind::Constant;V.Visibility=SymbolVisibility::Local;V.Source=Source;V.Type=ReadU32(P);V.Module=Module;V.Parent=CurrentFunction();V.Flags|=SymbolFlag::HasType;auto A=AddSym(V,N);if(!A)return Library::MakeUnexpected(A.Error());break;
				}
				case S_INLINESITE:
				{
					if(PSize<12)break;InlineSite Site{};Site.ParentFunction=CurrentFunction();for(auto i=Scopes.Size();i>0;--i){if(Scopes[i-1].InlineSite!=InvalidInlineSiteId){Site.ParentInlineSite=Scopes[i-1].InlineSite;break;}}Site.Inlinee=MakeIpiTypeIndex(ReadU32(P+8));const auto* T=Database.TypeByIndex(Site.Inlinee);if(T)Site.Name=T->Name;if(const auto* Parent=Database.SymbolById(Site.ParentFunction)){(void)InlineAnnotationRange(P+12,E,Parent->Rva,Site.StartRva,Site.EndRva);}auto A=Database.AddInlineSite(Site);if(!A)return Library::MakeUnexpected(DbError(A.Error()));if(!Scopes.PushBack({Site.ParentFunction,A.Value()}))return Library::MakeUnexpected(ErrorAt(Error::AllocationFailure));if(auto* Parent=Database.MutableSymbolById(Site.ParentFunction))Parent->Flags|=SymbolFlag::HasInlineSites;break;
				}
				case S_END: case S_INLINESITE_END:
					if(!Scopes.Empty())(void)Scopes.PopBack();break;
				default: break;
				}
				Off += 2 + Len;
			}
			return {};
		}

		struct ChecksumFile { Foundation::Uint32 Offset{}; StringId File{}; };
		struct InlineeLine { TypeIndex Inlinee{}; Foundation::Uint32 FileOffset{}; Foundation::Uint32 Line{}; };

		[[nodiscard]] auto FindFile(const Library::Vector<ChecksumFile>& Files, Foundation::Uint32 Offset) noexcept -> StringId
		{
			for(const auto& F:Files)if(F.Offset==Offset)return F.File; return EmptyStringId;
		}

		[[nodiscard]] auto ParseC13(
			const Foundation::Uint8* Data, Foundation::Uint64 Size, SymbolDatabase& Database,
			const Executable::Pe::Image& Image, ModuleId Module,
			const Foundation::Uint8* GlobalStringTable, Foundation::Uint64 GlobalStringTableSize
		) -> Library::Expected<Foundation::Void,ErrorInfo>
		{
			const Foundation::Uint8* StringTable = GlobalStringTable; Foundation::Uint64 StringTableSize = GlobalStringTableSize;
			const Foundation::Uint8* Checksums{}; Foundation::Uint64 ChecksumsSize{};
			Library::Vector<ChecksumFile> Files{Database.Allocator()}; Library::Vector<InlineeLine> Inlinees{Database.Allocator()};
			for(Foundation::Uint64 O=0;O+8<=Size;)
			{
				const auto K=ReadU32(Data+O),N=ReadU32(Data+O+4); if(O+8+N>Size)return Library::MakeUnexpected(ErrorAt(Error::InvalidLineInfo,0xFFFFFFFFu,O)); const auto* P=Data+O+8;
				if(K==DEBUG_S_STRINGTABLE){StringTable=P;StringTableSize=N;} else if(K==DEBUG_S_FILECHKSMS){Checksums=P;ChecksumsSize=N;}
				O=AlignUp(O+8+N,4);
			}
			if(StringTable&&Checksums)
			{
				for(Foundation::Uint64 O=0;O+6<=ChecksumsSize;)
				{
					const auto EntryOff=Foundation::Cast::Auto<Foundation::Uint32>(O); const auto NameOff=ReadU32(Checksums+O); const auto Cb=Checksums[O+4]; if(O+6+Cb>ChecksumsSize)break;
					if(NameOff<StringTableSize){auto N=CStr(StringTable+NameOff,StringTableSize-NameOff);if(N.Data()){StringId Id{};auto S=Intern(Database,N,Id);if(!S)return S;if(!Files.PushBack({EntryOff,Id}))return Library::MakeUnexpected(ErrorAt(Error::AllocationFailure));}}
					O=AlignUp(O+6+Cb,4);
				}
			}
			for(Foundation::Uint64 O=0;O+8<=Size;)
			{
				const auto K=ReadU32(Data+O),N=ReadU32(Data+O+4);if(O+8+N>Size)break;const auto* P=Data+O+8;const auto* E=P+N;
				if(K==DEBUG_S_INLINEELINES&&N>=4)
				{
					const auto Sig=ReadU32(P);P+=4;while(P+12<=E){InlineeLine I{MakeIpiTypeIndex(ReadU32(P)),ReadU32(P+4),ReadU32(P+8)};P+=12;if(!Inlinees.PushBack(I))return Library::MakeUnexpected(ErrorAt(Error::AllocationFailure));if(Sig==1&&P+4<=E){auto Extra=ReadU32(P);P+=4;if(P+Foundation::Cast::Auto<Foundation::Uint64>(Extra)*4>E)break;P+=Foundation::Cast::Auto<Foundation::Uint64>(Extra)*4;}}
				}
				else if(K==DEBUG_S_LINES&&N>=12)
				{
					const auto Reloc=ReadU32(P);const auto Seg=ReadU16(P+4);const auto Flags=ReadU16(P+6);const auto CodeSize=ReadU32(P+8);P+=12;Foundation::Uint64 Base{};if(!SegmentOffsetToRva(Image,Seg,Reloc,Base)){O=AlignUp(O+8+N,4);continue;}
					while(P+12<=E){const auto FileOff=ReadU32(P);const auto Count=ReadU32(P+4);const auto BlockSize=ReadU32(P+8);const auto* Block=P;P+=12;if(Block+BlockSize>E||P+Foundation::Cast::Auto<Foundation::Uint64>(Count)*8>E)break;const auto* Lines=P;const auto* Columns=Lines+Foundation::Cast::Auto<Foundation::Uint64>(Count)*8;for(Foundation::Uint32 i=0;i<Count;++i){const auto Delta=ReadU32(Lines+i*8);const auto LF=ReadU32(Lines+i*8+4);SourceLine L{};L.Rva=Base+Delta;L.EndRva=(i+1<Count)?Base+ReadU32(Lines+(i+1)*8):Base+CodeSize;L.Location.File=FindFile(Files,FileOff);L.Location.Line=LF&0x00FFFFFFu;L.Statement=(LF&0x80000000u)!=0;L.Module=Module;if((Flags&1)&&Columns+Foundation::Cast::Auto<Foundation::Uint64>(i+1)*4<=Block+BlockSize)L.Location.Column=ReadU16(Columns+i*4);auto A=Database.AddSourceLine(L);if(!A)return Library::MakeUnexpected(DbError(A.Error()));}P=Block+BlockSize;}
				}
				O=AlignUp(O+8+N,4);
			}
			for(auto& Site:Database.MutableInlineSites())
			{
				const auto* Parent=Database.SymbolById(Site.ParentFunction);if(!Parent||Parent->Module!=Module)continue;const auto* T=Database.TypeByIndex(Site.Inlinee);if(T&&T->Name)Site.Name=T->Name;for(const auto& I:Inlinees)if(I.Inlinee==Site.Inlinee){Site.Declaration.File=FindFile(Files,I.FileOffset);Site.Declaration.Line=I.Line;break;}
				// Annotation ranges vary by compiler. Use the containing function as a safe range fallback;
				// source/inline identity remains exact while avoiding false global matches.
				if(!Site.StartRva&&!Site.EndRva){Site.StartRva=Parent->Rva;Site.EndRva=Parent->Size?Parent->Rva+Parent->Size:Parent->Rva+1;}
			}
			return {};
		}

		[[nodiscard]] auto ParseSectionMap(const Foundation::Uint8* P, Foundation::Uint64 N, SymbolDatabase& Database)->Library::Expected<Foundation::Void,ErrorInfo>
		{
			if(N<4)return {};const auto Count=ReadU16(P);P+=4;N-=4;for(Foundation::Uint16 i=0;i<Count&&N>=20;++i,P+=20,N-=20){SectionMapEntry E{};E.Flags=ReadU16(P);E.Overlay=ReadU16(P+2);E.Group=ReadU16(P+4);E.Frame=ReadU16(P+6);E.SectionName=ReadU16(P+8);E.ClassName=ReadU16(P+10);E.Offset=ReadU32(P+12);E.Length=ReadU32(P+16);auto A=Database.AddSectionMap(E);if(!A)return Library::MakeUnexpected(DbError(A.Error()));}return {};
		}

		[[nodiscard]] auto ParseOmap(const MsfFile& Msf, Foundation::Uint16 StreamIndex, SymbolDatabase& Database, Foundation::Bool ToSource)->Library::Expected<Foundation::Void,ErrorInfo>
		{
			if(StreamIndex==InvalidStream)return {};auto S=Msf.Stream(StreamIndex);if(!S.Valid())return {};Library::Vector<Foundation::Uint8>B{Msf.Allocator()};auto R=S.ReadAll(B);if(!R)return R;for(Foundation::Uint64 O=0;O+8<=B.Size();O+=8){OmapEntry E{ReadU32(B.Data()+O),ReadU32(B.Data()+O+4)};auto A=ToSource?Database.AddOmapToSource(E):Database.AddOmapFromSource(E);if(!A)return Library::MakeUnexpected(DbError(A.Error()));}return {};
		}

		[[nodiscard]] auto ImageIdentity(const Executable::Pe::Image& Image, PdbIdentity& Out) noexcept -> Foundation::Bool
		{
			Out={};const auto D=Image.GetDirectory(Executable::Pe::DirectoryIndex::Debug);if(D.Empty()||D.Rva()+D.Size()>Image.ImageSize())return false;const auto* Entries=Image.RvaToPointer<ImageDebugDirectory>(D.Rva());if(!Entries)return false;const auto Count=D.Size()/sizeof(ImageDebugDirectory);for(Foundation::Uint32 i=0;i<Count;++i){const auto&E=Entries[i];if(E.Type!=IMAGE_DEBUG_TYPE_CODEVIEW||E.SizeOfData<sizeof(RsdsHeader)||E.AddressOfRawData+sizeof(RsdsHeader)>Image.ImageSize())continue;const auto*R=Image.RvaToPointer<RsdsHeader>(E.AddressOfRawData);if(R&&R->Signature==CV_SIGNATURE_RSDS){Out.Guid=R->Guid;Out.Age=R->Age;return true;}}return false;
		}

		[[nodiscard]] auto AddExports(const Executable::Pe::Image& Image, SymbolDatabase& Database)->Library::Expected<Foundation::Void,ErrorInfo>
		{
			for(const auto& E:Image.Exports()){if(E.Name.Empty()||E.Forwarded)continue;Symbol V{};V.Kind=SymbolKind::PublicFunction;V.Visibility=SymbolVisibility::Public;V.Source=ResolutionSource::PeExport;V.Flags=SymbolFlag::Code|SymbolFlag::Function;V.Rva=E.Rva;auto S=FillSymbolNames(Database,E.Name.View(),V);if(!S)return S;auto A=Database.AddSymbol(V);if(!A)return Library::MakeUnexpected(DbError(A.Error()));}return {};
		}
	}

	MsfFile::MsfFile(Memory::AllocatorStub Allocator) noexcept : Allocator_(Allocator),Streams_(Allocator),StreamBlocks_(Allocator) {}

	auto MsfFile::Reset()->Foundation::Void { const auto A=Allocator_; *this=MsfFile{A}; }
	auto MsfFile::Valid() const noexcept->Foundation::Bool{return Valid_;}
	auto MsfFile::StreamCount() const noexcept->Foundation::Uint32{return Foundation::Cast::Auto<Foundation::Uint32>(Streams_.Size());}
	auto MsfFile::Allocator() const noexcept->Memory::AllocatorStub{return Allocator_;}
	auto MsfFile::Stream(Foundation::Uint32 Index) const noexcept->MsfStream{return Index<Streams_.Size()&&Streams_[Index].Size!=DeletedStream?MsfStream{this,Index}:MsfStream{};}

	auto MsfFile::Open(const Foundation::Uint8* Data, Foundation::Uint64 Size)->Library::Expected<Foundation::Void,ErrorInfo>
	{
		Reset();if(!Data||Size<sizeof(MsfSuperBlock7))return Library::MakeUnexpected(ErrorAt(Error::InvalidArgument));const auto*H=Foundation::Cast::Auto<const MsfSuperBlock7*>(Data);if(Memory::Compare(H->Magic,Msf7Magic,sizeof(Msf7Magic))!=0)return Library::MakeUnexpected(ErrorAt(Error::InvalidMsf));if(!IsPowerOfTwo(H->BlockSize)||H->BlockSize<512||!H->NumBlocks||!H->NumDirectoryBytes)return Library::MakeUnexpected(ErrorAt(Error::InvalidSuperBlock));Foundation::Uint64 Full{};if(!CheckedMul(H->NumBlocks,H->BlockSize,Full)||Full>Size)return Library::MakeUnexpected(ErrorAt(Error::OutOfBounds));
		Data_=Data;Size_=Size;BlockSize_=H->BlockSize;BlockCount_=H->NumBlocks;const auto DirBlocks=AlignUp(H->NumDirectoryBytes,H->BlockSize)/H->BlockSize;Foundation::Uint64 MapOffset{},MapBytes{};if(!CheckedMul(H->BlockMapAddr,H->BlockSize,MapOffset)||!CheckedMul(DirBlocks,4,MapBytes)||MapOffset+MapBytes>Size)return Library::MakeUnexpected(ErrorAt(Error::InvalidStreamDirectory));Library::Vector<Foundation::Uint8>Dir{Allocator_};if(!Dir.Resize(H->NumDirectoryBytes))return Library::MakeUnexpected(ErrorAt(Error::AllocationFailure));Foundation::Uint64 Left=H->NumDirectoryBytes,Out=0;for(Foundation::Uint64 i=0;i<DirBlocks;++i){const auto B=ReadU32(Data+MapOffset+i*4);if(B>=BlockCount_)return Library::MakeUnexpected(ErrorAt(Error::InvalidStreamDirectory));const auto Copy=Left>BlockSize_?BlockSize_:Left;Memory::Copy(Dir.Data()+Out,Data+Foundation::Cast::Auto<Foundation::Uint64>(B)*BlockSize_,Copy);Out+=Copy;Left-=Copy;}
		if(Dir.Size()<4)return Library::MakeUnexpected(ErrorAt(Error::InvalidStreamDirectory));Foundation::Uint64 C=0;const auto Count=ReadU32(Dir.Data());C=4;if(Foundation::Cast::Auto<Foundation::Uint64>(Count)*4>Dir.Size()-C)return Library::MakeUnexpected(ErrorAt(Error::InvalidStreamDirectory));if(!Streams_.Resize(Count))return Library::MakeUnexpected(ErrorAt(Error::AllocationFailure));Foundation::Uint32 BlockCursor{};for(Foundation::Uint32 i=0;i<Count;++i){Streams_[i].Size=ReadU32(Dir.Data()+C);C+=4;Streams_[i].BlockBegin=BlockCursor;Streams_[i].BlockCount=Streams_[i].Size==DeletedStream?0:Foundation::Cast::Auto<Foundation::Uint32>(AlignUp(Streams_[i].Size,BlockSize_)/BlockSize_);if(Streams_[i].BlockCount>0xFFFFFFFFu-BlockCursor)return Library::MakeUnexpected(ErrorAt(Error::IntegerOverflow));BlockCursor+=Streams_[i].BlockCount;}for(Foundation::Uint32 i=0;i<Count;++i){for(Foundation::Uint32 j=0;j<Streams_[i].BlockCount;++j){if(C+4>Dir.Size())return Library::MakeUnexpected(ErrorAt(Error::InvalidStreamDirectory));const auto B=ReadU32(Dir.Data()+C);C+=4;if(B>=BlockCount_)return Library::MakeUnexpected(ErrorAt(Error::InvalidStreamDirectory));if(!StreamBlocks_.PushBack(B))return Library::MakeUnexpected(ErrorAt(Error::AllocationFailure));}}
		Valid_=true;return {};
	}

	auto MsfFile::ReadStream(Foundation::Uint32 I,Foundation::Uint64 O,Foundation::Void* D,Foundation::Size N) const->Library::Expected<Foundation::Void,ErrorInfo>
	{
		if(!Valid_||I>=Streams_.Size()||Streams_[I].Size==DeletedStream)return Library::MakeUnexpected(ErrorAt(Error::InvalidStream,I,O));if(!D&&N)return Library::MakeUnexpected(ErrorAt(Error::InvalidArgument,I,O));if(O>Streams_[I].Size||N>Streams_[I].Size-O)return Library::MakeUnexpected(ErrorAt(Error::OutOfBounds,I,O));auto*Out=Foundation::Cast::Auto<Foundation::Uint8*>(D);auto Left=Foundation::Cast::Auto<Foundation::Uint64>(N);auto Logical=O;while(Left){const auto Bi=Logical/BlockSize_,In=Logical%BlockSize_;if(Bi>=Streams_[I].BlockCount)return Library::MakeUnexpected(ErrorAt(Error::InvalidStream,I,O));const auto Physical=StreamBlocks_[Streams_[I].BlockBegin+Bi];const auto Take=(Left<BlockSize_-In)?Left:BlockSize_-In;const auto Src=Foundation::Cast::Auto<Foundation::Uint64>(Physical)*BlockSize_+In;if(Src+Take>Size_)return Library::MakeUnexpected(ErrorAt(Error::OutOfBounds,I,Logical));Memory::Copy(Out,Data_+Src,Foundation::Cast::Auto<Foundation::Size>(Take));Out+=Take;Logical+=Take;Left-=Take;}return {};
	}

	auto MsfStream::Valid() const noexcept->Foundation::Bool{return File_&&File_->Valid()&&Index_<File_->Streams_.Size()&&File_->Streams_[Index_].Size!=DeletedStream;}
	auto MsfStream::Size() const noexcept->Foundation::Uint64{return Valid()?File_->Streams_[Index_].Size:0;}
	auto MsfStream::Index() const noexcept->Foundation::Uint32{return Index_;}
	auto MsfStream::Read(Foundation::Uint64 O,Foundation::Void*D,Foundation::Size N) const->Library::Expected<Foundation::Void,ErrorInfo>{return Valid()?File_->ReadStream(Index_,O,D,N):Library::Expected<Foundation::Void,ErrorInfo>{Library::MakeUnexpected(ErrorAt(Error::InvalidStream,Index_,O))};}
	auto MsfStream::ReadAll(Library::Vector<Foundation::Uint8>& Out) const->Library::Expected<Foundation::Void,ErrorInfo>{Out.Clear();if(!Valid())return Library::MakeUnexpected(ErrorAt(Error::InvalidStream,Index_));if(!Out.Resize(Foundation::Cast::Auto<Foundation::Size>(Size())))return Library::MakeUnexpected(ErrorAt(Error::AllocationFailure,Index_));if(Out.Empty())return {};return Read(0,Out.Data(),Out.Size());}

	PdbFile::PdbFile(Memory::AllocatorStub Allocator) noexcept : Allocator_(Allocator),Msf_(Allocator) {}
	auto PdbFile::Reset()->Foundation::Void{const auto A=Allocator_;*this=PdbFile{A};}
	auto PdbFile::Valid() const noexcept->Foundation::Bool{return Valid_;}
	auto PdbFile::Identity() const noexcept->const PdbIdentity&{return Identity_;}
	auto PdbFile::Msf() const noexcept->const MsfFile&{return Msf_;}

	auto PdbFile::Open(const Foundation::Uint8*Data,Foundation::Uint64 Size)->Library::Expected<Foundation::Void,ErrorInfo>
	{
		Reset();auto O=Msf_.Open(Data,Size);if(!O)return O;auto S=Msf_.Stream(1);if(!S.Valid()||S.Size()<sizeof(InfoStreamHeader))return Library::MakeUnexpected(ErrorAt(Error::MissingPdbInfo,1));auto H=S.Read<InfoStreamHeader>();if(!H)return Library::MakeUnexpected(H.Error());Identity_.Guid=H.Value().Guid;Identity_.Age=H.Value().Age;if(!Identity_.Valid())return Library::MakeUnexpected(ErrorAt(Error::InvalidPdbInfo,1));Valid_=true;return {};
	}

	auto PdbFile::Populate(SymbolDatabase& Database,const Foundation::Void*ImageBase,const LoadOptions&Options) const->Library::Expected<Foundation::Void,ErrorInfo>
	{
		if(!Valid_||!ImageBase)return Library::MakeUnexpected(ErrorAt(Error::InvalidArgument));Executable::Pe::Image Image{ImageBase,Database.Allocator()};if(!Image.IsValid())return Library::MakeUnexpected(ErrorAt(Error::UnsupportedFormat));Database.Reset();Database.SetImage(ImageBase,Image.ImageSize());Database.SetIdentity(Identity_);auto Prim=AddPrimitiveTypes(Database);if(!Prim)return Prim;
		PdbIdentity Img{};const auto HasId=ImageIdentity(Image,Img);if(Options.IdentityPolicy!=MatchPolicy::Ignore){if(!HasId&&Options.IdentityPolicy==MatchPolicy::RequireExact)return Library::MakeUnexpected(ErrorAt(Error::MissingImageIdentity));if(HasId){if(Img.Guid!=Identity_.Guid)return Library::MakeUnexpected(ErrorAt(Error::IdentityMismatch));if(Img.Age!=Identity_.Age)return Library::MakeUnexpected(ErrorAt(Error::AgeMismatch));}}
		const auto Dbi=Msf_.Stream(3);if(!Dbi.Valid()||Dbi.Size()<sizeof(DbiStreamHeader))return Library::MakeUnexpected(ErrorAt(Error::MissingDbi,3));auto HR=Dbi.Read<DbiStreamHeader>();if(!HR)return Library::MakeUnexpected(HR.Error());const auto H=HR.Value();if(H.VersionSignature!=-1||H.ModInfoSize<0||H.SectionContributionSize<0||H.SectionMapSize<0||H.FileInfoSize<0||H.TypeServerMapSize<0||H.OptionalDbgHeaderSize<0||H.ECSubstreamSize<0)return Library::MakeUnexpected(ErrorAt(Error::InvalidDbi,3));if(H.Age!=Identity_.Age)return Library::MakeUnexpected(ErrorAt(Error::AgeMismatch,3));
		if(Options.ParseTypes){if(Msf_.Stream(2).Valid()){auto T=ParseTypeStream(Msf_,2,Database,false);if(!T)return T;}if(Msf_.Stream(4).Valid()){auto T=ParseTypeStream(Msf_,4,Database,true);if(!T)return T;}}
		Library::Vector<Foundation::Uint8>GlobalStringStorage{Allocator_};const Foundation::Uint8*GlobalStrings{};Foundation::Uint64 GlobalStringBytes{};if(Options.ParseSourceLines||Options.ParseInlineInformation){auto G=OpenGlobalStringTable(Msf_,GlobalStringStorage,GlobalStrings,GlobalStringBytes);if(!G)return G;}
		Library::Vector<Foundation::Uint8>DbiBytes{Allocator_};auto DR=Dbi.ReadAll(DbiBytes);if(!DR)return DR;Foundation::Uint64 ModuleOff=sizeof(DbiStreamHeader);Foundation::Uint64 SecContribOff=ModuleOff+H.ModInfoSize;Foundation::Uint64 SecMapOff=SecContribOff+H.SectionContributionSize;Foundation::Uint64 FileOff=SecMapOff+H.SectionMapSize;Foundation::Uint64 TypeServerOff=FileOff+H.FileInfoSize;Foundation::Uint64 EcOff=TypeServerOff+H.TypeServerMapSize;Foundation::Uint64 OptionalOff=EcOff+H.ECSubstreamSize;if(OptionalOff+H.OptionalDbgHeaderSize>DbiBytes.Size())return Library::MakeUnexpected(ErrorAt(Error::InvalidDbi,3));
		if(H.SectionMapSize){auto S=ParseSectionMap(DbiBytes.Data()+SecMapOff,H.SectionMapSize,Database);if(!S)return S;}
		if(H.OptionalDbgHeaderSize>=10){const auto*P=DbiBytes.Data()+OptionalOff;const auto OmapTo=ReadU16(P+6),OmapFrom=ReadU16(P+8);auto O=ParseOmap(Msf_,OmapTo,Database,true);if(!O)return O;O=ParseOmap(Msf_,OmapFrom,Database,false);if(!O)return O;}
		if((Options.ParsePublicSymbols||Options.ParseGlobalSymbolRecords)&&H.SymRecordStreamIndex!=InvalidStream){auto S=Msf_.Stream(H.SymRecordStreamIndex);if(S.Valid()){Library::Vector<Foundation::Uint8>B{Allocator_};auto R=S.ReadAll(B);if(!R)return R;auto P=ParseSymbolRecords(B.Data(),B.Size(),Database,Image,InvalidModuleId,ResolutionSource::ExactPdb,H.SymRecordStreamIndex);if(!P)return P;}}
		if(Options.ParseModuleSymbols||Options.ParseSourceLines||Options.ParseInlineInformation)
		{
			Foundation::Uint64 C=ModuleOff,End=ModuleOff+H.ModInfoSize;while(C<End){if(C+sizeof(ModuleInfoHeader)>End)return Library::MakeUnexpected(ErrorAt(Error::InvalidModuleInfo,3,C));const auto*MH=Foundation::Cast::Auto<const ModuleInfoHeader*>(DbiBytes.Data()+C);const auto NameOff=C+sizeof(ModuleInfoHeader);auto MN=CStr(DbiBytes.Data()+NameOff,End-NameOff);if(!MN.Data())return Library::MakeUnexpected(ErrorAt(Error::InvalidModuleInfo,3,C));const auto ObjOff=NameOff+MN.Size()+1;auto ON=CStr(DbiBytes.Data()+ObjOff,End-ObjOff);if(!ON.Data())return Library::MakeUnexpected(ErrorAt(Error::InvalidModuleInfo,3,C));ModuleInfo M{};M.StreamIndex=MH->ModuleDebugStream;M.SymbolBytes=MH->SymbolBytes;M.C11Bytes=MH->C11Bytes;M.C13Bytes=MH->C13Bytes;auto I=Intern(Database,MN,M.Name);if(!I)return I;I=Intern(Database,ON,M.ObjectFile);if(!I)return I;auto AM=Database.AddModule(M);if(!AM)return Library::MakeUnexpected(DbError(AM.Error()));const auto Mid=AM.Value();
				if(MH->ModuleDebugStream!=InvalidStream){auto MS=Msf_.Stream(MH->ModuleDebugStream);if(!MS.Valid())return Library::MakeUnexpected(ErrorAt(Error::InvalidModuleStream,MH->ModuleDebugStream));Foundation::Uint64 Total=Foundation::Cast::Auto<Foundation::Uint64>(MH->SymbolBytes)+MH->C11Bytes+MH->C13Bytes;if(Total>MS.Size())return Library::MakeUnexpected(ErrorAt(Error::InvalidModuleStream,MH->ModuleDebugStream));if(Options.ParseModuleSymbols&&MH->SymbolBytes>=4){Library::Vector<Foundation::Uint8>SB{Allocator_};if(!SB.Resize(MH->SymbolBytes-4))return Library::MakeUnexpected(ErrorAt(Error::AllocationFailure));auto R=MS.Read(4,SB.Data(),SB.Size());if(!R)return R;auto P=ParseSymbolRecords(SB.Data(),SB.Size(),Database,Image,Mid,ResolutionSource::ExactPdb,MH->ModuleDebugStream);if(!P)return P;}if((Options.ParseSourceLines||Options.ParseInlineInformation)&&MH->C13Bytes){Library::Vector<Foundation::Uint8>CB{Allocator_};if(!CB.Resize(MH->C13Bytes))return Library::MakeUnexpected(ErrorAt(Error::AllocationFailure));auto R=MS.Read(Foundation::Cast::Auto<Foundation::Uint64>(MH->SymbolBytes)+MH->C11Bytes,CB.Data(),CB.Size());if(!R)return R;auto P=ParseC13(CB.Data(),CB.Size(),Database,Image,Mid,GlobalStrings,GlobalStringBytes);if(!P)return P;}}
				C=AlignUp(ObjOff+ON.Size()+1,4);
			}
		}
		if(Options.AddPeExports){auto E=AddExports(Image,Database);if(!E)return E;}
		auto F=Database.Finalize();if(!F)return Library::MakeUnexpected(DbError(F.Error()));return {};
	}
}
