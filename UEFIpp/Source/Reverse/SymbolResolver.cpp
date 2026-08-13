#include <UEFIpp/Reverse/SymbolResolver.hpp>
#include <UEFIpp/Executable/Pe/Image.hpp>
#include <UEFIpp/IO/Text/Reader.hpp>
#include <UEFIpp/IO/Transport/File.hpp>

namespace UEFIpp::Reverse
{
	namespace
	{
		inline constexpr auto ImageScnMemExecute = Foundation::Uint32{ 0x20000000u };
		inline constexpr auto ImageScnMemRead = Foundation::Uint32{ 0x40000000u };
		inline constexpr auto ImageScnMemWrite = Foundation::Uint32{ 0x80000000u };
	}

	SymbolResolver::SymbolResolver(Memory::AllocatorStub Allocator) noexcept :
		Allocator_(Allocator), Database_(Allocator)
	{
	}

	auto SymbolResolver::Load(const FileSystem::Path& Path, const Foundation::Void* ImageBase, Foundation::Uint64) -> Foundation::Bool
	{
		IO::FileSource File{ Allocator_ };
		if (!File.Open(Path)) return false;
		auto Buffer = IO::ReadAll(File, Allocator_);
		if (!Buffer) return false;
		return Load(Buffer.Value().Data(), Buffer.Value().Size(), ImageBase);
	}

	auto SymbolResolver::Load(const Library::Vector<Foundation::Uint8>& Pdb, const Foundation::Void* ImageBase, Foundation::Uint64) -> Foundation::Bool
	{
		return Load(Pdb.Data(), Pdb.Size(), ImageBase);
	}

	auto SymbolResolver::Load(const Foundation::Uint8* PdbBase, Foundation::Uint64 PdbSize, const Foundation::Void* ImageBase, Foundation::Uint64) -> Foundation::Bool
	{
		return Foundation::Cast::Auto<Foundation::Bool>(LoadDetailed(PdbBase, PdbSize, ImageBase));
	}

	auto SymbolResolver::LoadDetailed(const Foundation::Uint8* PdbBase, Foundation::Uint64 PdbSize, const Foundation::Void* ImageBase, const Pdb::LoadOptions& Options)
		-> Library::Expected<Foundation::Void, Pdb::ErrorInfo>
	{
		Reset();
		Pdb::PdbFile File{ Allocator_ };
		auto Open = File.Open(PdbBase, PdbSize);
		if (!Open) { LastError_ = Open.Error(); return Library::MakeUnexpected(LastError_); }
		auto Populate = File.Populate(Database_, ImageBase, Options);
		if (!Populate) { LastError_ = Populate.Error(); Database_.Reset(); return Library::MakeUnexpected(LastError_); }
		LastError_ = {};
		return {};
	}

	auto SymbolResolver::Reset() -> Foundation::Void { Database_.Reset(); LastError_ = {}; }
	auto SymbolResolver::Ready() const noexcept -> Foundation::Bool { return Database_.Ready(); }
	auto SymbolResolver::Valid() const noexcept -> Foundation::Bool { return Ready(); }
	SymbolResolver::operator Foundation::Bool() const noexcept { return Ready(); }
	auto SymbolResolver::LastError() const noexcept -> const Pdb::ErrorInfo& { return LastError_; }
	auto SymbolResolver::Database() noexcept -> SymbolDatabase& { return Database_; }
	auto SymbolResolver::Database() const noexcept -> const SymbolDatabase& { return Database_; }

	auto SymbolResolver::FindRva(const Library::String& Name, Foundation::Uint64& Rva) const -> Foundation::Bool { return FindRva(Name.View(), Rva); }
	auto SymbolResolver::FindRva(const Foundation::Char* Name, Foundation::Uint64& Rva) const -> Foundation::Bool { return FindRva(Library::StringView{Name}, Rva); }
	auto SymbolResolver::FindRva(Library::StringView Name, Foundation::Uint64& Rva) const -> Foundation::Bool
	{
		Rva = 0; if (!Ready()) return false; const auto* Value = Database_.FindFirstExact(Name); if (!Value) return false; Rva = Value->Rva; return true;
	}

	auto SymbolResolver::RvaToPointer(Foundation::Uint64 Rva) const noexcept -> Foundation::Void*
	{
		if (!Database_.ImageBase() || Rva >= Database_.ImageSize()) return nullptr;
		return Foundation::Cast::Auto<Foundation::Void*>(Foundation::Cast::Auto<Foundation::Uint8*>(const_cast<Foundation::Void*>(Database_.ImageBase())) + Rva);
	}

