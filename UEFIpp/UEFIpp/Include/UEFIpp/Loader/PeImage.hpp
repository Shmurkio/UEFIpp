#pragma once

#include <UEFIpp/Loader/PeTypes.hpp>
#include <UEFIpp/Loader/PeDirectory.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/Containers/Optional.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Library/String/StringView.hpp>
#include <UEFIpp/Memory/Memory.hpp>

namespace UEFIpp::Loader
{
	struct PeSection
	{
		Library::String Name{};
		Foundation::Uint32 Rva{};
		Foundation::Uint64 Address{};
		Foundation::Uint32 VirtualSize{};
		Foundation::Uint32 RawSize{};
		Foundation::Uint32 RawOffset{};
		Foundation::Uint32 Characteristics{};
		const Pe::SectionHeader* Header{};
	};

	struct PeExport
	{
		Library::String Name{};
		Foundation::Uint16 Ordinal{};
		Foundation::Uint32 Rva{};
		Foundation::Uint64 Address{};
		Foundation::Bool Forwarded{};
		Library::String Forwarder{};
	};

	struct PeImport
	{
		Library::String Module{};
		Library::String Name{};
		Foundation::Uint16 Hint{};
		Foundation::Uint16 Ordinal{};
		Foundation::Bool ImportByOrdinal{};
		Foundation::Uint64 IatAddress{};
		Foundation::Uint32 ThunkRva{};
	};

	class PeImage
	{
	public:
		using SectionVector = Library::Vector<PeSection>;
		using ExportVector = Library::Vector<PeExport>;
		using ImportVector = Library::Vector<PeImport>;
		using DirectoryVector = Library::Vector<PeDirectory>;

	public:
		constexpr PeImage() = default;

		explicit PeImage(const Foundation::Void* Base) : Base_(Foundation::Cast::Auto<const Foundation::Uint8*>(Base))
		{
		}

		[[nodiscard]] auto IsValid() const -> Foundation::Bool
		{
			if (!Base_)
			{
				return false;
			}

			const auto* Dos = DosHeader();
			if (!Dos || !Pe::IsDosHeaderValid(*Dos))
			{
				return false;
			}

			const auto* Nt = NtSignaturePointer();
			if (!Nt || !Pe::IsNtSignatureValid(*Nt))
			{
				return false;
			}

			return Is32() || Is64();
		}

		[[nodiscard]] auto Is32() const -> Foundation::Bool
		{
			const auto* Optional = OptionalHeaderMagicPointer();
			return Optional && *Optional == Pe::OptionalHeader32Magic;
		}

		[[nodiscard]] auto Is64() const -> Foundation::Bool
		{
			const auto* Optional = OptionalHeaderMagicPointer();
			return Optional && *Optional == Pe::OptionalHeader64Magic;
		}

		[[nodiscard]] auto Base() const -> const Foundation::Void*
		{
			return Base_;
		}

		[[nodiscard]] auto BaseAddress() const -> Foundation::Uint64
		{
			return Foundation::Cast::Auto<Foundation::Uint64>(Base_);
		}

		[[nodiscard]] auto DosHeader() const -> const Pe::DosHeader*
		{
			return Foundation::Cast::Auto<const Pe::DosHeader*>(Base_);
		}

		[[nodiscard]] auto NtHeaders32() const -> const Pe::NtHeaders32*
		{
			return Foundation::Cast::Auto<const Pe::NtHeaders32*>(Base_ + DosHeader()->NtHeaderOffset);
		}

		[[nodiscard]] auto NtHeaders64() const -> const Pe::NtHeaders64*
		{
			return Foundation::Cast::Auto<const Pe::NtHeaders64*>(Base_ + DosHeader()->NtHeaderOffset);
		}

		[[nodiscard]] auto FileHeader() const -> const Pe::FileHeader*
		{
			if (!IsValid())
			{
				return nullptr;
			}

			return Is64() ? &NtHeaders64()->FileHeader : &NtHeaders32()->FileHeader;
		}

		[[nodiscard]] auto ImageSize() const -> Foundation::Uint32
		{
			if (!IsValid())
			{
				return 0;
			}

			return Is64() ? NtHeaders64()->OptionalHeader.SizeOfImage : NtHeaders32()->OptionalHeader.SizeOfImage;
		}

