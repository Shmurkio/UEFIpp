#include <UEFIpp/Crypto/PublicKey/X25519.hpp>

namespace UEFIpp::Crypto::PublicKey
{
    namespace
    {
        using FieldElement = Foundation::Int64[16];

        constexpr Foundation::Byte BasePoint[32]
        {
            9
        };

        const FieldElement D121665
        {
            0xDB41, 1
        };

        static auto Copy(FieldElement Out, const FieldElement In) -> Foundation::Void
        {
            for (Foundation::Size Index{}; Index < 16; ++Index)
            {
                Out[Index] = In[Index];
            }
        }

        static auto ConditionalSwap(FieldElement A, FieldElement B, Foundation::Int64 Swap) -> Foundation::Void
        {
            Swap = -Swap;

            for (Foundation::Size Index{}; Index < 16; ++Index)
            {
                const auto T = Swap & (A[Index] ^ B[Index]);
                A[Index] ^= T;
                B[Index] ^= T;
            }
        }

        static auto Carry(FieldElement Out) -> Foundation::Void
        {
            for (Foundation::Size Pass{}; Pass < 2; ++Pass)
            {
                for (Foundation::Size Index{}; Index < 16; ++Index)
                {
                    Out[Index] += 1LL << 16;

                    const auto CarryValue = Out[Index] >> 16;

                    if (Index < 15)
                    {
                        Out[Index + 1] += CarryValue - 1;
                    }
                    else
                    {
                        Out[0] += 38 * (CarryValue - 1);
                    }

                    Out[Index] -= CarryValue << 16;
                }
            }
        }

        static auto Add(FieldElement Out, const FieldElement A, const FieldElement B) -> Foundation::Void
        {
            for (Foundation::Size Index{}; Index < 16; ++Index)
            {
                Out[Index] = A[Index] + B[Index];
            }
        }

        static auto Subtract(FieldElement Out, const FieldElement A, const FieldElement B) -> Foundation::Void
        {
            for (Foundation::Size Index{}; Index < 16; ++Index)
            {
                Out[Index] = A[Index] - B[Index];
            }
        }

        static auto Multiply(FieldElement Out, const FieldElement A, const FieldElement B) -> Foundation::Void
        {
            FieldElement Product{};

            for (Foundation::Size Index{}; Index < 16; ++Index)
            {
                for (Foundation::Size Inner{}; Inner < 16; ++Inner)
                {
                    Product[Index] += A[Inner] * B[Index - Inner & 15] * (Index - Inner & 16 ? 38 : 1);
                }
            }

            Copy(Out, Product);
            Carry(Out);
            Carry(Out);
        }

        static auto Square(FieldElement Out, const FieldElement A) -> Foundation::Void
        {
            Multiply(Out, A, A);
        }

        static auto Unpack(FieldElement Out, const Foundation::Byte* In) -> Foundation::Void
        {
            for (Foundation::Size Index{}; Index < 16; ++Index)
            {
                Out[Index] = Foundation::Bit::FromByte<Foundation::Int64>(In[2 * Index], 0) | Foundation::Bit::FromByte<Foundation::Int64>(In[2 * Index + 1], 1);
            }

            Out[15] &= 0x7FFF;
        }

        static auto Pack(Foundation::Byte* Out, const FieldElement In) -> Foundation::Void
        {
            FieldElement T{};
            FieldElement M{};

            Copy(T, In);
            Carry(T);
            Carry(T);
            Carry(T);

            for (Foundation::Size Pass{}; Pass < 2; ++Pass)
            {
                M[0] = T[0] - 0xFFED;

                for (Foundation::Size Index{ 1 }; Index < 15; ++Index)
                {
                    M[Index] = T[Index] - 0xFFFF - ((M[Index - 1] >> 16) & 1);
                    M[Index - 1] &= 0xFFFF;
                }

                M[15] = T[15] - 0x7FFF - ((M[14] >> 16) & 1);
                const auto CarryValue = (M[15] >> 16) & 1;
                M[14] &= 0xFFFF;
                ConditionalSwap(T, M, 1 - CarryValue);
            }

            for (Foundation::Size Index{}; Index < 16; ++Index)
            {
                Out[2 * Index] = Foundation::Bit::Byte(T[Index], 0);
                Out[2 * Index + 1] = Foundation::Bit::Byte(T[Index], 1);
            }
        }

