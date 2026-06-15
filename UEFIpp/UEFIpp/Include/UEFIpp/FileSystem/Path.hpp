#pragma once

#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Library/String/StringView.hpp>

namespace UEFIpp::FileSystem
{
	class Path
	{
	public:
		// Constructs an empty path
		Path() = default;

		// Constructs a path from a string
		Path(const Library::String& Path) : Path_(Path)
		{
		}

		// Constructs a path from a C-style string
		Path(const Foundation::Char* Path) : Path_(Path)
		{
		}

		// Constructs a path from a string view
		Path(Library::StringView Path) : Path_(Path)
		{
		}

		// Copies another path
		Path(const Path&) = default;

		// Moves another path
		Path(Path&&) noexcept = default;

		// Destroys the path
		~Path() = default;

		// Assigns from another path
		auto operator=(const Path&) -> Path& = default;

		// Move-assigns from another path
		auto operator=(Path&&) noexcept -> Path& = default;

		// Assigns from a string view
		auto operator=(Library::StringView Path) -> FileSystem::Path&;

		// Assigns from a C-style string
		auto operator=(const Foundation::Char* Path) -> FileSystem::Path&;

		// Returns the underlying path string
		[[nodiscard]] auto String() const -> const Library::String&;

		// Returns a view of the path string
		[[nodiscard]] auto View() const -> Library::StringView;

		// Checks whether the path is empty
		[[nodiscard]] auto Empty() const -> Foundation::Bool;

		// Checks whether the path is absolute (starts with a backslash)
		[[nodiscard]] auto IsAbsolute() const -> Foundation::Bool;

		// Checks whether the path is the root path (a single backslash)
		[[nodiscard]] auto IsRoot() const -> Foundation::Bool;

		// Returns the components of the path as a vector of string views, split by backslashes
		[[nodiscard]] auto Components() const -> Library::Vector<Library::StringView>;

		// 1) Converts '/' to '\'
		// 2) Removes repeated separators
		// 3) Removes "." components
		// 4) Resolves ".." components
		// 5) Removes trailing separators (except for root)
		// 6) Preserves absolute paths starting with '\'
		[[nodiscard]] auto Normalized() const -> Path;

		// Normalizes the path in-place
		auto Normalize() -> Foundation::Void;

		// Joins two paths together, inserting a backslash if necessary and normalizing the result
		[[nodiscard]] static auto Join(const Path& Left, const Path& Right) -> Path;

		// Joins this path with another path, inserting a backslash if necessary and normalizing the result
		[[nodiscard]] auto Join(const Path& Other) const -> Path;

		// Returns the parent path by removing the last component, or an empty path if there is no parent
		[[nodiscard]] auto Parent() const -> Path;

		// Returns the file name component of the path (the last component), or an empty string view if there are no components
		[[nodiscard]] auto FileName() const -> Library::StringView;

		// Returns the file extension of the path (the substring after the last '.' in the last component), or an empty string view if there is no extension
		[[nodiscard]] auto Extension() const -> Library::StringView;

		// Returns the file stem of the path (the last component without the extension)
		[[nodiscard]] auto Stem() const -> Library::StringView;

		// Returns the number of components in the path
		[[nodiscard]] auto NameCount() const -> Foundation::Size;

		// Checks whether the path has a file extension
		[[nodiscard]] auto HasExtension() const -> Foundation::Bool;

		// Returns a new path with the file extension replaced by the given extension, or added if there is no existing extension. The extension should not include the '.' character
		[[nodiscard]] auto ReplaceExtension(Library::StringView Extension) const -> Path;

		// Returns a new path with the file extension removed
		[[nodiscard]] auto RemoveExtension() const -> Path;

		// Returns a new path that is relative to the given base path, by removing the common prefix components and adding ".." components as necessary
		[[nodiscard]] auto RelativeTo(const Path& Base) const -> Path;

		// Compares two paths for equality
		[[nodiscard]] auto operator==(const Path& Other) const->Foundation::Bool = default;

	private:
		Library::String Path_{};
	};
}