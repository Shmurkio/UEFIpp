#pragma once

#include <UEFIpp/Loader/PeTypes.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Loader
{
	class PeDirectory
	{
	public:
		constexpr PeDirectory() = default;

		constexpr PeDirectory(Pe::DirectoryIndex Index, Foundation::Uint32 Rva, Foundation::Uint32 Size) : Index_(Index), Rva_(Rva), Size_(Size)
		{
		}

		[[nodiscard]] constexpr auto Index() const -> Pe::DirectoryIndex
		{
			return Index_;
		}

		[[nodiscard]] constexpr auto Rva() const -> Foundation::Uint32
		{
			return Rva_;
		}

		[[nodiscard]] constexpr auto Size() const -> Foundation::Uint32
		{
			return Size_;
		}

		[[nodiscard]] constexpr auto Empty() const -> Foundation::Bool
		{
			return Rva_ == 0 || Size_ == 0;
		}

	private:
		Pe::DirectoryIndex Index_{};
		Foundation::Uint32 Rva_{};
		Foundation::Uint32 Size_{};
	};
}