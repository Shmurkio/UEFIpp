#include "SymbolResolver.hpp"
#include "../Memory/Memory.hpp"
#include "../Util/Util.hpp"

namespace Hash
{
	static constexpr uint64_t FNV_OFFSET = 0xCBF29CE484222325ULL;
	static constexpr uint64_t FNV_PRIME = 0x100000001B3ULL;

	static auto Fnv1a(const char* Str, uint64_t Length) -> uint64_t
	{
		if (!Str || !*Str || !Length)
		{
			return 0;
		}

		uint64_t Hash = FNV_OFFSET;

		for (uint64_t i = 0; i < Length; ++i)
		{
			Hash ^= Cast::To<uint8_t>(Str[i]);
			Hash *= FNV_PRIME;
		}

		return Hash;
	}
}

typedef struct _ENTRY
{
	uint64_t NameHash;
	uint64_t Rva;
} ENTRY, * PENTRY;

typedef struct _MAP
{
	PENTRY Table;
	uint64_t Capacity;
	uint64_t Used;
} MAP, * PMAP;

static constexpr uint16_t S_PUB32 = 0x110E;
static constexpr uint16_t S_GPROC32 = 0x110F;
static constexpr uint16_t S_LPROC32 = 0x1110;
static constexpr uint16_t S_GPROC32_ID = 0x1147;
static constexpr uint16_t S_LPROC32_ID = 0x1146;

static auto GetStringLength(const String& Value) -> uint64_t
{
	const char* Str = Value.CharStr();

	if (!Str)
	{
		return 0;
	}

	uint64_t Length = 0;

	while (Str[Length])
	{
		++Length;
	}

	return Length;
}

auto SymbolResolver::InitMap(PMAP Map, uint64_t Capacity) -> bool
{
	if (!Map || !Capacity || (Capacity & (Capacity - 1)))
	{
		return false;
	}

	Map->Capacity = Capacity;
	Map->Used = 0;
	Map->Table = nullptr;

	PENTRY Table = nullptr;

	if (!Memory::AllocatePool(Table, Capacity * sizeof(ENTRY), false, true) || !Table)
	{
		return false;
	}

	Map->Table = Table;
	return true;
}

auto SymbolResolver::FreeMap(PMAP Map) -> void
{
	if (!Map)
	{
		return;
	}

	if (Map->Table)
	{
		Memory::FreePool(Map->Table, false);
	}

	Map->Table = nullptr;
	Map->Capacity = 0;
	Map->Used = 0;
}

static auto Insert(PMAP Map, uint64_t NameHash, uint64_t Rva) -> bool
{
	if (!Map || !Map->Table || !NameHash)
	{
		return false;
	}

	uint64_t Mask = Map->Capacity - 1;
	uint64_t Idx = NameHash & Mask;

	for (uint64_t i = 0; i < Map->Capacity; ++i)
	{
		auto& Entry = Map->Table[Idx];

		if (!Entry.NameHash)
		{
			Entry.NameHash = NameHash;
			Entry.Rva = Rva;
			++Map->Used;
			return true;
		}

		if (Entry.NameHash == NameHash)
		{
			return true;
		}

		Idx = (Idx + 1) & Mask;
	}

	return false;
}

static auto Find(const PMAP Map, uint64_t NameHash, uint64_t& Rva) -> bool
{
	Rva = 0;

	if (!Map || !Map->Table || !NameHash)
	{
		return false;
	}

	uint64_t Mask = Map->Capacity - 1;
	uint64_t Idx = NameHash & Mask;

	for (uint64_t i = 0; i < Map->Capacity; ++i)
	{
		const auto& Entry = Map->Table[Idx];

		if (!Entry.NameHash)
		{
			return false;
		}

		if (Entry.NameHash == NameHash)
		{
			Rva = Entry.Rva;
			return true;
		}

		Idx = (Idx + 1) & Mask;
	}

	return false;
}

