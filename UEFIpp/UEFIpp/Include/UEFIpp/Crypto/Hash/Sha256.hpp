#pragma once

#include <UEFIpp/Crypto/Hash/Hash.hpp>
#include <UEFIpp/Crypto/SecureZero.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Array.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>

namespace UEFIpp::Crypto::Hash
{
	class Sha256
	{
	public:
		using Byte = Foundation::Uint8;
		using Word = Foundation::Uint32;
		using SizeType = Foundation::Size;
		using ByteSpan = Library::Span<const Byte>;
		using MutableByteSpan = Library::Span<Byte>;
		using DigestType = Sha256Digest;

		static constexpr auto BlockSize = SizeType{ 64 };
		static constexpr auto DigestSize = SizeType{ 32 };
		static constexpr auto AlgorithmId = Algorithm::Sha256;

	public:
		Sha256()
		{
			Reset();
		}

		auto Reset() -> Foundation::Void
		{
			State_[0] = 0x6A09E667U;
			State_[1] = 0xBB67AE85U;
			State_[2] = 0x3C6EF372U;
			State_[3] = 0xA54FF53AU;
			State_[4] = 0x510E527FU;
			State_[5] = 0x9B05688CU;
			State_[6] = 0x1F83D9ABU;
			State_[7] = 0x5BE0CD19U;

			BitCount_ = 0;
			BufferSize_ = 0;
			Finalized_ = false;
			Buffer_.Fill(0);
		}

		auto Update(ByteSpan Data) -> Foundation::Void
		{
			if (Finalized_ || Data.Empty())
			{
				return;
			}

			BitCount_ += Foundation::Cast::Auto<Foundation::Uint64>(Data.Size()) * 8ULL;

			SizeType Offset{};

			if (BufferSize_ != 0)
			{
				const auto Space = BlockSize - BufferSize_;
				const auto Count = Data.Size() < Space ? Data.Size() : Space;

				CopyBytes(Buffer_.Data() + BufferSize_, Data.Data(), Count);
				BufferSize_ += Count;
				Offset += Count;

				if (BufferSize_ == BlockSize)
				{
					Transform(Buffer_.Data());
					BufferSize_ = 0;
				}
			}

			while (Offset + BlockSize <= Data.Size())
			{
				Transform(Data.Data() + Offset);
				Offset += BlockSize;
			}

			if (Offset < Data.Size())
			{
				BufferSize_ = Data.Size() - Offset;
				CopyBytes(Buffer_.Data(), Data.Data() + Offset, BufferSize_);
			}
		}

		[[nodiscard]] auto Final(MutableByteSpan Digest) -> Foundation::Bool
		{
			if (Digest.Size() < DigestSize)
			{
				return false;
			}

			if (!Finalized_)
			{
				FinalizeInternal();
			}

			for (SizeType i = 0; i < 8; ++i)
			{
				StoreBig32(State_[i], Digest.Data() + (i * 4));
			}

			return true;
		}

		[[nodiscard]] auto Final() -> DigestType
		{
			DigestType Digest{};
			Final(Digest.View());
			return Digest;
		}

		[[nodiscard]] static auto Compute(ByteSpan Data, MutableByteSpan Digest) -> Foundation::Bool
		{
			Sha256 Context{};
			Context.Update(Data);
			return Context.Final(Digest);
		}

		[[nodiscard]] static auto Compute(ByteSpan Data) -> DigestType
		{
			DigestType Digest{};
			Compute(Data, Digest.View());
			return Digest;
		}

	private:
		static constexpr Word K_[64]
		{
			0x428A2F98U, 0x71374491U, 0xB5C0FBCFU, 0xE9B5DBA5U,
			0x3956C25BU, 0x59F111F1U, 0x923F82A4U, 0xAB1C5ED5U,
			0xD807AA98U, 0x12835B01U, 0x243185BEU, 0x550C7DC3U,
			0x72BE5D74U, 0x80DEB1FEU, 0x9BDC06A7U, 0xC19BF174U,
			0xE49B69C1U, 0xEFBE4786U, 0x0FC19DC6U, 0x240CA1CCU,
			0x2DE92C6FU, 0x4A7484AAU, 0x5CB0A9DCU, 0x76F988DAU,
			0x983E5152U, 0xA831C66DU, 0xB00327C8U, 0xBF597FC7U,
			0xC6E00BF3U, 0xD5A79147U, 0x06CA6351U, 0x14292967U,
			0x27B70A85U, 0x2E1B2138U, 0x4D2C6DFCU, 0x53380D13U,
			0x650A7354U, 0x766A0ABBU, 0x81C2C92EU, 0x92722C85U,
			0xA2BFE8A1U, 0xA81A664BU, 0xC24B8B70U, 0xC76C51A3U,
			0xD192E819U, 0xD6990624U, 0xF40E3585U, 0x106AA070U,
			0x19A4C116U, 0x1E376C08U, 0x2748774CU, 0x34B0BCB5U,
			0x391C0CB3U, 0x4ED8AA4AU, 0x5B9CCA4FU, 0x682E6FF3U,
			0x748F82EEU, 0x78A5636FU, 0x84C87814U, 0x8CC70208U,
			0x90BEFFFAU, 0xA4506CEBU, 0xBEF9A3F7U, 0xC67178F2U,
		};

		[[nodiscard]] static constexpr auto Choose(Word X, Word Y, Word Z) -> Word
		{
			return (X & Y) ^ (~X & Z);
		}

