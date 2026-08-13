#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::UEFI
{
	using StatusValue = Foundation::UintN;

	inline constexpr StatusValue ErrorBit = Foundation::Bit::Mask<StatusValue>(sizeof(StatusValue) * 8 - 1);

	[[nodiscard]] constexpr auto MakeError(StatusValue Code) -> StatusValue
	{
		return ErrorBit | Code;
	}

	enum class StatusCode : StatusValue
	{
		Success = 0,

		LoadError = MakeError(1),
		InvalidParameter = MakeError(2),
		Unsupported = MakeError(3),
		BadBufferSize = MakeError(4),
		BufferTooSmall = MakeError(5),
		NotReady = MakeError(6),
		DeviceError = MakeError(7),
		WriteProtected = MakeError(8),
		OutOfResources = MakeError(9),
		VolumeCorrupted = MakeError(10),
		VolumeFull = MakeError(11),
		NoMedia = MakeError(12),
		MediaChanged = MakeError(13),
		NotFound = MakeError(14),
		AccessDenied = MakeError(15),
		NoResponse = MakeError(16),
		NoMapping = MakeError(17),
		Timeout = MakeError(18),
		NotStarted = MakeError(19),
		AlreadyStarted = MakeError(20),
		Aborted = MakeError(21),
		IcmpError = MakeError(22),
		TftpError = MakeError(23),
		ProtocolError = MakeError(24),
		IncompatibleVersion = MakeError(25),
		SecurityViolation = MakeError(26),
		CrcError = MakeError(27),
		EndOfMedia = MakeError(28),
		EndOfFile = MakeError(31),
		InvalidLanguage = MakeError(32),
		CompromisedData = MakeError(33),
		IpAddressConflict = MakeError(34),
		HttpError = MakeError(35),

		NetworkUnreachable = MakeError(100),
		HostUnreachable = MakeError(101),
		ProtocolUnreachable = MakeError(102),
		PortUnreachable = MakeError(103),

		ConnectionFin = MakeError(104),
		ConnectionReset = MakeError(105),
		ConnectionRefused = MakeError(106),

		WarnUnknownGlyph = 1,
		WarnDeleteFailure = 2,
		WarnWriteFailure = 3,
		WarnBufferTooSmall = 4,
		WarnStaleData = 5,
		WarnFileSystem = 6,
		WarnResetRequired = 7
	};

	[[nodiscard]] constexpr auto ToStatusValue(StatusCode Code) -> StatusValue
	{
		return Foundation::Cast::Auto<StatusValue>(Code);
	}

	[[nodiscard]] constexpr auto IsError(StatusCode Code) -> Foundation::Bool
	{
		return Foundation::Bit::IsSet(Foundation::Cast::Auto<StatusValue>(Code), ErrorBit);
	}

	[[nodiscard]] constexpr auto IsWarning(StatusCode Code) -> Foundation::Bool
	{
		return Code != StatusCode::Success && !IsError(Code);
	}

	[[nodiscard]] constexpr auto IsSuccess(StatusCode Code) -> Foundation::Bool
	{
		return Code == StatusCode::Success;
	}

	[[nodiscard]] constexpr auto StatusName(UEFI::StatusCode Code) -> const Foundation::Char*
	{
		switch (Code)
		{
		case UEFI::StatusCode::Success: return "Success";

		case UEFI::StatusCode::LoadError: return "LoadError";
		case UEFI::StatusCode::InvalidParameter: return "InvalidParameter";
		case UEFI::StatusCode::Unsupported: return "Unsupported";
		case UEFI::StatusCode::BadBufferSize: return "BadBufferSize";
		case UEFI::StatusCode::BufferTooSmall: return "BufferTooSmall";
		case UEFI::StatusCode::NotReady: return "NotReady";
		case UEFI::StatusCode::DeviceError: return "DeviceError";
		case UEFI::StatusCode::WriteProtected: return "WriteProtected";
		case UEFI::StatusCode::OutOfResources: return "OutOfResources";
		case UEFI::StatusCode::VolumeCorrupted: return "VolumeCorrupted";
		case UEFI::StatusCode::VolumeFull: return "VolumeFull";
		case UEFI::StatusCode::NoMedia: return "NoMedia";
		case UEFI::StatusCode::MediaChanged: return "MediaChanged";
		case UEFI::StatusCode::NotFound: return "NotFound";
		case UEFI::StatusCode::AccessDenied: return "AccessDenied";
		case UEFI::StatusCode::NoResponse: return "NoResponse";
		case UEFI::StatusCode::NoMapping: return "NoMapping";
		case UEFI::StatusCode::Timeout: return "Timeout";
		case UEFI::StatusCode::NotStarted: return "NotStarted";
		case UEFI::StatusCode::AlreadyStarted: return "AlreadyStarted";
		case UEFI::StatusCode::Aborted: return "Aborted";
		case UEFI::StatusCode::IcmpError: return "IcmpError";
		case UEFI::StatusCode::TftpError: return "TftpError";
		case UEFI::StatusCode::ProtocolError: return "ProtocolError";
		case UEFI::StatusCode::IncompatibleVersion: return "IncompatibleVersion";
		case UEFI::StatusCode::SecurityViolation: return "SecurityViolation";
		case UEFI::StatusCode::CrcError: return "CrcError";
		case UEFI::StatusCode::EndOfMedia: return "EndOfMedia";
		case UEFI::StatusCode::EndOfFile: return "EndOfFile";
		case UEFI::StatusCode::InvalidLanguage: return "InvalidLanguage";
		case UEFI::StatusCode::CompromisedData: return "CompromisedData";
		case UEFI::StatusCode::IpAddressConflict: return "IpAddressConflict";
		case UEFI::StatusCode::HttpError: return "HttpError";

		case UEFI::StatusCode::NetworkUnreachable: return "NetworkUnreachable";
		case UEFI::StatusCode::HostUnreachable: return "HostUnreachable";
		case UEFI::StatusCode::ProtocolUnreachable: return "ProtocolUnreachable";
		case UEFI::StatusCode::PortUnreachable: return "PortUnreachable";

		case UEFI::StatusCode::ConnectionFin: return "ConnectionFin";
		case UEFI::StatusCode::ConnectionReset: return "ConnectionReset";
		case UEFI::StatusCode::ConnectionRefused: return "ConnectionRefused";

		case UEFI::StatusCode::WarnUnknownGlyph: return "WarnUnknownGlyph";
		case UEFI::StatusCode::WarnDeleteFailure: return "WarnDeleteFailure";
		case UEFI::StatusCode::WarnWriteFailure: return "WarnWriteFailure";
		case UEFI::StatusCode::WarnBufferTooSmall: return "WarnBufferTooSmall";
		case UEFI::StatusCode::WarnStaleData: return "WarnStaleData";
		case UEFI::StatusCode::WarnFileSystem: return "WarnFileSystem";
		case UEFI::StatusCode::WarnResetRequired: return "WarnResetRequired";
		}

		return nullptr;
	}

	class Status
	{
	public:
		constexpr Status() noexcept = default;

		constexpr Status(StatusCode Code) noexcept : Code_(Code)
		{
		}

		constexpr explicit Status(StatusValue Value) noexcept : Code_(Foundation::Cast::Auto<StatusCode>(Value))
		{
		}

		[[nodiscard]] constexpr auto Code() const noexcept -> StatusCode
		{
			return Code_;
		}

		[[nodiscard]] constexpr auto Value() const noexcept -> StatusValue
		{
			return Foundation::Cast::Auto<StatusValue>(Code_);
		}

		[[nodiscard]] constexpr auto Failed() const noexcept -> Foundation::Bool
		{
			return IsError(Code_);
		}

		[[nodiscard]] constexpr auto Succeeded() const noexcept -> Foundation::Bool
		{
			return !Failed();
		}

		[[nodiscard]] constexpr auto Warning() const noexcept -> Foundation::Bool
		{
			return IsWarning(Code_);
		}

		[[nodiscard]] constexpr explicit operator Foundation::Bool() const noexcept
		{
			return Succeeded();
		}

		[[nodiscard]] constexpr explicit operator StatusCode() const noexcept
		{
			return Code_;
		}

		[[nodiscard]] constexpr explicit operator StatusValue() const noexcept
		{
			return Value();
		}

		[[nodiscard]] constexpr auto operator<=>(const Status&) const = default;

	private:
		StatusCode Code_{ StatusCode::Success };
	};
}