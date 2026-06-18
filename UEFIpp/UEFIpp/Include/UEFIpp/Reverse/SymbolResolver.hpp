#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/FileSystem/Path.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Loader/PeImage.hpp>

namespace UEFIpp::Reverse
{
	class SymbolResolver
	{
	public:
		static constexpr auto DefaultCapacity = Foundation::Uint64{ 1ull << 16 };

	public:
		SymbolResolver() = default;
		~SymbolResolver();

		SymbolResolver(const SymbolResolver&) = delete;
		auto operator=(const SymbolResolver&) -> SymbolResolver & = delete;

		SymbolResolver(SymbolResolver&& Other) noexcept;
		auto operator=(SymbolResolver&& Other) noexcept -> SymbolResolver&;

		[[nodiscard]] auto Load(const FileSystem::Path& PdbPath, const Foundation::Void* ImageBase, Foundation::Uint64 InitialCapacity = DefaultCapacity) -> Foundation::Bool;
		[[nodiscard]] auto Load(const Library::Vector<Foundation::Uint8>& Pdb, const Foundation::Void* ImageBase, Foundation::Uint64 InitialCapacity = DefaultCapacity) -> Foundation::Bool;
		[[nodiscard]] auto Load(const Foundation::Uint8* PdbBase, Foundation::Uint64 PdbSize, const Foundation::Void* ImageBase, Foundation::Uint64 InitialCapacity = DefaultCapacity) -> Foundation::Bool;

		auto Reset() -> Foundation::Void;

		[[nodiscard]] auto Ready() const -> Foundation::Bool;
		[[nodiscard]] auto Valid() const -> Foundation::Bool;
		[[nodiscard]] explicit operator Foundation::Bool() const;

		[[nodiscard]] auto FindRva(const Library::String& Name, Foundation::Uint64& Rva) const -> Foundation::Bool;
		[[nodiscard]] auto FindRva(Library::StringView Name, Foundation::Uint64& Rva) const -> Foundation::Bool;
		[[nodiscard]] auto FindRva(const Foundation::Char* Name, Foundation::Uint64& Rva) const -> Foundation::Bool;

		[[nodiscard]] auto FindVa(const Library::String& Name, Foundation::Void*& Address) const -> Foundation::Bool;
		[[nodiscard]] auto FindVa(Library::StringView Name, Foundation::Void*& Address) const -> Foundation::Bool;
		[[nodiscard]] auto FindVa(const Foundation::Char* Name, Foundation::Void*& Address) const -> Foundation::Bool;

		template<typename T>
		[[nodiscard]] auto FindVa(const Library::String& Name) const -> T
		{
			Foundation::Void* Address{};
			return FindVa(Name, Address) ? Foundation::Cast::Auto<T>(Address) : nullptr;
		}

		template<typename T>
		[[nodiscard]] auto FindVa(Library::StringView Name) const -> T
		{
			Foundation::Void* Address{};
			return FindVa(Name, Address) ? Foundation::Cast::Auto<T>(Address) : nullptr;
		}

		template<typename T>
		[[nodiscard]] auto FindVa(const Foundation::Char* Name) const -> T
		{
			Foundation::Void* Address{};
			return FindVa(Name, Address) ? Foundation::Cast::Auto<T>(Address) : nullptr;
		}

		[[nodiscard]] auto FindRvaByHash(Foundation::Uint64 NameHash, Foundation::Uint64& Rva) const -> Foundation::Bool;
		[[nodiscard]] auto FindVaByHash(Foundation::Uint64 NameHash, Foundation::Void*& Address) const -> Foundation::Bool;

		template<typename T>
		[[nodiscard]] auto FindVaByHash(Foundation::Uint64 NameHash) const -> T
		{
			Foundation::Void* Address{};
			return FindVaByHash(NameHash, Address) ? Foundation::Cast::Auto<T>(Address) : nullptr;
		}

		[[nodiscard]] auto Capacity() const -> Foundation::Uint64;
		[[nodiscard]] auto Used() const -> Foundation::Uint64;
		[[nodiscard]] auto ImageBase() const -> const Foundation::Void*;
		[[nodiscard]] auto ImageSize() const -> Foundation::Uint64;

		[[nodiscard]] static auto HashName(const Foundation::Char* Name) -> Foundation::Uint64;
		[[nodiscard]] static auto HashName(Library::StringView Name) -> Foundation::Uint64;
		[[nodiscard]] static auto HashName(const Library::String& Name) -> Foundation::Uint64;

	private:
		struct Entry
		{
			Foundation::Uint64 NameHash{};
			Foundation::Uint64 Rva{};
		};

		[[nodiscard]] auto Reserve(Foundation::Uint64 NewCapacity) -> Foundation::Bool;
		auto FreeMap() -> Foundation::Void;
		[[nodiscard]] auto Insert(Foundation::Uint64 NameHash, Foundation::Uint64 Rva) -> Foundation::Bool;
		[[nodiscard]] auto Find(Foundation::Uint64 NameHash, Foundation::Uint64& Rva) const -> Foundation::Bool;

		[[nodiscard]] auto ParsePdb(const Foundation::Uint8* PdbBase, Foundation::Uint64 PdbSize) -> Foundation::Bool;
		[[nodiscard]] auto LoadImageLayout(const Foundation::Void* ImageBase) -> Foundation::Bool;
		[[nodiscard]] auto SegmentOffsetToRva(Foundation::Uint16 Segment, Foundation::Uint64 Offset, Foundation::Uint64& Rva) const -> Foundation::Bool;

	private:
		Entry* Entries_{};
		Foundation::Uint64 Capacity_{};
		Foundation::Uint64 Used_{};

		const Foundation::Uint8* ImageBase_{};
		const PeImage::SectionHeader* Sections_{};
		Foundation::Uint16 SectionCount_{};
		Foundation::Uint64 ImageSize_{};
		Foundation::Bool Ready_{};
	};
}