		[[nodiscard]] auto EntryPointRva() const -> Foundation::Uint32
		{
			if (!IsValid())
			{
				return 0;
			}

			return Is64() ? NtHeaders64()->OptionalHeader.EntryPointRva : NtHeaders32()->OptionalHeader.EntryPointRva;
		}

		[[nodiscard]] auto EntryPoint() const -> const Foundation::Void*
		{
			return RvaToPointer(EntryPointRva());
		}

		[[nodiscard]] auto EntryPointAddress() const -> Foundation::Uint64
		{
			return RvaToAddress(EntryPointRva());
		}

		[[nodiscard]] auto Directory(Pe::DirectoryIndex Index) const -> PeDirectory
		{
			const auto Id = Foundation::Cast::Underlying(Index);
			if (!IsValid() || Id >= Pe::DirectoryCount)
			{
				return {};
			}

			if (Is64())
			{
				if (Id >= NtHeaders64()->OptionalHeader.NumberOfRvaAndSizes)
				{
					return {};
				}

				const auto& Directory = NtHeaders64()->OptionalHeader.Directories[Id];
				return { Index, Directory.VirtualAddress, Directory.Size };
			}

			if (Id >= NtHeaders32()->OptionalHeader.NumberOfRvaAndSizes)
			{
				return {};
			}

			const auto& Directory = NtHeaders32()->OptionalHeader.Directories[Id];
			return { Index, Directory.VirtualAddress, Directory.Size };
		}

		[[nodiscard]] auto Directories() const -> DirectoryVector
		{
			DirectoryVector Result{};

			for (Foundation::Uint32 i = 0; i < Pe::DirectoryCount; ++i)
			{
				const auto Directory = this->Directory(Foundation::Cast::Auto<Pe::DirectoryIndex>(i));
				Result.PushBack(Directory);
			}

			return Result;
		}

		[[nodiscard]] auto SectionHeaders() const -> const Pe::SectionHeader*
		{
			if (!IsValid())
			{
				return nullptr;
			}

			const auto* Dos = DosHeader();
			const auto* File = FileHeader();
			const auto* NtBase = Base_ + Dos->NtHeaderOffset;
			return Foundation::Cast::Auto<const Pe::SectionHeader*>(NtBase + sizeof(Foundation::Uint32) + sizeof(Pe::FileHeader) + File->OptionalHeaderSize);
		}

		[[nodiscard]] auto SectionCount() const -> Foundation::Uint16
		{
			const auto* Header = FileHeader();
			return Header ? Header->NumberOfSections : 0;
		}

		[[nodiscard]] auto Sections() const -> SectionVector
		{
			SectionVector Result{};
			const auto* Headers = SectionHeaders();
			const auto Count = SectionCount();

			if (!Headers)
			{
				return Result;
			}

			for (Foundation::Uint16 i = 0; i < Count; ++i)
			{
				const auto& Header = Headers[i];

				PeSection Section{};
				Section.Name = ReadShortName(Header.Name);
				Section.Rva = Header.VirtualAddress;
				Section.Address = RvaToAddress(Header.VirtualAddress);
				Section.VirtualSize = Header.Misc.VirtualSize;
				Section.RawSize = Header.SizeOfRawData;
				Section.RawOffset = Header.PointerToRawData;
				Section.Characteristics = Header.Characteristics;
				Section.Header = &Header;

				Result.PushBack(Section);
			}

			return Result;
		}

		[[nodiscard]] auto Exports() const -> ExportVector
		{
			ExportVector Result{};
			const auto Directory = this->Directory(Pe::DirectoryIndex::Export);

			if (Directory.Empty())
			{
				return Result;
			}

			const auto* ExportDirectory = RvaToPointer<Pe::ExportDirectory>(Directory.Rva());
			if (!ExportDirectory)
			{
				return Result;
			}

			const auto* Functions = RvaToPointer<Foundation::Uint32>(ExportDirectory->AddressOfFunctionsRva);
			const auto* Names = RvaToPointer<Foundation::Uint32>(ExportDirectory->AddressOfNamesRva);
			const auto* Ordinals = RvaToPointer<Foundation::Uint16>(ExportDirectory->AddressOfNameOrdinalsRva);

			if (!Functions)
			{
				return Result;
			}

			for (Foundation::Uint32 i = 0; i < ExportDirectory->NumberOfFunctions; ++i)
			{
				const auto FunctionRva = Functions[i];
				if (!FunctionRva)
				{
					continue;
				}

				PeExport Export{};
				Export.Ordinal = Foundation::Cast::Auto<Foundation::Uint16>(ExportDirectory->Base + i);
				Export.Rva = FunctionRva;
				Export.Address = RvaToAddress(FunctionRva);

				if (FunctionRva >= Directory.Rva() && FunctionRva < Directory.Rva() + Directory.Size())
				{
					Export.Forwarded = true;
					Export.Forwarder = ReadAsciiString(RvaToPointer<Foundation::Char>(FunctionRva));
				}

				if (Names && Ordinals)
				{
					for (Foundation::Uint32 n = 0; n < ExportDirectory->NumberOfNames; ++n)
					{
						if (Ordinals[n] == i)
						{
							Export.Name = ReadAsciiString(RvaToPointer<Foundation::Char>(Names[n]));
							break;
						}
					}
				}

				Result.PushBack(Export);
			}

			return Result;
		}