static auto IsCStrTerminatedWithin(const char* Str, uint64_t MaxLength, uint64_t& Length) -> bool
{
	Length = 0;

	if (!Str || !MaxLength)
	{
		return false;
	}

	for (uint64_t i = 0; i < MaxLength; ++i)
	{
		if (Str[i] == '\0')
		{
			Length = i;
			return true;
		}
	}

	return false;
}

static auto SegmentOffsetToRva(PIMAGE_SECTION_HEADER Sections, uint16_t SectionCount, uint64_t ImageSize, uint16_t Segment, uint64_t Offset, uint64_t& Rva) -> bool
{
	Rva = 0;

	if (!Sections || !Segment || !SectionCount || !ImageSize)
	{
		return false;
	}

	auto Idx = Cast::To<uint64_t>(Segment - 1);

	if (Idx >= SectionCount)
	{
		return false;
	}

	uint64_t TempRva = Cast::To<uint64_t>(Sections[Idx].VirtualAddress) + Offset;

	if (TempRva >= ImageSize)
	{
		return false;
	}

	Rva = TempRva;
	return true;
}

static auto ParseSymRecordStream(uint8_t* Stream, uint64_t StreamSize, PIMAGE_SECTION_HEADER Sections, uint16_t SectionCount, uint64_t ImageSize, PMAP Map) -> bool
{
	if (!Stream || !StreamSize || !Sections || !SectionCount || !ImageSize || !Map)
	{
		return false;
	}

	uint8_t* End = Stream + StreamSize;

	for (auto i = Stream; i + 4 <= End; )
	{
		uint16_t RecordLength = 0;

		if (!Memory::ReadU16(i, RecordLength))
		{
			return false;
		}

		i += 2;

		if (RecordLength < 2 || i + RecordLength > End)
		{
			break;
		}

		uint16_t RecordType = 0;

		if (!Memory::ReadU16(i, RecordType))
		{
			return false;
		}

		i += 2;

		uint8_t* Payload = i;
		uint64_t PayloadSize = RecordLength - 2;

		uint16_t Segment = 0;
		uint32_t Offset = 0;
		char* Name = nullptr;
		uint64_t NameLength = 0;

		switch (RecordType)
		{
		case S_PUB32:
		{
			if (PayloadSize >= 10)
			{
				if (!Memory::ReadU32(Payload + 4, Offset) || !Memory::ReadU16(Payload + 8, Segment))
				{
					return false;
				}

				Name = Cast::To<char*>(Payload + 10);
				uint64_t MaxNameLength = PayloadSize - 10;

				if (!IsCStrTerminatedWithin(Name, MaxNameLength, NameLength) || !NameLength)
				{
					Name = nullptr;
				}
			}

			break;
		}
		case S_GPROC32:
		case S_LPROC32:
		case S_GPROC32_ID:
		case S_LPROC32_ID:
		{
			if (PayloadSize >= 35)
			{
				if (!Memory::ReadU32(Payload + 28, Offset) || !Memory::ReadU16(Payload + 32, Segment))
				{
					return false;
				}

				Name = Cast::To<char*>(Payload + 35);
				uint64_t MaxNameLength = PayloadSize - 35;

				if (!IsCStrTerminatedWithin(Name, MaxNameLength, NameLength) || !NameLength)
				{
					Name = nullptr;
				}
			}

			break;
		}
		default:
		{
			break;
		}
		}

		if (Name)
		{
			uint64_t Rva = 0;

			if (SegmentOffsetToRva(Sections, SectionCount, ImageSize, Segment, Offset, Rva))
			{
				uint64_t NameHash = Hash::Fnv1a(Name, NameLength);
				Insert(Map, NameHash, Rva);
			}
		}

		i = Payload + PayloadSize;

		if (Cast::To<uint64_t>(i) & 1)
		{
			++i;
		}
	}

	return true;
}

