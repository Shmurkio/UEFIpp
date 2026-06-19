#include <UEFIpp/Reverse/SymbolResolver.hpp>

#include <UEFIpp/Memory/Memory.hpp>
#include <UEFIpp/Stream/File/FileInputStream.hpp>

namespace UEFIpp::Reverse
{
	namespace
	{
		inline constexpr auto FnvOffset = Foundation::Uint64{ 0xCBF29CE484222325ull };
		inline constexpr auto FnvPrime = Foundation::Uint64{ 0x100000001B3ull };
		inline constexpr Foundation::Char Msf7Magic[32]{ 'M', 'i', 'c', 'r', 'o', 's', 'o', 'f', 't', ' ', 'C', '/', 'C', '+', '+', ' ', 'M', 'S', 'F', ' ', '7', '.', '0', '0', '\r', '\n', Foundation::Char{ 0x1A }, 'D', 'S', 0, 0, 0 };
		inline constexpr auto DeletedStreamSize = Foundation::Uint32{ 0xFFFFFFFFu };
		inline constexpr auto S_PUB32 = Foundation::Uint16{ 0x110E };
		inline constexpr auto S_GPROC32 = Foundation::Uint16{ 0x110F };
		inline constexpr auto S_LPROC32 = Foundation::Uint16{ 0x1110 };
		inline constexpr auto S_LPROC32_ID = Foundation::Uint16{ 0x1146 };
		inline constexpr auto S_GPROC32_ID = Foundation::Uint16{ 0x1147 };

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

		struct DbiStreamHeader
		{
			Foundation::Uint32 VersionSignature;
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
#pragma pack(pop)

		struct MsfContext
		{
			const Foundation::Uint8* Base{};
			Foundation::Uint64 Size{};
			MsfSuperBlock7 SuperBlock{};
			Foundation::Uint32 StreamCount{};
			Foundation::Uint32* StreamSizes{};
			Foundation::Uint32* StreamBlockCounts{};
			Foundation::Uint32** StreamBlocks{};
		};

		[[nodiscard]] auto AlignUp(Foundation::Uint32 Value, Foundation::Uint32 Alignment) -> Foundation::Uint32
		{
			return (Value + Alignment - 1) / Alignment;
		}

		[[nodiscard]] auto IsPowerOfTwo(Foundation::Uint64 Value) -> Foundation::Bool
		{
			return Value && ((Value & (Value - 1)) == 0);
		}

		[[nodiscard]] auto NextPowerOfTwo(Foundation::Uint64 Value) -> Foundation::Uint64
		{
			if (Value <= 2)
			{
				return 2;
			}

			--Value;

			for (Foundation::Uint64 Shift = 1; Shift < 64; Shift <<= 1)
			{
				Value |= Value >> Shift;
			}

			return Value + 1;
		}

		[[nodiscard]] auto ReadU16(const Foundation::Uint8* Data) -> Foundation::Uint16
		{
			return Foundation::Cast::Auto<Foundation::Uint16>(Data[0]) | (Foundation::Cast::Auto<Foundation::Uint16>(Data[1]) << 8);
		}

		[[nodiscard]] auto ReadU32(const Foundation::Uint8* Data) -> Foundation::Uint32
		{
			return Foundation::Cast::Auto<Foundation::Uint32>(Data[0]) | (Foundation::Cast::Auto<Foundation::Uint32>(Data[1]) << 8) | (Foundation::Cast::Auto<Foundation::Uint32>(Data[2]) << 16) | (Foundation::Cast::Auto<Foundation::Uint32>(Data[3]) << 24);
		}

		[[nodiscard]] auto CStrLengthWithin(const Foundation::Char* Text, Foundation::Uint64 MaxLength, Foundation::Uint64& Length) -> Foundation::Bool
		{
			Length = 0;

			if (!Text || !MaxLength)
			{
				return false;
			}

			for (Foundation::Uint64 i = 0; i < MaxLength; ++i)
			{
				if (Text[i] == 0)
				{
					Length = i;
					return true;
				}
			}

			return false;
		}

