#pragma once

#include <UEFIpp/Architecture/X64/X64.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Memory/Memory.hpp>

namespace UEFIpp::Hooking
{
	class WritePatchOptions
	{
	public:
		Foundation::Bool BypassWriteProtect{ true };
		Foundation::Bool DisableInterrupts{ true };
		Foundation::Bool FlushTlb{ true };
	};

	class CodePatch
	{
	public:
		CodePatch() = delete;

		[[nodiscard]] static auto Write(Foundation::Void* Destination, const Foundation::Void* Source, Foundation::Size Size, const WritePatchOptions& Options = {}) -> Foundation::Bool
		{
			if (!Destination || !Source || !Size)
			{
				return false;
			}

			const auto OldFlags = Architecture::X64::Cpu::ReadFlags();
			Architecture::X64::Register OldCr0{};

			if (Options.DisableInterrupts)
			{
				Architecture::X64::Cpu::DisableInterrupts();
			}

			if (Options.BypassWriteProtect)
			{
				OldCr0 = Architecture::X64::Cpu::DisableWriteProtect();
			}

			Memory::Copy(Destination, Source, Size);

			if (Options.FlushTlb)
			{
				Architecture::X64::Tlb::Invalidate(Destination);
			}

			if (Options.BypassWriteProtect)
			{
				Architecture::X64::Cpu::RestoreCr0(OldCr0);
			}

			if (Options.DisableInterrupts)
			{
				Architecture::X64::Cpu::RestoreInterrupts(OldFlags);
			}

			return true;
		}

		[[nodiscard]] static auto Read(Foundation::Void* Destination, const Foundation::Void* Source, Foundation::Size Size) -> Foundation::Bool
		{
			if (!Destination || !Source || !Size)
			{
				return false;
			}

			Memory::Copy(Destination, Source, Size);
			return true;
		}
	};
}