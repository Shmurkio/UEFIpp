#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Loader::Pe
{
	inline constexpr auto DosSignature = Foundation::Uint16{ 0x5A4D };
	inline constexpr auto NtSignature = Foundation::Uint32{ 0x00004550 };

	inline constexpr auto OptionalHeader32Magic = Foundation::Uint16{ 0x10B };
	inline constexpr auto OptionalHeader64Magic = Foundation::Uint16{ 0x20B };

	inline constexpr auto MachineI386 = Foundation::Uint16{ 0x014C };
	inline constexpr auto MachineX64 = Foundation::Uint16{ 0x8664 };
	inline constexpr auto MachineArm64 = Foundation::Uint16{ 0xAA64 };

	inline constexpr auto DirectoryCount = Foundation::Size{ 16 };
	inline constexpr auto ShortNameSize = Foundation::Size{ 8 };

	enum class DirectoryIndex : Foundation::Uint32
	{
		Export = 0,
		Import = 1,
		Resource = 2,
		Exception = 3,
		Security = 4,
		BaseRelocation = 5,
		Debug = 6,
		Architecture = 7,
		GlobalPointer = 8,
		Tls = 9,
		LoadConfig = 10,
		BoundImport = 11,
		Iat = 12,
		DelayImport = 13,
		ClrRuntime = 14,
		Reserved = 15
	};

	struct DosHeader
	{
		Foundation::Uint16 Magic;
		Foundation::Uint16 LastPageSize;
		Foundation::Uint16 PageCount;
		Foundation::Uint16 RelocationCount;
		Foundation::Uint16 HeaderSize;
		Foundation::Uint16 MinimumAllocation;
		Foundation::Uint16 MaximumAllocation;
		Foundation::Uint16 InitialStackSegment;
		Foundation::Uint16 InitialStackPointer;
		Foundation::Uint16 Checksum;
		Foundation::Uint16 InitialInstructionPointer;
		Foundation::Uint16 InitialCodeSegment;
		Foundation::Uint16 RelocationTableOffset;
		Foundation::Uint16 OverlayNumber;
		Foundation::Uint16 Reserved0[4];
		Foundation::Uint16 OemId;
		Foundation::Uint16 OemInfo;
		Foundation::Uint16 Reserved1[10];
		Foundation::Uint32 NtHeaderOffset;
	};

	struct FileHeader
	{
		Foundation::Uint16 Machine;
		Foundation::Uint16 NumberOfSections;
		Foundation::Uint32 TimeDateStamp;
		Foundation::Uint32 SymbolTablePointer;
		Foundation::Uint32 NumberOfSymbols;
		Foundation::Uint16 OptionalHeaderSize;
		Foundation::Uint16 Characteristics;
	};

	struct DataDirectory
	{
		Foundation::Uint32 VirtualAddress;
		Foundation::Uint32 Size;
	};

	struct OptionalHeader32
	{
		Foundation::Uint16 Magic;
		Foundation::Uint8 MajorLinkerVersion;
		Foundation::Uint8 MinorLinkerVersion;
		Foundation::Uint32 SizeOfCode;
		Foundation::Uint32 SizeOfInitializedData;
		Foundation::Uint32 SizeOfUninitializedData;
		Foundation::Uint32 EntryPointRva;
		Foundation::Uint32 BaseOfCode;
		Foundation::Uint32 BaseOfData;
		Foundation::Uint32 ImageBase;
		Foundation::Uint32 SectionAlignment;
		Foundation::Uint32 FileAlignment;
		Foundation::Uint16 MajorOperatingSystemVersion;
		Foundation::Uint16 MinorOperatingSystemVersion;
		Foundation::Uint16 MajorImageVersion;
		Foundation::Uint16 MinorImageVersion;
		Foundation::Uint16 MajorSubsystemVersion;
		Foundation::Uint16 MinorSubsystemVersion;
		Foundation::Uint32 Win32VersionValue;
		Foundation::Uint32 SizeOfImage;
		Foundation::Uint32 SizeOfHeaders;
		Foundation::Uint32 Checksum;
		Foundation::Uint16 Subsystem;
		Foundation::Uint16 DllCharacteristics;
		Foundation::Uint32 SizeOfStackReserve;
		Foundation::Uint32 SizeOfStackCommit;
		Foundation::Uint32 SizeOfHeapReserve;
		Foundation::Uint32 SizeOfHeapCommit;
		Foundation::Uint32 LoaderFlags;
		Foundation::Uint32 NumberOfRvaAndSizes;
		DataDirectory Directories[DirectoryCount];
	};

	struct OptionalHeader64
	{
		Foundation::Uint16 Magic;
		Foundation::Uint8 MajorLinkerVersion;
		Foundation::Uint8 MinorLinkerVersion;
		Foundation::Uint32 SizeOfCode;
		Foundation::Uint32 SizeOfInitializedData;
		Foundation::Uint32 SizeOfUninitializedData;
		Foundation::Uint32 EntryPointRva;
		Foundation::Uint32 BaseOfCode;
		Foundation::Uint64 ImageBase;
		Foundation::Uint32 SectionAlignment;
		Foundation::Uint32 FileAlignment;
		Foundation::Uint16 MajorOperatingSystemVersion;
		Foundation::Uint16 MinorOperatingSystemVersion;
		Foundation::Uint16 MajorImageVersion;
		Foundation::Uint16 MinorImageVersion;
		Foundation::Uint16 MajorSubsystemVersion;
		Foundation::Uint16 MinorSubsystemVersion;
		Foundation::Uint32 Win32VersionValue;
		Foundation::Uint32 SizeOfImage;
		Foundation::Uint32 SizeOfHeaders;
		Foundation::Uint32 Checksum;
		Foundation::Uint16 Subsystem;
		Foundation::Uint16 DllCharacteristics;
		Foundation::Uint64 SizeOfStackReserve;
		Foundation::Uint64 SizeOfStackCommit;
		Foundation::Uint64 SizeOfHeapReserve;
		Foundation::Uint64 SizeOfHeapCommit;
		Foundation::Uint32 LoaderFlags;
		Foundation::Uint32 NumberOfRvaAndSizes;
		DataDirectory Directories[DirectoryCount];
	};

	struct NtHeaders32
	{
		Foundation::Uint32 Signature;
		FileHeader FileHeader;
		OptionalHeader32 OptionalHeader;
	};

	struct NtHeaders64
	{
		Foundation::Uint32 Signature;
		FileHeader FileHeader;
		OptionalHeader64 OptionalHeader;
	};

	struct SectionHeader
	{
		Foundation::Uint8 Name[ShortNameSize];

		union
		{
			Foundation::Uint32 PhysicalAddress;
			Foundation::Uint32 VirtualSize;
		} Misc;

		Foundation::Uint32 VirtualAddress;
		Foundation::Uint32 SizeOfRawData;
		Foundation::Uint32 PointerToRawData;
		Foundation::Uint32 PointerToRelocations;
		Foundation::Uint32 PointerToLineNumbers;
		Foundation::Uint16 NumberOfRelocations;
		Foundation::Uint16 NumberOfLineNumbers;
		Foundation::Uint32 Characteristics;
	};

	struct ExportDirectory
	{
		Foundation::Uint32 Characteristics;
		Foundation::Uint32 TimeDateStamp;
		Foundation::Uint16 MajorVersion;
		Foundation::Uint16 MinorVersion;
		Foundation::Uint32 NameRva;
		Foundation::Uint32 Base;
		Foundation::Uint32 NumberOfFunctions;
		Foundation::Uint32 NumberOfNames;
		Foundation::Uint32 AddressOfFunctionsRva;
		Foundation::Uint32 AddressOfNamesRva;
		Foundation::Uint32 AddressOfNameOrdinalsRva;
	};

	struct ImportDescriptor
	{
		Foundation::Uint32 OriginalFirstThunk;
		Foundation::Uint32 TimeDateStamp;
		Foundation::Uint32 ForwarderChain;
		Foundation::Uint32 NameRva;
		Foundation::Uint32 FirstThunk;
	};

	struct ImportByName
	{
		Foundation::Uint16 Hint;
		Foundation::Char Name[1];
	};

	struct ThunkData32
	{
		union
		{
			Foundation::Uint32 ForwarderString;
			Foundation::Uint32 Function;
			Foundation::Uint32 Ordinal;
			Foundation::Uint32 AddressOfData;
		} u1;
	};

	struct ThunkData64
	{
		union
		{
			Foundation::Uint64 ForwarderString;
			Foundation::Uint64 Function;
			Foundation::Uint64 Ordinal;
			Foundation::Uint64 AddressOfData;
		} u1;
	};

	inline constexpr auto OrdinalFlag32 = Foundation::Uint32{ 0x80000000u };
	inline constexpr auto OrdinalFlag64 = Foundation::Uint64{ 0x8000000000000000ull };

	[[nodiscard]] inline auto DosHeaderOf(const Foundation::Void* ImageBase) -> const DosHeader*
	{
		return static_cast<const DosHeader*>(ImageBase);
	}

	[[nodiscard]] inline auto NtHeaders32Of(const Foundation::Void* ImageBase) -> const NtHeaders32*
	{
		const auto* Base = static_cast<const Foundation::Uint8*>(ImageBase);
		const auto* Dos = DosHeaderOf(ImageBase);
		return reinterpret_cast<const NtHeaders32*>(Base + Dos->NtHeaderOffset);
	}

	[[nodiscard]] inline auto NtHeaders64Of(const Foundation::Void* ImageBase) -> const NtHeaders64*
	{
		const auto* Base = static_cast<const Foundation::Uint8*>(ImageBase);
		const auto* Dos = DosHeaderOf(ImageBase);
		return reinterpret_cast<const NtHeaders64*>(Base + Dos->NtHeaderOffset);
	}

	[[nodiscard]] inline auto IsDosHeaderValid(const DosHeader& Header) -> Foundation::Bool
	{
		return Header.Magic == DosSignature;
	}

	[[nodiscard]] inline auto IsNtSignatureValid(Foundation::Uint32 Signature) -> Foundation::Bool
	{
		return Signature == NtSignature;
	}
}