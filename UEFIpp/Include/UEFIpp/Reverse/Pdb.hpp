#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Expected.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Memory/AllocatorStub.hpp>
#include <UEFIpp/Reverse/SymbolDatabase.hpp>

namespace UEFIpp::Reverse::Pdb
{
	enum class MatchPolicy : Foundation::Uint8
	{
		RequireExact,
		AllowMissingImageIdentity,
		Ignore
	};

	enum class Error : Foundation::Uint8
	{
		None,
		InvalidArgument,
		InvalidMsf,
		InvalidSuperBlock,
		InvalidStreamDirectory,
		InvalidStream,
		OutOfBounds,
		IntegerOverflow,
		AllocationFailure,
		MissingPdbInfo,
		InvalidPdbInfo,
		MissingDbi,
		InvalidDbi,
		AgeMismatch,
		IdentityMismatch,
		MissingImageIdentity,
		InvalidCodeViewRecord,
		InvalidTypeStream,
		InvalidTypeRecord,
		InvalidModuleInfo,
		InvalidModuleStream,
		InvalidLineInfo,
		UnsupportedFormat
	};

	struct ErrorInfo
	{
		Error Code{ Error::None };
		Foundation::Uint32 Stream{ 0xFFFFFFFFu };
		Foundation::Uint64 Offset{};
		Foundation::Uint16 RecordKind{};

		[[nodiscard]] constexpr explicit operator Foundation::Bool() const noexcept
		{
			return Code != Error::None;
		}
	};

	struct LoadOptions
	{
		MatchPolicy IdentityPolicy{ MatchPolicy::RequireExact };
		Foundation::Bool ParsePublicSymbols{ true };
		Foundation::Bool ParseGlobalSymbolRecords{ true };
		Foundation::Bool ParseModuleSymbols{ true };
		Foundation::Bool ParseTypes{ true };
		Foundation::Bool ParseSourceLines{ true };
		Foundation::Bool ParseInlineInformation{ true };
		Foundation::Bool AddPeExports{ true };
	};

	class MsfFile;

	class MsfStream
	{
	public:
		constexpr MsfStream() = default;

		[[nodiscard]] auto Valid() const noexcept -> Foundation::Bool;
		[[nodiscard]] auto Size() const noexcept -> Foundation::Uint64;
		[[nodiscard]] auto Index() const noexcept -> Foundation::Uint32;

		[[nodiscard]] auto Read(
			Foundation::Uint64 Offset,
			Foundation::Void* Destination,
			Foundation::Size Size
		) const -> Library::Expected<Foundation::Void, ErrorInfo>;

		template<typename T>
		[[nodiscard]] auto Read(Foundation::Uint64 Offset = 0) const
			-> Library::Expected<T, ErrorInfo>
		{
			T Result{};
			auto Status = Read(Offset, &Result, sizeof(T));
			if (!Status)
			{
				return Library::MakeUnexpected(Status.Error());
			}
			return Result;
		}

		[[nodiscard]] auto ReadAll(Library::Vector<Foundation::Uint8>& Output) const
			-> Library::Expected<Foundation::Void, ErrorInfo>;

	private:
		friend class MsfFile;
		constexpr MsfStream(const MsfFile* File, Foundation::Uint32 Index) noexcept :
			File_(File), Index_(Index)
		{
		}

		const MsfFile* File_{};
		Foundation::Uint32 Index_{ 0xFFFFFFFFu };
	};

	class MsfFile
	{
	public:
		constexpr MsfFile() = default;
		explicit MsfFile(Memory::AllocatorStub Allocator) noexcept;

		MsfFile(const MsfFile&) = delete;
		auto operator=(const MsfFile&) -> MsfFile& = delete;
		MsfFile(MsfFile&&) noexcept = default;
		auto operator=(MsfFile&&) noexcept -> MsfFile& = default;

		[[nodiscard]] auto Open(const Foundation::Uint8* Data, Foundation::Uint64 Size)
			-> Library::Expected<Foundation::Void, ErrorInfo>;
		auto Reset() -> Foundation::Void;

		[[nodiscard]] auto Valid() const noexcept -> Foundation::Bool;
		[[nodiscard]] auto StreamCount() const noexcept -> Foundation::Uint32;
		[[nodiscard]] auto Stream(Foundation::Uint32 Index) const noexcept -> MsfStream;
		[[nodiscard]] auto Allocator() const noexcept -> Memory::AllocatorStub;

	private:
		friend class MsfStream;

		struct StreamDescriptor
		{
			Foundation::Uint32 Size{ 0xFFFFFFFFu };
			Foundation::Uint32 BlockBegin{};
			Foundation::Uint32 BlockCount{};
		};

		[[nodiscard]] auto ReadStream(
			Foundation::Uint32 StreamIndex,
			Foundation::Uint64 Offset,
			Foundation::Void* Destination,
			Foundation::Size Size
		) const -> Library::Expected<Foundation::Void, ErrorInfo>;

		Memory::AllocatorStub Allocator_{};
		const Foundation::Uint8* Data_{};
		Foundation::Uint64 Size_{};
		Foundation::Uint32 BlockSize_{};
		Foundation::Uint32 BlockCount_{};
		Library::Vector<StreamDescriptor> Streams_{};
		Library::Vector<Foundation::Uint32> StreamBlocks_{};
		Foundation::Bool Valid_{};
	};

	class PdbFile
	{
	public:
		constexpr PdbFile() = default;
		explicit PdbFile(Memory::AllocatorStub Allocator) noexcept;

		PdbFile(const PdbFile&) = delete;
		auto operator=(const PdbFile&) -> PdbFile& = delete;
		PdbFile(PdbFile&&) noexcept = default;
		auto operator=(PdbFile&&) noexcept -> PdbFile& = default;

		[[nodiscard]] auto Open(const Foundation::Uint8* Data, Foundation::Uint64 Size)
			-> Library::Expected<Foundation::Void, ErrorInfo>;
		auto Reset() -> Foundation::Void;

		[[nodiscard]] auto Valid() const noexcept -> Foundation::Bool;
		[[nodiscard]] auto Identity() const noexcept -> const PdbIdentity&;
		[[nodiscard]] auto Msf() const noexcept -> const MsfFile&;

		[[nodiscard]] auto Populate(
			SymbolDatabase& Database,
			const Foundation::Void* ImageBase,
			const LoadOptions& Options = {}
		) const -> Library::Expected<Foundation::Void, ErrorInfo>;

	private:
		Memory::AllocatorStub Allocator_{};
		MsfFile Msf_{};
		PdbIdentity Identity_{};
		Foundation::Bool Valid_{};
	};
}