		[[nodiscard]] auto Imports() const -> ImportVector
		{
			ImportVector Result{};
			const auto Directory = this->Directory(Pe::DirectoryIndex::Import);

			if (Directory.Empty())
			{
				return Result;
			}

			const auto* Descriptor = RvaToPointer<Pe::ImportDescriptor>(Directory.Rva());
			if (!Descriptor)
			{
				return Result;
			}

			for (; Descriptor->NameRva != 0; ++Descriptor)
			{
				const auto Module = ReadAsciiString(RvaToPointer<Foundation::Char>(Descriptor->NameRva));
				const auto ThunkRva = Descriptor->OriginalFirstThunk ? Descriptor->OriginalFirstThunk : Descriptor->FirstThunk;

				if (Is64())
				{
					ReadImports64(Result, Module, ThunkRva, Descriptor->FirstThunk);
				}
				else
				{
					ReadImports32(Result, Module, ThunkRva, Descriptor->FirstThunk);
				}
			}

			return Result;
		}

		[[nodiscard]] auto RvaToPointer(Foundation::Uint32 Rva) const -> const Foundation::Void*
		{
			if (!Base_ || Rva == 0)
			{
				return nullptr;
			}

			return Base_ + Rva;
		}

		template<typename T>
		[[nodiscard]] auto RvaToPointer(Foundation::Uint32 Rva) const -> const T*
		{
			return Foundation::Cast::Auto<const T*>(RvaToPointer(Rva));
		}

		[[nodiscard]] auto RvaToAddress(Foundation::Uint32 Rva) const -> Foundation::Uint64
		{
			if (!Base_ || Rva == 0)
			{
				return 0;
			}

			return BaseAddress() + Rva;
		}

		[[nodiscard]] auto Section(Library::StringView Name) const -> Library::Optional<PeSection>
		{
			for (const auto& Section : Sections())
			{
				if (Section.Name.View() == Name)
				{
					return Section;
				}
			}

			return Library::NullOpt;
		}

		[[nodiscard]] auto SectionByRva(Foundation::Uint32 Rva) const -> Library::Optional<PeSection>
		{
			for (const auto& Section : Sections())
			{
				const auto Start = Section.Rva;
				const auto End = Start + Section.VirtualSize;

				if (Rva >= Start && Rva < End)
				{
					return Section;
				}
			}

			return Library::NullOpt;
		}

		[[nodiscard]] auto SectionByAddress(Foundation::Uint64 Address) const -> Library::Optional<PeSection>
		{
			for (const auto& Section : Sections())
			{
				const auto Start = Section.Address;
				const auto End = Start + Section.VirtualSize;

				if (Address >= Start && Address < End)
				{
					return Section;
				}
			}

			return Library::NullOpt;
		}

		[[nodiscard]] auto Export(Library::StringView Name) const -> Library::Optional<PeExport>
		{
			for (const auto& Export : Exports())
			{
				if (Export.Name.View() == Name)
				{
					return Export;
				}
			}

			return Library::NullOpt;
		}

		[[nodiscard]] auto Import(Library::StringView Name) const -> Library::Optional<PeImport>
		{
			for (const auto& Import : Imports())
			{
				if (Import.Name.View() == Name)
				{
					return Import;
				}
			}

			return Library::NullOpt;
		}

