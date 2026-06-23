#include <UEFIpp/Crypto/Cipher/AesGcm.hpp>

namespace UEFIpp::Crypto::Cipher
{
    namespace
    {
        static auto XorBlock(Foundation::Byte* Left, const Foundation::Byte* Right) -> Foundation::Void
        {
            for (Foundation::Size Index{}; Index < 16; ++Index)
            {
                Left[Index] ^= Right[Index];
            }
        }

        static auto RightShiftBlock(Foundation::Byte* Block) -> Foundation::Void
        {
            Foundation::Byte Carry{};

            for (Foundation::Size Index{}; Index < 16; ++Index)
            {
                const auto NewCarry = Foundation::Cast::Auto<Foundation::Byte>(Block[Index] & 1);
                Block[Index] = Foundation::Cast::Auto<Foundation::Byte>((Block[Index] >> 1) | (Carry << 7));
                Carry = NewCarry;
            }
        }

        static auto GaloisMultiply(const Foundation::Byte X[16], const Foundation::Byte Y[16], Foundation::Byte Out[16]) -> Foundation::Void
        {
            Foundation::Byte Z[16]{};
            Foundation::Byte V[16]{};

            for (Foundation::Size Index{}; Index < 16; ++Index)
            {
                V[Index] = Y[Index];
            }

            for (Foundation::Size Bit{}; Bit < 128; ++Bit)
            {
                const auto ByteIndex = Bit / 8;
                const auto BitIndex = 7 - (Bit % 8);

                if ((X[ByteIndex] >> BitIndex) & 1)
                {
                    XorBlock(Z, V);
                }

                const auto Lsb = Foundation::Cast::Auto<Foundation::Bool>(V[15] & 1);

                RightShiftBlock(V);

                if (Lsb)
                {
                    V[0] ^= 0xE1;
                }
            }

            for (Foundation::Size Index{}; Index < 16; ++Index)
            {
                Out[Index] = Z[Index];
            }
        }

        static auto GhashBlock(Foundation::Byte Y[16], const Foundation::Byte H[16], const Foundation::Byte Block[16]) -> Foundation::Void
        {
            XorBlock(Y, Block);

            Foundation::Byte Product[16]{};

            GaloisMultiply(Y, H, Product);

            for (Foundation::Size Index{}; Index < 16; ++Index)
            {
                Y[Index] = Product[Index];
            }
        }

        static auto WriteUint64Be(Foundation::Byte* Out, Foundation::Uint64 Value) -> Foundation::Void
        {
            for (Foundation::Size Index{}; Index < 8; ++Index)
            {
                Out[Index] = Foundation::Cast::Auto<Foundation::Byte>((Value >> ((7 - Index) * 8)) & 0xFF);
            }
        }

        static auto Increment32(Foundation::Byte Counter[16]) -> Foundation::Void
        {
            for (Foundation::Size Index{ 16 }; Index > 12; --Index)
            {
                ++Counter[Index - 1];

                if (Counter[Index - 1] != 0)
                {
                    break;
                }
            }
        }

        static auto ConstantTimeEqual(Library::Span<const Foundation::Byte> Left, Library::Span<const Foundation::Byte> Right) -> Foundation::Bool
        {
            if (Left.Size() != Right.Size())
            {
                return false;
            }

            Foundation::Byte Difference{};

            for (Foundation::Size Index{}; Index < Left.Size(); ++Index)
            {
                Difference |= Foundation::Cast::Auto<Foundation::Byte>(Left[Index] ^ Right[Index]);
            }

            return Difference == 0;
        }

        static auto GHash(Library::Span<const Foundation::Byte> H, Library::Span<const Foundation::Byte> Aad, Library::Span<const Foundation::Byte> Ciphertext, Library::Span<Foundation::Byte> Out) -> Foundation::Bool
        {
            if (H.Size() != 16 || Out.Size() != 16)
            {
                return false;
            }

            Foundation::Byte Y[16]{};
            Foundation::Byte Block[16]{};

            Foundation::Size Offset{};

            while (Offset < Aad.Size())
            {
                for (Foundation::Size Index{}; Index < 16; ++Index)
                {
                    Block[Index] = 0;
                }

                const auto Remaining = Aad.Size() - Offset;
                const auto Size = Remaining < 16 ? Remaining : 16;

                for (Foundation::Size Index{}; Index < Size; ++Index)
                {
                    Block[Index] = Aad[Offset + Index];
                }

                GhashBlock(Y, H.Data(), Block);
                Offset += Size;
            }

            Offset = 0;

            while (Offset < Ciphertext.Size())
            {
                for (Foundation::Size Index{}; Index < 16; ++Index)
                {
                    Block[Index] = 0;
                }

                const auto Remaining = Ciphertext.Size() - Offset;
                const auto Size = Remaining < 16 ? Remaining : 16;

                for (Foundation::Size Index{}; Index < Size; ++Index)
                {
                    Block[Index] = Ciphertext[Offset + Index];
                }

                GhashBlock(Y, H.Data(), Block);
                Offset += Size;
            }

            for (Foundation::Size Index{}; Index < 16; ++Index)
            {
                Block[Index] = 0;
            }

            WriteUint64Be(Block, Foundation::Cast::Auto<Foundation::Uint64>(Aad.Size() * 8));
            WriteUint64Be(Block + 8, Foundation::Cast::Auto<Foundation::Uint64>(Ciphertext.Size() * 8));

            GhashBlock(Y, H.Data(), Block);

            for (Foundation::Size Index{}; Index < 16; ++Index)
            {
                Out[Index] = Y[Index];
            }

            return true;
        }
    }

