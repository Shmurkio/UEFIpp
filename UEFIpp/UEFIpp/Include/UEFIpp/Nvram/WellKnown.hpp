#pragma once

#include <UEFIpp/UEFI/Guid.hpp>
#include <UEFIpp/Nvram/VariableId.hpp>
#include <UEFIpp/Text/Format.hpp>

namespace UEFIpp::Nvram::WellKnown
{
	inline constexpr UEFI::Guid GlobalVariableGuid{ 0x8BE4DF61, 0x93CA, 0x11D2, 0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C };

	[[nodiscard]] inline auto BootCurrent() -> VariableId
	{
		return { "BootCurrent", GlobalVariableGuid };
	}

	[[nodiscard]] inline auto BootNext() -> VariableId
	{
		return { "BootNext", GlobalVariableGuid };
	}

	[[nodiscard]] inline auto BootOrder() -> VariableId
	{
		return VariableId{ Library::StringView{ "BootOrder" }, GlobalVariableGuid };
	}

	[[nodiscard]] inline auto Timeout() -> VariableId
	{
		return { "Timeout", GlobalVariableGuid };
	}

	[[nodiscard]] inline auto SecureBoot() -> VariableId
	{
		return { "SecureBoot", GlobalVariableGuid };
	}

	[[nodiscard]] inline auto SetupMode() -> VariableId
	{
		return { "SetupMode", GlobalVariableGuid };
	}

	[[nodiscard]] inline auto AuditMode() -> VariableId
	{
		return { "AuditMode", GlobalVariableGuid };
	}

	[[nodiscard]] inline auto DeployedMode() -> VariableId
	{
		return { "DeployedMode", GlobalVariableGuid };
	}

	[[nodiscard]] inline auto PlatformKey() -> VariableId
	{
		return { "PK", GlobalVariableGuid };
	}

	[[nodiscard]] inline auto KeyExchangeKey() -> VariableId
	{
		return { "KEK", GlobalVariableGuid };
	}

	[[nodiscard]] inline auto SignatureDatabase() -> VariableId
	{
		return { "db", GlobalVariableGuid };
	}

	[[nodiscard]] inline auto ForbiddenSignatureDatabase() -> VariableId
	{
		return { "dbx", GlobalVariableGuid };
	}

	[[nodiscard]] inline auto MokList() -> VariableId
	{
		return { "MokList", UEFI::Guid{ 0x605DAB50, 0xE046, 0x4300, 0xAB, 0xB6, 0x3D, 0xD8, 0x10, 0xDD, 0x8B, 0x23 } };
	}

	[[nodiscard]] inline auto BootOption(Foundation::Uint16 Number) -> VariableId
	{
		auto Name = Library::String{ "Boot" };
		Name += Text::Format::HexUInt64<Library::String>(Number, 4);
		return { Name.View(), GlobalVariableGuid };
	}

	[[nodiscard]] inline auto DriverOption(Foundation::Uint16 Number) -> VariableId
	{
		auto Name = Library::String{ "Driver" };
		Name += Text::Format::HexUInt64<Library::String>(Number, 4);
		return { Name.View(), GlobalVariableGuid };
	}
}