		[[nodiscard]] auto HashBytes(const Foundation::Char* Text, Foundation::Uint64 Length) -> Foundation::Uint64
		{
			if (!Text || !Length)
			{
				return 0;
			}

			auto Hash = FnvOffset;

			for (Foundation::Uint64 i = 0; i < Length; ++i)
			{
				Hash ^= Foundation::Cast::Auto<Foundation::Uint8>(Text[i]);
				Hash *= FnvPrime;
			}

			return Hash;
		}

		auto FreeMsf(MsfContext& Context) -> Foundation::Void
		{
			if (Context.StreamBlocks)
			{
				for (Foundation::Uint32 i = 0; i < Context.StreamCount; ++i)
				{
					delete[] Context.StreamBlocks[i];
				}
			}

			delete[] Context.StreamBlocks;
			delete[] Context.StreamBlockCounts;
			delete[] Context.StreamSizes;

			Context = {};
		}

		[[nodiscard]] auto ValidateBlockRange(const MsfContext& Context, Foundation::Uint32 BlockIndex, Foundation::Uint32 Bytes) -> Foundation::Bool
		{
			if (BlockIndex >= Context.SuperBlock.NumBlocks)
			{
				return false;
			}

			const auto Offset = Foundation::Cast::Auto<Foundation::Uint64>(BlockIndex) * Context.SuperBlock.BlockSize;
			return Offset + Bytes <= Context.Size;
		}

		[[nodiscard]] auto ReadMsfStream(const MsfContext& Context, Foundation::Uint32 StreamIndex, Library::Vector<Foundation::Uint8>& Buffer) -> Foundation::Bool
		{
			Buffer.Clear();

			if (!Context.Base || StreamIndex >= Context.StreamCount)
			{
				return false;
			}

			const auto StreamSize = Context.StreamSizes[StreamIndex];

			if (StreamSize == DeletedStreamSize)
			{
				return false;
			}

			if (!Buffer.Resize(StreamSize))
			{
				return false;
			}

			if (!StreamSize)
			{
				return true;
			}

			const auto BlockSize = Context.SuperBlock.BlockSize;
			const auto BlockCount = Context.StreamBlockCounts[StreamIndex];
			const auto* Blocks = Context.StreamBlocks[StreamIndex];
			auto* Out = Buffer.Data();
			auto Left = StreamSize;

			for (Foundation::Uint32 i = 0; i < BlockCount&& Left; ++i)
			{
				const auto BlockIndex = Blocks[i];

				if (!ValidateBlockRange(Context, BlockIndex, BlockSize))
				{
					Buffer.Clear();
					return false;
				}

				const auto Offset = Foundation::Cast::Auto<Foundation::Uint64>(BlockIndex) * BlockSize;
				const auto CopySize = Left > BlockSize ? BlockSize : Left;

				Memory::Copy(Out, Context.Base + Offset, CopySize);

				Out += CopySize;
				Left -= CopySize;
			}

			if (Left)
			{
				Buffer.Clear();
				return false;
			}

			return true;
		}

