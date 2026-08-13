#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Types.hpp>
#include <UEFIpp/UEFI/TableHeader.hpp>
#include <UEFIpp/UEFI/ConfigurationTable.hpp>

namespace UEFIpp::Protocols
{
	class SimpleTextInput;
	class SimpleTextOutput;
}

namespace UEFIpp::UEFI::Table
{
	class BootServices;
	class RuntimeServices;

	class System
	{
	public:
		Header Header;
		Foundation::WChar* FirmwareVendor;
		Foundation::Uint32 FirmwareRevision;

		Handle ConsoleInHandle;
		Protocols::SimpleTextInput* ConsoleIn;

		Handle ConsoleOutHandle;
		Protocols::SimpleTextOutput* ConsoleOut;

		Handle StandardErrorHandle;
		Protocols::SimpleTextOutput* StandardError;

		RuntimeServices* RuntimeServices;
		BootServices* BootServices;

		Foundation::UintN NumberOfTableEntries;
		Configuration* ConfigurationTable;
	};
}