#include <UEFIpp/FileSystem/FileAttributes.hpp>

namespace UEFIpp::FileSystem
{
	namespace
	{
		[[nodiscard]] constexpr auto ToMask(FileAttribute Attribute) -> Foundation::Uint64
		{
			return Foundation::Cast::Auto<Foundation::Uint64>(Attribute);
		}
	}

	auto FileAttributes::Mask() const -> Foundation::Uint64
	{
		return Mask_;
	}

	auto FileAttributes::Has(FileAttribute Attribute) const -> Foundation::Bool
	{
		return Foundation::Bit::HasAny(Mask_, ToMask(Attribute));
	}

	auto FileAttributes::Set(FileAttribute Attribute) -> Foundation::Void
	{
		Mask_ = Foundation::Bit::Set(Mask_, ToMask(Attribute));
	}

	auto FileAttributes::Clear(FileAttribute Attribute) -> Foundation::Void
	{
		Mask_ = Foundation::Bit::Clear(Mask_, ToMask(Attribute));
	}

	auto FileAttributes::Toggle(FileAttribute Attribute) -> Foundation::Void
	{
		Mask_ = Foundation::Bit::Toggle(Mask_, ToMask(Attribute));
	}

	auto FileAttributes::IsReadOnly() const -> Foundation::Bool
	{
		return Has(FileAttribute::ReadOnly);
	}

	auto FileAttributes::IsHidden() const -> Foundation::Bool
	{
		return Has(FileAttribute::Hidden);
	}

	auto FileAttributes::IsSystem() const -> Foundation::Bool
	{
		return Has(FileAttribute::System);
	}

	auto FileAttributes::IsDirectory() const -> Foundation::Bool
	{
		return Has(FileAttribute::Directory);
	}

	auto FileAttributes::IsArchive() const -> Foundation::Bool
	{
		return Has(FileAttribute::Archive);
	}
}