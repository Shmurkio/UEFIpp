#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/FileSystem/Path.hpp>
#include <UEFIpp/Library/Containers/Expected.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Memory/AllocatorStub.hpp>
#include <UEFIpp/Reverse/Pdb.hpp>
#include <UEFIpp/Reverse/SymbolDatabase.hpp>

namespace UEFIpp::Reverse
{
	class SymbolResolver
	{
	public:
		static constexpr auto DefaultCapacity = Foundation::Uint64{ 1ull << 16 };

	public:
		constexpr SymbolResolver() = default;
		explicit SymbolResolver(Memory::AllocatorStub Allocator) noexcept;

		SymbolResolver(const SymbolResolver&) = delete;
		auto operator=(const SymbolResolver&) -> SymbolResolver& = delete;
		SymbolResolver(SymbolResolver&&) noexcept = default;
		auto operator=(SymbolResolver&&) noexcept -> SymbolResolver& = default;

		[[nodiscard]] auto Load(const FileSystem::Path& PdbPath, const Foundation::Void* ImageBase, Foundation::Uint64 InitialCapacity = DefaultCapacity) -> Foundation::Bool;
		[[nodiscard]] auto Load(const Library::Vector<Foundation::Uint8>& Pdb, const Foundation::Void* ImageBase, Foundation::Uint64 InitialCapacity = DefaultCapacity) -> Foundation::Bool;
		[[nodiscard]] auto Load(const Foundation::Uint8* PdbBase, Foundation::Uint64 PdbSize, const Foundation::Void* ImageBase, Foundation::Uint64 InitialCapacity = DefaultCapacity) -> Foundation::Bool;

		[[nodiscard]] auto LoadDetailed(
			const Foundation::Uint8* PdbBase,
			Foundation::Uint64 PdbSize,
			const Foundation::Void* ImageBase,
			const Pdb::LoadOptions& Options = {}
		) -> Library::Expected<Foundation::Void, Pdb::ErrorInfo>;

		auto Reset() -> Foundation::Void;

		[[nodiscard]] auto Ready() const noexcept -> Foundation::Bool;
		[[nodiscard]] auto Valid() const noexcept -> Foundation::Bool;
		[[nodiscard]] explicit operator Foundation::Bool() const noexcept;
		[[nodiscard]] auto LastError() const noexcept -> const Pdb::ErrorInfo&;

		[[nodiscard]] auto Database() noexcept -> SymbolDatabase&;
		[[nodiscard]] auto Database() const noexcept -> const SymbolDatabase&;

		[[nodiscard]] auto FindRva(const Library::String& Name, Foundation::Uint64& Rva) const -> Foundation::Bool;
		[[nodiscard]] auto FindRva(Library::StringView Name, Foundation::Uint64& Rva) const -> Foundation::Bool;
		[[nodiscard]] auto FindRva(const Foundation::Char* Name, Foundation::Uint64& Rva) const -> Foundation::Bool;

		[[nodiscard]] auto FindVa(const Library::String& Name, Foundation::Void*& Address) const -> Foundation::Bool;
		[[nodiscard]] auto FindVa(Library::StringView Name, Foundation::Void*& Address) const -> Foundation::Bool;
		[[nodiscard]] auto FindVa(const Foundation::Char* Name, Foundation::Void*& Address) const -> Foundation::Bool;

		template<typename T>
		[[nodiscard]] auto FindVa(Library::StringView Name) const -> T
		{
			Foundation::Void* Address{};
			return FindVa(Name, Address) ? Foundation::Cast::Auto<T>(Address) : nullptr;
		}

		template<typename T>
		[[nodiscard]] auto FindVa(const Library::String& Name) const -> T { return FindVa<T>(Name.View()); }
		template<typename T>
		[[nodiscard]] auto FindVa(const Foundation::Char* Name) const -> T { return FindVa<T>(Library::StringView{Name}); }

		template<typename TFunction>
		[[nodiscard]] auto FindFunction(Library::StringView Name) const -> TFunction
		{
			const auto* Value = Database_.FindFunction(Name);
			if (!Value || !IsExecutableRva(Value->Rva)) return nullptr;
			return Foundation::Cast::Auto<TFunction>(RvaToPointer(Value->Rva));
		}

		template<typename TObject>
		[[nodiscard]] auto FindGlobal(Library::StringView Name) const -> TObject*
		{
			const auto* Value = Database_.FindGlobal(Name);
			if (!Value || !IsDataRva(Value->Rva)) return nullptr;
			return Foundation::Cast::Auto<TObject*>(RvaToPointer(Value->Rva));
		}

		[[nodiscard]] auto FindRvaByHash(Foundation::Uint64 NameHash, Foundation::Uint64& Rva) const -> Foundation::Bool;
		[[nodiscard]] auto FindVaByHash(Foundation::Uint64 NameHash, Foundation::Void*& Address) const -> Foundation::Bool;

		template<typename T>
		[[nodiscard]] auto FindVaByHash(Foundation::Uint64 NameHash) const -> T
		{
			Foundation::Void* Address{};
			return FindVaByHash(NameHash, Address) ? Foundation::Cast::Auto<T>(Address) : nullptr;
		}

		[[nodiscard]] auto Symbolize(Foundation::Uint64 AddressOrRva, Foundation::Bool IsRva = false) const -> SymbolizedAddress;
		[[nodiscard]] auto FieldOffset(Library::StringView TypeName, Library::StringView FieldName, Foundation::Uint64& Offset) const -> Foundation::Bool;

		[[nodiscard]] auto Capacity() const noexcept -> Foundation::Uint64;
		[[nodiscard]] auto Used() const noexcept -> Foundation::Uint64;
		[[nodiscard]] auto ImageBase() const noexcept -> const Foundation::Void*;
		[[nodiscard]] auto ImageSize() const noexcept -> Foundation::Uint64;
		[[nodiscard]] constexpr auto Allocator() const noexcept -> Memory::AllocatorStub { return Allocator_; }

		[[nodiscard]] static auto HashName(const Foundation::Char* Name) -> Foundation::Uint64;
		[[nodiscard]] static auto HashName(Library::StringView Name) -> Foundation::Uint64;
		[[nodiscard]] static auto HashName(const Library::String& Name) -> Foundation::Uint64;

	private:
		[[nodiscard]] auto RvaToPointer(Foundation::Uint64 Rva) const noexcept -> Foundation::Void*;
		[[nodiscard]] auto IsExecutableRva(Foundation::Uint64 Rva) const noexcept -> Foundation::Bool;
		[[nodiscard]] auto IsDataRva(Foundation::Uint64 Rva) const noexcept -> Foundation::Bool;

	private:
		Memory::AllocatorStub Allocator_{};
		SymbolDatabase Database_{};
		Pdb::ErrorInfo LastError_{};
	};

}
