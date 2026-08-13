#include <UEFIpp/Protocols/SimpleTextInputEx.hpp>

namespace UEFIpp::Protocols {
auto SimpleTextInputEx::ResetDevice(Foundation::Bool ExtendedVerification)
    -> UEFI::StatusValue {
  return Reset(this, ExtendedVerification);
}

auto SimpleTextInputEx::ReadKey(KeyData &Data) -> UEFI::StatusValue {
  return ReadKeyStrokeEx(this, &Data);
}

auto SimpleTextInputEx::SetToggleState(KeyToggleState &State)
    -> UEFI::StatusValue {
  return SetState(this, &State);
}

auto SimpleTextInputEx::RegisterNotify(KeyData &Data, KeyNotifyFn Function,
                                       Foundation::Void *&NotifyHandle)
    -> UEFI::StatusValue {
  return RegisterKeyNotify(this, &Data, Function, &NotifyHandle);
}

auto SimpleTextInputEx::UnregisterNotify(Foundation::Void *NotifyHandle)
    -> UEFI::StatusValue {
  return UnregisterKeyNotify(this, NotifyHandle);
}
} // namespace UEFIpp::Protocols
