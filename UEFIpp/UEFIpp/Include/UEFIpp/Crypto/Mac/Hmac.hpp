#pragma once

#include <UEFIpp/Crypto/SecureZero.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Array.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>

namespace UEFIpp::Crypto::Mac
{
	template<typename THash>
	class Hmac
	{
	public:
		using HashType = THash;
		using Byte = Foundation::Uint8;
		using SizeType = Foundation::Size;
		using ByteSpan = Library::Span<const Byte>;
		using MutableByteSpan = Library::Span<Byte>;
		using DigestType = typename HashType::DigestType;

		static constexpr auto BlockSize = HashType::BlockSize;
		static constexpr auto DigestSize = HashType::DigestSize;

	public:
		Hmac() = default;

		explicit Hmac(ByteSpan Key)
		{
			Reset(Key);
		}

		~Hmac()
		{
			Clear();
		}

		auto Reset(ByteSpan Key) -> Foundation::Void
		{
			ClearPads();

			Library::Array<Byte, DigestSize> KeyDigest{};
			ByteSpan EffectiveKey{ Key };

			if (Key.Size() > BlockSize)
			{
				HashType Hash{};
				Hash.Update(Key);
				Hash.Final(MutableByteSpan{ KeyDigest.Data(), KeyDigest.Size() });

				EffectiveKey = ByteSpan{ KeyDigest.Data(), KeyDigest.Size() };
			}

			InnerPad_.Fill(0x36);
			OuterPad_.Fill(0x5C);

			for (SizeType i = 0; i < EffectiveKey.Size(); ++i)
			{
				InnerPad_[i] = static_cast<Byte>(InnerPad_[i] ^ EffectiveKey[i]);
				OuterPad_[i] = static_cast<Byte>(OuterPad_[i] ^ EffectiveKey[i]);
			}

			Inner_.Reset();
			Inner_.Update(ByteSpan{ InnerPad_.Data(), InnerPad_.Size() });

			Initialized_ = true;

			Crypto::SecureZero::Memory(KeyDigest.Data(), KeyDigest.SizeInBytes());
		}

		auto Update(ByteSpan Data) -> Foundation::Void
		{
			if (!Initialized_ || Finalized_ || Data.Empty())
			{
				return;
			}

			Inner_.Update(Data);
		}

		[[nodiscard]] auto Final(MutableByteSpan Digest) -> Foundation::Bool
		{
			if (!Initialized_ || Digest.Size() < DigestSize)
			{
				return false;
			}

			if (!Finalized_)
			{
				Library::Array<Byte, DigestSize> InnerDigest{};

				Inner_.Final(MutableByteSpan{ InnerDigest.Data(), InnerDigest.Size() });

				HashType Outer{};
				Outer.Update(ByteSpan{ OuterPad_.Data(), OuterPad_.Size() });
				Outer.Update(ByteSpan{ InnerDigest.Data(), InnerDigest.Size() });
				Outer.Final(MutableByteSpan{ Digest_.Data(), Digest_.Size() });

				Crypto::SecureZero::Memory(InnerDigest.Data(), InnerDigest.SizeInBytes());

				Finalized_ = true;
			}

			CopyBytes(Digest.Data(), Digest_.Data(), DigestSize);
			return true;
		}

		[[nodiscard]] auto Final() -> DigestType
		{
			DigestType Digest{};
			Final(Digest.View());
			return Digest;
		}

		[[nodiscard]] static auto Compute(ByteSpan Key, ByteSpan Data, MutableByteSpan Digest) -> Foundation::Bool
		{
			Hmac Context{ Key };
			Context.Update(Data);
			return Context.Final(Digest);
		}

		[[nodiscard]] static auto Compute(ByteSpan Key, ByteSpan Data) -> DigestType
		{
			DigestType Digest{};
			Compute(Key, Data, Digest.View());
			return Digest;
		}

		auto Clear() -> Foundation::Void
		{
			Inner_ = HashType{};
			ClearPads();
			Crypto::SecureZero::Memory(Digest_.Data(), Digest_.SizeInBytes());
			Initialized_ = false;
			Finalized_ = false;
		}

	private:
		static constexpr auto CopyBytes(Byte* Destination, const Byte* Source, SizeType Size) -> Foundation::Void
		{
			for (SizeType i = 0; i < Size; ++i)
			{
				Destination[i] = Source[i];
			}
		}

		auto ClearPads() -> Foundation::Void
		{
			Crypto::SecureZero::Memory(InnerPad_.Data(), InnerPad_.SizeInBytes());
			Crypto::SecureZero::Memory(OuterPad_.Data(), OuterPad_.SizeInBytes());
			Finalized_ = false;
		}

	private:
		HashType Inner_{};
		Library::Array<Byte, BlockSize> InnerPad_{};
		Library::Array<Byte, BlockSize> OuterPad_{};
		Library::Array<Byte, DigestSize> Digest_{};
		Foundation::Bool Initialized_{};
		Foundation::Bool Finalized_{};
	};
}