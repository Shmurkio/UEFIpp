#pragma once

#include <cstdint>
#include "../String/String.hpp"
#include "../Cast/Cast.hpp"
#include "../../UEFIpp.hpp"

#pragma pack(push, 1)
typedef struct _MSF_SUPERBLOCK7
{
	char Magic[32];
	uint32_t BlockSize;
	uint32_t FreeBlockMapBlock;
	uint32_t NumBlocks;
	uint32_t NumDirectoryBytes;
	uint32_t Unknown;
	uint32_t BlockMapAddr;
} MSF_SUPERBLOCK7, *PMSF_SUPERBLOCK7;

using CMSF_SUPERBLOCK7 = const MSF_SUPERBLOCK7;
using PCMSF_SUPERBLOCK7 = const MSF_SUPERBLOCK7*;

typedef struct _DBI_STREAM_HEADER
{
	uint32_t VersionSignature;
	uint32_t VersionHeader;
	uint32_t Age;
	uint16_t GlobalSymbolStreamIndex;
	uint16_t BuildNumber;
	uint16_t PublicSymbolStreamIndex;
	uint16_t PdbDllVersion;
	uint16_t SymRecordStreamIndex;
	uint16_t PdbDllRbld;
	int32_t	ModInfoSize;
	int32_t	SectionContributionSize;
	int32_t	SectionMapSize;
	int32_t	FileInfoSize;
	int32_t	TypeServerMapSize;
	uint32_t MFCTypeServerIndex;
	int32_t	OptionalDbgHeaderSize;
	int32_t	ECSubstreamSize;
	uint16_t Flags;
	uint16_t Machine;
	uint32_t Padding;
} DBI_STREAM_HEADER, *PDBI_STREAM_HEADER;

using CDBI_STREAM_HEADER = const DBI_STREAM_HEADER;
using PCDBI_STREAM_HEADER = const DBI_STREAM_HEADER*;
#pragma pack(pop)

static constexpr char MSF7_MAGIC[32] = { 'M','i','c','r','o','s','o','f','t',' ','C','/','C','+','+',' ','M','S','F',' ','7','.','0','0','\r','\n',0x1A,'D','S',0,0,0 };

typedef struct _MSF_CONTEXT
{
	uint8_t* PdbBase;
	uint32_t PdbSize;
	MSF_SUPERBLOCK7	Sb;
	uint32_t StreamCount;
	uint32_t* StreamSizes;
	uint32_t** StreamBlocks;
	uint32_t* StreamNumBlocks;
} MSF_CONTEXT, *PMSF_CONTEXT;

using CMSF_CONTEXT = const MSF_CONTEXT;
using PCMSF_CONTEXT = const MSF_CONTEXT*;

class SymbolResolver
{
public:
	SymbolResolver();
	~SymbolResolver();

	SymbolResolver(const SymbolResolver&) = delete;
	SymbolResolver& operator=(const SymbolResolver&) = delete;

	auto Init(void* PdbBase, uint64_t PdbSize, void* ImageBase) -> bool;

	auto Reset() -> void;

	auto IsReady() const -> bool;

	auto FindRva(String Name, uint64_t& Rva) const -> bool;

	auto FindVa(String Name) const -> void*;

	template<typename T>
	auto FindVa(String Name) const -> T
	{
		void* RawVa = FindVa(Name);
		return RawVa ? Cast::To<T>(RawVa) : T{};
	}

	auto FindRvaByHash(uint64_t NameHash, uint64_t& Rva) const -> bool;

	auto FindVaByHash(uint64_t NameHash, void*& Va) const -> bool;

	template<typename T>
	auto FindVaByHash(uint64_t NameHash, T*& Va) const -> bool
	{
		Va = {};
		void* RawVa = nullptr;

		if (!FindVaByHash(NameHash, RawVa))
		{
			return false;
		}

		Va = Cast::To<T*>(RawVa);

		return true;
	}

	auto Capacity() const -> uint64_t;

	auto Used() const -> uint64_t;

private:
	typedef struct _MAP MAP, *PMAP;
	PMAP Map_;

	uint8_t* PdbBase_;
	uint64_t PdbSize_;

	PIMAGE_SECTION_HEADER Sections_;
	uint16_t SectionCount_;
	uint64_t ImageSize;

	bool Ready_;

	auto InitMap(PMAP Map, uint64_t Capacity) -> bool;

	auto FreeMap(PMAP Map) -> void;

	auto LoadSymbolsFromPdb(uint8_t* PdbBase, uint64_t PdbSize, PIMAGE_SECTION_HEADER Sections, uint16_t SectionCount, uint32_t ImageSize, PMAP Map) -> bool;

	auto InitMsf(PMSF_CONTEXT Ctx, uint8_t* PdbBase, uint64_t PdbSize) -> bool;

	auto ReadMsfStream(PMSF_CONTEXT Ctx, uint32_t StreamIndex, uint8_t*& Buffer, uint64_t& Size) -> bool;

	auto FreeMsf(PMSF_CONTEXT Ctx) -> void;

	auto FindDbiStreamIndex(PMSF_CONTEXT Ctx, uint32_t& DbiStreamIndex, DBI_STREAM_HEADER& DbiHeader) -> bool;
};