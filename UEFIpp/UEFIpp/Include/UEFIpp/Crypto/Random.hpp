#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Memory/Memory.hpp>
#include <UEFIpp/Architecture/X64/Cpu/Random.hpp>

namespace UEFIpp::Crypto
{
	enum class RandomSource : Foundation::Uint8
	{
		RdRand,
		RdSeed,
		PreferRdSeed,
		PreferRdRand,
	};

	class Random
	{
	public:
		using Byte = Foundation::Uint8;
		using ByteSpan = Library::Span<Byte>;

		static constexpr Foundation::Uint32 DefaultRetries = 10;

	public:
		constexpr Random() = default;

		constexpr explicit Random(RandomSource Source) : Source_(Source)
		{
		}

		auto Fill(ByteSpan Buffer) const -> Foundation::Bool
		{
			switch (Source_)
			{
			case RandomSource::RdRand: return FillRdRand(Buffer);
			case RandomSource::RdSeed: return FillRdSeed(Buffer);
			case RandomSource::PreferRdSeed:
			{
				if (FillRdSeed(Buffer))
				{
					return true;
				}

				return FillRdRand(Buffer);
			}
			case RandomSource::PreferRdRand:
			{
				if (FillRdRand(Buffer))
				{
					return true;
				}

				return FillRdSeed(Buffer);
			}
			default: return false;
			}
		}

		auto Fill(Byte* Data, Foundation::Size Size) const -> Foundation::Bool
		{
			return Fill(ByteSpan{ Data, Size });
		}

		auto FillRdRand(ByteSpan Buffer) const -> Foundation::Bool
		{
			auto Offset = Foundation::Size{};

			while (Offset + sizeof(Foundation::Uint64) <= Buffer.Size())
			{
				Foundation::Uint64 Value{};

				if (!RdRand64(Value))
				{
					return false;
				}

				Memory::Copy(Buffer.Data() + Offset, &Value, sizeof(Value));
				Offset += sizeof(Value);
			}

			if (Offset < Buffer.Size())
			{
				Foundation::Uint64 Value{};

				if (!RdRand64(Value))
				{
					return false;
				}

				Memory::Copy(Buffer.Data() + Offset, &Value, Buffer.Size() - Offset);
			}

			return true;
		}

		auto FillRdSeed(ByteSpan Buffer) const -> Foundation::Bool
		{
			auto Offset = Foundation::Size{};

			while (Offset + sizeof(Foundation::Uint64) <= Buffer.Size())
			{
				Foundation::Uint64 Value{};

				if (!RdSeed64(Value))
				{
					return false;
				}

				Memory::Copy(Buffer.Data() + Offset, &Value, sizeof(Value));
				Offset += sizeof(Value);
			}

			if (Offset < Buffer.Size())
			{
				Foundation::Uint64 Value{};

				if (!RdSeed64(Value))
				{
					return false;
				}

				Memory::Copy(Buffer.Data() + Offset, &Value, Buffer.Size() - Offset);
			}

			return true;
		}

		[[nodiscard]] static auto RdRand16(Foundation::Uint16& Value) -> Foundation::Bool
		{
			for (Foundation::Uint32 Attempt{}; Attempt < DefaultRetries; ++Attempt)
			{
				if (Architecture::X64::CpuRandom::RdRand16(Value))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]] static auto RdRand32(Foundation::Uint32& Value) -> Foundation::Bool
		{
			for (Foundation::Uint32 Attempt{}; Attempt < DefaultRetries; ++Attempt)
			{
				if (Architecture::X64::CpuRandom::RdRand32(Value))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]] static auto RdRand64(Foundation::Uint64& Value) -> Foundation::Bool
		{
			for (Foundation::Uint32 Attempt{}; Attempt < DefaultRetries; ++Attempt)
			{
				if (Architecture::X64::CpuRandom::RdRand64(Value))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]] static auto RdSeed16(Foundation::Uint16& Value) -> Foundation::Bool
		{
			for (Foundation::Uint32 Attempt{}; Attempt < DefaultRetries; ++Attempt)
			{
				if (Architecture::X64::CpuRandom::RdSeed16(Value))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]] static auto RdSeed32(Foundation::Uint32& Value) -> Foundation::Bool
		{
			for (Foundation::Uint32 Attempt{}; Attempt < DefaultRetries; ++Attempt)
			{
				if (Architecture::X64::CpuRandom::RdSeed32(Value))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]] static auto RdSeed64(Foundation::Uint64& Value) -> Foundation::Bool
		{
			for (Foundation::Uint32 Attempt{}; Attempt < DefaultRetries; ++Attempt)
			{
				if (Architecture::X64::CpuRandom::RdSeed64(Value))
				{
					return true;
				}
			}

			return false;
		}

	private:
		RandomSource Source_{ RandomSource::PreferRdSeed };
	};
}