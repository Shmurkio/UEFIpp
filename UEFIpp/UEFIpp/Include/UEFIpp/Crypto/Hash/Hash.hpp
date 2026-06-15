#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Array.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>

namespace UEFIpp::Crypto::Hash
{
	enum class Algorithm : Foundation::Uint8
	{
		Sha256,
		Sha384,
		Sha512,
	};

	template<Foundation::Size N>
	using Digest = Library::Array<Foundation::Uint8, N>;

	using Sha256Digest = Digest<32>;
	using Sha384Digest = Digest<48>;
	using Sha512Digest = Digest<64>;

	template<typename THash>
	concept HashProvider = requires(THash Hash, Library::Span<const Foundation::Uint8> Input, Library::Span<Foundation::Uint8> Output)
	{
		{ THash::BlockSize };
		{ THash::DigestSize };
		{ Hash.Reset() } -> Foundation::Concepts::Same<Foundation::Void>;
		{ Hash.Update(Input) } -> Foundation::Concepts::Same<Foundation::Void>;
		{ Hash.Final(Output) } -> Foundation::Concepts::Same<Foundation::Bool>;
	};
}