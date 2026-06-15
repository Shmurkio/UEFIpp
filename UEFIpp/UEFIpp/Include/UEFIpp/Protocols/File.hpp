#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Types.hpp>
#include <UEFIpp/UEFI/Guid.hpp>

namespace UEFIpp::Protocols
{
	class File;

	enum class FileOpenMode : Foundation::Uint64
	{
		Read = Foundation::Bit::Mask<Foundation::Uint64>(0),
		Write = Foundation::Bit::Mask<Foundation::Uint64>(1),
		Create = Foundation::Bit::Mask<Foundation::Uint64>(63)
	};

	enum class FileAttribute : Foundation::Uint64
	{
		ReadOnly = Foundation::Bit::Mask<Foundation::Uint64>(0),
		Hidden = Foundation::Bit::Mask<Foundation::Uint64>(1),
		System = Foundation::Bit::Mask<Foundation::Uint64>(2),
		Reserved = Foundation::Bit::Mask<Foundation::Uint64>(3),
		Directory = Foundation::Bit::Mask<Foundation::Uint64>(4),
		Archive = Foundation::Bit::Mask<Foundation::Uint64>(5)
	};

	[[nodiscard]] constexpr auto ToMask(FileOpenMode Mode) -> Foundation::Uint64
	{
		return Foundation::Cast::Auto<Foundation::Uint64>(Mode);
	}

	[[nodiscard]] constexpr auto ToMask(FileAttribute Attribute) -> Foundation::Uint64
	{
		return Foundation::Cast::Auto<Foundation::Uint64>(Attribute);
	}

	[[nodiscard]] constexpr auto operator|(FileOpenMode Left, FileOpenMode Right) -> Foundation::Uint64
	{
		return ToMask(Left) | ToMask(Right);
	}

	[[nodiscard]] constexpr auto operator|(Foundation::Uint64 Left, FileOpenMode Right) -> Foundation::Uint64
	{
		return Left | ToMask(Right);
	}

	[[nodiscard]] constexpr auto operator|(FileAttribute Left, FileAttribute Right) -> Foundation::Uint64
	{
		return ToMask(Left) | ToMask(Right);
	}

	[[nodiscard]] constexpr auto operator|(Foundation::Uint64 Left, FileAttribute Right) -> Foundation::Uint64
	{
		return Left | ToMask(Right);
	}

	using FileOpenFn = UEFI::StatusCode(*)(File* This, File** NewHandle, const Foundation::WChar* FileName, Foundation::Uint64 OpenMode, Foundation::Uint64 Attributes);
	using FileCloseFn = UEFI::StatusCode(*)(File* This);
	using FileDeleteFn = UEFI::StatusCode(*)(File* This);
	using FileReadFn = UEFI::StatusCode(*)(File* This, Foundation::UintN* BufferSize, Foundation::Void* Buffer);
	using FileWriteFn = UEFI::StatusCode(*)(File* This, Foundation::UintN* BufferSize, const Foundation::Void* Buffer);
	using FileGetPositionFn = UEFI::StatusCode(*)(File* This, Foundation::Uint64* Position);
	using FileSetPositionFn = UEFI::StatusCode(*)(File* This, Foundation::Uint64 Position);
	using FileGetInfoFn = UEFI::StatusCode(*)(File* This, const UEFI::Guid* InformationType, Foundation::UintN* BufferSize, Foundation::Void* Buffer);
	using FileSetInfoFn = UEFI::StatusCode(*)(File* This, const UEFI::Guid* InformationType, Foundation::UintN BufferSize, const Foundation::Void* Buffer);
	using FileFlushFn = UEFI::StatusCode(*)(File* This);

	class File
	{
	public:
		Foundation::Uint64 Revision;
		FileOpenFn Open;
		FileCloseFn Close;
		FileDeleteFn Delete;
		FileReadFn Read;
		FileWriteFn Write;
		FileGetPositionFn GetPosition;
		FileSetPositionFn SetPosition;
		FileGetInfoFn GetInfo;
		FileSetInfoFn SetInfo;
		FileFlushFn Flush;

		[[nodiscard]] auto OpenFile(File*& NewHandle, const Foundation::WChar* FileName, Foundation::Uint64 OpenMode, Foundation::Uint64 Attributes = 0) -> UEFI::StatusCode
		{
			return Open(this, &NewHandle, FileName, OpenMode, Attributes);

		}

		[[nodiscard]] auto CloseFile() -> UEFI::StatusCode
		{
			return Close(this);
		}

		[[nodiscard]] auto DeleteFile() -> UEFI::StatusCode
		{
			return Delete(this);
		}

		[[nodiscard]] auto ReadFile(Foundation::Void* Buffer, Foundation::UintN& BufferSize) -> UEFI::StatusCode
		{
			return Read(this, &BufferSize, Buffer);
		}

		[[nodiscard]] auto WriteFile(const Foundation::Void* Buffer, Foundation::UintN& BufferSize) -> UEFI::StatusCode
		{
			return Write(this, &BufferSize, Buffer);
		}

		[[nodiscard]] auto CurrentPosition(Foundation::Uint64& Position) -> UEFI::StatusCode
		{
			return GetPosition(this, &Position);
		}

		[[nodiscard]] auto SetCurrentPosition(Foundation::Uint64 Position) -> UEFI::StatusCode
		{
			return SetPosition(this, Position);
		}

		[[nodiscard]] auto QueryInfo(UEFI::Guid& InformationType, Foundation::Void* Buffer, Foundation::UintN& BufferSize) -> UEFI::StatusCode
		{
			return GetInfo(this, &InformationType, &BufferSize, Buffer);
		}

		[[nodiscard]] auto UpdateInfo(UEFI::Guid& InformationType, Foundation::Void* Buffer, Foundation::UintN BufferSize) -> UEFI::StatusCode
		{
			return SetInfo(this, &InformationType, BufferSize, Buffer);
		}

		[[nodiscard]] auto FlushFile() -> UEFI::StatusCode
		{
			return Flush(this);
		}
	};

	static_assert(sizeof(File) == 88);
	static_assert(Foundation::Traits::IsStandardLayout<File>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<File>::Value);
}