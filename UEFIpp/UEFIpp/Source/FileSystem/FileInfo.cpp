#include <UEFIpp/FileSystem/FileInfo.hpp>

namespace UEFIpp::FileSystem
{
	auto FileInfo::Path() const -> const FileSystem::Path&
	{
		return Path_;
	}

	auto FileInfo::Name() const -> Library::StringView
	{
		return Path_.FileName();
	}

	auto FileInfo::FileName() const -> Library::StringView
	{
		return Path_.FileName();
	}

	auto FileInfo::Extension() const -> Library::StringView
	{
		return Path_.Extension();
	}

	auto FileInfo::Stem() const -> Library::StringView
	{
		return Path_.Stem();
	}

	auto FileInfo::HasExtension() const -> Foundation::Bool
	{
		return Path_.HasExtension();
	}

	auto FileInfo::Size() const -> Foundation::Uint64
	{
		return Size_;
	}

	auto FileInfo::PhysicalSize() const -> Foundation::Uint64
	{
		return PhysicalSize_;
	}

	auto FileInfo::Empty() const -> Foundation::Bool
	{
		return Size_ == 0;
	}

	auto FileInfo::CreatedTime() const -> const FileSystem::Time&
	{
		return CreatedTime_;
	}

	auto FileInfo::ModifiedTime() const -> const FileSystem::Time&
	{
		return ModifiedTime_;
	}

	auto FileInfo::AccessedTime() const -> const FileSystem::Time&
	{
		return AccessedTime_;
	}

	auto FileInfo::Attributes() const -> const FileSystem::FileAttributes&
	{
		return Attributes_;
	}

	auto FileInfo::IsFile() const -> Foundation::Bool
	{
		return !IsDirectory();
	}

	auto FileInfo::IsDirectory() const -> Foundation::Bool
	{
		return Attributes_.IsDirectory();
	}

	auto FileInfo::IsReadOnly() const -> Foundation::Bool
	{
		return Attributes_.IsReadOnly();
	}

	auto FileInfo::IsHidden() const -> Foundation::Bool
	{
		return Attributes_.IsHidden();
	}

	auto FileInfo::IsSystem() const -> Foundation::Bool
	{
		return Attributes_.IsSystem();
	}

	auto FileInfo::IsArchive() const -> Foundation::Bool
	{
		return Attributes_.IsArchive();
	}
}