		[[nodiscard]] auto CalculateChecksum() const -> Foundation::Uint32
		{
			if (!IsValid())
			{
				return 0;
			}

			const auto* Data = Base_;
			const auto Size = ImageSize();

			const auto ChecksumOffset = Is64() ? Foundation::Cast::Auto<Foundation::UintPtr>(&NtHeaders64()->OptionalHeader.Checksum) - BaseAddress() : Foundation::Cast::Auto<Foundation::UintPtr>(&NtHeaders32()->OptionalHeader.Checksum) - BaseAddress();
			Foundation::Uint64 Sum{};

			for (Foundation::Uint32 Offset = 0; Offset < Size; Offset += 2)
			{
				if (Offset == ChecksumOffset || Offset == ChecksumOffset + 2)
				{
					continue;
				}

				Foundation::Uint16 Word{};

				if (Offset + 1 < Size)
				{
					Word = Foundation::Cast::Auto<Foundation::Uint16>(Foundation::Cast::Auto<Foundation::Uint16>(Data[Offset]) | Foundation::Cast::Auto<Foundation::Uint16>(Data[Offset + 1] << 8));
				}
				else
				{
					Word = Data[Offset];
				}

				Sum += Word;
				Sum = (Sum & 0xFFFF) + (Sum >> 16);
			}

			Sum = (Sum & 0xFFFF) + (Sum >> 16);
			Sum += Size;

			return Foundation::Cast::Auto<Foundation::Uint32>(Sum);
		}

		[[nodiscard]] auto ChecksumField() -> Foundation::Uint32*
		{
			if (!IsValid())
			{
				return nullptr;
			}

			if (Is64())
			{
				auto* Header = Foundation::Cast::Auto<Pe::NtHeaders64*>(NtHeaders64());
				return &Header->OptionalHeader.Checksum;
			}

			auto* Header = Foundation::Cast::Auto<Pe::NtHeaders32*>(NtHeaders32());

			return &Header->OptionalHeader.Checksum;
		}

		[[nodiscard]] auto ChecksumField() const -> const Foundation::Uint32*
		{
			if (!IsValid())
			{
				return nullptr;
			}

			return Is64() ? &NtHeaders64()->OptionalHeader.Checksum : &NtHeaders32()->OptionalHeader.Checksum;
		}

		[[nodiscard]] auto StoredChecksum() const -> Foundation::Uint32
		{
			const auto* Field = ChecksumField();
			return Field ? *Field : 0;
		}

		auto ClearChecksum() -> Foundation::Bool
		{
			return SetChecksum(0);
		}

		auto SetChecksum(Foundation::Uint32 Checksum) -> Foundation::Bool
		{
			auto* Field = ChecksumField();

			if (!Field)
			{
				return false;
			}

			*Field = Checksum;
			return true;
		}

		[[nodiscard]] auto HasValidChecksum() const -> Foundation::Bool
		{
			const auto Stored = StoredChecksum();
			return Stored != 0 && Stored == CalculateChecksum();
		}

		[[nodiscard]] auto FixChecksum() -> Foundation::Bool
		{
			return SetChecksum(CalculateChecksum());
		}

		[[nodiscard]] auto CanAddSection() const -> Foundation::Bool
		{
			if (!IsValid())
			{
				return false;
			}

			const auto* Headers = SectionHeaders();
			const auto Count = SectionCount();

			if (!Headers || Count == 0)
			{
				return false;
			}

			const auto SectionTableEnd = Foundation::Cast::Auto<Foundation::UintPtr>(&Headers[Count]);
			const auto FirstSectionRaw = BaseAddress() + Headers[0].PointerToRawData;

			return SectionTableEnd + sizeof(Pe::SectionHeader) <= FirstSectionRaw;
		}

		[[nodiscard]] auto SectionAlignment() const -> Foundation::Uint32
		{
			if (!IsValid())
			{
				return 0;
			}

			return Is64() ? NtHeaders64()->OptionalHeader.SectionAlignment : NtHeaders32()->OptionalHeader.SectionAlignment;
		}

		[[nodiscard]] auto FileAlignment() const -> Foundation::Uint32
		{
			if (!IsValid())
			{
				return 0;
			}

			return Is64() ? NtHeaders64()->OptionalHeader.FileAlignment : NtHeaders32()->OptionalHeader.FileAlignment;
		}

