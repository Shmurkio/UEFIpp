#pragma once

#include <UEFIpp/Memory/AllocatorStub.hpp>

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/String/BasicString.hpp>
#include <UEFIpp/UEFI/Guid.hpp>
#include <UEFIpp/UEFI/Status.hpp>

namespace UEFIpp::Text {
class Format {
public:
  Format() = delete;

  template <typename TChar>
  [[nodiscard]] static constexpr auto
  HexCharacter(Foundation::Uint8 Value, Foundation::Bool Uppercase = true)
      -> TChar {
    Value &= Foundation::Bit::LowMask<Foundation::Uint8>(4);

    if (Value < 10) {
      return Foundation::Cast::Auto<TChar>(TChar('0') + Value);
    }

    return Foundation::Cast::Auto<TChar>((Uppercase ? TChar('A') : TChar('a')) +
                                         (Value - 10));
  }

  template <typename TString>
  [[nodiscard]] static auto UInt64(Foundation::Uint64 Value,
                                   Memory::AllocatorStub Allocator = {})
      -> TString {
    using CharType = typename TString::ValueType;

    TString Result{Allocator};

    if (Value == 0) {
      (void)Result.PushBack(CharType('0'));
      return Result;
    }

    CharType Buffer[20]{};
    Foundation::Size Count{};

    while (Value) {
      Buffer[Count++] =
          Foundation::Cast::Auto<CharType>(CharType('0') + (Value % 10));
      Value /= 10;
    }

    while (Count) {
      if (!Result.PushBack(Buffer[--Count])) {
        Result.Clear();
        return Result;
      }
    }

    return Result;
  }

  template <typename TString>
  [[nodiscard]] static auto Int64(Foundation::Int64 Value,
                                  Memory::AllocatorStub Allocator = {})
      -> TString {
    using CharType = typename TString::ValueType;

    TString Result{Allocator};

    if (Value < 0) {
      if (!Result.PushBack(CharType('-'))) {
        return Result;
      }

      const auto Magnitude = UInt64<TString>(
          Foundation::Cast::Auto<Foundation::Uint64>(-(Value + 1)) + 1,
          Allocator);

      if (Magnitude.Empty() || !Result.Append(Magnitude.View())) {
        Result.Clear();
      }

      return Result;
    }

    return UInt64<TString>(Foundation::Cast::Auto<Foundation::Uint64>(Value),
                           Allocator);
  }

  template <typename TString>
  [[nodiscard]] static auto HexUInt64(Foundation::Uint64 Value,
                                      Foundation::Size Width = 0,
                                      Foundation::Bool Uppercase = true,
                                      Memory::AllocatorStub Allocator = {})
      -> TString {
    using CharType = typename TString::ValueType;

    TString Result{Allocator};
    CharType Buffer[16]{};

    constexpr auto NibbleMask = Foundation::Bit::LowMask<Foundation::Uint64>(4);

    Foundation::Size Count{};

    if (!Value) {
      Buffer[Count++] = CharType('0');
    } else {
      while (Value) {
        const auto Nibble =
            Foundation::Cast::Auto<Foundation::Uint8>(Value & NibbleMask);
        Buffer[Count++] = HexCharacter<CharType>(Nibble, Uppercase);
        Value >>= 4;
      }
    }

    for (Foundation::Size i = Count; i < Width; ++i) {
      if (!Result.PushBack(CharType('0'))) {
        Result.Clear();
        return Result;
      }
    }

    while (Count) {
      if (!Result.PushBack(Buffer[--Count])) {
        Result.Clear();
        return Result;
      }
    }

    return Result;
  }

  template <typename TString>
  [[nodiscard]] static auto Guid(const UEFI::Guid &Value,
                                 Memory::AllocatorStub Allocator = {})
      -> TString {
    using CharType = typename TString::ValueType;

    TString Result{Allocator};
    if (!Result.Reserve(37)) {
      return Result;
    }

    const auto AppendHex = [&](Foundation::Uint64 Part,
                               Foundation::Size Width) -> Foundation::Bool {
      const auto Text = HexUInt64<TString>(Part, Width, true, Allocator);
      return Text.Size() == Width && Result.Append(Text.View());
    };

    const auto AppendSeparator = [&]() -> Foundation::Bool {
      return Result.PushBack(CharType('-'));
    };

    if (!AppendHex(Value.Data1, 8) || !AppendSeparator() ||
        !AppendHex(Value.Data2, 4) || !AppendSeparator() ||
        !AppendHex(Value.Data3, 4) || !AppendSeparator() ||
        !AppendHex(Value.Data4[0], 2) || !AppendHex(Value.Data4[1], 2) ||
        !AppendSeparator()) {
      Result.Clear();
      return Result;
    }

    for (Foundation::Size i = 2; i < 8; ++i) {
      if (!AppendHex(Value.Data4[i], 2)) {
        Result.Clear();
        return Result;
      }
    }

    return Result;
  }

  template <typename TString>
  [[nodiscard]] static auto Status(UEFI::Status Value,
                                   Memory::AllocatorStub Allocator = {})
      -> TString {
    using CharType = typename TString::ValueType;

    if (const auto *Name = UEFI::StatusName(Value.Code())) {
      TString Result{Allocator};

      while (*Name) {
        if (!Result.PushBack(Foundation::Cast::Auto<CharType>(*Name++))) {
          Result.Clear();
          return Result;
        }
      }

      return Result;
    }

    TString Result{Allocator};
    const auto *Prefix = "UnknownStatus(0x";

    while (*Prefix) {
      if (!Result.PushBack(Foundation::Cast::Auto<CharType>(*Prefix++))) {
        Result.Clear();
        return Result;
      }
    }

    const auto Number = HexUInt64<TString>(Value.Value(), 0, true, Allocator);
    if (Number.Empty() || !Result.Append(Number.View()) ||
        !Result.PushBack(CharType(')'))) {
      Result.Clear();
    }

    return Result;
  }

  [[nodiscard]] static auto UInt64(Foundation::Uint64 Value,
                                   Memory::AllocatorStub Allocator = {})
      -> Library::String;

  [[nodiscard]] static auto Int64(Foundation::Int64 Value,
                                  Memory::AllocatorStub Allocator = {})
      -> Library::String;

  [[nodiscard]] static auto Guid(const UEFI::Guid &Value,
                                 Memory::AllocatorStub Allocator = {})
      -> Library::String;

  [[nodiscard]] static auto Status(UEFI::Status Value,
                                   Memory::AllocatorStub Allocator = {})
      -> Library::String;
};
} // namespace UEFIpp::Text