		[[nodiscard]] auto InitMsf(MsfContext& Context, const Foundation::Uint8* PdbBase, Foundation::Uint64 PdbSize) -> Foundation::Bool
		{
			Context = {};

			if (!PdbBase || PdbSize < sizeof(MsfSuperBlock7))
			{
				return false;
			}

			const auto* SuperBlock = Foundation::Cast::Auto<const MsfSuperBlock7*>(PdbBase);

			if (Memory::Compare(SuperBlock->Magic, Msf7Magic, sizeof(Msf7Magic)) != 0)
			{
				return false;
			}

			if (!IsPowerOfTwo(SuperBlock->BlockSize) || !SuperBlock->NumBlocks || !SuperBlock->NumDirectoryBytes)
			{
				return false;
			}

			Context.Base = PdbBase;
			Context.Size = PdbSize;
			Context.SuperBlock = *SuperBlock;

			const auto BlockSize = Context.SuperBlock.BlockSize;
			const auto DirectoryBytes = Context.SuperBlock.NumDirectoryBytes;
			const auto DirectoryBlocks = AlignUp(DirectoryBytes, BlockSize);
			const auto BlockMapOffset = Foundation::Cast::Auto<Foundation::Uint64>(Context.SuperBlock.BlockMapAddr) * BlockSize;

			if (!DirectoryBlocks || BlockMapOffset + Foundation::Cast::Auto<Foundation::Uint64>(DirectoryBlocks) * sizeof(Foundation::Uint32) > PdbSize)
			{
				return false;
			}

			const auto* DirectoryBlockList = Foundation::Cast::Auto<const Foundation::Uint32*>(PdbBase + BlockMapOffset);
			Library::Vector<Foundation::Uint8> Directory{};

			if (!Directory.Resize(DirectoryBlocks * BlockSize))
			{
				return false;
			}

			for (Foundation::Uint32 i = 0; i < DirectoryBlocks; ++i)
			{
				const auto Block = DirectoryBlockList[i];

				if (!ValidateBlockRange(Context, Block, BlockSize))
				{
					return false;
				}

				const auto Offset = Foundation::Cast::Auto<Foundation::Uint64>(Block) * BlockSize;
				Memory::Copy(Directory.Data() + Foundation::Cast::Auto<Foundation::Uint64>(i) * BlockSize, PdbBase + Offset, BlockSize);
			}

			if (DirectoryBytes < sizeof(Foundation::Uint32))
			{
				return false;
			}

			const auto* DirectoryData = Directory.Data();
			const auto* DirectoryEnd = DirectoryData + DirectoryBytes;
			auto* Cursor = DirectoryData;

			Context.StreamCount = ReadU32(Cursor);
			Cursor += sizeof(Foundation::Uint32);

			if (!Context.StreamCount || Cursor + Foundation::Cast::Auto<Foundation::Uint64>(Context.StreamCount) * sizeof(Foundation::Uint32) > DirectoryEnd)
			{
				FreeMsf(Context);
				return false;
			}

			Context.StreamSizes = new Foundation::Uint32[Context.StreamCount]{};
			Context.StreamBlockCounts = new Foundation::Uint32[Context.StreamCount]{};
			Context.StreamBlocks = new Foundation::Uint32 * [Context.StreamCount] {};

			if (!Context.StreamSizes || !Context.StreamBlockCounts || !Context.StreamBlocks)
			{
				FreeMsf(Context);
				return false;
			}

			for (Foundation::Uint32 i = 0; i < Context.StreamCount; ++i)
			{
				Context.StreamSizes[i] = ReadU32(Cursor);
				Cursor += sizeof(Foundation::Uint32);

				if (Context.StreamSizes[i] != DeletedStreamSize)
				{
					Context.StreamBlockCounts[i] = AlignUp(Context.StreamSizes[i], BlockSize);
				}
			}

			for (Foundation::Uint32 i = 0; i < Context.StreamCount; ++i)
			{
				const auto BlockCount = Context.StreamBlockCounts[i];

				if (!BlockCount)
				{
					continue;
				}

				if (Cursor + Foundation::Cast::Auto<Foundation::Uint64>(BlockCount) * sizeof(Foundation::Uint32) > DirectoryEnd)
				{
					FreeMsf(Context);
					return false;
				}

				Context.StreamBlocks[i] = new Foundation::Uint32[BlockCount]{};

				if (!Context.StreamBlocks[i])
				{
					FreeMsf(Context);
					return false;
				}

				for (Foundation::Uint32 j = 0; j < BlockCount; ++j)
				{
					Context.StreamBlocks[i][j] = ReadU32(Cursor);
					Cursor += sizeof(Foundation::Uint32);

					if (Context.StreamBlocks[i][j] >= Context.SuperBlock.NumBlocks)
					{
						FreeMsf(Context);
						return false;
					}
				}
			}

			return true;
		}