auto SymbolResolver::FreeMsf(PMSF_CONTEXT Ctx) -> void
{
	if (!Ctx)
	{
		return;
	}

	if (Ctx->StreamBlocks)
	{
		for (uint64_t i = 0; i < Ctx->StreamCount; ++i)
		{
			if (Ctx->StreamBlocks[i])
			{
				Memory::FreePool(Ctx->StreamBlocks[i], false);
			}
		}

		Memory::FreePool(Ctx->StreamBlocks, false);
	}

	if (Ctx->StreamSizes)
	{
		Memory::FreePool(Ctx->StreamSizes, false);
	}

	if (Ctx->StreamNumBlocks)
	{
		Memory::FreePool(Ctx->StreamNumBlocks, false);
	}

	Ctx->PdbBase = nullptr;
	Ctx->PdbSize = 0;
	Ctx->StreamCount = 0;
	Ctx->StreamSizes = nullptr;
	Ctx->StreamBlocks = nullptr;
	Ctx->StreamNumBlocks = nullptr;
}

auto SymbolResolver::InitMsf(PMSF_CONTEXT Ctx, uint8_t* PdbBase, uint64_t PdbSize) -> bool
{
	auto Fail = [&](uint8_t* Dir) -> bool
		{
			if (Dir)
			{
				Memory::FreePool(Dir, false);
			}

			FreeMsf(Ctx);
			return false;
		};

	if (!Ctx || !PdbBase || PdbSize < sizeof(MSF_SUPERBLOCK7))
	{
		return false;
	}

	Ctx->PdbBase = PdbBase;
	Ctx->PdbSize = Cast::To<uint32_t>(PdbSize);
	Ctx->StreamCount = 0;
	Ctx->StreamSizes = nullptr;
	Ctx->StreamBlocks = nullptr;
	Ctx->StreamNumBlocks = nullptr;

	auto Sb = Cast::To<PMSF_SUPERBLOCK7>(PdbBase);
	uint64_t MatchingBytes = 0;

	if (!Memory::CompareMemory(Sb->Magic, Cast::To<void*>(const_cast<char*>(MSF7_MAGIC)), sizeof(MSF7_MAGIC), MatchingBytes) || MatchingBytes != sizeof(MSF7_MAGIC))
	{
		return Fail(nullptr);
	}

	Ctx->Sb = *Sb;

	uint32_t Bs = Ctx->Sb.BlockSize;

	if (!Bs || (Bs & (Bs - 1)))
	{
		return Fail(nullptr);
	}

	uint64_t MapOff = Cast::To<uint64_t>(Ctx->Sb.BlockMapAddr) * Cast::To<uint64_t>(Bs);

	if (MapOff + Bs > PdbSize)
	{
		return Fail(nullptr);
	}

	auto DirBlockList = Cast::To<uint32_t*>(PdbBase + MapOff);
	uint32_t DirBytes = Ctx->Sb.NumDirectoryBytes;
	uint32_t DirBlocks = (DirBytes + Bs - 1) / Bs;

	if (!DirBlocks)
	{
		return Fail(nullptr);
	}

	uint8_t* Dir = nullptr;

	if (!Memory::AllocatePool(Dir, Cast::To<uint64_t>(DirBlocks) * Bs, false, false) || !Dir)
	{
		return Fail(nullptr);
	}

	for (uint32_t i = 0; i < DirBlocks; ++i)
	{
		uint32_t B = DirBlockList[i];

		if (B >= Ctx->Sb.NumBlocks)
		{
			return Fail(Dir);
		}

		uint64_t Off = Cast::To<uint64_t>(B) * Cast::To<uint64_t>(Bs);

		if (Off + Bs > PdbSize)
		{
			return Fail(Dir);
		}

		if (!Memory::CopyMemory(Dir + (Cast::To<uint64_t>(i) * Bs), PdbBase + Off, Bs))
		{
			return Fail(Dir);
		}
	}

	if (DirBytes < 4)
	{
		return Fail(Dir);
	}

	if (!Memory::ReadU32(Dir, Ctx->StreamCount) || !Ctx->StreamCount)
	{
		return Fail(Dir);
	}

	uint64_t MinNeed = 4ULL + 4ULL * Ctx->StreamCount;

	if (MinNeed > DirBytes)
	{
		return Fail(Dir);
	}

	if (!Memory::AllocatePool(Ctx->StreamSizes, Ctx->StreamCount * sizeof(uint32_t), false, false) ||
		!Memory::AllocatePool(Ctx->StreamNumBlocks, Ctx->StreamCount * sizeof(uint32_t), false, false) ||
		!Memory::AllocatePool(Ctx->StreamBlocks, Ctx->StreamCount * sizeof(uint32_t*), false, false) ||
		!Ctx->StreamSizes || !Ctx->StreamNumBlocks || !Ctx->StreamBlocks)
	{
		return Fail(Dir);
	}

	for (uint32_t i = 0; i < Ctx->StreamCount; ++i)
	{
		Ctx->StreamBlocks[i] = nullptr;
	}

	uint8_t* P = Dir + 4;
	uint8_t* DirEnd = Dir + DirBytes;

	for (uint32_t i = 0; i < Ctx->StreamCount; ++i)
	{
		if (P + 4 > DirEnd)
		{
			return Fail(Dir);
		}

		uint32_t Sz = 0;

		if (!Memory::ReadU32(P, Sz))
		{
			return Fail(Dir);
		}

		P += 4;
		Ctx->StreamSizes[i] = Sz;

		if (Sz == 0xFFFFFFFFU)
		{
			Ctx->StreamNumBlocks[i] = 0;
			continue;
		}

		Ctx->StreamNumBlocks[i] = (Sz + Bs - 1) / Bs;
	}

	for (uint32_t i = 0; i < Ctx->StreamCount; ++i)
	{
		uint32_t Sz = Ctx->StreamSizes[i];
		uint32_t Nb = Ctx->StreamNumBlocks[i];

		if (Sz == 0xFFFFFFFFU || !Nb)
		{
			continue;
		}

		if (P + (Cast::To<uint64_t>(Nb) * 4) > DirEnd)
		{
			return Fail(Dir);
		}

		if (!Memory::AllocatePool(Ctx->StreamBlocks[i], Nb * sizeof(uint32_t), false, false) || !Ctx->StreamBlocks[i])
		{
			return Fail(Dir);
		}

		for (uint32_t j = 0; j < Nb; ++j)
		{
			uint32_t Blk = 0;

			if (!Memory::ReadU32(P, Blk))
			{
				return Fail(Dir);
			}

			P += 4;

			if (Blk >= Ctx->Sb.NumBlocks)
			{
				return Fail(Dir);
			}

			Ctx->StreamBlocks[i][j] = Blk;
		}
	}

	Memory::FreePool(Dir, false);
	return true;
}

