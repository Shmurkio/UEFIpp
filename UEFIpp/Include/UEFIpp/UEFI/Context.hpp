#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Memory.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/SystemTable.hpp>
#include <UEFIpp/UEFI/Types.hpp>

namespace UEFIpp::UEFI {
class Context {
public:
  Context() = delete;

  [[nodiscard]] static auto
  Attach(Table::System *SystemTable, Foundation::Uint16 SerialPort = 0x3F8,
         MemoryType CrtMemoryType = MemoryType::BootServicesData)
      -> Foundation::Bool;

  [[nodiscard]] static auto
  Normalize(Handle ImageHandle, Table::System *SystemTable,
            Foundation::Uint16 SerialPort = 0x3F8,
            MemoryType CrtMemoryType = MemoryType::BootServicesData)
      -> Foundation::Bool;

  [[nodiscard]] static auto BootServices() -> Table::BootServices &;
  [[nodiscard]] static auto RuntimeServices() -> Table::RuntimeServices &;
  [[nodiscard]] static auto SystemTable() -> Table::System &;
  [[nodiscard]] static auto ConsoleIn() -> Protocols::SimpleTextInput &;
  [[nodiscard]] static auto ConsoleOut() -> Protocols::SimpleTextOutput &;
  [[nodiscard]] static auto HasConsoleIn() noexcept -> Foundation::Bool;
  [[nodiscard]] static auto HasConsoleOut() noexcept -> Foundation::Bool;
  [[nodiscard]] static auto HasImageHandle() noexcept -> Foundation::Bool;
  [[nodiscard]] static auto ImageHandle() -> Handle;
  [[nodiscard]] static auto CrtMemoryType() -> MemoryType;
  [[nodiscard]] static auto IsInitialized() noexcept -> Foundation::Bool;

private:
  static Handle ImageHandle_;
  static Table::System *SystemTable_;
  static Table::BootServices *BootServices_;
  static Table::RuntimeServices *RuntimeServices_;
  static MemoryType CrtMemoryType_;
};
} // namespace UEFIpp::UEFI
