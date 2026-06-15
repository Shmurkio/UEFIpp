#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>

namespace UEFIpp::Crypto
{
	class SecureZero
	{
	public:
		SecureZero() = delete;

		static auto Memory(Foundation::Void* Data, Foundation::Size Size) -> Foundation::Void
		{
			volatile auto* Bytes = Foundation::Cast::Auto<volatile Foundation::Uint8*>(Data);

			while (Size--)
			{
				*Bytes++ = 0;
			}
		}

		static auto Bytes(Library::Span<Foundation::Uint8> Data) -> Foundation::Void
		{
			Memory(Data.Data(), Data.Size());
		}

		template<typename T>
		static auto Object(T& Value) -> Foundation::Void
		{
			Memory(&Value, sizeof(T));
		}
	};
}