auto SymbolResolver::ReadMsfStream(PMSF_CONTEXT Ctx, uint32_t StreamIndex, uint8_t*& Buffer, uint64_t& Size) -> bool
{
	Buffer = nullptr;
	Size = 0;

	if (!Ctx || !Ctx->PdbBase || StreamIndex >= Ctx->StreamCount)
	{
		return false;
	}

	uint32_t Sz = Ctx->StreamSizes[StreamIndex];

	if (Sz == 0xFFFFFFFFU)
	{
		return false;
	}

	Size = Sz;
	uint64_t Bytes = Sz ? Sz : 1;

	if (!Memory::AllocatePool(Buffer, Bytes, false, false) || !Buffer)
	{
		Size = 0;
		return false;
	}

	uint32_t Bs = Ctx->Sb.BlockSize;
	uint32_t Nb = Ctx->StreamNumBlocks[StreamIndex];
	auto Bl = Ctx->StreamBlocks[StreamIndex];
	uint32_t Left = Sz;
	uint8_t* W = Buffer;

	for (uint32_t i = 0; i < Nb; ++i)
	{
		uint32_t BlockIndex = Bl[i];

		if (BlockIndex >= Ctx->Sb.NumBlocks)
		{
			Memory::FreePool(Buffer, false);
			Buffer = nullptr;
			Size = 0;
			return false;
		}

		uint64_t Off = Cast::To<uint64_t>(BlockIndex) * Cast::To<uint64_t>(Bs);

		if (Off + Bs > Ctx->PdbSize)
		{
			Memory::FreePool(Buffer, false);
			Buffer = nullptr;
			Size = 0;
			return false;
		}

		uint32_t Copy = (Left > Bs) ? Bs : Left;

		if (Copy && !Memory::CopyMemory(W, Ctx->PdbBase + Off, Copy))
		{
			Memory::FreePool(Buffer, false);
			Buffer = nullptr;
			Size = 0;
			return false;
		}

		W += Copy;
		Left -= Copy;

		if (!Left)
		{
			break;
		}
	}

	return true;
}

