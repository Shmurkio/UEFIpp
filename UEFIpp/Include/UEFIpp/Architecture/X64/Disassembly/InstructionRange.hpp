#pragma once

#include <UEFIpp/Architecture/X64/Disassembly/Decoder.hpp>

namespace UEFIpp::Architecture::X64::Disassembly {
class InstructionRange {
public:
  class Iterator {
  public:
    constexpr Iterator() noexcept = default;

    Iterator(const Decoder *DecoderInstance,
             Library::Span<const Foundation::Byte> Code,
             Foundation::Uint64 BaseAddress, Foundation::Size Offset) noexcept;

    [[nodiscard]] auto operator*() const noexcept
        -> const InstructionSet::DecodedInstruction &;

    [[nodiscard]] auto operator->() const noexcept
        -> const InstructionSet::DecodedInstruction *;

    auto operator++() noexcept -> Iterator &;

    [[nodiscard]] constexpr auto
    operator==(const Iterator &Other) const noexcept -> Foundation::Bool {
      if (AtEnd_ && Other.AtEnd_) {
        return true;
      }

      return Decoder_ == Other.Decoder_ && Code_.Data() == Other.Code_.Data() &&
             Offset_ == Other.Offset_ && AtEnd_ == Other.AtEnd_;
    }

    [[nodiscard]] constexpr auto Error() const noexcept
        -> const Library::Optional<DecodeError> & {
      return Error_;
    }

  private:
    auto DecodeCurrent() noexcept -> Foundation::Void;

    const Decoder *Decoder_{};
    Library::Span<const Foundation::Byte> Code_{};
    Foundation::Uint64 BaseAddress_{};
    Foundation::Size Offset_{};
    InstructionSet::DecodedInstruction Current_{};
    Library::Optional<DecodeError> Error_{};
    Foundation::Bool AtEnd_{true};
  };

public:
  constexpr InstructionRange(const Decoder &DecoderInstance,
                             Library::Span<const Foundation::Byte> Code,
                             Foundation::Uint64 BaseAddress = 0) noexcept
      : Decoder_(&DecoderInstance), Code_(Code), BaseAddress_(BaseAddress) {}

  [[nodiscard]] auto begin() const noexcept -> Iterator;

  [[nodiscard]] constexpr auto end() const noexcept -> Iterator { return {}; }

private:
  const Decoder *Decoder_{};
  Library::Span<const Foundation::Byte> Code_{};
  Foundation::Uint64 BaseAddress_{};
};
} // namespace UEFIpp::Architecture::X64::Disassembly
