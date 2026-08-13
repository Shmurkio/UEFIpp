#include <UEFIpp/Architecture/X64/Instruction/Mnemonic.hpp>

#include "../Zydis/Conversion.hpp"

namespace UEFIpp::Architecture::X64::InstructionSet
{
	auto MnemonicName(
		Mnemonic Value
	) noexcept -> Library::StringView
	{
		const auto* Name = ZydisMnemonicGetString(
			ZydisBackend::ToZydis(Value)
		);

		return Name
			? Library::StringView{ Name }
			: Library::StringView{};
	}
}