static auto LooksLikeDbi(PCDBI_STREAM_HEADER H, uint64_t StreamSize, uint32_t TotalStreams) -> bool
{
	if (!H || StreamSize < sizeof(DBI_STREAM_HEADER))
	{
		return false;
	}

	if (H->VersionSignature != 0xFFFFFFFFU)
	{
		return false;
	}

	if (H->SymRecordStreamIndex >= TotalStreams)
	{
		return false;
	}

	if (H->ModInfoSize < 0 || H->SectionMapSize < 0 || H->FileInfoSize < 0)
	{
		return false;
	}

	return true;
}

auto SymbolResolver::FindDbiStreamIndex(PMSF_CONTEXT Ctx, uint32_t& DbiStreamIndex, DBI_STREAM_HEADER& DbiHeader) -> bool
{
	DbiStreamIndex = 0xFFFFFFFFU;
	DbiHeader = {};

	if (!Ctx)
	{
		return false;
	}

	for (uint32_t i = 0; i < Ctx->StreamCount; ++i)
	{
		uint8_t* Buf = nullptr;
		uint64_t Sz = 0;

		if (!ReadMsfStream(Ctx, i, Buf, Sz) || !Buf)
		{
			continue;
		}

		if (Sz >= sizeof(DBI_STREAM_HEADER))
		{
			auto H = Cast::To<PCDBI_STREAM_HEADER>(Buf);

			if (LooksLikeDbi(H, Sz, Ctx->StreamCount))
			{
				DbiStreamIndex = i;
				DbiHeader = *H;
				Memory::FreePool(Buf, false);
				return true;
			}
		}

		Memory::FreePool(Buf, false);
	}

	return false;
}

auto SymbolResolver::LoadSymbolsFromPdb(uint8_t* PdbBase, uint64_t PdbSize, PIMAGE_SECTION_HEADER Sections, uint16_t SectionCount, uint32_t ImageSizeValue, PMAP Map) -> bool
{
	if (!PdbBase || !PdbSize || !Sections || !SectionCount || !ImageSizeValue || !Map)
	{
		return false;
	}

	MSF_CONTEXT Msf{};

	if (!InitMsf(&Msf, PdbBase, PdbSize))
	{
		return false;
	}

	uint32_t DbiIndex = 0xFFFFFFFFU;
	DBI_STREAM_HEADER DbiHdr{};

	if (!FindDbiStreamIndex(&Msf, DbiIndex, DbiHdr))
	{
		FreeMsf(&Msf);
		return false;
	}

	uint8_t* SymRec = nullptr;
	uint64_t SymRecSize = 0;

	if (!ReadMsfStream(&Msf, DbiHdr.SymRecordStreamIndex, SymRec, SymRecSize) || !SymRec || !SymRecSize)
	{
		if (SymRec)
		{
			Memory::FreePool(SymRec, false);
		}

		FreeMsf(&Msf);
		return false;
	}

	if (!Map->Table)
	{
		if (!InitMap(Map, 1ULL << 16))
		{
			Memory::FreePool(SymRec, false);
			FreeMsf(&Msf);
			return false;
		}
	}

	bool Status = ParseSymRecordStream(SymRec, SymRecSize, Sections, SectionCount, ImageSizeValue, Map);

	Memory::FreePool(SymRec, false);
	FreeMsf(&Msf);

	return Status;
}

