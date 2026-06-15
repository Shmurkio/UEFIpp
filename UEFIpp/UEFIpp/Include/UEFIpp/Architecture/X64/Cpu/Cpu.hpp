#pragma once

#include <UEFIpp/Architecture/X64/Common/Registers.hpp>
#include <intrin.h>

namespace UEFIpp::Architecture::X64
{
	class CpuidResult
	{
	public:
		Foundation::Int32 Eax{};
		Foundation::Int32 Ebx{};
		Foundation::Int32 Ecx{};
		Foundation::Int32 Edx{};
	};

	class Cpu
	{
	public:
		Cpu() = delete;

		[[nodiscard]] static auto ReadCr0() -> Register
		{
			return __readcr0();
		}

		static auto WriteCr0(Register Value) -> Foundation::Void
		{
			__writecr0(Value);
		}

		[[nodiscard]] static auto ReadCr2() -> Register
		{
			return __readcr2();
		}

		[[nodiscard]] static auto ReadCr3() -> Register
		{
			return __readcr3();
		}

		static auto WriteCr3(Register Value) -> Foundation::Void
		{
			__writecr3(Value);
		}

		[[nodiscard]] static auto ReadCr4() -> Register
		{
			return __readcr4();
		}

		static auto WriteCr4(Register Value) -> Foundation::Void
		{
			__writecr4(Value);
		}

		[[nodiscard]] static auto ReadFlags() -> Register
		{
			return __readeflags();
		}

		static auto WriteFlags(Register Value) -> Foundation::Void
		{
			__writeeflags(Value);
		}

		[[nodiscard]] static auto ReadTsc() -> Foundation::Uint64
		{
			return __rdtsc();
		}

		[[nodiscard]] static auto Cpuid(Foundation::Int32 Leaf, Foundation::Int32 Subleaf = 0) -> CpuidResult
		{
			Foundation::Int32 Values[4]{};
			__cpuidex(Values, Leaf, Subleaf);
			return { Values[0], Values[1], Values[2], Values[3] };
		}

		static auto DisableWriteProtect() -> Register
		{
			const auto Old = ReadCr0();
			WriteCr0(Foundation::Bit::Clear(Old, Cr0::WriteProtect));
			return Old;
		}

		static auto RestoreCr0(Register Value) -> Foundation::Void
		{
			WriteCr0(Value);
		}

		[[nodiscard]] static auto InterruptsEnabled() -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(ReadFlags(), Rflags::InterruptEnable);
		}

		static auto EnableInterrupts() -> Foundation::Void
		{
			_enable();
		}

		static auto DisableInterrupts() -> Foundation::Void
		{
			_disable();
		}

		static auto RestoreInterrupts(Register OldFlags) -> Foundation::Void
		{
			if (Foundation::Bit::IsSet(OldFlags, Rflags::InterruptEnable))
			{
				EnableInterrupts();
			}
			else
			{
				DisableInterrupts();
			}
		}

		static auto Halt() -> Foundation::Void
		{
			__halt();
		}

		static auto Pause() -> Foundation::Void
		{
			_mm_pause();
		}
	};
}