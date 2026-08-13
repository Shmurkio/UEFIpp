#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::UEFI::Table
{
	class Header
	{
	public:
		Foundation::Uint64 Signature;
		Foundation::Uint32 Revision;
		Foundation::Uint32 HeaderSize;
		Foundation::Uint32 Crc32;
		Foundation::Uint32 Reserved;

		[[nodiscard]] constexpr auto operator<=>(const Header&) const = default;
	};

	static_assert(sizeof(Header) == 24);
	static_assert(Foundation::Traits::IsStandardLayout<Header>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<Header>::Value);
}