		[[nodiscard]] auto LooksLikeDbi(const DbiStreamHeader& Header, Foundation::Uint64 StreamSize, Foundation::Uint32 StreamCount) -> Foundation::Bool
		{
			if (StreamSize < sizeof(DbiStreamHeader))
			{
				return false;
			}

			if (Header.VersionSignature != 0xFFFFFFFFu || Header.SymRecordStreamIndex >= StreamCount)
			{
				return false;
			}

			if (Header.ModInfoSize < 0 || Header.SectionMapSize < 0 || Header.FileInfoSize < 0)
			{
				return false;
			}

			return true;
		}

		[[nodiscard]] auto FindDbiStream(const MsfContext& Context, Foundation::Uint32& StreamIndex, DbiStreamHeader& Header) -> Foundation::Bool
		{
			StreamIndex = 0xFFFFFFFFu;
			Header = {};

			Library::Vector<Foundation::Uint8> Stream{};

			for (Foundation::Uint32 i = 0; i < Context.StreamCount; ++i)
			{
				if (!ReadMsfStream(Context, i, Stream))
				{
					continue;
				}

				if (Stream.Size() >= sizeof(DbiStreamHeader))
				{
					const auto* Candidate = Foundation::Cast::Auto<const DbiStreamHeader*>(Stream.Data());

					if (LooksLikeDbi(*Candidate, Stream.Size(), Context.StreamCount))
					{
						StreamIndex = i;
						Header = *Candidate;
						return true;
					}
				}
			}

			return false;
		}
	}

	SymbolResolver::~SymbolResolver()
	{
		Reset();
	}

	SymbolResolver::SymbolResolver(SymbolResolver&& Other) noexcept
	{
		*this = Foundation::Utility::Move(Other);
	}

	auto SymbolResolver::operator=(SymbolResolver&& Other) noexcept -> SymbolResolver&
	{
		if (this == &Other)
		{
			return *this;
		}

		Reset();

		Entries_ = Other.Entries_;
		Capacity_ = Other.Capacity_;
		Used_ = Other.Used_;
		ImageBase_ = Other.ImageBase_;
		Sections_ = Other.Sections_;
		SectionCount_ = Other.SectionCount_;
		ImageSize_ = Other.ImageSize_;
		Ready_ = Other.Ready_;

		Other.Entries_ = nullptr;
		Other.Capacity_ = 0;
		Other.Used_ = 0;
		Other.ImageBase_ = nullptr;
		Other.Sections_ = nullptr;
		Other.SectionCount_ = 0;
		Other.ImageSize_ = 0;
		Other.Ready_ = false;

		return *this;
	}

	auto SymbolResolver::Load(const FileSystem::Path& PdbPath, const Foundation::Void* ImageBase, Foundation::Uint64 InitialCapacity) -> Foundation::Bool
	{
		Stream::FileInputStream PdbFile{};

		if (!PdbFile.Open(PdbPath))
		{
			return false;
		}

		const auto& Buffer = PdbFile.Buffer();
		return Load(Buffer.Data(), Foundation::Cast::Auto<Foundation::Uint64>(Buffer.Size()), ImageBase, InitialCapacity);
	}

	auto SymbolResolver::Load(const Library::Vector<Foundation::Uint8>& Pdb, const Foundation::Void* ImageBase, Foundation::Uint64 InitialCapacity) -> Foundation::Bool
	{
		return Load(Pdb.Data(), Foundation::Cast::Auto<Foundation::Uint64>(Pdb.Size()), ImageBase, InitialCapacity);
	}

	auto SymbolResolver::Load(const Foundation::Uint8* PdbBase, Foundation::Uint64 PdbSize, const Foundation::Void* ImageBase, Foundation::Uint64 InitialCapacity) -> Foundation::Bool
	{
		Reset();

		if (!PdbBase || !PdbSize || !ImageBase)
		{
			return false;
		}

		if (!LoadImageLayout(ImageBase))
		{
			Reset();
			return false;
		}

		if (!Reserve(InitialCapacity))
		{
			Reset();
			return false;
		}

		if (!ParsePdb(PdbBase, PdbSize))
		{
			Reset();
			return false;
		}

		Ready_ = true;
		return true;
	}

