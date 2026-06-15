#pragma once

#include <UEFIpp/Crypto/Hash/Sha256.hpp>
#include <UEFIpp/Crypto/Mac/Hmac.hpp>

namespace UEFIpp::Crypto::Mac
{
	using HmacSha256 = Hmac<Hash::Sha256>;
}