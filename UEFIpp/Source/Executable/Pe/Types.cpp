#include <UEFIpp/Executable/Pe/Types.hpp>

namespace UEFIpp::Executable::Pe {
auto IsDosHeaderValid(const DosHeader &Header) -> Foundation::Bool {
  return Header.Magic == DosSignature;
}

auto IsNtSignatureValid(Foundation::Uint32 Signature) -> Foundation::Bool {
  return Signature == NtSignature;
}
} // namespace UEFIpp::Executable::Pe
