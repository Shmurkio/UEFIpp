#include <UEFIpp/UEFI/Context.hpp>

#include <UEFIpp/Protocols/Access.hpp>
#include <UEFIpp/Protocols/SimpleTextInputEx.hpp>
#include <UEFIpp/IO/Context.hpp>

namespace UEFIpp::UEFI {
Handle Context::ImageHandle_{};
Table::System *Context::SystemTable_{};
Table::BootServices *Context::BootServices_{};
Table::RuntimeServices *Context::RuntimeServices_{};
MemoryType Context::CrtMemoryType_{MemoryType::BootServicesData};

auto Context::Attach(Table::System *SystemTable, Foundation::Uint16 SerialPort,
                     MemoryType CrtMemoryType) -> Foundation::Bool {
  if (IsInitialized()) {
    return SystemTable_ == SystemTable;
  }

  if (!SystemTable || !SystemTable->BootServices ||
      !SystemTable->RuntimeServices) {
    return false;
  }

  SystemTable_ = SystemTable;
  BootServices_ = SystemTable->BootServices;
  RuntimeServices_ = SystemTable->RuntimeServices;

  Protocols::SimpleTextInputEx *ExtendedInput{};

  if (SystemTable->ConsoleIn && SystemTable->ConsoleInHandle) {
    Protocols::Access Access(BootServices_);
    auto InputEx = Access.Handle<Protocols::SimpleTextInputEx>(
        SystemTable->ConsoleInHandle);

    if (InputEx) {
      ExtendedInput = *InputEx;
    }
  }

  IO::SystemIO().Attach(BootServices_, ExtendedInput, SystemTable->ConsoleIn,
                        SystemTable->ConsoleOut, SerialPort);

  CrtMemoryType_ = CrtMemoryType;
  return true;
}

auto Context::Normalize(Handle ImageHandle, Table::System *SystemTable,
                        Foundation::Uint16 SerialPort, MemoryType CrtMemoryType)
    -> Foundation::Bool {
  if (!ImageHandle || !Attach(SystemTable, SerialPort, CrtMemoryType)) {
    return false;
  }

  if (ImageHandle_ && ImageHandle_ != ImageHandle) {
    return false;
  }

  ImageHandle_ = ImageHandle;
  return true;
}

auto Context::BootServices() -> Table::BootServices & {
  UEFIPP_ASSERT(HasBootServices());
  return *BootServices_;
}

auto Context::RuntimeServices() -> Table::RuntimeServices & {
  UEFIPP_ASSERT(IsInitialized());
  return *RuntimeServices_;
}

auto Context::SystemTable() -> Table::System & {
  UEFIPP_ASSERT(IsInitialized());
  return *SystemTable_;
}

auto Context::ConsoleIn() -> Protocols::SimpleTextInput & {
  UEFIPP_ASSERT(HasConsoleIn());
  return *SystemTable_->ConsoleIn;
}

auto Context::ConsoleOut() -> Protocols::SimpleTextOutput & {
  UEFIPP_ASSERT(HasConsoleOut());
  return *SystemTable_->ConsoleOut;
}

auto Context::HasConsoleIn() noexcept -> Foundation::Bool {
  return SystemTable_ && SystemTable_->ConsoleIn;
}

auto Context::HasConsoleOut() noexcept -> Foundation::Bool {
  return SystemTable_ && SystemTable_->ConsoleOut;
}

auto Context::HasImageHandle() noexcept -> Foundation::Bool {
  return ImageHandle_ != nullptr;
}

auto Context::HasBootServices() noexcept -> Foundation::Bool {
  return BootServices_ != nullptr;
}

auto Context::PrepareExitBootServices() -> Foundation::Bool {
  return HasBootServices() &&
         Foundation::Cast::Auto<Foundation::Bool>(
             IO::SystemIO().PrepareExitBootServices());
}

auto Context::ExitBootServicesSucceeded() noexcept -> Foundation::Void {
  IO::SystemIO().ExitBootServices();
  BootServices_ = nullptr;
}

auto Context::ImageHandle() -> Handle {
  UEFIPP_ASSERT(HasImageHandle());
  return ImageHandle_;
}

auto Context::CrtMemoryType() -> MemoryType { return CrtMemoryType_; }

auto Context::IsInitialized() noexcept -> Foundation::Bool {
  return SystemTable_ && RuntimeServices_;
}

} // namespace UEFIpp::UEFI
