#pragma once

#include <UEFIpp/Crypto/Mac/HmacSha256.hpp>
#include <UEFIpp/Crypto/SecureZero.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Array.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>

namespace UEFIpp::Crypto::Kdf
{
	class Pbkdf2HmacSha256
	{
	public:
		using Byte = Foundation::Uint8;
		using SizeType = Foundation::Size;
		using ByteSpan = Library::Span<const Byte>;
		using MutableByteSpan = Library::Span<Byte>;
		using MacType = Mac::HmacSha256;

		static constexpr auto DigestSize = MacType::DigestSize;

	public:
		Pbkdf2HmacSha256() = delete;

		[[nodiscard]] static auto Derive(ByteSpan Password, ByteSpan Salt, Foundation::Uint32 Iterations, MutableByteSpan Output) -> Foundation::Bool
		{
			if (Iterations == 0 || Output.Empty())
			{
				return false;
			}

			const auto BlockCount = (Output.Size() + DigestSize - SizeType{ 1 }) / DigestSize;

			if (BlockCount > static_cast<SizeType>(0xFFFFFFFFULL))
			{
				return false;
			}

			Library::Array<Byte, DigestSize> U{};
			Library::Array<Byte, DigestSize> T{};
			Byte Counter[4]{};

			SizeType Written{};

			for (SizeType BlockIndex = 1; BlockIndex <= BlockCount; ++BlockIndex)
			{
				StoreBig32(static_cast<Foundation::Uint32>(BlockIndex), Counter);

				MacType Mac{ Password };
				Mac.Update(Salt);
				Mac.Update(ByteSpan{ Counter, sizeof(Counter) });

				if (!Mac.Final(MutableByteSpan{ U.Data(), U.Size() }))
				{
					Clear(U, T, Counter);
					return false;
				}

				CopyBytes(T.Data(), U.Data(), DigestSize);

				for (Foundation::Uint32 Iteration = 1; Iteration < Iterations; ++Iteration)
				{
					if (!MacType::Compute(Password, ByteSpan{ U.Data(), U.Size() }, MutableByteSpan{ U.Data(), U.Size() }))
					{
						Clear(U, T, Counter);
						return false;
					}

					XorBytes(T.Data(), U.Data(), DigestSize);
				}

				const auto Remaining = Output.Size() - Written;
				const auto Count = Remaining < DigestSize ? Remaining : DigestSize;

				CopyBytes(Output.Data() + Written, T.Data(), Count);
				Written += Count;
			}

			Clear(U, T, Counter);
			return true;
		}

	private:
		static constexpr auto StoreBig32(Foundation::Uint32 Value, Byte* Data) -> Foundation::Void
		{
			Data[0] = Foundation::Cast::Auto<Byte>(Value >> 24);
			Data[1] = Foundation::Cast::Auto<Byte>(Value >> 16);
			Data[2] = Foundation::Cast::Auto<Byte>(Value >> 8);
			Data[3] = Foundation::Cast::Auto<Byte>(Value);
		}

		static constexpr auto CopyBytes(Byte* Destination, const Byte* Source, SizeType Size) -> Foundation::Void
		{
			for (SizeType i = 0; i < Size; ++i)
			{
				Destination[i] = Source[i];
			}
		}

		static constexpr auto XorBytes(Byte* Destination, const Byte* Source, SizeType Size) -> Foundation::Void
		{
			for (SizeType i = 0; i < Size; ++i)
			{
				Destination[i] = static_cast<Byte>(Destination[i] ^ Source[i]);
			}
		}

		static auto Clear(Library::Array<Byte, DigestSize>& U, Library::Array<Byte, DigestSize>& T, Byte(&Counter)[4]) -> Foundation::Void
		{
			Crypto::SecureZero::Memory(U.Data(), U.SizeInBytes());
			Crypto::SecureZero::Memory(T.Data(), T.SizeInBytes());
			Crypto::SecureZero::Memory(Counter, sizeof(Counter));
		}
	};

	using Pbkdf2 = Pbkdf2HmacSha256;
}