	auto SymbolResolver::Reset() -> Foundation::Void
	{
		Ready_ = false;
		FreeMap();

		ImageBase_ = nullptr;
		Sections_ = nullptr;
		SectionCount_ = 0;
		ImageSize_ = 0;
	}

	auto SymbolResolver::Ready() const -> Foundation::Bool
	{
		return Ready_;
	}

	auto SymbolResolver::Valid() const -> Foundation::Bool
	{
		return Ready();
	}

	SymbolResolver::operator Foundation::Bool() const
	{
		return Valid();
	}

	auto SymbolResolver::FindRva(const Library::String& Name, Foundation::Uint64& Rva) const -> Foundation::Bool
	{
		return FindRva(Name.View(), Rva);
	}

	auto SymbolResolver::FindRva(Library::StringView Name, Foundation::Uint64& Rva) const -> Foundation::Bool
	{
		Rva = 0;

		if (!Ready_)
		{
			return false;
		}

		return Find(HashName(Name), Rva);
	}

	auto SymbolResolver::FindRva(const Foundation::Char* Name, Foundation::Uint64& Rva) const -> Foundation::Bool
	{
		return FindRva(Library::StringView(Name), Rva);
	}

	auto SymbolResolver::FindVa(const Library::String& Name, Foundation::Void*& Address) const -> Foundation::Bool
	{
		return FindVa(Name.View(), Address);
	}

	auto SymbolResolver::FindVa(Library::StringView Name, Foundation::Void*& Address) const -> Foundation::Bool
	{
		Address = nullptr;

		Foundation::Uint64 Rva{};

		if (!FindRva(Name, Rva) || !ImageBase_)
		{
			return false;
		}

		Address = Foundation::Cast::Auto<Foundation::Void*>(Foundation::Cast::PointerToAddress<Foundation::Uint64>(ImageBase_) + Rva);
		return true;
	}

	auto SymbolResolver::FindVa(const Foundation::Char* Name, Foundation::Void*& Address) const -> Foundation::Bool
	{
		return FindVa(Library::StringView(Name), Address);
	}

	auto SymbolResolver::FindRvaByHash(Foundation::Uint64 NameHash, Foundation::Uint64& Rva) const -> Foundation::Bool
	{
		Rva = 0;

		if (!Ready_ || !NameHash)
		{
			return false;
		}

		return Find(NameHash, Rva);
	}

	auto SymbolResolver::FindVaByHash(Foundation::Uint64 NameHash, Foundation::Void*& Address) const -> Foundation::Bool
	{
		Address = nullptr;

		Foundation::Uint64 Rva{};

		if (!FindRvaByHash(NameHash, Rva) || !ImageBase_)
		{
			return false;
		}

		Address = Foundation::Cast::Auto<Foundation::Void*>(Foundation::Cast::PointerToAddress<Foundation::Uint64>(ImageBase_) + Rva);
		return true;
	}

	auto SymbolResolver::Capacity() const -> Foundation::Uint64
	{
		return Capacity_;
	}

	auto SymbolResolver::Used() const -> Foundation::Uint64
	{
		return Used_;
	}

	auto SymbolResolver::ImageBase() const -> const Foundation::Void*
	{
		return ImageBase_;
	}

	auto SymbolResolver::ImageSize() const -> Foundation::Uint64
	{
		return ImageSize_;
	}

	auto SymbolResolver::HashName(const Foundation::Char* Name) -> Foundation::Uint64
	{
		return HashName(Library::StringView(Name));
	}

	auto SymbolResolver::HashName(Library::StringView Name) -> Foundation::Uint64
	{
		return HashBytes(Name.Data(), Foundation::Cast::Auto<Foundation::Uint64>(Name.Size()));
	}

