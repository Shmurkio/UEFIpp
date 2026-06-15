#pragma once

#include <UEFIpp/Architecture/X64/Cpu/Cpu.hpp>

namespace UEFIpp::Architecture::X64
{
	class Tlb
	{
	public:
		Tlb() = delete;

		static auto Invalidate(const Foundation::Void* Address) -> Foundation::Void
		{
			if (!Address)
			{
				return;
			}

			__invlpg(Foundation::Cast::RemoveConst(Address));
		}

		static auto Flush() -> Foundation::Void
		{
			const auto Cr3 = Cpu::ReadCr3();
			Cpu::WriteCr3(Cr3);
		}

		static auto FlushGlobal() -> Foundation::Void
		{
			auto Cr4Value = Cpu::ReadCr4();

			if (!Foundation::Bit::IsSet(Cr4Value, Cr4::PageGlobalEnable))
			{
				Flush();
				return;
			}

			Cpu::WriteCr4(Foundation::Bit::Clear(Cr4Value, Cr4::PageGlobalEnable));
			Cpu::WriteCr4(Cr4Value);
		}
	};
}