		[[nodiscard]] auto AddSection(Library::StringView Name, Foundation::Uint32 VirtualSize, Foundation::Uint32 RawSize, Foundation::Uint32 Characteristics, Foundation::Bool FixChecksumAfterAdding = false) -> Library::Optional<PeSection>
		{
			if (!IsValid() || Name.Empty() || VirtualSize == 0)
			{
				return Library::NullOpt;
			}

			auto* File = Foundation::Cast::Auto<Pe::FileHeader*>(FileHeader());
			auto* Headers = Foundation::Cast::Auto<Pe::SectionHeader*>(SectionHeaders());

			if (!File || !Headers || File->NumberOfSections == 0)
			{
				return Library::NullOpt;
			}

			const auto Count = File->NumberOfSections;
			auto& Last = Headers[Count - 1];

			const auto SectionTableEnd = Foundation::Cast::Auto<Foundation::UintPtr>(&Headers[Count]);

			const auto FirstRaw = BaseAddress() + Headers[0].PointerToRawData;

			if (SectionTableEnd + sizeof(Pe::SectionHeader) > FirstRaw)
			{
				return Library::NullOpt;
			}

			auto& New = Headers[Count];

			Memory::Zero(&New, sizeof(New));

			const auto NameLength = Foundation::Utility::Min(Name.Size(), Pe::ShortNameSize);

			for (Foundation::Size i = 0; i < NameLength; ++i)
			{
				New.Name[i] = Foundation::Cast::Auto<Foundation::Uint8>(Name[i]);
			}

			const auto SectionAlign = SectionAlignment();
			const auto FileAlign = FileAlignment();

			if (SectionAlign == 0 || FileAlign == 0)
			{
				return Library::NullOpt;
			}

			New.VirtualAddress = Foundation::Bit::AlignUp(Last.VirtualAddress + Foundation::Utility::Max(Last.Misc.VirtualSize, Last.SizeOfRawData), SectionAlign);

			New.Misc.VirtualSize = VirtualSize;
			New.SizeOfRawData = Foundation::Bit::AlignUp(RawSize, FileAlign);
			New.PointerToRawData = Foundation::Bit::AlignUp(Last.PointerToRawData + Last.SizeOfRawData, FileAlign);

			New.Characteristics = Characteristics;

			++File->NumberOfSections;

			if (Is64())
			{
				auto* Header = Foundation::Cast::Auto<Pe::NtHeaders64*>(NtHeaders64());
				Header->OptionalHeader.SizeOfImage = Foundation::Bit::AlignUp(New.VirtualAddress + VirtualSize, SectionAlign);
			}
			else
			{
				auto* Header = Foundation::Cast::Auto<Pe::NtHeaders32*>(NtHeaders32());
				Header->OptionalHeader.SizeOfImage = Foundation::Bit::AlignUp(New.VirtualAddress + VirtualSize, SectionAlign);
			}

			if (FixChecksumAfterAdding)
			{
				FixChecksum();
			}

			PeSection Section{};
			Section.Name = ReadShortName(New.Name);
			Section.Rva = New.VirtualAddress;
			Section.Address = RvaToAddress(New.VirtualAddress);
			Section.VirtualSize = New.Misc.VirtualSize;
			Section.RawSize = New.SizeOfRawData;
			Section.RawOffset = New.PointerToRawData;
			Section.Characteristics = New.Characteristics;
			Section.Header = &New;

			return Section;
		}

	private:
		const Foundation::Uint8* Base_{};

		[[nodiscard]] auto NtSignaturePointer() const -> const Foundation::Uint32*
		{
			if (!Base_)
			{
				return nullptr;
			}

			const auto* Dos = DosHeader();
			if (!Dos || !Pe::IsDosHeaderValid(*Dos))
			{
				return nullptr;
			}

			return Foundation::Cast::Auto<const Foundation::Uint32*>(Base_ + Dos->NtHeaderOffset);
		}

		[[nodiscard]] auto OptionalHeaderMagicPointer() const -> const Foundation::Uint16*
		{
			const auto* Signature = NtSignaturePointer();
			if (!Signature || !Pe::IsNtSignatureValid(*Signature))
			{
				return nullptr;
			}

			const auto* Bytes = Foundation::Cast::Auto<const Foundation::Uint8*>(Signature);
			return Foundation::Cast::Auto<const Foundation::Uint16*>(Bytes + sizeof(Foundation::Uint32) + sizeof(Pe::FileHeader));
		}

		[[nodiscard]] static auto ReadShortName(const Foundation::Uint8 Name[Pe::ShortNameSize]) -> Library::String
		{
			Foundation::Size Length{};
			while (Length < Pe::ShortNameSize && Name[Length])
			{
				++Length;
			}

			return Library::String{ Library::StringView{ Foundation::Cast::Auto<const Foundation::Char*>(Name), Length } };
		}