	auto SymbolResolver::HashName(const Library::String& Name) -> Foundation::Uint64
	{
		return HashName(Name.View());
	}

	auto SymbolResolver::Reserve(Foundation::Uint64 NewCapacity) -> Foundation::Bool
	{
		NewCapacity = NextPowerOfTwo(NewCapacity);

		if (NewCapacity <= Capacity_)
		{
			return true;
		}

		auto* OldEntries = Entries_;
		const auto OldCapacity = Capacity_;

		Entries_ = new Entry[NewCapacity]{};

		if (!Entries_)
		{
			Entries_ = OldEntries;
			return false;
		}

		Capacity_ = NewCapacity;
		Used_ = 0;

		if (OldEntries)
		{
			for (Foundation::Uint64 i = 0; i < OldCapacity; ++i)
			{
				if (OldEntries[i].NameHash)
				{
					Insert(OldEntries[i].NameHash, OldEntries[i].Rva);
				}
			}
		}

		delete[] OldEntries;
		return true;
	}

	auto SymbolResolver::FreeMap() -> Foundation::Void
	{
		delete[] Entries_;
		Entries_ = nullptr;
		Capacity_ = 0;
		Used_ = 0;
	}

	auto SymbolResolver::Insert(Foundation::Uint64 NameHash, Foundation::Uint64 Rva) -> Foundation::Bool
	{
		if (!NameHash)
		{
			return false;
		}

		if (!Capacity_ || ((Used_ + 1) * 10 >= Capacity_ * 7))
		{
			if (!Reserve(Capacity_ ? Capacity_ * 2 : DefaultCapacity))
			{
				return false;
			}
		}

		const auto Mask = Capacity_ - 1;
		auto Index = NameHash & Mask;

		for (Foundation::Uint64 i = 0; i < Capacity_; ++i)
		{
			auto& Entry = Entries_[Index];

			if (!Entry.NameHash)
			{
				Entry.NameHash = NameHash;
				Entry.Rva = Rva;
				++Used_;
				return true;
			}

			if (Entry.NameHash == NameHash)
			{
				Entry.Rva = Rva;
				return true;
			}

			Index = (Index + 1) & Mask;
		}

		return false;
	}

	auto SymbolResolver::Find(Foundation::Uint64 NameHash, Foundation::Uint64& Rva) const -> Foundation::Bool
	{
		Rva = 0;

		if (!Entries_ || !Capacity_ || !NameHash)
		{
			return false;
		}

		const auto Mask = Capacity_ - 1;
		auto Index = NameHash & Mask;

		for (Foundation::Uint64 i = 0; i < Capacity_; ++i)
		{
			const auto& Entry = Entries_[Index];

			if (!Entry.NameHash)
			{
				return false;
			}

			if (Entry.NameHash == NameHash)
			{
				Rva = Entry.Rva;
				return true;
			}

			Index = (Index + 1) & Mask;
		}

		return false;
	}

	auto SymbolResolver::LoadImageLayout(const Foundation::Void* ImageBase) -> Foundation::Bool
	{
		if (!ImageBase || !PeImage::IsImage64(ImageBase))
		{
			return false;
		}

		const auto* Base = Foundation::Cast::Auto<const Foundation::Uint8*>(ImageBase);
		const auto* Dos = PeImage::DosHeaderOf(ImageBase);
		const auto* Nt = PeImage::NtHeaders64Of(ImageBase);

		const auto SectionOffset = Foundation::Cast::Auto<Foundation::Uint64>(Dos->NtHeaderOffset) +
			sizeof(Foundation::Uint32) +
			sizeof(PeImage::FileHeader) +
			Nt->FileHeader.OptionalHeaderSize;

		ImageBase_ = Base;
		Sections_ = Foundation::Cast::Auto<const PeImage::SectionHeader*>(Base + SectionOffset);
		SectionCount_ = Nt->FileHeader.NumberOfSections;
		ImageSize_ = Nt->OptionalHeader.SizeOfImage;

		return Sections_ && SectionCount_ && ImageSize_;
	}