SymbolResolver::SymbolResolver()
	: Map_(nullptr),
	PdbBase_(nullptr),
	PdbSize_(0),
	Sections_(nullptr),
	SectionCount_(0),
	ImageSize(0),
	Ready_(false)
{
}

SymbolResolver::~SymbolResolver()
{
	Reset();
}

auto SymbolResolver::Init(void* PdbBase, uint64_t PdbSize, void* ImageBase) -> bool
{
	Reset();

	if (!PdbBase || !PdbSize || !ImageBase)
	{
		return false;
	}

	PIMAGE_NT_HEADERS64 Nt = nullptr;

	if (!Util::GetNtHeaders64(ImageBase, Nt) || !Nt)
	{
		return false;
	}

	PIMAGE_SECTION_HEADER Sec = nullptr;
	uint16_t SecCount = 0;

	if (!Util::GetSectionHeaders(ImageBase, Sec, SecCount) || !Sec)
	{
		return false;
	}

	Map_ = nullptr;

	if (!Memory::AllocatePool(Map_, sizeof(MAP), false, true) || !Map_)
	{
		return false;
	}

	if (!InitMap(Map_, 1ULL << 16))
	{
		Memory::FreePool(Map_, false);
		Map_ = nullptr;
		return false;
	}

	PdbBase_ = Cast::To<uint8_t*>(PdbBase);
	PdbSize_ = PdbSize;
	Sections_ = Sec;
	SectionCount_ = SecCount;
	ImageSize = Nt->OptionalHeader.SizeOfImage;

	if (!LoadSymbolsFromPdb(PdbBase_, PdbSize_, Sections_, SectionCount_, Cast::To<uint32_t>(ImageSize), Map_))
	{
		Reset();
		return false;
	}

	PdbBase_ = Cast::To<uint8_t*>(ImageBase);
	Ready_ = true;
	return true;
}

auto SymbolResolver::Reset() -> void
{
	Ready_ = false;

	if (Map_)
	{
		FreeMap(Map_);
		Memory::FreePool(Map_, false);
		Map_ = nullptr;
	}

	PdbBase_ = nullptr;
	PdbSize_ = 0;
	Sections_ = nullptr;
	SectionCount_ = 0;
	ImageSize = 0;
}

auto SymbolResolver::IsReady() const -> bool
{
	return Ready_;
}

auto SymbolResolver::FindRva(String Name, uint64_t& Rva) const -> bool
{
	Rva = 0;

	if (!Ready_ || !Map_)
	{
		return false;
	}

	const char* RawName = Name.CharStr();
	uint64_t Length = GetStringLength(Name);

	if (!RawName || !Length)
	{
		return false;
	}

	uint64_t NameHash = Hash::Fnv1a(RawName, Length);
	return Find(Map_, NameHash, Rva);
}

auto SymbolResolver::FindVa(String Name) const -> void*
{
	void* Va = nullptr;
	uint64_t Rva = 0;

	if (!FindRva(Name, Rva))
	{
		return nullptr;
	}

	if (!Sections_)
	{
		return nullptr;
	}

	Va = Cast::To<void*>(Cast::To<uint64_t>(PdbBase_) + Rva);
	return Va;
}

auto SymbolResolver::FindRvaByHash(uint64_t NameHash, uint64_t& Rva) const -> bool
{
	Rva = 0;

	if (!Ready_ || !Map_ || !NameHash)
	{
		return false;
	}

	return Find(Map_, NameHash, Rva);
}

auto SymbolResolver::FindVaByHash(uint64_t NameHash, void*& Va) const -> bool
{
	Va = nullptr;

	uint64_t Rva = 0;

	if (!FindRvaByHash(NameHash, Rva))
	{
		return false;
	}

	if (!Sections_)
	{
		return false;
	}

	Va = Cast::To<void*>(Cast::To<uint64_t>(PdbBase_) + Rva);
	return true;
}

auto SymbolResolver::Capacity() const -> uint64_t
{
	return Map_ ? Map_->Capacity : 0;
}

auto SymbolResolver::Used() const -> uint64_t
{
	return Map_ ? Map_->Used : 0;
}