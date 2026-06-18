#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/Protocols/File.hpp>
#include <UEFIpp/FileSystem/Path.hpp>
#include <UEFIpp/FileSystem/FileInfo.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>

namespace UEFIpp::FileSystem
{
	enum class FileOpenMode : Foundation::Uint64
	{
		Read = Protocols::FileOpenMode::Read,
		Write = Protocols::FileOpenMode::Write,
		Create = Protocols::FileOpenMode::Create,
		ReadWrite = Protocols::FileOpenMode::Read | Protocols::FileOpenMode::Write,
		CreateReadWrite = Protocols::FileOpenMode::Read | Protocols::FileOpenMode::Write | Protocols::FileOpenMode::Create
	};

	class File
	{
	private:
		Protocols::File* Handle_{};
		FileInfo Info_{};

	public:
		// Constructs an empty file
		File() = default;

		// Files own firmware handles and cannot be copied
		File(const File&) = delete;

		// Moves another file
		File(File&& Other) noexcept;

		// Closes the file if it owns the handle
		~File();

		// Files own firmware handles and cannot be copy-assigned
		auto operator=(const File&) -> File & = delete;

		// Move-assigns from another file
		auto operator=(File&& Other) noexcept -> File&;

		// Opens a file on the current file system
		[[nodiscard]] auto Open(const Path& Path, FileOpenMode Mode = FileOpenMode::Read) -> Foundation::Bool;

		// Opens a file on the specified file system
		[[nodiscard]] auto Open(const Path& Path, UEFI::Handle FsHandle, FileOpenMode Mode = FileOpenMode::Read) -> Foundation::Bool;

		// Checks whether the file has a valid handle
		[[nodiscard]] auto Valid() const -> Foundation::Bool;

		// Returns the native file protocol handle
		[[nodiscard]] auto Handle() const -> Protocols::File&;

		// Returns the opened file information
		[[nodiscard]] auto Info() const -> const FileInfo&;

		// Closes the file
		[[nodiscard]] auto Close() -> Foundation::Bool;

		// Deletes the file
		[[nodiscard]] auto Delete() -> Foundation::Bool;

		// Flushes the file buffers
		[[nodiscard]] auto Flush() -> Foundation::Bool;

		// Reads the file data into a vector
		[[nodiscard]] auto Read(Library::Vector<Foundation::Uint8>& Buffer) -> Foundation::Bool;

		// Reads the file data into a buffer (allocated by callee) and returns the buffer size
		[[nodiscard]] auto Read(Foundation::Uint8*& Buffer, Foundation::Uint64& BufferSize) -> Foundation::Bool;

		// Writes the given data to the file
		[[nodiscard]] auto Write(const Library::Vector<Foundation::Uint8>& Buffer) -> Foundation::Bool;

		// Writes the given data to the file, in form of a buffer and its size
		[[nodiscard]] auto Write(const Foundation::Uint8* Buffer, Foundation::Uint64 BufferSize) -> Foundation::Bool;

		// Writes the given data to the file, starting at the given position
		[[nodiscard]] auto Write(const Library::Vector<Foundation::Uint8>& Buffer, Foundation::Uint64 Position) -> Foundation::Bool;

		// Writes the given data to the file, in form of a buffer and its size, starting at the given position
		[[nodiscard]] auto Write(const Foundation::Uint8* Buffer, Foundation::Uint64 BufferSize, Foundation::Uint64 Position) -> Foundation::Bool;

		// Appends the given data to the end of the file
		[[nodiscard]] auto Append(const Library::Vector<Foundation::Uint8>& Buffer) -> Foundation::Bool;

		// Appends the given data to the end of the file, in form of a buffer and its size
		[[nodiscard]] auto Append(const Foundation::Uint8* Buffer, Foundation::Uint64 BufferSize) -> Foundation::Bool;

		// Returns the current file position
		[[nodiscard]] auto Position(Foundation::Uint64& Position) -> Foundation::Bool;
	};
}