		[[nodiscard]] static auto ReadAsciiString(const Foundation::Char* Text) -> Library::String
		{
			if (!Text)
			{
				return {};
			}

			return Library::String{ Text };
		}

		auto ReadImports64(ImportVector& Result, const Library::String& Module, Foundation::Uint32 LookupThunkRva, Foundation::Uint32 IatRva) const -> Foundation::Void
		{
			const auto* LookupThunk = RvaToPointer<Pe::ThunkData64>(LookupThunkRva);
			if (!LookupThunk)
			{
				return;
			}

			for (Foundation::Uint32 Index = 0; LookupThunk[Index].u1.AddressOfData != 0; ++Index)
			{
				PeImport Import{};
				Import.Module = Module;
				Import.ThunkRva = LookupThunkRva + Index * sizeof(Pe::ThunkData64);
				Import.IatAddress = RvaToAddress(IatRva + Index * sizeof(Pe::ThunkData64));

				const auto Value = LookupThunk[Index].u1.AddressOfData;
				if ((Value & Pe::OrdinalFlag64) != 0)
				{
					Import.ImportByOrdinal = true;
					Import.Ordinal = Foundation::Cast::Auto<Foundation::Uint16>(Value & 0xFFFF);
				}
				else
				{
					const auto* ByName = RvaToPointer<Pe::ImportByName>(Foundation::Cast::Auto<Foundation::Uint32>(Value));
					if (ByName)
					{
						Import.Hint = ByName->Hint;
						Import.Name = ReadAsciiString(ByName->Name);
					}
				}

				Result.PushBack(Import);
			}
		}

		auto ReadImports32(ImportVector& Result, const Library::String& Module, Foundation::Uint32 LookupThunkRva, Foundation::Uint32 IatRva) const -> Foundation::Void
		{
			const auto* LookupThunk = RvaToPointer<Pe::ThunkData32>(LookupThunkRva);
			if (!LookupThunk)
			{
				return;
			}

			for (Foundation::Uint32 Index = 0; LookupThunk[Index].u1.AddressOfData != 0; ++Index)
			{
				PeImport Import{};
				Import.Module = Module;
				Import.ThunkRva = LookupThunkRva + Index * sizeof(Pe::ThunkData32);
				Import.IatAddress = RvaToAddress(IatRva + Index * sizeof(Pe::ThunkData32));

				const auto Value = LookupThunk[Index].u1.AddressOfData;
				if ((Value & Pe::OrdinalFlag32) != 0)
				{
					Import.ImportByOrdinal = true;
					Import.Ordinal = Foundation::Cast::Auto<Foundation::Uint16>(Value & 0xFFFF);
				}
				else
				{
					const auto* ByName = RvaToPointer<Pe::ImportByName>(Value);
					if (ByName)
					{
						Import.Hint = ByName->Hint;
						Import.Name = ReadAsciiString(ByName->Name);
					}
				}

				Result.PushBack(Import);
			}
		}
	};
}

namespace UEFIpp::PeImage
{
	using namespace UEFIpp::Loader::Pe;

	[[nodiscard]] inline auto DosHeaderOf(const Foundation::Void* ImageBase) -> const DosHeader*
	{
		return Loader::Pe::DosHeaderOf(ImageBase);
	}

	[[nodiscard]] inline auto NtHeaders64Of(const Foundation::Void* ImageBase) -> const NtHeaders64*
	{
		return Loader::Pe::NtHeaders64Of(ImageBase);
	}

	[[nodiscard]] inline auto IsDosHeaderValid(const DosHeader& Header) -> Foundation::Bool
	{
		return Loader::Pe::IsDosHeaderValid(Header);
	}

	[[nodiscard]] inline auto IsNtHeaderValid(const NtHeaders64& Header) -> Foundation::Bool
	{
		return Header.Signature == NtSignature && Header.OptionalHeader.Magic == OptionalHeader64Magic;
	}

	[[nodiscard]] inline auto IsImage64(const Foundation::Void* ImageBase) -> Foundation::Bool
	{
		Loader::PeImage Image{ ImageBase };
		return Image.IsValid() && Image.Is64();
	}

	[[nodiscard]] inline auto ImageSize64(const Foundation::Void* ImageBase) -> Foundation::Uint32
	{
		return Loader::Pe::NtHeaders64Of(ImageBase)->OptionalHeader.SizeOfImage;
	}
}