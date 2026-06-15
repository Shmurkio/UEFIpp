#pragma once

#include <UEFIpp/Crypto/Mac/HmacSha256.hpp>
#include <UEFIpp/Crypto/SecureZero.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Array.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>

namespace UEFIpp::Crypto::Kdf
{
	class HkdfSha256
	{
	public:
		using Byte = Foundation::Uint8;
		using SizeType = Foundation::Size;
		using ByteSpan = Library::Span<const Byte>;
		using MutableByteSpan = Library::Span<Byte>;
		using MacType = Mac::HmacSha256;

		static constexpr auto DigestSize = MacType::DigestSize;
		static constexpr auto MaxOutputSize = DigestSize * SizeType{ 255 };

	public:
		HkdfSha256() = delete;

		[[nodiscard]] static auto Extract(ByteSpan Salt, ByteSpan InputKeyMaterial, MutableByteSpan PseudoRandomKey) -> Foundation::Bool
		{
			if (PseudoRandomKey.Size() < DigestSize)
			{
				return false;
			}

			if (!Salt.Empty())
			{
				return MacType::Compute(Salt, InputKeyMaterial, PseudoRandomKey);
			}

			Library::Array<Byte, DigestSize> ZeroSalt{};

			const auto Success = MacType::Compute(ByteSpan{ ZeroSalt.Data(), ZeroSalt.Size() }, InputKeyMaterial, PseudoRandomKey);

			Crypto::SecureZero::Memory(ZeroSalt.Data(), ZeroSalt.SizeInBytes());
			return Success;
		}

		[[nodiscard]] static auto Expand(ByteSpan PseudoRandomKey, ByteSpan Info, MutableByteSpan OutputKeyMaterial) -> Foundation::Bool
		{
			if (PseudoRandomKey.Size() < DigestSize || OutputKeyMaterial.Size() > MaxOutputSize)
			{
				return false;
			}

			if (OutputKeyMaterial.Empty())
			{
				return true;
			}

			Library::Array<Byte, DigestSize> Previous{};
			Library::Array<Byte, DigestSize> Current{};

			SizeType PreviousSize{};
			SizeType Written{};
			Byte Counter{ 1 };

			while (Written < OutputKeyMaterial.Size())
			{
				MacType Mac{ PseudoRandomKey };

				if (PreviousSize != 0)
				{
					Mac.Update(ByteSpan{ Previous.Data(), PreviousSize });
				}

				Mac.Update(Info);
				Mac.Update(ByteSpan{ &Counter, 1 });

				if (!Mac.Final(MutableByteSpan{ Current.Data(), Current.Size() }))
				{
					Clear(Previous, Current, Counter);
					return false;
				}

				CopyBytes(Previous.Data(), Current.Data(), DigestSize);
				PreviousSize = DigestSize;

				const auto Remaining = OutputKeyMaterial.Size() - Written;
				const auto Count = Remaining < DigestSize ? Remaining : DigestSize;

				CopyBytes(OutputKeyMaterial.Data() + Written, Current.Data(), Count);
				Written += Count;
				++Counter;
			}

			Clear(Previous, Current, Counter);

			return true;
		}

		[[nodiscard]] static auto Derive(ByteSpan Salt, ByteSpan InputKeyMaterial, ByteSpan Info, MutableByteSpan OutputKeyMaterial) -> Foundation::Bool
		{
			Library::Array<Byte, DigestSize> PseudoRandomKey{};

			if (!Extract(Salt, InputKeyMaterial, MutableByteSpan{ PseudoRandomKey.Data(), PseudoRandomKey.Size() }))
			{
				Crypto::SecureZero::Memory(PseudoRandomKey.Data(), PseudoRandomKey.SizeInBytes());
				return false;
			}

			const auto Success = Expand(ByteSpan{ PseudoRandomKey.Data(), PseudoRandomKey.Size() }, Info, OutputKeyMaterial);
			Crypto::SecureZero::Memory(PseudoRandomKey.Data(), PseudoRandomKey.SizeInBytes());

			return Success;
		}

	private:
		static constexpr auto CopyBytes(Byte* Destination, const Byte* Source, SizeType Size) -> Foundation::Void
		{
			for (SizeType i = 0; i < Size; ++i)
			{
				Destination[i] = Source[i];
			}
		}

		static auto Clear(Library::Array<Byte, DigestSize>& Previous, Library::Array<Byte, DigestSize>& Current, Byte& Counter) -> Foundation::Void
		{
			Crypto::SecureZero::Memory(Previous.Data(), Previous.SizeInBytes());
			Crypto::SecureZero::Memory(Current.Data(), Current.SizeInBytes());
			Counter = 0;
		}
	};

	using Hkdf = HkdfSha256;
}