		[[nodiscard]] static constexpr auto Majority(Word X, Word Y, Word Z) -> Word
		{
			return (X & Y) ^ (X & Z) ^ (Y & Z);
		}

		[[nodiscard]] static constexpr auto BigSigma0(Word X) -> Word
		{
			return Foundation::Bit::RotateRight(X, 2) ^ Foundation::Bit::RotateRight(X, 13) ^ Foundation::Bit::RotateRight(X, 22);
		}

		[[nodiscard]] static constexpr auto BigSigma1(Word X) -> Word
		{
			return Foundation::Bit::RotateRight(X, 6) ^ Foundation::Bit::RotateRight(X, 11) ^ Foundation::Bit::RotateRight(X, 25);
		}

		[[nodiscard]] static constexpr auto SmallSigma0(Word X) -> Word
		{
			return Foundation::Bit::RotateRight(X, 7) ^ Foundation::Bit::RotateRight(X, 18) ^ (X >> 3);
		}

		[[nodiscard]] static constexpr auto SmallSigma1(Word X) -> Word
		{
			return Foundation::Bit::RotateRight(X, 17) ^ Foundation::Bit::RotateRight(X, 19) ^ (X >> 10);
		}

		[[nodiscard]] static constexpr auto LoadBig32(const Byte* Data) -> Word
		{
			return (Foundation::Cast::Auto<Word>(Data[0]) << 24) | (Foundation::Cast::Auto<Word>(Data[1]) << 16) | (Foundation::Cast::Auto<Word>(Data[2]) << 8) | Foundation::Cast::Auto<Word>(Data[3]);
		}

		static constexpr auto StoreBig32(Word Value, Byte* Data) -> Foundation::Void
		{
			Data[0] = Foundation::Cast::Auto<Byte>(Value >> 24);
			Data[1] = Foundation::Cast::Auto<Byte>(Value >> 16);
			Data[2] = Foundation::Cast::Auto<Byte>(Value >> 8);
			Data[3] = Foundation::Cast::Auto<Byte>(Value);
		}

		static constexpr auto StoreBig64(Foundation::Uint64 Value, Byte* Data) -> Foundation::Void
		{
			Data[0] = Foundation::Cast::Auto<Byte>(Value >> 56);
			Data[1] = Foundation::Cast::Auto<Byte>(Value >> 48);
			Data[2] = Foundation::Cast::Auto<Byte>(Value >> 40);
			Data[3] = Foundation::Cast::Auto<Byte>(Value >> 32);
			Data[4] = Foundation::Cast::Auto<Byte>(Value >> 24);
			Data[5] = Foundation::Cast::Auto<Byte>(Value >> 16);
			Data[6] = Foundation::Cast::Auto<Byte>(Value >> 8);
			Data[7] = Foundation::Cast::Auto<Byte>(Value);
		}

		static constexpr auto CopyBytes(Byte* Destination, const Byte* Source, SizeType Size) -> Foundation::Void
		{
			for (SizeType i = 0; i < Size; ++i)
			{
				Destination[i] = Source[i];
			}
		}

		auto Transform(const Byte* Block) -> Foundation::Void
		{
			Word W[64]{};

			for (SizeType i = 0; i < 16; ++i)
			{
				W[i] = LoadBig32(Block + (i * 4));
			}

			for (SizeType i = 16; i < 64; ++i)
			{
				W[i] = SmallSigma1(W[i - 2]) + W[i - 7] + SmallSigma0(W[i - 15]) + W[i - 16];
			}

			auto A = State_[0];
			auto B = State_[1];
			auto C = State_[2];
			auto D = State_[3];
			auto E = State_[4];
			auto F = State_[5];
			auto G = State_[6];
			auto H = State_[7];

			for (SizeType i = 0; i < 64; ++i)
			{
				const auto T1 = H + BigSigma1(E) + Choose(E, F, G) + K_[i] + W[i];
				const auto T2 = BigSigma0(A) + Majority(A, B, C);

				H = G;
				G = F;
				F = E;
				E = D + T1;
				D = C;
				C = B;
				B = A;
				A = T1 + T2;
			}

			State_[0] += A;
			State_[1] += B;
			State_[2] += C;
			State_[3] += D;
			State_[4] += E;
			State_[5] += F;
			State_[6] += G;
			State_[7] += H;

			Crypto::SecureZero::Memory(W, sizeof(W));
		}

		auto FinalizeInternal() -> Foundation::Void
		{
			Buffer_[BufferSize_++] = 0x80;

			if (BufferSize_ > 56)
			{
				while (BufferSize_ < BlockSize)
				{
					Buffer_[BufferSize_++] = 0;
				}

				Transform(Buffer_.Data());
				BufferSize_ = 0;
			}

			while (BufferSize_ < 56)
			{
				Buffer_[BufferSize_++] = 0;
			}

			StoreBig64(BitCount_, Buffer_.Data() + 56);
			Transform(Buffer_.Data());

			BufferSize_ = 0;
			Finalized_ = true;
			Crypto::SecureZero::Memory(Buffer_.Data(), Buffer_.SizeInBytes());
		}

	private:
		Library::Array<Word, 8> State_{};
		Library::Array<Byte, BlockSize> Buffer_{};
		Foundation::Uint64 BitCount_{};
		SizeType BufferSize_{};
		Foundation::Bool Finalized_{};
	};
}