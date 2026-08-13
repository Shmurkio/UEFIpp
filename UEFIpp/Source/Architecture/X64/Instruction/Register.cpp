#include <UEFIpp/Architecture/X64/Instruction/Register.hpp>

#include "../Zydis/Conversion.hpp"

namespace UEFIpp::Architecture::X64::InstructionSet
{
	auto Register::Class() const noexcept
		-> RegisterClass
	{
		switch (ZydisRegisterGetClass(
			ZydisBackend::ToZydis(*this)
	))
		{
		case ZYDIS_REGCLASS_GPR8:
			return RegisterClass::GeneralPurpose8;
		case ZYDIS_REGCLASS_GPR16:
			return RegisterClass::GeneralPurpose16;
		case ZYDIS_REGCLASS_GPR32:
			return RegisterClass::GeneralPurpose32;
		case ZYDIS_REGCLASS_GPR64:
			return RegisterClass::GeneralPurpose64;
		case ZYDIS_REGCLASS_X87:
			return RegisterClass::FloatingPoint;
		case ZYDIS_REGCLASS_MMX:
			return RegisterClass::Mmx;
		case ZYDIS_REGCLASS_XMM:
			return RegisterClass::Xmm;
		case ZYDIS_REGCLASS_YMM:
			return RegisterClass::Ymm;
		case ZYDIS_REGCLASS_ZMM:
			return RegisterClass::Zmm;
		case ZYDIS_REGCLASS_TMM:
			return RegisterClass::Tile;
		case ZYDIS_REGCLASS_FLAGS:
			return RegisterClass::Flags;
		case ZYDIS_REGCLASS_IP:
			return RegisterClass::InstructionPointer;
		case ZYDIS_REGCLASS_SEGMENT:
			return RegisterClass::Segment;
		case ZYDIS_REGCLASS_TEST:
			return RegisterClass::Test;
		case ZYDIS_REGCLASS_CONTROL:
			return RegisterClass::Control;
		case ZYDIS_REGCLASS_DEBUG:
			return RegisterClass::Debug;
		case ZYDIS_REGCLASS_MASK:
			return RegisterClass::Mask;
		case ZYDIS_REGCLASS_BOUND:
			return RegisterClass::Bound;
		case ZYDIS_REGCLASS_TABLE:
			return RegisterClass::Other;
		default:
			return RegisterClass::Invalid;
		}
	}

	auto Register::Width(
		MachineMode Mode
	) const noexcept -> Foundation::Uint16
	{
		return static_cast<Foundation::Uint16>(
			ZydisRegisterGetWidth(
				ZydisBackend::ToZydis(Mode),
				ZydisBackend::ToZydis(*this)
			)
		);
	}

	auto Register::Root(
		MachineMode Mode
	) const noexcept -> Register
	{
		return ZydisBackend::FromZydis(
			ZydisRegisterGetLargestEnclosing(
				ZydisBackend::ToZydis(Mode),
				ZydisBackend::ToZydis(*this)
			)
		);
	}

	auto Register::Name() const noexcept
		-> Library::StringView
	{
		const auto* Name = ZydisRegisterGetString(
			ZydisBackend::ToZydis(*this)
		);

		return Name
			? Library::StringView{ Name }
			: Library::StringView{};
	}
}