    auto AesGcm::Decrypt(Library::Span<const Foundation::Byte> Key, Library::Span<const Foundation::Byte> Iv, Library::Span<const Foundation::Byte> Aad, Library::Span<const Foundation::Byte> Ciphertext, Library::Span<const Foundation::Byte> Tag, Library::Span<Foundation::Byte> Plaintext) -> Foundation::Bool
    {
        if (Key.Size() != Aes128KeySize || Iv.Size() != IvSize || Tag.Size() != TagSize || Plaintext.Size() != Ciphertext.Size())
        {
            return false;
        }

        Aes128 Aes{ Key };

        Foundation::Byte Zero[16]{};
        Foundation::Byte H[16]{};

        if (!Aes.EncryptBlock(Library::Span<const Foundation::Byte>{ Zero, 16 }, Library::Span<Foundation::Byte>{ H, 16 }))
        {
            return false;
        }

        Foundation::Byte J0[16]{};

        for (Foundation::Size Index{}; Index < IvSize; ++Index)
        {
            J0[Index] = Iv[Index];
        }

        J0[15] = 1;

        Foundation::Byte Counter[16]{};

        for (Foundation::Size Index{}; Index < 16; ++Index)
        {
            Counter[Index] = J0[Index];
        }

        Foundation::Size Offset{};

        while (Offset < Ciphertext.Size())
        {
            Increment32(Counter);

            Foundation::Byte Stream[16]{};

            if (!Aes.EncryptBlock(Library::Span<const Foundation::Byte>{ Counter, 16 }, Library::Span<Foundation::Byte>{ Stream, 16 }))
            {
                return false;
            }

            const auto Remaining = Ciphertext.Size() - Offset;
            const auto BlockSize = Remaining < 16 ? Remaining : 16;

            for (Foundation::Size Index{}; Index < BlockSize; ++Index)
            {
                Plaintext[Offset + Index] =
                    Foundation::Cast::Auto<Foundation::Byte>(Ciphertext[Offset + Index] ^ Stream[Index]);
            }

            Offset += BlockSize;
        }

        Foundation::Byte S[16]{};

        if (!GHash(Library::Span<const Foundation::Byte>{ H, 16 }, Aad, Ciphertext, Library::Span<Foundation::Byte>{ S, 16 }))
        {
            return false;
        }

        Foundation::Byte EJ0[16]{};

        if (!Aes.EncryptBlock(Library::Span<const Foundation::Byte>{ J0, 16 }, Library::Span<Foundation::Byte>{ EJ0, 16 }))
        {
            return false;
        }

        Foundation::Byte ComputedTag[16]{};

        for (Foundation::Size Index{}; Index < 16; ++Index)
        {
            ComputedTag[Index] =
                Foundation::Cast::Auto<Foundation::Byte>(EJ0[Index] ^ S[Index]);
        }

        return ConstantTimeEqual(Library::Span<const Foundation::Byte>{ ComputedTag, 16 }, Tag);
    }

    auto AesGcm::Encrypt(Library::Span<const Foundation::Byte> Key, Library::Span<const Foundation::Byte> Iv, Library::Span<const Foundation::Byte> Aad, Library::Span<const Foundation::Byte> Plaintext, Library::Span<Foundation::Byte> Ciphertext, Library::Span<Foundation::Byte> Tag) -> Foundation::Bool
    {
        if (Key.Size() != Aes128KeySize || Iv.Size() != IvSize || Ciphertext.Size() != Plaintext.Size() || Tag.Size() != TagSize)
        {
            return false;
        }

        Aes128 Aes{ Key };

        Foundation::Byte H[16]{};
        Foundation::Byte Zero[16]{};

        if (!Aes.EncryptBlock(Library::Span<const Foundation::Byte>{ Zero, 16 }, Library::Span<Foundation::Byte>{ H, 16 }))
        {
            return false;
        }

        Foundation::Byte J0[16]{};

        for (Foundation::Size Index{}; Index < IvSize; ++Index)
        {
            J0[Index] = Iv[Index];
        }

        J0[15] = 1;

        Foundation::Byte Counter[16]{};

        for (Foundation::Size Index{}; Index < 16; ++Index)
        {
            Counter[Index] = J0[Index];
        }

        Increment32(Counter);

        for (Foundation::Size Offset{}; Offset < Plaintext.Size(); Offset += 16)
        {
            Foundation::Byte Stream[16]{};

            if (!Aes.EncryptBlock(Library::Span<const Foundation::Byte>{ Counter, 16 }, Library::Span<Foundation::Byte>{ Stream, 16 }))
            {
                return false;
            }

            const auto BlockSize = Plaintext.Size() - Offset < 16 ? Plaintext.Size() - Offset : 16;

            for (Foundation::Size Index{}; Index < BlockSize; ++Index)
            {
                Ciphertext[Offset + Index] = Foundation::Cast::Auto<Foundation::Byte>(Plaintext[Offset + Index] ^ Stream[Index]);
            }

            Increment32(Counter);
        }

        Foundation::Byte S[16]{};
        GHash(Library::Span<const Foundation::Byte>{ H, 16 }, Aad, Library::Span<const Foundation::Byte>{ Ciphertext.Data(), Ciphertext.Size() }, Library::Span<Foundation::Byte>{ S, 16 });
        Foundation::Byte E0[16]{};

        if (!Aes.EncryptBlock(Library::Span<const Foundation::Byte>{ J0, 16 }, Library::Span<Foundation::Byte>{ E0, 16 }))
        {
            return false;
        }

        for (Foundation::Size Index{}; Index < TagSize; ++Index)
        {
            Tag[Index] = E0[Index] ^ S[Index];
        }

        return true;
    }
}