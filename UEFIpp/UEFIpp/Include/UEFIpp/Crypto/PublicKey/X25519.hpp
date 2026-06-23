#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>

namespace UEFIpp::Crypto::PublicKey
{
    class X25519
    {
    public:
        static constexpr Foundation::Size KeySize = 32;

    public:
        X25519() = delete;

        [[nodiscard]] static auto GeneratePrivateKey(Library::Span<const Foundation::Byte> Random, Library::Span<Foundation::Byte> PrivateKey) -> Foundation::Bool;

        [[nodiscard]] static auto ComputePublicKey(Library::Span<const Foundation::Byte> PrivateKey, Library::Span<Foundation::Byte> PublicKey) -> Foundation::Bool;
        [[nodiscard]] static auto ComputeSharedSecret(Library::Span<const Foundation::Byte> PrivateKey, Library::Span<const Foundation::Byte> PeerPublicKey, Library::Span<Foundation::Byte> SharedSecret) -> Foundation::Bool;
        
        static auto ClampPrivateKey(Library::Span<Foundation::Byte> PrivateKey) -> Foundation::Void;

    private:
        [[nodiscard]] static auto ScalarMult(const Foundation::Byte* Scalar, const Foundation::Byte* Point, Foundation::Byte* Result) -> Foundation::Bool;
    };
}