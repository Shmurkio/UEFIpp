#include <UEFIpp/Executable/Pe/Image.hpp>

namespace UEFIpp::Executable::Pe {
Image::Image(const Foundation::Void *Base, Memory::AllocatorStub Allocator)
    : Base_(Foundation::Cast::Auto<const Foundation::Uint8 *>(Base)),
      Allocator_(Allocator) {}

auto Image::IsValid() const -> Foundation::Bool {
  if (!Base_) {
    return false;
  }

  const auto *Dos = DosHeader();
  if (!Dos || !IsDosHeaderValid(*Dos)) {
    return false;
  }

  const auto *Nt = NtSignaturePointer();
  if (!Nt || !IsNtSignatureValid(*Nt)) {
    return false;
  }

  return Is32() || Is64();
}

auto Image::Is32() const -> Foundation::Bool {
  const auto *Optional = OptionalHeaderMagicPointer();
  return Optional && *Optional == OptionalHeader32Magic;
}

auto Image::Is64() const -> Foundation::Bool {
  const auto *Optional = OptionalHeaderMagicPointer();
  return Optional && *Optional == OptionalHeader64Magic;
}

auto Image::Base() const -> const Foundation::Void * { return Base_; }

auto Image::BaseAddress() const -> Foundation::Uint64 {
  return Foundation::Cast::Auto<Foundation::Uint64>(Base_);
}

auto Image::DosHeader() const -> const Pe::DosHeader * {
  return Foundation::Cast::Auto<const Pe::DosHeader *>(Base_);
}

auto Image::NtHeaders32() const -> const Pe::NtHeaders32 * {
  return Foundation::Cast::Auto<const Pe::NtHeaders32 *>(
      Base_ + DosHeader()->NtHeaderOffset);
}

auto Image::NtHeaders64() const -> const Pe::NtHeaders64 * {
  return Foundation::Cast::Auto<const Pe::NtHeaders64 *>(
      Base_ + DosHeader()->NtHeaderOffset);
}

auto Image::FileHeader() const -> const Pe::FileHeader * {
  if (!IsValid()) {
    return nullptr;
  }

  return Is64() ? &NtHeaders64()->FileHeader : &NtHeaders32()->FileHeader;
}

auto Image::ImageSize() const -> Foundation::Uint32 {
  if (!IsValid()) {
    return 0;
  }

  return Is64() ? NtHeaders64()->OptionalHeader.SizeOfImage
                : NtHeaders32()->OptionalHeader.SizeOfImage;
}

auto Image::EntryPointRva() const -> Foundation::Uint32 {
  if (!IsValid()) {
    return 0;
  }

  return Is64() ? NtHeaders64()->OptionalHeader.EntryPointRva
                : NtHeaders32()->OptionalHeader.EntryPointRva;
}

auto Image::GetDirectory(DirectoryIndex Index) const -> Directory {
  const auto Id = Foundation::Cast::Underlying(Index);
  if (!IsValid() || Id >= DirectoryCount) {
    return {};
  }

  if (Is64()) {
    if (Id >= NtHeaders64()->OptionalHeader.NumberOfRvaAndSizes) {
      return {};
    }

    const auto &Entry = NtHeaders64()->OptionalHeader.Directories[Id];
    return {Index, Entry.VirtualAddress, Entry.Size};
  }

  if (Id >= NtHeaders32()->OptionalHeader.NumberOfRvaAndSizes) {
    return {};
  }

  const auto &Entry = NtHeaders32()->OptionalHeader.Directories[Id];
  return {Index, Entry.VirtualAddress, Entry.Size};
}

auto Image::SectionHeaders() const -> const Pe::SectionHeader * {
  if (!IsValid()) {
    return nullptr;
  }

  const auto *Dos = DosHeader();
  const auto *File = FileHeader();
  const auto *NtBase = Base_ + Dos->NtHeaderOffset;
  return Foundation::Cast::Auto<const Pe::SectionHeader *>(
      NtBase + sizeof(Foundation::Uint32) + sizeof(Pe::FileHeader) +
      File->OptionalHeaderSize);
}

auto Image::SectionCount() const -> Foundation::Uint16 {
  const auto *Header = FileHeader();
  return Header ? Header->NumberOfSections : 0;
}

auto Image::Exports() const -> ExportVector {
  ExportVector Result{Allocator_};
  const auto Directory = GetDirectory(DirectoryIndex::Export);

  if (Directory.Empty()) {
    return Result;
  }

  const auto *ExportHeader =
      RvaToPointer<ExportDirectory>(Directory.Rva());
  if (!ExportHeader) {
    return Result;
  }

  const auto *Functions = RvaToPointer<Foundation::Uint32>(
      ExportHeader->AddressOfFunctionsRva);
  const auto *Names =
      RvaToPointer<Foundation::Uint32>(ExportHeader->AddressOfNamesRva);
  const auto *Ordinals = RvaToPointer<Foundation::Uint16>(
      ExportHeader->AddressOfNameOrdinalsRva);

  if (!Functions) {
    return Result;
  }

  for (Foundation::Uint32 i = 0; i < ExportHeader->NumberOfFunctions; ++i) {
    const auto FunctionRva = Functions[i];
    if (!FunctionRva) {
      continue;
    }

    Export Entry{Allocator_};
    Entry.Ordinal =
        Foundation::Cast::Auto<Foundation::Uint16>(ExportHeader->Base + i);
    Entry.Rva = FunctionRva;
    Entry.Address = BaseAddress() + FunctionRva;

    if (FunctionRva >= Directory.Rva() &&
        FunctionRva < Directory.Rva() + Directory.Size()) {
      Entry.Forwarded = true;
      Entry.Forwarder =
          ReadAsciiString(RvaToPointer<Foundation::Char>(FunctionRva));
    }

    if (Names && Ordinals) {
      for (Foundation::Uint32 n = 0; n < ExportHeader->NumberOfNames; ++n) {
        if (Ordinals[n] == i) {
          Entry.Name =
              ReadAsciiString(RvaToPointer<Foundation::Char>(Names[n]));
          break;
        }
      }
    }

    if (!Result.PushBack(Entry)) {
      Result.Clear();
      return Result;
    }
  }

  return Result;
}

auto Image::RvaToPointer(Foundation::Uint32 Rva) const
    -> const Foundation::Void * {
  if (!Base_ || Rva == 0 || Rva >= ImageSize()) {
    return nullptr;
  }

  return Base_ + Rva;
}

auto Image::NtSignaturePointer() const -> const Foundation::Uint32 * {
  if (!Base_) {
    return nullptr;
  }

  const auto *Dos = DosHeader();
  if (!Dos || !IsDosHeaderValid(*Dos)) {
    return nullptr;
  }

  return Foundation::Cast::Auto<const Foundation::Uint32 *>(
      Base_ + Dos->NtHeaderOffset);
}

auto Image::OptionalHeaderMagicPointer() const -> const Foundation::Uint16 * {
  const auto *Signature = NtSignaturePointer();
  if (!Signature || !IsNtSignatureValid(*Signature)) {
    return nullptr;
  }

  const auto *Bytes =
      Foundation::Cast::Auto<const Foundation::Uint8 *>(Signature);
  return Foundation::Cast::Auto<const Foundation::Uint16 *>(
      Bytes + sizeof(Foundation::Uint32) + sizeof(Pe::FileHeader));
}

auto Image::ReadAsciiString(const Foundation::Char *Text) const
    -> Library::String {
  if (!Text) {
    return Library::String{Allocator_};
  }

  return Library::String{Text, Allocator_};
}
} // namespace UEFIpp::Executable::Pe