	auto SymbolResolver::FindVa(const Library::String& Name, Foundation::Void*& Address) const -> Foundation::Bool { return FindVa(Name.View(), Address); }
	auto SymbolResolver::FindVa(const Foundation::Char* Name, Foundation::Void*& Address) const -> Foundation::Bool { return FindVa(Library::StringView{Name}, Address); }
	auto SymbolResolver::FindVa(Library::StringView Name, Foundation::Void*& Address) const -> Foundation::Bool
	{
		Address = nullptr; Foundation::Uint64 Rva{}; if (!FindRva(Name, Rva)) return false; Address = RvaToPointer(Rva); return Address != nullptr;
	}

	auto SymbolResolver::FindRvaByHash(Foundation::Uint64 NameHash, Foundation::Uint64& Rva) const -> Foundation::Bool
	{
		Rva = 0; if (!Ready()) return false;
		for (const auto& Value : Database_.Symbols())
		{
			const StringId Names[4] = { Value.Name, Value.DecoratedName, Value.QualifiedName, Value.BaseName };
			for (const auto Id : Names)
			{
				if (Id != EmptyStringId && SymbolDatabase::HashName(Database_.String(Id)) == NameHash) { Rva = Value.Rva; return true; }
			}
		}
		return false;
	}

	auto SymbolResolver::FindVaByHash(Foundation::Uint64 NameHash, Foundation::Void*& Address) const -> Foundation::Bool
	{
		Address=nullptr;Foundation::Uint64 Rva{};if(!FindRvaByHash(NameHash,Rva))return false;Address=RvaToPointer(Rva);return Address!=nullptr;
	}

	auto SymbolResolver::Symbolize(Foundation::Uint64 AddressOrRva, Foundation::Bool IsRva) const -> SymbolizedAddress { return Database_.Symbolize(AddressOrRva,IsRva); }
	auto SymbolResolver::FieldOffset(Library::StringView TypeName, Library::StringView FieldName, Foundation::Uint64& Offset) const -> Foundation::Bool { return Database_.FieldOffset(TypeName,FieldName,Offset); }

	auto SymbolResolver::IsExecutableRva(Foundation::Uint64 Rva) const noexcept -> Foundation::Bool
	{
		Executable::Pe::Image Image{Database_.ImageBase(),Allocator_};if(!Image.IsValid())return false;const auto* S=Image.SectionHeaders();for(Foundation::Uint16 i=0;i<Image.SectionCount();++i){const auto Start=S[i].VirtualAddress;const auto Span=S[i].Misc.VirtualSize>S[i].SizeOfRawData?S[i].Misc.VirtualSize:S[i].SizeOfRawData;if(Rva>=Start&&Rva<Foundation::Cast::Auto<Foundation::Uint64>(Start)+Span)return (S[i].Characteristics&ImageScnMemExecute)!=0;}return false;
	}
	auto SymbolResolver::IsDataRva(Foundation::Uint64 Rva) const noexcept -> Foundation::Bool
	{
		Executable::Pe::Image Image{Database_.ImageBase(),Allocator_};if(!Image.IsValid())return false;const auto* S=Image.SectionHeaders();for(Foundation::Uint16 i=0;i<Image.SectionCount();++i){const auto Start=S[i].VirtualAddress;const auto Span=S[i].Misc.VirtualSize>S[i].SizeOfRawData?S[i].Misc.VirtualSize:S[i].SizeOfRawData;if(Rva>=Start&&Rva<Foundation::Cast::Auto<Foundation::Uint64>(Start)+Span)return (S[i].Characteristics&(ImageScnMemRead|ImageScnMemWrite))!=0;}return false;
	}

	auto SymbolResolver::Capacity() const noexcept -> Foundation::Uint64 { return Database_.Symbols().Size(); }
	auto SymbolResolver::Used() const noexcept -> Foundation::Uint64 { return Database_.Symbols().Size(); }
	auto SymbolResolver::ImageBase() const noexcept -> const Foundation::Void* { return Database_.ImageBase(); }
	auto SymbolResolver::ImageSize() const noexcept -> Foundation::Uint64 { return Database_.ImageSize(); }
	auto SymbolResolver::HashName(const Foundation::Char* Name) -> Foundation::Uint64 { return SymbolDatabase::HashName(Library::StringView{Name}); }
	auto SymbolResolver::HashName(Library::StringView Name) -> Foundation::Uint64 { return SymbolDatabase::HashName(Name); }
	auto SymbolResolver::HashName(const Library::String& Name) -> Foundation::Uint64 { return SymbolDatabase::HashName(Name.View()); }
}
