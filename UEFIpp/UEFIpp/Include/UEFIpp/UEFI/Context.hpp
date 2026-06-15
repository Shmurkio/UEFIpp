#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Types.hpp>
#include <UEFIpp/UEFI/SystemTable.hpp>
#include <UEFIpp/Stream/Out/Out.hpp>
#include <UEFIpp/Stream/In/In.hpp>
#include <UEFIpp/Protocols/Access.hpp>
#include <UEFIpp/Protocols/SimpleTextInputEx.hpp>

namespace UEFIpp::UEFI
{
	class Context
	{
	public:
		Context() = delete;

		static auto Normalize(Handle ImageHandle, Table::System* SystemTable, Foundation::Uint16 SerialPort = 0x3F8, MemoryType CrtMemoryType = MemoryType::BootServicesData) -> Foundation::Bool
		{
			if (IsInitialized())
			{
				return true;
			}

			if (!ImageHandle || !SystemTable || !SystemTable->BootServices || !SystemTable->RuntimeServices || !SystemTable->ConsoleOut || !SystemTable->ConsoleIn)
			{
				return false;
			}

			ImageHandle_ = ImageHandle;
			SystemTable_ = SystemTable;

			BootServices_ = SystemTable->BootServices;
			RuntimeServices_ = SystemTable->RuntimeServices;

			Stream::Out::Serial.SetSink(Stream::SerialSink(SerialPort));
			Stream::Out::Console.SetSink(Stream::ConsoleOutputSink(SystemTable->ConsoleOut));

			Protocols::Access Access(BootServices_);
			auto InputEx = Access.Handle<Protocols::SimpleTextInputEx>(SystemTable->ConsoleInHandle);

			if (InputEx)
			{
				Stream::In::Console.SetSource(Stream::ConsoleInputSource(BootServices_, *InputEx));
			}

			CrtMemoryType_ = CrtMemoryType;

			return true;
		}

		[[nodiscard]] static auto BootServices() -> Table::BootServices&
		{
			UEFIPP_ASSERT(IsInitialized());
			return *BootServices_;
		}

		[[nodiscard]] static auto RuntimeServices() -> Table::RuntimeServices&
		{
			UEFIPP_ASSERT(IsInitialized());
			return *RuntimeServices_;
		}

		[[nodiscard]] static auto SystemTable() -> Table::System&
		{
			UEFIPP_ASSERT(IsInitialized());
			return *SystemTable_;
		}

		[[nodiscard]] static auto ConsoleIn() -> Protocols::SimpleTextInput&
		{
			UEFIPP_ASSERT(IsInitialized());
			return *SystemTable_->ConsoleIn;
		}

		[[nodiscard]] static auto ConsoleOut() -> Protocols::SimpleTextOutput&
		{
			UEFIPP_ASSERT(IsInitialized());
			return *SystemTable_->ConsoleOut;
		}

		[[nodiscard]] static auto ImageHandle() -> Handle
		{
			UEFIPP_ASSERT(IsInitialized());
			return ImageHandle_;
		}

		[[nodiscard]] static auto CrtMemoryType() -> MemoryType
		{
			return CrtMemoryType_;
		}


		[[nodiscard]] static auto IsInitialized() -> Foundation::Bool
		{
			return ImageHandle_ && SystemTable_ && BootServices_ && RuntimeServices_ && SystemTable_->ConsoleOut && SystemTable_->ConsoleIn;
		}

		[[nodiscard]] constexpr auto operator<=>(const Context&) const = default;

	private:
		inline static Handle ImageHandle_{};
		inline static Table::System* SystemTable_{};
		inline static Table::BootServices* BootServices_{};
		inline static Table::RuntimeServices* RuntimeServices_{};
		inline static MemoryType CrtMemoryType_{ MemoryType::BootServicesData };
	};
}