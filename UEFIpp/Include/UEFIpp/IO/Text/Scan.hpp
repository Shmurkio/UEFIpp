#pragma once

#include <UEFIpp/IO/Text/Reader.hpp>
#include <UEFIpp/UEFI/Guid.hpp>

namespace UEFIpp::IO {
struct ParseOptions final {
  Foundation::Uint8 Base{}; // 0 detects 0x, 0b and 0o prefixes.
  Foundation::Bool AllowSign{true};
  Foundation::Bool AllowSeparators{true};
  Foundation::Bool RequireEntireToken{true};
};

template <typename T>
struct Parser;

namespace Detail {
[[nodiscard]] constexpr auto DigitValue(Foundation::Char8 Character)
    -> Foundation::Uint8 {
  if (Character >= u8'0' && Character <= u8'9') {
    return Foundation::Cast::Auto<Foundation::Uint8>(Character - u8'0');
  }
  if (Character >= u8'a' && Character <= u8'z') {
    return Foundation::Cast::Auto<Foundation::Uint8>(Character - u8'a' + 10);
  }
  if (Character >= u8'A' && Character <= u8'Z') {
    return Foundation::Cast::Auto<Foundation::Uint8>(Character - u8'A' + 10);
  }
  return 0xFF;
}

template <typename T>
[[nodiscard]] constexpr auto ParseInteger(Library::U8StringView Text,
                                          ParseOptions Options) -> Result<T> {
  Foundation::Size Position{};
  Foundation::Bool Negative{};
  if (Options.AllowSign && Position < Text.Size() &&
      (Text[Position] == u8'+' || Text[Position] == u8'-')) {
    Negative = Text[Position++] == u8'-';
  }
  if (Position == Text.Size()) {
    return Failure(Error::Semantic(ErrorCode::InvalidFormat, Operation::Parse,
                                   Position));
  }

  auto Base = Options.Base;
  if (!Base) {
    Base = 10;
    if (Position + 1 < Text.Size() && Text[Position] == u8'0') {
      const auto Prefix = Text[Position + 1];
      if (Prefix == u8'x' || Prefix == u8'X') Base = 16;
      else if (Prefix == u8'b' || Prefix == u8'B') Base = 2;
      else if (Prefix == u8'o' || Prefix == u8'O') Base = 8;
      if (Base != 10) Position += 2;
    }
  } else if (Position + 1 < Text.Size() && Text[Position] == u8'0') {
    const auto Prefix = Text[Position + 1];
    if ((Base == 16 && (Prefix == u8'x' || Prefix == u8'X')) ||
        (Base == 2 && (Prefix == u8'b' || Prefix == u8'B')) ||
        (Base == 8 && (Prefix == u8'o' || Prefix == u8'O'))) {
      Position += 2;
    }
  }
  if (Base < 2 || Base > 36) {
    return Failure(
        Error::Semantic(ErrorCode::InvalidArgument, Operation::Parse));
  }

  Foundation::Uint64 Magnitude{};
  Foundation::Bool SawDigit{};
  Foundation::Bool PreviousSeparator{};
  for (; Position < Text.Size(); ++Position) {
    if (Options.AllowSeparators && Text[Position] == u8'_') {
      if (!SawDigit || PreviousSeparator) {
        return Failure(Error::Semantic(ErrorCode::InvalidFormat,
                                       Operation::Parse, Position));
      }
      PreviousSeparator = true;
      continue;
    }
    const auto Digit = DigitValue(Text[Position]);
    if (Digit >= Base) {
      if (Options.RequireEntireToken) {
        return Failure(Error::Semantic(ErrorCode::InvalidFormat,
                                       Operation::Parse, Position));
      }
      break;
    }
    SawDigit = true;
    PreviousSeparator = false;
    if (Magnitude > (Foundation::Uint64(-1) - Digit) / Base) {
      return Failure(
          Error::Semantic(ErrorCode::Overflow, Operation::Parse, Position));
    }
    Magnitude = Magnitude * Base + Digit;
  }
  if (!SawDigit || PreviousSeparator) {
    return Failure(
        Error::Semantic(ErrorCode::InvalidFormat, Operation::Parse, Position));
  }

  if constexpr (T(-1) < T(0)) {
    constexpr auto Maximum = Foundation::Cast::Auto<Foundation::Uint64>(
        (Foundation::Uint64{1} << (sizeof(T) * 8 - 1)) - 1);
    const auto Limit = Negative ? Maximum + 1 : Maximum;
    if (Magnitude > Limit) {
      return Failure(
          Error::Semantic(ErrorCode::Overflow, Operation::Parse, Position));
    }
    if (Negative) {
      if (Magnitude == Maximum + 1) {
        return Foundation::Cast::Auto<T>(
            -Foundation::Cast::Auto<Foundation::Int64>(Maximum) - 1);
      }
      return Foundation::Cast::Auto<T>(
          -Foundation::Cast::Auto<Foundation::Int64>(Magnitude));
    }
  } else if (Negative || Magnitude > Foundation::Uint64(T(-1))) {
    return Failure(
        Error::Semantic(ErrorCode::Overflow, Operation::Parse, Position));
  }
  return Foundation::Cast::Auto<T>(Magnitude);
}

template <typename T>
[[nodiscard]] auto ParseFloating(Library::U8StringView Text) -> Result<T> {
  Foundation::Size Position{};
  Foundation::Bool Negative{};
  if (Position < Text.Size() &&
      (Text[Position] == u8'+' || Text[Position] == u8'-')) {
    Negative = Text[Position++] == u8'-';
  }
  const auto EqualsIgnoreCase = [&](Library::U8StringView Word) {
    if (Text.Size() - Position != Word.Size()) return false;
    for (Foundation::Size Index{}; Index < Word.Size(); ++Index) {
      auto Left = Text[Position + Index];
      auto Right = Word[Index];
      if (Left >= u8'A' && Left <= u8'Z') Left += u8'a' - u8'A';
      if (Right >= u8'A' && Right <= u8'Z') Right += u8'a' - u8'A';
      if (Left != Right) return false;
    }
    return true;
  };
  if (EqualsIgnoreCase(u8"inf") || EqualsIgnoreCase(u8"infinity")) {
    auto Bits = Foundation::Uint64{0x7FF0000000000000ull};
    if (Negative) Bits |= Foundation::Uint64{1} << 63;
    return Foundation::Cast::Auto<T>(
        Foundation::Cast::Bitwise<Foundation::Float64>(Bits));
  }
  if (EqualsIgnoreCase(u8"nan")) {
    auto Bits = Foundation::Uint64{0x7FF8000000000000ull};
    if (Negative) Bits |= Foundation::Uint64{1} << 63;
    return Foundation::Cast::Auto<T>(
        Foundation::Cast::Bitwise<Foundation::Float64>(Bits));
  }
  Foundation::Float64 Value{};
  Foundation::Bool SawDigit{};
  while (Position < Text.Size() && Text[Position] >= u8'0' &&
         Text[Position] <= u8'9') {
    SawDigit = true;
    Value = Value * 10.0 + (Text[Position++] - u8'0');
  }
  if (Position < Text.Size() && Text[Position] == u8'.') {
    ++Position;
    Foundation::Float64 Scale{0.1};
    while (Position < Text.Size() && Text[Position] >= u8'0' &&
           Text[Position] <= u8'9') {
      SawDigit = true;
      Value += (Text[Position++] - u8'0') * Scale;
      Scale *= 0.1;
    }
  }
  if (!SawDigit) {
    return Failure(Error::Semantic(ErrorCode::InvalidFormat, Operation::Parse,
                                   Position));
  }
  if (Position < Text.Size() &&
      (Text[Position] == u8'e' || Text[Position] == u8'E')) {
    ++Position;
    Foundation::Bool NegativeExponent{};
    if (Position < Text.Size() &&
        (Text[Position] == u8'+' || Text[Position] == u8'-')) {
      NegativeExponent = Text[Position++] == u8'-';
    }
    Foundation::Uint32 Exponent{};
    const auto ExponentStart = Position;
    while (Position < Text.Size() && Text[Position] >= u8'0' &&
           Text[Position] <= u8'9') {
      if (Exponent > 1000) {
        return Failure(Error::Semantic(ErrorCode::Overflow, Operation::Parse,
                                       Position));
      }
      Exponent = Exponent * 10 + Text[Position++] - u8'0';
    }
    if (Position == ExponentStart) {
      return Failure(Error::Semantic(ErrorCode::InvalidFormat,
                                     Operation::Parse, Position));
    }
    while (Exponent--) {
      Value = NegativeExponent ? Value / 10.0 : Value * 10.0;
    }
  }
  if (Position != Text.Size()) {
    return Failure(Error::Semantic(ErrorCode::InvalidFormat, Operation::Parse,
                                   Position));
  }
  const auto Converted = Foundation::Cast::Auto<T>(Negative ? -Value : Value);
  if constexpr (sizeof(T) == sizeof(Foundation::Uint32)) {
    if ((Foundation::Cast::Bitwise<Foundation::Uint32>(Converted) &
         0x7F800000u) == 0x7F800000u) {
      return Failure(
          Error::Semantic(ErrorCode::Overflow, Operation::Parse, Position));
    }
  } else {
    if ((Foundation::Cast::Bitwise<Foundation::Uint64>(Converted) &
         0x7FF0000000000000ull) == 0x7FF0000000000000ull) {
      return Failure(
          Error::Semantic(ErrorCode::Overflow, Operation::Parse, Position));
    }
  }
  return Converted;
}
} // namespace Detail

template <>
struct Parser<UEFI::Guid> {
  [[nodiscard]] static auto Parse(Library::U8StringView Text, ParseOptions)
      -> Result<UEFI::Guid> {
    if (Text.Size() != 36 || Text[8] != u8'-' || Text[13] != u8'-' ||
        Text[18] != u8'-' || Text[23] != u8'-') {
      return Failure(
          Error::Semantic(ErrorCode::InvalidFormat, Operation::Parse));
    }
    const auto Hex = [&](Foundation::Size Offset,
                         Foundation::Size Count) -> Result<Foundation::Uint64> {
      Foundation::Uint64 Value{};
      for (Foundation::Size Index{}; Index < Count; ++Index) {
        const auto Digit = Detail::DigitValue(Text[Offset + Index]);
        if (Digit >= 16) {
          return Failure(Error::Semantic(ErrorCode::InvalidFormat,
                                         Operation::Parse, Offset + Index));
        }
        Value = (Value << 4) | Digit;
      }
      return Value;
    };
    auto Data1 = Hex(0, 8);
    auto Data2 = Hex(9, 4);
    auto Data3 = Hex(14, 4);
    if (!Data1) return Failure(Data1.Error());
    if (!Data2) return Failure(Data2.Error());
    if (!Data3) return Failure(Data3.Error());
    UEFI::Guid Value{};
    Value.Data1 = Foundation::Cast::Auto<Foundation::Uint32>(Data1.Value());
    Value.Data2 = Foundation::Cast::Auto<Foundation::Uint16>(Data2.Value());
    Value.Data3 = Foundation::Cast::Auto<Foundation::Uint16>(Data3.Value());
    constexpr Foundation::Size Offsets[]{19, 21, 24, 26, 28, 30, 32, 34};
    for (Foundation::Size Index{}; Index < 8; ++Index) {
      auto Byte = Hex(Offsets[Index], 2);
      if (!Byte) return Failure(Byte.Error());
      Value.Data4[Index] =
          Foundation::Cast::Auto<Foundation::Uint8>(Byte.Value());
    }
    return Value;
  }
};

template <typename T>
[[nodiscard]] auto Parse(Library::U8StringView Text,
                         ParseOptions Options = {}) -> Result<T> {
  if constexpr (Foundation::Concepts::Integral<T>) {
    if constexpr (Foundation::Traits::IsSame<T, Foundation::Bool>::Value) {
      if (Text == Library::U8StringView{u8"true"} ||
          Text == Library::U8StringView{u8"1"}) return true;
      if (Text == Library::U8StringView{u8"false"} ||
          Text == Library::U8StringView{u8"0"}) return false;
      return Failure(
          Error::Semantic(ErrorCode::InvalidFormat, Operation::Parse));
    } else {
      return Detail::ParseInteger<T>(Text, Options);
    }
  } else if constexpr (Foundation::Concepts::FloatingPoint<T>) {
    return Detail::ParseFloating<T>(Text);
  } else if constexpr (Foundation::Concepts::Enum<T>) {
    auto Value = Parse<__underlying_type(T)>(Text, Options);
    if (!Value) return Failure(Value.Error());
    return Foundation::Cast::Auto<T>(Value.Value());
  } else if constexpr (Foundation::Traits::IsSame<T, Library::U8String>::Value) {
    Library::U8String Value{};
    if (!Value.Assign(Text)) {
      return Failure(
          Error::Semantic(ErrorCode::OutOfMemory, Operation::Allocate));
    }
    return Value;
  } else if constexpr (requires { Parser<T>::Parse(Text, Options); }) {
    return Parser<T>::Parse(Text, Options);
  } else {
    static_assert(sizeof(T) == 0, "No IO::Parse implementation for this type");
  }
}

template <typename T, InputSource TSource, Foundation::Size Capacity>
[[nodiscard]] auto Scan(TextReader<TSource, Capacity> &Reader,
                        ParseOptions Options = {}) -> Result<T> {
  auto Token = Reader.ReadToken();
  if (!Token) return Failure(Token.Error());
  return Parse<T>(Token.Value().View(), Options);
}

namespace Detail {
template <Foundation::Size Index, typename TTuple, typename TFirst,
          typename... TRest,
          InputSource TSource,
          Foundation::Size Capacity>
[[nodiscard]] auto ScanInto(TextReader<TSource, Capacity> &Reader,
                            TTuple &Values,
                            ParseOptions Options) -> Result<> {
  auto First = Scan<TFirst>(Reader, Options);
  if (!First) return Failure(First.Error());
  Library::Get<Index>(Values) = Foundation::Utility::Move(First.Value());
  if constexpr (sizeof...(TRest)) {
    return ScanInto<Index + 1, TTuple, TRest...>(Reader, Values, Options);
  } else {
    return {};
  }
}
} // namespace Detail

template <typename TFirst, typename TSecond, typename... TRest,
          InputSource TSource, Foundation::Size Capacity>
[[nodiscard]] auto Scan(TextReader<TSource, Capacity> &Reader,
                        ParseOptions Options = {})
    -> Result<Library::Tuple<TFirst, TSecond, TRest...>> {
  Library::Tuple<TFirst, TSecond, TRest...> Values{};
  if constexpr (Seekable<TSource>) {
    auto Checkpoint = Reader.Save();
    if (!Checkpoint) return Failure(Checkpoint.Error());
    auto Scanned = Detail::ScanInto<
        0, Library::Tuple<TFirst, TSecond, TRest...>, TFirst, TSecond,
        TRest...>(
        Reader, Values, Options);
    if (!Scanned) {
      auto Restored = Reader.Restore(Checkpoint.Value());
      if (!Restored) return Failure(Restored.Error());
      return Failure(Scanned.Error());
    }
    return Values;
  } else {
    auto Scanned = Detail::ScanInto<
        0, Library::Tuple<TFirst, TSecond, TRest...>, TFirst, TSecond,
        TRest...>(
        Reader, Values, Options);
    if (!Scanned) return Failure(Scanned.Error());
    return Values;
  }
}
} // namespace UEFIpp::IO