        static auto Invert(FieldElement Out, const FieldElement In) -> Foundation::Void
        {
            FieldElement C{};

            Copy(C, In);

            for (Foundation::Int32 Index{ 253 }; Index >= 0; --Index)
            {
                Square(C, C);

                if (Index != 2 && Index != 4)
                {
                    Multiply(C, C, In);
                }
            }

            Copy(Out, C);
        }
    }

    auto X25519::ClampPrivateKey(Library::Span<Foundation::Byte> PrivateKey) -> Foundation::Void
    {
        if (PrivateKey.Size() != KeySize)
        {
            return;
        }

        PrivateKey[0] &= 248;
        PrivateKey[31] &= 127;
        PrivateKey[31] |= 64;
    }

    auto X25519::GeneratePrivateKey(Library::Span<const Foundation::Byte> Random, Library::Span<Foundation::Byte> PrivateKey) -> Foundation::Bool
    {
        if (Random.Size() != KeySize || PrivateKey.Size() != KeySize)
        {
            return false;
        }

        for (Foundation::Size Index{}; Index < KeySize; ++Index)
        {
            PrivateKey[Index] = Random[Index];
        }

        ClampPrivateKey(PrivateKey);

        return true;
    }

    auto X25519::ComputePublicKey(Library::Span<const Foundation::Byte> PrivateKey, Library::Span<Foundation::Byte> PublicKey) -> Foundation::Bool
    {
        if (PrivateKey.Size() != KeySize || PublicKey.Size() != KeySize)
        {
            return false;
        }

        return ScalarMult(PrivateKey.Data(), BasePoint, PublicKey.Data());
    }

    auto X25519::ComputeSharedSecret(Library::Span<const Foundation::Byte> PrivateKey, Library::Span<const Foundation::Byte> PeerPublicKey, Library::Span<Foundation::Byte> SharedSecret) -> Foundation::Bool
    {
        if (PrivateKey.Size() != KeySize || PeerPublicKey.Size() != KeySize || SharedSecret.Size() != KeySize)
        {
            return false;
        }

        return ScalarMult(PrivateKey.Data(), PeerPublicKey.Data(), SharedSecret.Data());
    }

    auto X25519::ScalarMult(const Foundation::Byte* Scalar, const Foundation::Byte* Point, Foundation::Byte* Result) -> Foundation::Bool
    {
        if (!Scalar || !Point || !Result)
        {
            return false;
        }

        Foundation::Byte ClampedScalar[32]{};

        for (Foundation::Size Index{}; Index < KeySize; ++Index)
        {
            ClampedScalar[Index] = Scalar[Index];
        }

        ClampedScalar[0] &= 248;
        ClampedScalar[31] &= 127;
        ClampedScalar[31] |= 64;

        FieldElement X{};
        FieldElement A{};
        FieldElement B{};
        FieldElement C{};
        FieldElement D{};
        FieldElement E{};
        FieldElement F{};

        Unpack(X, Point);

        for (Foundation::Size Index{}; Index < 16; ++Index)
        {
            B[Index] = X[Index];
            D[Index] = Index == 0 ? 1 : 0;
            A[Index] = Index == 0 ? 1 : 0;
            C[Index] = 0;
        }

        for (Foundation::Int32 Index{ 254 }; Index >= 0; --Index)
        {
            const auto Bit = Foundation::Cast::To<Foundation::Int64>((ClampedScalar[Index >> 3] >> (Index & 7)) & 1);

            ConditionalSwap(A, B, Bit);
            ConditionalSwap(C, D, Bit);

            Add(E, A, C);
            Subtract(A, A, C);
            Add(C, B, D);
            Subtract(B, B, D);

            Square(D, E);
            Square(F, A);
            Multiply(A, C, A);
            Multiply(C, B, E);
            Add(E, A, C);
            Subtract(A, A, C);
            Square(B, A);
            Subtract(C, D, F);
            Multiply(A, C, D121665);
            Add(A, A, D);
            Multiply(C, C, A);
            Multiply(A, D, F);
            Multiply(D, B, X);
            Square(B, E);

            ConditionalSwap(A, B, Bit);
            ConditionalSwap(C, D, Bit);
        }

        Invert(C, C);
        Multiply(A, A, C);
        Pack(Result, A);

        return true;
    }
}