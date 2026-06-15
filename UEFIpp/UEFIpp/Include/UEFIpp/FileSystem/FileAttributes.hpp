#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::FileSystem
{
    enum class FileAttribute : Foundation::Uint64
    {
        None = 0,
        ReadOnly = Foundation::Bit::Mask<Foundation::Uint64>(0),
        Hidden = Foundation::Bit::Mask<Foundation::Uint64>(1),
        System = Foundation::Bit::Mask<Foundation::Uint64>(2),
        Directory = Foundation::Bit::Mask<Foundation::Uint64>(4),
        Archive = Foundation::Bit::Mask<Foundation::Uint64>(5)
    };

    class FileAttributes
    {
	public:
		// Constructs empty file attributes
        FileAttributes() = default;

		// Constructs file attributes from an attribute
		FileAttributes(FileAttribute Attribute) : Mask_(Foundation::Cast::Auto<Foundation::Uint64>(Attribute))
		{
		}

		// Constructs file attributes from a raw mask
		FileAttributes(Foundation::Uint64 Mask) : Mask_(Mask)
		{
		}

		// Copies another FileAttributes
		FileAttributes(const FileAttributes&) = default;

		// Moves another FileAttributes
		FileAttributes(FileAttributes&&) noexcept = default;

		// Destroys the file attributes
		~FileAttributes() = default;

		// Assigns from another FileAttributes
		auto operator=(const FileAttributes&) -> FileAttributes& = default;

		// Move-assigns from another FileAttributes
		auto operator=(FileAttributes&&) noexcept -> FileAttributes& = default;

		// Returns the underlying attribute mask
        [[nodiscard]] auto Mask() const -> Foundation::Uint64;

		// Checks whether an attribute is set
		[[nodiscard]] auto Has(FileAttribute Attribute) const -> Foundation::Bool;

		// Sets an attribute
		auto Set(FileAttribute Attribute) -> Foundation::Void;

		// Clears an attribute
		auto Clear(FileAttribute Attribute) -> Foundation::Void;

		// Toggles an attribute
		auto Toggle(FileAttribute Attribute) -> Foundation::Void;

		// Checks whether the file is read-only
		[[nodiscard]] auto IsReadOnly() const -> Foundation::Bool;

		// Checks whether the file is hidden
		[[nodiscard]] auto IsHidden() const -> Foundation::Bool;

		// Checks whether the file is a system file
		[[nodiscard]] auto IsSystem() const -> Foundation::Bool;

		// Checks whether the file is a directory
		[[nodiscard]] auto IsDirectory() const -> Foundation::Bool;

		// Checks whether the file is an archive
		[[nodiscard]] auto IsArchive() const -> Foundation::Bool;

    private:
        Foundation::Uint64 Mask_{};
    };
}