#include <UEFIpp/Bus/PciDevice.hpp>

#include <UEFIpp/Foundation/Cast.hpp>

namespace UEFIpp::Bus
{
	auto PciDevice::Segment() const -> Foundation::Uint16
	{
		Foundation::UintN Segment{}, Bus{}, Device{}, Function{};
		PciIo_->GetLocation(PciIo_, &Segment, &Bus, &Device, &Function);
		return Foundation::Cast::Auto<Foundation::Uint16>(Segment);
	}

	auto PciDevice::Bus() const -> Foundation::Uint8
	{
		Foundation::UintN Segment{}, Bus{}, Device{}, Function{};
		PciIo_->GetLocation(PciIo_, &Segment, &Bus, &Device, &Function);
		return Foundation::Cast::Auto<Foundation::Uint8>(Bus);
	}

	auto PciDevice::Device() const -> Foundation::Uint8
	{
		Foundation::UintN Segment{}, Bus{}, Device{}, Function{};
		PciIo_->GetLocation(PciIo_, &Segment, &Bus, &Device, &Function);
		return Foundation::Cast::Auto<Foundation::Uint8>(Device);
	}

	auto PciDevice::Function() const -> Foundation::Uint8
	{
		Foundation::UintN Segment{}, Bus{}, Device{}, Function{};
		PciIo_->GetLocation(PciIo_, &Segment, &Bus, &Device, &Function);
		return Foundation::Cast::Auto<Foundation::Uint8>(Function);
	}

	auto PciDevice::VendorId() const -> Foundation::Uint16
	{
		return Read16(0x00);
	}

	auto PciDevice::DeviceId() const -> Foundation::Uint16
	{
		return Read16(0x02);
	}

	auto PciDevice::ClassCode() const -> Foundation::Uint8
	{
		return Read8(0x0B);
	}

	auto PciDevice::SubClass() const -> Foundation::Uint8
	{
		return Read8(0x0A);
	}

	auto PciDevice::ProgrammingInterface() const -> Foundation::Uint8
	{
		return Read8(0x09);
	}

	auto PciDevice::RevisionId() const -> Foundation::Uint8
	{
		return Read8(0x08);
	}

	auto PciDevice::Read8(Foundation::Uint16 Offset) const -> Foundation::Uint8
	{
		Foundation::Uint8 Value{};
		PciIo_->ReadConfig8(Offset, Value);
		return Value;
	}

	auto PciDevice::Read16(Foundation::Uint16 Offset) const -> Foundation::Uint16
	{
		Foundation::Uint16 Value{};
		PciIo_->ReadConfig16(Offset, Value);
		return Value;
	}

	auto PciDevice::Read32(Foundation::Uint16 Offset) const -> Foundation::Uint32
	{
		Foundation::Uint32 Value{};
		PciIo_->ReadConfig32(Offset, Value);
		return Value;
	}

	auto PciDevice::Write8(Foundation::Uint16 Offset, Foundation::Uint8 Value) -> Foundation::Void
	{
		PciIo_->Pci.Write(PciIo_, Protocols::PciIoWidth::Uint8, Offset, 1, &Value);
	}

	auto PciDevice::Write16(Foundation::Uint16 Offset, Foundation::Uint16 Value) -> Foundation::Void
	{
		PciIo_->WriteConfig16(Offset, Value);
	}

	auto PciDevice::Write32(Foundation::Uint16 Offset, Foundation::Uint32 Value) -> Foundation::Void
	{
		PciIo_->Pci.Write(PciIo_, Protocols::PciIoWidth::Uint32, Offset, 1, &Value);
	}

	auto PciDevice::Bar(Foundation::Uint8 Index) const -> Foundation::Uint64
	{
		if (Index >= 6)
		{
			return 0;
		}

		const auto Offset = Foundation::Cast::Auto<Foundation::Uint16>(0x10 + Index * 4);
		const auto Low = Read32(Offset);

		if (Low & 1)
		{
			return Low & ~0x3ull;
		}

		if ((Low & 0x6) == 0x4 && Index < 5)
		{
			const auto High = Read32(Offset + 4);
			return (Foundation::Cast::Auto<Foundation::Uint64>(High) << 32) | (Low & ~0xFull);
		}

		return Low & ~0xFull;
	}

	auto PciDevice::EnableBusMaster() -> Foundation::Void
	{
		auto Command = Read16(0x04);
		Command |= (1 << 2);
		Write16(0x04, Command);
	}

	auto PciDevice::EnableMemorySpace() -> Foundation::Void
	{
		auto Command = Read16(0x04);
		Command |= (1 << 1);
		Write16(0x04, Command);
	}
}