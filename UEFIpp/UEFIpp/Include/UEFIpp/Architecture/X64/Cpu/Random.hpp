#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

extern "C"
{
	UEFIpp::Foundation::Uint8 UEFIpp_X64_RdRand16(UEFIpp::Foundation::Uint16* Value);
	UEFIpp::Foundation::Uint8 UEFIpp_X64_RdRand32(UEFIpp::Foundation::Uint32* Value);
	UEFIpp::Foundation::Uint8 UEFIpp_X64_RdRand64(UEFIpp::Foundation::Uint64* Value);

	UEFIpp::Foundation::Uint8 UEFIpp_X64_RdSeed16(UEFIpp::Foundation::Uint16* Value);
	UEFIpp::Foundation::Uint8 UEFIpp_X64_RdSeed32(UEFIpp::Foundation::Uint32* Value);
	UEFIpp::Foundation::Uint8 UEFIpp_X64_RdSeed64(UEFIpp::Foundation::Uint64* Value);
}

namespace UEFIpp::Architecture::X64
{
	class CpuRandom
	{
	public:
		CpuRandom() = delete;

		static auto RdRand16(Foundation::Uint16& Value) -> Foundation::Bool
		{
			return UEFIpp_X64_RdRand16(&Value) != 0;
		}

		static auto RdRand32(Foundation::Uint32& Value) -> Foundation::Bool
		{
			return UEFIpp_X64_RdRand32(&Value) != 0;
		}

		static auto RdRand64(Foundation::Uint64& Value) -> Foundation::Bool
		{
			return UEFIpp_X64_RdRand64(&Value) != 0;
		}

		static auto RdSeed16(Foundation::Uint16& Value) -> Foundation::Bool
		{
			return UEFIpp_X64_RdSeed16(&Value) != 0;
		}

		static auto RdSeed32(Foundation::Uint32& Value) -> Foundation::Bool
		{
			return UEFIpp_X64_RdSeed32(&Value) != 0;
		}

		static auto RdSeed64(Foundation::Uint64& Value) -> Foundation::Bool
		{
			return UEFIpp_X64_RdSeed64(&Value) != 0;
		}
	};
}