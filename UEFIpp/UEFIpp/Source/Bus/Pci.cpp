#include <UEFIpp/Bus/Pci.hpp>

#include <UEFIpp/Bus/PciDevice.hpp>

#include <UEFIpp/Protocols/Access.hpp>
#include <UEFIpp/UEFI/Context.hpp>

namespace UEFIpp::Bus
{
	auto Pci::Enumerate() -> Library::Vector<PciDevice>
	{
		Library::Vector<PciDevice> Devices;
		Protocols::Access Access{ &UEFI::Context::BootServices() };
		const auto Handles = Access.LocateHandles<Protocols::PciIo>();

		if (!Handles)
		{
			return Devices;
		}

		for (auto Handle : *Handles)
		{
			const auto PciIo = Access.Handle<Protocols::PciIo>(Handle);

			if (!PciIo)
			{
				continue;
			}

			PciDevice Device{};
			Device.PciIo_ = *PciIo;

			if (Device.VendorId() == 0xFFFF)
			{
				continue;
			}

			Devices.PushBack(Foundation::Utility::Move(Device));
		}

		return Devices;
	}

	auto Pci::FindByClass(const Foundation::Uint8 Class, const Foundation::Uint8 SubClass, const Foundation::Uint8 ProgIf) -> Library::Vector<PciDevice>
	{
		Library::Vector<PciDevice> Matches;

		for (const auto& Device : Enumerate())
		{
			if (Device.ClassCode() != Class)
			{
				continue;
			}

			if (Device.SubClass() != SubClass)
			{
				continue;
			}

			if (ProgIf != 0xFF && Device.ProgrammingInterface() != ProgIf)
			{
				continue;
			}

			Matches.PushBack(Device);
		}

		return Matches;
	}

	auto Pci::FindById(const Foundation::Uint16 VendorId, const Foundation::Uint16 DeviceId) -> Library::Optional<PciDevice>
	{
		for (const auto& Device : Enumerate())
		{
			if (Device.VendorId() == VendorId && Device.DeviceId() == DeviceId)
			{
				return Device;
			}
		}

		return {};
	}
}