	auto SymbolResolver::SegmentOffsetToRva(Foundation::Uint16 Segment, Foundation::Uint64 Offset, Foundation::Uint64& Rva) const -> Foundation::Bool
	{
		Rva = 0;

		if (!Sections_ || !Segment || !SectionCount_ || !ImageSize_)
		{
			return false;
		}

		const auto Index = Foundation::Cast::Auto<Foundation::Uint64>(Segment - 1);

		if (Index >= SectionCount_)
		{
			return false;
		}

		const auto TempRva = Foundation::Cast::Auto<Foundation::Uint64>(Sections_[Index].VirtualAddress) + Offset;

		if (TempRva >= ImageSize_)
		{
			return false;
		}

		Rva = TempRva;
		return true;
	}

	auto SymbolResolver::ParsePdb(const Foundation::Uint8* PdbBase, Foundation::Uint64 PdbSize) -> Foundation::Bool
	{
		MsfContext Msf{};

		if (!InitMsf(Msf, PdbBase, PdbSize))
		{
			return false;
		}

		Foundation::Uint32 DbiStreamIndex{};
		DbiStreamHeader DbiHeader{};

		if (!FindDbiStream(Msf, DbiStreamIndex, DbiHeader))
		{
			FreeMsf(Msf);
			return false;
		}

		Library::Vector<Foundation::Uint8> SymbolStream{};

		if (!ReadMsfStream(Msf, DbiHeader.SymRecordStreamIndex, SymbolStream) || SymbolStream.Empty())
		{
			FreeMsf(Msf);
			return false;
		}

		const auto* Stream = SymbolStream.Data();
		const auto* End = Stream + SymbolStream.Size();

		for (auto* Cursor = Stream; Cursor + 4 <= End; )
		{
			const auto RecordLength = ReadU16(Cursor);
			Cursor += sizeof(Foundation::Uint16);

			if (RecordLength < sizeof(Foundation::Uint16) || Cursor + RecordLength > End)
			{
				break;
			}

			const auto RecordType = ReadU16(Cursor);
			Cursor += sizeof(Foundation::Uint16);

			const auto* Payload = Cursor;
			const auto PayloadSize = Foundation::Cast::Auto<Foundation::Uint64>(RecordLength - sizeof(Foundation::Uint16));

			Foundation::Uint16 Segment{};
			Foundation::Uint32 Offset{};
			const Foundation::Char* Name{};
			Foundation::Uint64 NameLength{};

			switch (RecordType)
			{
			case S_PUB32:
				if (PayloadSize >= 10)
				{
					Offset = ReadU32(Payload + 4);
					Segment = ReadU16(Payload + 8);
					Name = Foundation::Cast::Auto<const Foundation::Char*>(Payload + 10);

					if (!CStrLengthWithin(Name, PayloadSize - 10, NameLength) || !NameLength)
					{
						Name = nullptr;
					}
				}
				break;

			case S_GPROC32:
			case S_LPROC32:
			case S_GPROC32_ID:
			case S_LPROC32_ID:
				if (PayloadSize >= 35)
				{
					Offset = ReadU32(Payload + 28);
					Segment = ReadU16(Payload + 32);
					Name = Foundation::Cast::Auto<const Foundation::Char*>(Payload + 35);

					if (!CStrLengthWithin(Name, PayloadSize - 35, NameLength) || !NameLength)
					{
						Name = nullptr;
					}
				}
				break;

			default:
				break;
			}

			if (Name)
			{
				Foundation::Uint64 Rva{};

				if (SegmentOffsetToRva(Segment, Offset, Rva))
				{
					Insert(HashBytes(Name, NameLength), Rva);
				}
			}

			Cursor = Payload + PayloadSize;

			if (Foundation::Cast::PointerToAddress<Foundation::Uint64>(Cursor) & 1)
			{
				++Cursor;
			}
		}

		FreeMsf(Msf);
		return true;
	}
}