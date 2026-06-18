#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

#include <UEFIpp/Protocols/PciIo.hpp>

namespace UEFIpp::Bus
{
	class Pci;

	class PciDevice
	{
	public:
		[[nodiscard]] auto Segment() const -> Foundation::Uint16;
		[[nodiscard]] auto Bus() const -> Foundation::Uint8;
		[[nodiscard]] auto Device() const -> Foundation::Uint8;
		[[nodiscard]] auto Function() const -> Foundation::Uint8;

		[[nodiscard]] auto VendorId() const -> Foundation::Uint16;
		[[nodiscard]] auto DeviceId() const -> Foundation::Uint16;

		[[nodiscard]] auto ClassCode() const -> Foundation::Uint8;
		[[nodiscard]] auto SubClass() const -> Foundation::Uint8;
		[[nodiscard]] auto ProgrammingInterface() const -> Foundation::Uint8;
		[[nodiscard]] auto RevisionId() const -> Foundation::Uint8;

		[[nodiscard]] auto Bar(Foundation::Uint8 Index) const -> Foundation::Uint64;

		[[nodiscard]] auto Read8(Foundation::Uint16 Offset) const -> Foundation::Uint8;
		[[nodiscard]] auto Read16(Foundation::Uint16 Offset) const -> Foundation::Uint16;
		[[nodiscard]] auto Read32(Foundation::Uint16 Offset) const -> Foundation::Uint32;

		auto Write8(Foundation::Uint16 Offset, Foundation::Uint8 Value) -> Foundation::Void;
		auto Write16(Foundation::Uint16 Offset, Foundation::Uint16 Value) -> Foundation::Void;
		auto Write32(Foundation::Uint16 Offset, Foundation::Uint32 Value) -> Foundation::Void;

		auto EnableBusMaster() -> Foundation::Void;
		auto EnableMemorySpace() -> Foundation::Void;

	private:
		friend class Pci;

		Protocols::PciIo* PciIo_{};
	};
}