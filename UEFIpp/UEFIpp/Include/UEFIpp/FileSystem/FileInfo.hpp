#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/String/StringView.hpp>
#include <UEFIpp/FileSystem/Path.hpp>
#include <UEFIpp/FileSystem/Time.hpp>
#include <UEFIpp/FileSystem/FileAttributes.hpp>

namespace UEFIpp::FileSystem
{
	class FileInfo
	{
	public:
		// Constructs empty file information
		FileInfo() = default;

		// Constructs file information from its components
		FileInfo(const FileSystem::Path& Path, Foundation::Uint64 Size, Foundation::Uint64 PhysicalSize, const FileSystem::Time& CreatedTime, const FileSystem::Time& ModifiedTime, const FileSystem::Time& AccessedTime, const FileSystem::FileAttributes& Attributes) : Path_(Path), Size_(Size), PhysicalSize_(PhysicalSize), CreatedTime_(CreatedTime), ModifiedTime_(ModifiedTime), AccessedTime_(AccessedTime), Attributes_(Attributes)
		{
		}

		// Copies another FileInfo
		FileInfo(const FileInfo&) = default;

		// Moves another FileInfo
		FileInfo(FileInfo&&) noexcept = default;

		// Destroys the file information
		~FileInfo() = default;

		// Assigns from another FileInfo
		auto operator=(const FileInfo&) -> FileInfo & = default;

		// Move-assigns from another FileInfo
		auto operator=(FileInfo&&) noexcept -> FileInfo & = default;

		// Returns the full file path
		[[nodiscard]] auto Path() const -> const FileSystem::Path&;

		// Returns the file name component
		[[nodiscard]] auto Name() const -> Library::StringView;

		// Returns the file name component
		[[nodiscard]] auto FileName() const -> Library::StringView;

		// Returns the file extension
		[[nodiscard]] auto Extension() const -> Library::StringView;

		// Returns the file stem
		[[nodiscard]] auto Stem() const -> Library::StringView;

		// Checks whether the file has an extension
		[[nodiscard]] auto HasExtension() const -> Foundation::Bool;

		// Returns the logical file size
		[[nodiscard]] auto Size() const -> Foundation::Uint64;

		// Returns the physical file size
		[[nodiscard]] auto PhysicalSize() const -> Foundation::Uint64;

		// Checks whether the file size is zero
		[[nodiscard]] auto Empty() const -> Foundation::Bool;

		// Returns the creation time
		[[nodiscard]] auto CreatedTime() const -> const FileSystem::Time&;

		// Returns the modification time
		[[nodiscard]] auto ModifiedTime() const -> const FileSystem::Time&;

		// Returns the access time
		[[nodiscard]] auto AccessedTime() const -> const FileSystem::Time&;

		// Returns the file attributes
		[[nodiscard]] auto Attributes() const -> const FileSystem::FileAttributes&;

		// Checks whether the entry is a regular file
		[[nodiscard]] auto IsFile() const -> Foundation::Bool;

		// Checks whether the entry is a directory
		[[nodiscard]] auto IsDirectory() const -> Foundation::Bool;

		// Checks whether the file is read-only
		[[nodiscard]] auto IsReadOnly() const -> Foundation::Bool;

		// Checks whether the file is hidden
		[[nodiscard]] auto IsHidden() const -> Foundation::Bool;

		// Checks whether the file is a system file
		[[nodiscard]] auto IsSystem() const -> Foundation::Bool;

		// Checks whether the file is an archive
		[[nodiscard]] auto IsArchive() const -> Foundation::Bool;

		// Checks whether two file information objects are equal
		[[nodiscard]] auto operator==(const FileInfo&) const->Foundation::Bool = default;

	private:
		FileSystem::Path Path_{};

		Foundation::Uint64 Size_{};
		Foundation::Uint64 PhysicalSize_{};

		FileSystem::Time CreatedTime_{};
		FileSystem::Time ModifiedTime_{};
		FileSystem::Time AccessedTime_{};

		FileSystem::FileAttributes Attributes_{};
	};
}
