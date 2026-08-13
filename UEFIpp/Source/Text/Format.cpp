#include <UEFIpp/Text/Format.hpp>

namespace UEFIpp::Text {
auto Format::UInt64(Foundation::Uint64 Value, Memory::AllocatorStub Allocator)
    -> Library::String {
  return UInt64<Library::String>(Value, Allocator);
}

auto Format::Int64(Foundation::Int64 Value, Memory::AllocatorStub Allocator)
    -> Library::String {
  return Int64<Library::String>(Value, Allocator);
}

auto Format::Guid(const UEFI::Guid &Value, Memory::AllocatorStub Allocator)
    -> Library::String {
  return Guid<Library::String>(Value, Allocator);
}

auto Format::Status(UEFI::Status Value, Memory::AllocatorStub Allocator)
    -> Library::String {
  return Status<Library::String>(Value, Allocator);
}
} // namespace UEFIpp::Text
