#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>

namespace UEFIpp::Crypto::Cipher
{
    class Aes128
    {
    public:
        static constexpr Foundation::Size KeySize = 16;
        static constexpr Foundation::Size BlockSize = 16;
        static constexpr Foundation::Size ExpandedKeySize = 176;

    public:
        Aes128() = default;

        explicit Aes128(Library::Span<const Foundation::Byte> Key)
        {
            SetKey(Key);
        }

        [[nodiscard]] auto SetKey(Library::Span<const Foundation::Byte> Key) -> Foundation::Bool;
        [[nodiscard]] auto EncryptBlock(Library::Span<const Foundation::Byte> Input, Library::Span<Foundation::Byte> Output) const -> Foundation::Bool;

    private:
        Foundation::Byte RoundKey_[ExpandedKeySize]{};
        Foundation::Bool HasKey_{};
    };
}