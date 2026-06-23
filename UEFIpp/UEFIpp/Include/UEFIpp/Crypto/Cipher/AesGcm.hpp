#pragma once

#include <UEFIpp/Crypto/Cipher/Aes.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>

namespace UEFIpp::Crypto::Cipher
{
    class AesGcm
    {
    public:
        static constexpr Foundation::Size Aes128KeySize = 16;
        static constexpr Foundation::Size IvSize = 12;
        static constexpr Foundation::Size TagSize = 16;

    public:
        AesGcm() = delete;

        [[nodiscard]] static auto Decrypt(Library::Span<const Foundation::Byte> Key, Library::Span<const Foundation::Byte> Iv, Library::Span<const Foundation::Byte> Aad, Library::Span<const Foundation::Byte> Ciphertext, Library::Span<const Foundation::Byte> Tag, Library::Span<Foundation::Byte> Plaintext) -> Foundation::Bool;
        [[nodiscard]] static auto Encrypt(Library::Span<const Foundation::Byte> Key, Library::Span<const Foundation::Byte> Iv, Library::Span<const Foundation::Byte> Aad, Library::Span<const Foundation::Byte> Plaintext, Library::Span<Foundation::Byte> Ciphertext, Library::Span<Foundation::Byte> Tag) -> Foundation::Bool;
    };
}