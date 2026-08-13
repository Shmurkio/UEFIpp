#include <UEFIpp/Diagnostics/Trace.hpp>

namespace UEFIpp::Diagnostics {
__declspec(noinline) auto CurrentInstructionAddress() noexcept
    -> const Foundation::Void * {
  return _ReturnAddress();
}
} // namespace UEFIpp::Diagnostics
