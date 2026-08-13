#pragma once

#include <UEFIpp/IO/Adapter/Utility.hpp>
#include <UEFIpp/IO/Core/WriterRef.hpp>
#include <UEFIpp/IO/Text/Writer.hpp>
#include <UEFIpp/Library/Containers/Optional.hpp>
#include <UEFIpp/Library/Containers/Expected.hpp>
#include <UEFIpp/Library/Containers/Tuple.hpp>
#include <UEFIpp/UEFI/Guid.hpp>
#include <UEFIpp/UEFI/Status.hpp>

namespace UEFIpp::IO {
enum class Alignment : Foundation::Uint8 { Default, Left, Right, Center };
enum class Sign : Foundation::Uint8 { NegativeOnly, Always, Space };

struct FormatSpec final {
  Foundation::Char8 Fill{u8' '};
  Alignment Align{Alignment::Default};
  Sign SignMode{Sign::NegativeOnly};
  Foundation::Size Width{};
  Foundation::Size Precision{Foundation::Size(-1)};
  Foundation::Char8 Presentation{};
  Foundation::Bool Alternate{};
  Foundation::Bool ZeroPad{};
  Foundation::Bool Uppercase{};
  Foundation::Bool Debug{};
};

template <typename... TArguments>
class FormatString final {
public:
  template <Foundation::Size N>
  consteval FormatString(const Foundation::Char (&Text)[N])
      : Text_(Text), Size_(N - 1) {
    Foundation::Size Fields{};
    for (Foundation::Size Index{}; Index < Size_; ++Index) {
      if (Text[Index] == '{') {
        if (Index + 1 < Size_ && Text[Index + 1] == '{') {
          ++Index;
          continue;
        }
        ++Fields;
        const auto FieldStart = Index + 1;
        while (++Index < Size_ && Text[Index] != '}') {
          if (Text[Index] == '{') {
            throw "nested replacement field";
          }
        }
        if (Index == Size_) {
          throw "unterminated replacement field";
        }
        const auto FieldSize = Index - FieldStart;
        if (FieldSize) {
          if (Text_[FieldStart] != ':') {
            throw "only automatic replacement fields are supported";
          }
          ValidateSpec(Text_ + FieldStart + 1, FieldSize - 1);
        }
      } else if (Text[Index] == '}') {
        if (Index + 1 < Size_ && Text[Index + 1] == '}') {
          ++Index;
          continue;
        }
        throw "unmatched closing brace";
      }
    }
    if (Fields != sizeof...(TArguments)) {
      throw "replacement field count does not match argument count";
    }
  }

  [[nodiscard]] constexpr auto View() const noexcept -> Library::StringView {
    return {Text_, Size_};
  }

private:
  static consteval auto ValidateSpec(const Foundation::Char *Text,
                                     Foundation::Size Size)
      -> Foundation::Void {
    Foundation::Size Index{};
    if (Index + 1 < Size &&
        (Text[Index + 1] == '<' || Text[Index + 1] == '>' ||
         Text[Index + 1] == '^')) {
      ++Index;
    }
    if (Index < Size &&
        (Text[Index] == '<' || Text[Index] == '>' || Text[Index] == '^'))
      ++Index;
    if (Index < Size &&
        (Text[Index] == '+' || Text[Index] == '-' || Text[Index] == ' '))
      ++Index;
    if (Index < Size && Text[Index] == '#') ++Index;
    if (Index < Size && Text[Index] == '0') ++Index;
    while (Index < Size && Text[Index] >= '0' && Text[Index] <= '9') ++Index;
    if (Index < Size && Text[Index] == '.') {
      ++Index;
      while (Index < Size && Text[Index] >= '0' && Text[Index] <= '9') ++Index;
    }
    if (Index < Size) {
      constexpr Library::StringView Presentations{"aAeEfFgGboxXBdpc?"};
      if (!Presentations.Contains(Text[Index])) {
        throw "unknown presentation type";
      }
      ++Index;
    }
    if (Index != Size) throw "invalid format specification";
  }

  const Foundation::Char *Text_{};
  Foundation::Size Size_{};
};

namespace Detail {
[[nodiscard]] inline auto ValidatePresentation(
    const FormatSpec &Spec, Library::U8StringView Allowed) -> Result<> {
  if (!Spec.Presentation || Allowed.Contains(Spec.Presentation)) return {};
  return Failure(
      Error::Semantic(ErrorCode::InvalidFormat, Operation::Format));
}

[[nodiscard]] inline auto WriteBytes(WriterRef Writer,
                                     const Foundation::Char8 *Data,
                                     Foundation::Size Size) -> Result<> {
  return WriteAll(Writer,
                  {Foundation::Cast::Auto<const Foundation::Byte *>(Data), Size});
}

[[nodiscard]] inline auto WriteAscii(WriterRef Writer,
                                     const Foundation::Char *Data,
                                     Foundation::Size Size) -> Result<> {
  return WriteAll(Writer,
                  {Foundation::Cast::Auto<const Foundation::Byte *>(Data), Size});
}

[[nodiscard]] inline auto Repeat(WriterRef Writer, Foundation::Char8 Value,
                                 Foundation::Size Count) -> Result<> {
  Foundation::Char8 Buffer[32]{};
  for (auto &Entry : Buffer) {
    Entry = Value;
  }
  while (Count) {
    const auto Chunk = Count < sizeof(Buffer) ? Count : sizeof(Buffer);
    auto Result = WriteBytes(Writer, Buffer, Chunk);
    if (!Result) {
      return Result;
    }
    Count -= Chunk;
  }
  return {};
}

[[nodiscard]] inline auto WritePadded(WriterRef Writer,
                                      Library::U8StringView Value,
                                      const FormatSpec &Spec) -> Result<> {
  const auto Padding = Spec.Width > Value.Size() ? Spec.Width - Value.Size() : 0;
  const auto AlignmentValue =
      Spec.Align == Alignment::Default ? Alignment::Left : Spec.Align;
  const auto Left = AlignmentValue == Alignment::Right
                        ? Padding
                        : AlignmentValue == Alignment::Center ? Padding / 2 : 0;
  const auto Right = Padding - Left;

  auto Result = Repeat(Writer, Spec.Fill, Left);
  if (!Result) {
    return Result;
  }
  Result = WriteText(Writer, Value);
  if (!Result) {
    return Result;
  }
  return Repeat(Writer, Spec.Fill, Right);
}

[[nodiscard]] inline auto WriteFormattedText(WriterRef Writer,
                                             Library::U8StringView Value,
                                             const FormatSpec &Spec)
    -> Result<> {
  auto Length = Value.Size();
  if (Spec.Precision != Foundation::Size(-1) && Spec.Precision < Length) {
    Length = Spec.Precision;
    while (Length && Length < Value.Size() &&
           (Value[Length] & 0xC0) == 0x80) {
      --Length;
    }
  }
  Value = {Value.Data(), Length};
  if (!Spec.Debug) return WritePadded(Writer, Value, Spec);

  Foundation::Size EncodedLength{2};
  for (const auto Character : Value) {
    const auto Byte = Foundation::Cast::Auto<Foundation::Byte>(Character);
    EncodedLength += Character == u8'"' || Character == u8'\\' ||
                             Character == u8'\n' || Character == u8'\r' ||
                             Character == u8'\t'
                         ? 2
                         : Byte < 0x20 || Byte == 0x7F ? 4 : 1;
  }
  const auto Padding = Spec.Width > EncodedLength
                           ? Spec.Width - EncodedLength
                           : Foundation::Size{};
  const auto Align = Spec.Align == Alignment::Default ? Alignment::Left
                                                       : Spec.Align;
  const auto Left = Align == Alignment::Right
                        ? Padding
                        : Align == Alignment::Center ? Padding / 2 : 0;
  auto Result = Repeat(Writer, Spec.Fill, Left);
  if (!Result) return Result;
  Result = WriteBytes(Writer, u8"\"", 1);
  if (!Result) return Result;
  for (const auto Character : Value) {
    Foundation::Char8 Escaped[4]{};
    Foundation::Size Count{};
    switch (Character) {
    case u8'"': Escaped[0] = u8'\\'; Escaped[1] = u8'"'; Count = 2; break;
    case u8'\\': Escaped[0] = u8'\\'; Escaped[1] = u8'\\'; Count = 2; break;
    case u8'\n': Escaped[0] = u8'\\'; Escaped[1] = u8'n'; Count = 2; break;
    case u8'\r': Escaped[0] = u8'\\'; Escaped[1] = u8'r'; Count = 2; break;
    case u8'\t': Escaped[0] = u8'\\'; Escaped[1] = u8't'; Count = 2; break;
    default: {
      const auto Byte = Foundation::Cast::Auto<Foundation::Byte>(Character);
      if (Byte < 0x20 || Byte == 0x7F) {
        Escaped[0] = u8'\\';
        Escaped[1] = u8'x';
        const auto High = Foundation::Cast::Auto<Foundation::Uint8>(Byte >> 4);
        const auto Low = Foundation::Cast::Auto<Foundation::Uint8>(Byte & 0xF);
        Escaped[2] = Foundation::Cast::Auto<Foundation::Char8>(
            High < 10 ? u8'0' + High : u8'A' + High - 10);
        Escaped[3] = Foundation::Cast::Auto<Foundation::Char8>(
            Low < 10 ? u8'0' + Low : u8'A' + Low - 10);
        Count = 4;
      } else {
        Escaped[0] = Character;
        Count = 1;
      }
      break;
    }
    }
    Result = WriteBytes(Writer, Escaped, Count);
    if (!Result) return Result;
  }
  Result = WriteBytes(Writer, u8"\"", 1);
  if (!Result) return Result;
  return Repeat(Writer, Spec.Fill, Padding - Left);
}

[[nodiscard]] constexpr auto Digit(Foundation::Uint8 Value,
                                   Foundation::Bool Uppercase)
    -> Foundation::Char8 {
  return Value < 10
             ? Foundation::Cast::Auto<Foundation::Char8>(u8'0' + Value)
             : Foundation::Cast::Auto<Foundation::Char8>(
                   (Uppercase ? u8'A' : u8'a') + Value - 10);
}

[[nodiscard]] inline auto FormatUnsigned(WriterRef Writer,
                                         Foundation::Uint64 Value,
                                         Foundation::Bool Negative,
                                         FormatSpec Spec) -> Result<> {
  Foundation::Uint8 Base{10};
  switch (Spec.Presentation) {
  case u8'b':
  case u8'B':
    Base = 2;
    break;
  case u8'o':
    Base = 8;
    break;
  case u8'x':
  case u8'X':
  case u8'p':
    Base = 16;
    break;
  default:
    break;
  }
  Spec.Uppercase = Spec.Uppercase || Spec.Presentation == u8'X' ||
                   Spec.Presentation == u8'B';

  Foundation::Char8 Digits[65]{};
  Foundation::Size DigitCount{};
  do {
    Digits[DigitCount++] = Digit(
        Foundation::Cast::Auto<Foundation::Uint8>(Value % Base),
        Spec.Uppercase);
    Value /= Base;
  } while (Value);

  Foundation::Char8 Prefix[3]{};
  Foundation::Size PrefixLength{};
  if (Negative) {
    Prefix[PrefixLength++] = u8'-';
  } else if (Spec.SignMode == Sign::Always) {
    Prefix[PrefixLength++] = u8'+';
  } else if (Spec.SignMode == Sign::Space) {
    Prefix[PrefixLength++] = u8' ';
  }
  if (Spec.Alternate || Spec.Presentation == u8'p') {
    if (Base == 2) {
      Prefix[PrefixLength++] = u8'0';
      Prefix[PrefixLength++] = Spec.Uppercase ? u8'B' : u8'b';
    } else if (Base == 8) {
      Prefix[PrefixLength++] = u8'0';
    } else if (Base == 16) {
      Prefix[PrefixLength++] = u8'0';
      Prefix[PrefixLength++] = Spec.Uppercase ? u8'X' : u8'x';
    }
  }

  const auto RawLength = PrefixLength + DigitCount;
  const auto Padding = Spec.Width > RawLength ? Spec.Width - RawLength : 0;
  auto AlignmentValue =
      Spec.Align == Alignment::Default ? Alignment::Right : Spec.Align;
  if (Spec.ZeroPad && Spec.Align == Alignment::Default) {
    auto Result = WriteBytes(Writer, Prefix, PrefixLength);
    if (!Result) {
      return Result;
    }
    Result = Repeat(Writer, u8'0', Padding);
    if (!Result) {
      return Result;
    }
  } else {
    const auto Left = AlignmentValue == Alignment::Right
                          ? Padding
                          : AlignmentValue == Alignment::Center ? Padding / 2
                                                                 : 0;
    auto Result = Repeat(Writer, Spec.Fill, Left);
    if (!Result) {
      return Result;
    }
    Result = WriteBytes(Writer, Prefix, PrefixLength);
    if (!Result) {
      return Result;
    }
  }

  while (DigitCount) {
    auto Result = WriteBytes(Writer, &Digits[--DigitCount], 1);
    if (!Result) {
      return Result;
    }
  }

  if (!(Spec.ZeroPad && Spec.Align == Alignment::Default)) {
    const auto Left = AlignmentValue == Alignment::Right
                          ? Padding
                          : AlignmentValue == Alignment::Center ? Padding / 2
                                                                 : 0;
    return Repeat(Writer, Spec.Fill, Padding - Left);
  }
  return {};
}

[[nodiscard]] inline auto FormatFloat(WriterRef Writer,
                                      Foundation::Float64 Value,
                                      FormatSpec Spec) -> Result<> {
  const auto Bits = Foundation::Cast::Bitwise<Foundation::Uint64>(Value);
  const auto Negative = (Bits >> 63) != 0;
  const auto ExponentBits = (Bits >> 52) & 0x7FF;
  const auto FractionBits = Bits & ((Foundation::Uint64{1} << 52) - 1);
  const auto Uppercase = Spec.Presentation == u8'E' ||
                         Spec.Presentation == u8'F' ||
                         Spec.Presentation == u8'G' ||
                         Spec.Presentation == u8'A';

  Foundation::Char8 Buffer[160]{};
  Foundation::Size Length{};
  const auto Put = [&](Foundation::Char8 Character) {
    if (Length < sizeof(Buffer)) {
      Buffer[Length++] = Character;
    }
  };

  if (Negative) {
    Put(u8'-');
    Value = -Value;
  } else if (Spec.SignMode == Sign::Always) {
    Put(u8'+');
  } else if (Spec.SignMode == Sign::Space) {
    Put(u8' ');
  }

  if (ExponentBits == 0x7FF) {
    const auto *Word = FractionBits ? (Uppercase ? u8"NAN" : u8"nan")
                                    : (Uppercase ? u8"INF" : u8"inf");
    for (Foundation::Size Index{}; Index < 3; ++Index) {
      Put(Word[Index]);
    }
    return WritePadded(Writer, {Buffer, Length}, Spec);
  }

  if (Spec.Presentation == u8'a' || Spec.Presentation == u8'A') {
    Put(u8'0');
    Put(Uppercase ? u8'X' : u8'x');
    Put(ExponentBits ? u8'1' : u8'0');
    const auto HexPrecision =
        Spec.Precision == Foundation::Size(-1)
            ? Foundation::Size{13}
            : (Spec.Precision > 13 ? Foundation::Size{13} : Spec.Precision);
    if (HexPrecision || Spec.Alternate) {
      Put(u8'.');
    }
    auto Fraction = FractionBits;
    for (Foundation::Size Index{}; Index < HexPrecision; ++Index) {
      const auto Shift = 48 - Index * 4;
      Put(Digit(Foundation::Cast::Auto<Foundation::Uint8>(
                    (Fraction >> Shift) & 0xF),
                Uppercase));
    }
    Put(Uppercase ? u8'P' : u8'p');
    const auto BinaryExponent = ExponentBits
                                    ? Foundation::Cast::Auto<Foundation::Int32>(
                                          ExponentBits) -
                                          1023
                                    : FractionBits ? -1022 : 0;
    Put(BinaryExponent < 0 ? u8'-' : u8'+');
    auto Magnitude = Foundation::Cast::Auto<Foundation::Uint32>(
        BinaryExponent < 0 ? -BinaryExponent : BinaryExponent);
    Foundation::Char8 Exponent[12]{};
    Foundation::Size Count{};
    do {
      Exponent[Count++] =
          Foundation::Cast::Auto<Foundation::Char8>(u8'0' + Magnitude % 10);
      Magnitude /= 10;
    } while (Magnitude);
    while (Count) {
      Put(Exponent[--Count]);
    }
    return WritePadded(Writer, {Buffer, Length}, Spec);
  }

  auto Precision = Spec.Precision == Foundation::Size(-1)
                       ? Foundation::Size{6}
                       : (Spec.Precision > 18 ? Foundation::Size{18}
                                              : Spec.Precision);

  Foundation::Int32 DecimalExponent{};
  auto Normalized = Value;
  if (Normalized != 0.0) {
    while (Normalized >= 10.0 && DecimalExponent < 308) {
      Normalized /= 10.0;
      ++DecimalExponent;
    }
    while (Normalized < 1.0 && DecimalExponent > -308) {
      Normalized *= 10.0;
      --DecimalExponent;
    }
  }

  const auto General = !Spec.Presentation || Spec.Presentation == u8'g' ||
                       Spec.Presentation == u8'G';
  if (General && !Precision) Precision = 1;
  const auto Scientific =
      Spec.Presentation == u8'e' || Spec.Presentation == u8'E' ||
      (General &&
       (DecimalExponent < -4 ||
        DecimalExponent >=
            Foundation::Cast::Auto<Foundation::Int32>(Precision)));
  auto FractionPrecision = Precision;
  if (General) {
    const auto Needed = Foundation::Cast::Auto<Foundation::Int32>(Precision) -
                        (Scientific ? 1 : DecimalExponent + 1);
    FractionPrecision = Needed > 0
                            ? Foundation::Cast::Auto<Foundation::Size>(Needed)
                            : Foundation::Size{};
  }
  auto RenderValue = Scientific ? Normalized : Value;
  Foundation::Uint64 Scale{1};
  for (Foundation::Size Index{}; Index < FractionPrecision; ++Index) {
    Scale *= 10;
  }
  if (RenderValue > Foundation::Cast::Auto<Foundation::Float64>(
                        Foundation::Uint64(-1)) /
                        Foundation::Cast::Auto<Foundation::Float64>(Scale)) {
    return Failure(Error::Semantic(ErrorCode::Overflow, Operation::Format));
  }
  auto Scaled = Foundation::Cast::Auto<Foundation::Uint64>(
      RenderValue * Foundation::Cast::Auto<Foundation::Float64>(Scale) + 0.5);
  if (Scientific && Scale && Scaled >= Scale * 10) {
    Scaled /= 10;
    ++DecimalExponent;
  }
  auto Integer = Scale ? Scaled / Scale : Scaled;
  auto Fraction = Scale ? Scaled % Scale : 0;

  Foundation::Char8 IntegerDigits[24]{};
  Foundation::Size IntegerCount{};
  do {
    IntegerDigits[IntegerCount++] = Foundation::Cast::Auto<Foundation::Char8>(
        u8'0' + Integer % 10);
    Integer /= 10;
  } while (Integer);
  while (IntegerCount) {
    Put(IntegerDigits[--IntegerCount]);
  }
  if (FractionPrecision || Spec.Alternate) {
    Put(u8'.');
  }
  auto Divisor = Scale / 10;
  while (Divisor) {
    Put(Foundation::Cast::Auto<Foundation::Char8>(
        u8'0' + (Fraction / Divisor) % 10));
    Divisor /= 10;
  }
  if (General && !Spec.Alternate && FractionPrecision) {
    while (Length && Buffer[Length - 1] == u8'0') --Length;
    if (Length && Buffer[Length - 1] == u8'.') --Length;
  }
  if (Scientific) {
    Put(Uppercase ? u8'E' : u8'e');
    Put(DecimalExponent < 0 ? u8'-' : u8'+');
    auto Exponent = Foundation::Cast::Auto<Foundation::Uint32>(
        DecimalExponent < 0 ? -DecimalExponent : DecimalExponent);
    if (Exponent < 10) {
      Put(u8'0');
    }
    Foundation::Char8 Digits[4]{};
    Foundation::Size Count{};
    do {
      Digits[Count++] = Foundation::Cast::Auto<Foundation::Char8>(
          u8'0' + Exponent % 10);
      Exponent /= 10;
    } while (Exponent);
    while (Count) {
      Put(Digits[--Count]);
    }
  }
  return WritePadded(Writer, {Buffer, Length}, Spec);
}

[[nodiscard]] inline auto ParseFormatSpec(Library::StringView Text,
                                          FormatSpec &Spec) -> Result<> {
  Foundation::Size Index{};
  if (Index + 1 < Text.Size() &&
      (Text[Index + 1] == '<' || Text[Index + 1] == '>' ||
       Text[Index + 1] == '^')) {
    Spec.Fill = Foundation::Cast::Auto<Foundation::Char8>(Text[Index++]);
  }
  if (Index < Text.Size()) {
    if (Text[Index] == '<') {
      Spec.Align = Alignment::Left;
      ++Index;
    } else if (Text[Index] == '>') {
      Spec.Align = Alignment::Right;
      ++Index;
    } else if (Text[Index] == '^') {
      Spec.Align = Alignment::Center;
      ++Index;
    }
  }
  if (Index < Text.Size() &&
      (Text[Index] == '+' || Text[Index] == '-' || Text[Index] == ' ')) {
    Spec.SignMode = Text[Index] == '+'   ? Sign::Always
                    : Text[Index] == ' ' ? Sign::Space
                                         : Sign::NegativeOnly;
    ++Index;
  }
  if (Index < Text.Size() && Text[Index] == '#') {
    Spec.Alternate = true;
    ++Index;
  }
  if (Index < Text.Size() && Text[Index] == '0') {
    Spec.ZeroPad = true;
    ++Index;
  }
  while (Index < Text.Size() && Text[Index] >= '0' && Text[Index] <= '9') {
    if (Spec.Width > (Foundation::Size(-1) - 9) / 10) {
      return Failure(Error::Semantic(ErrorCode::Overflow, Operation::Format));
    }
    Spec.Width = Spec.Width * 10 + Text[Index++] - '0';
  }
  if (Index < Text.Size() && Text[Index] == '.') {
    ++Index;
    Spec.Precision = 0;
    while (Index < Text.Size() && Text[Index] >= '0' && Text[Index] <= '9') {
      if (Spec.Precision > (Foundation::Size(-1) - 9) / 10) {
        return Failure(
            Error::Semantic(ErrorCode::Overflow, Operation::Format));
      }
      Spec.Precision = Spec.Precision * 10 + Text[Index++] - '0';
    }
  }
  if (Index < Text.Size()) {
    Spec.Presentation = Foundation::Cast::Auto<Foundation::Char8>(Text[Index++]);
    Spec.Uppercase = Spec.Presentation >= u8'A' && Spec.Presentation <= u8'Z';
    Spec.Debug = Spec.Presentation == u8'?';
  }
  if (Index != Text.Size()) {
    return Failure(
        Error::Semantic(ErrorCode::InvalidFormat, Operation::Format, Index));
  }
  return {};
}
} // namespace Detail

template <typename T>
struct Formatter;

template <Foundation::Concepts::Integral T>
struct Formatter<T> {
  [[nodiscard]] auto Format(WriterRef Writer, T Value,
                            const FormatSpec &Spec) const -> Result<> {
    if constexpr (Foundation::Traits::IsSame<T, Foundation::Bool>::Value) {
      auto Valid = Detail::ValidatePresentation(Spec, u8"s?");
      if (!Valid) return Valid;
      const auto Text = Value ? Library::U8StringView{u8"true"}
                              : Library::U8StringView{u8"false"};
      return Detail::WritePadded(Writer, Text, Spec);
    } else if constexpr (Foundation::Traits::IsSame<T, Foundation::Char>::Value ||
                         Foundation::Traits::IsSame<T, Foundation::Char8>::Value) {
      auto Valid = Detail::ValidatePresentation(Spec, u8"c?");
      if (!Valid) return Valid;
      const Foundation::Char8 Character =
          Foundation::Cast::Auto<Foundation::Char8>(Value);
      return Detail::WritePadded(Writer, {&Character, 1}, Spec);
    } else {
      auto Valid = Detail::ValidatePresentation(Spec, u8"dbBoxX");
      if (!Valid) return Valid;
      const auto Signed = T(-1) < T(0);
      const auto Negative = Signed && Value < 0;
      const auto Magnitude = Negative
                                 ? Foundation::Cast::Auto<Foundation::Uint64>(
                                       -(Foundation::Cast::Auto<Foundation::Int64>(
                                             Value) +
                                         1)) +
                                       1
                                 : Foundation::Cast::Auto<Foundation::Uint64>(Value);
      return Detail::FormatUnsigned(Writer, Magnitude, Negative, Spec);
    }
  }
};

template <Foundation::Concepts::FloatingPoint T>
struct Formatter<T> {
  [[nodiscard]] auto Format(WriterRef Writer, T Value,
                            const FormatSpec &Spec) const -> Result<> {
    auto Valid = Detail::ValidatePresentation(Spec, u8"aAeEfFgG");
    if (!Valid) return Valid;
    return Detail::FormatFloat(
        Writer, Foundation::Cast::Auto<Foundation::Float64>(Value), Spec);
  }
};

template <typename T>
struct Formatter<T *> {
  [[nodiscard]] auto Format(WriterRef Writer, const T *Value,
                            FormatSpec Spec) const -> Result<> {
    auto Valid = Detail::ValidatePresentation(Spec, u8"p");
    if (!Valid) return Valid;
    Spec.Presentation = u8'p';
    Spec.Alternate = true;
    if (!Spec.Width) {
      Spec.Width = sizeof(Foundation::UintPtr) * 2 + 2;
      Spec.ZeroPad = true;
    }
    return Detail::FormatUnsigned(
        Writer, Foundation::Cast::PointerToAddress<Foundation::UintPtr>(Value),
        false, Spec);
  }
};

template <typename TChar>
struct Formatter<Library::BasicStringView<TChar>> {
  [[nodiscard]] auto Format(WriterRef Writer,
                            Library::BasicStringView<TChar> Value,
                            const FormatSpec &Spec) const -> Result<> {
    auto Valid = Detail::ValidatePresentation(Spec, u8"s?");
    if (!Valid) return Valid;
    if constexpr (Foundation::Traits::IsSame<TChar, Foundation::Char8>::Value) {
      return Detail::WriteFormattedText(Writer, Value, Spec);
    } else if constexpr (Foundation::Traits::IsSame<TChar, Foundation::Char>::Value) {
      return Detail::WriteFormattedText(Writer, AsUtf8(Value), Spec);
    } else if constexpr (
        Foundation::Traits::IsSame<TChar, Foundation::Char16>::Value ||
        Foundation::Traits::IsSame<TChar, Foundation::WChar>::Value) {
      auto Converted = Utf16ToUtf8(
          {Foundation::Cast::Auto<const Foundation::Char16 *>(Value.Data()),
           Value.Size()});
      if (!Converted) return Failure(Converted.Error());
      return Detail::WriteFormattedText(Writer, Converted.Value().View(), Spec);
    } else {
      MemorySink Buffer{};
      for (const auto Unit : Value) {
        Foundation::Char32 CodePoint =
            Foundation::Cast::Auto<Foundation::Char32>(Unit);
        Foundation::Byte Encoded[4]{};
        auto Count = EncodeUtf8(CodePoint, Encoded);
        if (!Count) {
          return Failure(Count.Error());
        }
        auto Result = WriteAll(Buffer, {Encoded, Count.Value()});
        if (!Result) {
          return Result;
        }
      }
      return Detail::WriteFormattedText(
          Writer,
          {Foundation::Cast::Auto<const Foundation::Char8 *>(
               Buffer.Buffer().Data()),
           Buffer.Buffer().Size()},
          Spec);
    }
  }
};

template <typename TChar>
struct Formatter<Library::BasicString<TChar>> {
  [[nodiscard]] auto Format(WriterRef Writer,
                            const Library::BasicString<TChar> &Value,
                            const FormatSpec &Spec) const -> Result<> {
    return Formatter<Library::BasicStringView<TChar>>{}.Format(Writer,
                                                               Value.View(), Spec);
  }
};

template <>
struct Formatter<const Foundation::Char *> {
  [[nodiscard]] auto Format(WriterRef Writer, const Foundation::Char *Value,
                            const FormatSpec &Spec) const -> Result<> {
    return Formatter<Library::StringView>{}.Format(
        Writer, Value ? Library::StringView{Value} : Library::StringView{"null"},
        Spec);
  }
};

template <>
struct Formatter<Foundation::Char *> : Formatter<const Foundation::Char *> {};

template <>
struct Formatter<const Foundation::Char8 *> {
  [[nodiscard]] auto Format(WriterRef Writer, const Foundation::Char8 *Value,
                            const FormatSpec &Spec) const -> Result<> {
    return Formatter<Library::U8StringView>{}.Format(
        Writer,
        Value ? Library::U8StringView{Value} : Library::U8StringView{u8"null"},
        Spec);
  }
};

template <>
struct Formatter<Foundation::Char8 *>
    : Formatter<const Foundation::Char8 *> {};

template <>
struct Formatter<const Foundation::WChar *> {
  [[nodiscard]] auto Format(WriterRef Writer, const Foundation::WChar *Value,
                            const FormatSpec &Spec) const -> Result<> {
    return Formatter<Library::WideStringView>{}.Format(
        Writer, Value ? Library::WideStringView{Value}
                      : Library::WideStringView{L"null"},
        Spec);
  }
};

template <>
struct Formatter<Foundation::WChar *>
    : Formatter<const Foundation::WChar *> {};

template <>
struct Formatter<const Foundation::Char16 *> {
  [[nodiscard]] auto Format(WriterRef Writer, const Foundation::Char16 *Value,
                            const FormatSpec &Spec) const -> Result<> {
    return Formatter<Library::U16StringView>{}.Format(
        Writer, Value ? Library::U16StringView{Value}
                      : Library::U16StringView{u"null"},
        Spec);
  }
};

template <>
struct Formatter<Foundation::Char16 *>
    : Formatter<const Foundation::Char16 *> {};

template <>
struct Formatter<const Foundation::Char32 *> {
  [[nodiscard]] auto Format(WriterRef Writer, const Foundation::Char32 *Value,
                            const FormatSpec &Spec) const -> Result<> {
    return Formatter<Library::U32StringView>{}.Format(
        Writer, Value ? Library::U32StringView{Value}
                      : Library::U32StringView{U"null"},
        Spec);
  }
};

template <>
struct Formatter<Foundation::Char32 *>
    : Formatter<const Foundation::Char32 *> {};

template <>
struct Formatter<Foundation::Char16> {
  [[nodiscard]] auto Format(WriterRef Writer, Foundation::Char16 Value,
                            const FormatSpec &Spec) const -> Result<> {
    auto Valid = Detail::ValidatePresentation(Spec, u8"c?");
    if (!Valid) return Valid;
    if (Value >= 0xD800 && Value <= 0xDFFF)
      return Failure(
          Error::Semantic(ErrorCode::InvalidEncoding, Operation::Format));
    Foundation::Byte Encoded[4]{};
    auto Count = EncodeUtf8(Value, Encoded);
    if (!Count) return Failure(Count.Error());
    return Detail::WriteFormattedText(
        Writer,
        {Foundation::Cast::Auto<const Foundation::Char8 *>(&Encoded[0]),
         Count.Value()},
        Spec);
  }
};

template <>
struct Formatter<Foundation::Char32> {
  [[nodiscard]] auto Format(WriterRef Writer, Foundation::Char32 Value,
                            const FormatSpec &Spec) const -> Result<> {
    auto Valid = Detail::ValidatePresentation(Spec, u8"c?");
    if (!Valid) return Valid;
    Foundation::Byte Encoded[4]{};
    auto Count = EncodeUtf8(Value, Encoded);
    if (!Count) return Failure(Count.Error());
    return Detail::WriteFormattedText(
        Writer,
        {Foundation::Cast::Auto<const Foundation::Char8 *>(&Encoded[0]),
         Count.Value()},
        Spec);
  }
};

template <>
struct Formatter<Foundation::WChar> {
  [[nodiscard]] auto Format(WriterRef Writer, Foundation::WChar Value,
                            const FormatSpec &Spec) const -> Result<> {
    return Formatter<Foundation::Char16>{}.Format(
        Writer, Foundation::Cast::Auto<Foundation::Char16>(Value), Spec);
  }
};

template <Foundation::Size N>
struct Formatter<Foundation::Char[N]> {
  [[nodiscard]] auto Format(WriterRef Writer,
                            const Foundation::Char (&Value)[N],
                            const FormatSpec &Spec) const -> Result<> {
    return Formatter<Library::StringView>{}.Format(
        Writer, {Value, N && Value[N - 1] == 0 ? N - 1 : N}, Spec);
  }
};

template <Foundation::Size N>
struct Formatter<Foundation::Char8[N]> {
  [[nodiscard]] auto Format(WriterRef Writer,
                            const Foundation::Char8 (&Value)[N],
                            const FormatSpec &Spec) const -> Result<> {
    return Formatter<Library::U8StringView>{}.Format(
        Writer, {Value, N && Value[N - 1] == 0 ? N - 1 : N}, Spec);
  }
};

template <Foundation::Size N>
struct Formatter<Foundation::Char16[N]> {
  [[nodiscard]] auto Format(WriterRef Writer,
                            const Foundation::Char16 (&Value)[N],
                            const FormatSpec &Spec) const -> Result<> {
    return Formatter<Library::U16StringView>{}.Format(
        Writer, {Value, N && Value[N - 1] == 0 ? N - 1 : N}, Spec);
  }
};

template <Foundation::Size N>
struct Formatter<Foundation::WChar[N]> {
  [[nodiscard]] auto Format(WriterRef Writer,
                            const Foundation::WChar (&Value)[N],
                            const FormatSpec &Spec) const -> Result<> {
    return Formatter<Library::WideStringView>{}.Format(
        Writer, {Value, N && Value[N - 1] == 0 ? N - 1 : N}, Spec);
  }
};

template <Foundation::Size N>
struct Formatter<Foundation::Char32[N]> {
  [[nodiscard]] auto Format(WriterRef Writer,
                            const Foundation::Char32 (&Value)[N],
                            const FormatSpec &Spec) const -> Result<> {
    return Formatter<Library::U32StringView>{}.Format(
        Writer, {Value, N && Value[N - 1] == 0 ? N - 1 : N}, Spec);
  }
};

template <>
struct Formatter<UEFI::Guid> {
  [[nodiscard]] auto Format(WriterRef Writer, const UEFI::Guid &Value,
                            const FormatSpec &Spec) const -> Result<> {
    auto Valid = Detail::ValidatePresentation(Spec, u8"xX?");
    if (!Valid) return Valid;
    FormatSpec Part = Spec;
    Part.Width = 0;
    Part.ZeroPad = true;
    Part.Presentation = Spec.Uppercase ? u8'X' : u8'x';
    const auto Hex = [&](Foundation::Uint64 Number,
                         Foundation::Size Width) -> Result<> {
      Part.Width = Width;
      return Detail::FormatUnsigned(Writer, Number, false, Part);
    };
    auto Result = Hex(Value.Data1, 8);
    if (!Result) return Result;
    Result = Detail::WriteBytes(Writer, u8"-", 1);
    if (!Result) return Result;
    Result = Hex(Value.Data2, 4);
    if (!Result) return Result;
    Result = Detail::WriteBytes(Writer, u8"-", 1);
    if (!Result) return Result;
    Result = Hex(Value.Data3, 4);
    if (!Result) return Result;
    Result = Detail::WriteBytes(Writer, u8"-", 1);
    if (!Result) return Result;
    for (Foundation::Size Index{}; Index < 8; ++Index) {
      if (Index == 2) {
        Result = Detail::WriteBytes(Writer, u8"-", 1);
        if (!Result) return Result;
      }
      Result = Hex(Value.Data4[Index], 2);
      if (!Result) return Result;
    }
    return {};
  }
};

template <>
struct Formatter<UEFI::Status> {
  [[nodiscard]] auto Format(WriterRef Writer, UEFI::Status Value,
                            const FormatSpec &Spec) const -> Result<> {
    auto Valid = Detail::ValidatePresentation(Spec, u8"s?");
    if (!Valid) return Valid;
    if (const auto *Name = UEFI::StatusName(Value.Code())) {
      return Formatter<const Foundation::Char *>{}.Format(Writer, Name, Spec);
    }
    auto Result = Detail::WriteBytes(Writer, u8"UnknownStatus(", 14);
    if (!Result) return Result;
    FormatSpec Hex{};
    Hex.Presentation = u8'X';
    Hex.Alternate = true;
    Result = Detail::FormatUnsigned(Writer, Value.Value(), false, Hex);
    if (!Result) return Result;
    return Detail::WriteBytes(Writer, u8")", 1);
  }
};

template <>
struct Formatter<Error> {
  [[nodiscard]] auto Format(WriterRef Writer, const Error &Value,
                            const FormatSpec &Spec) const -> Result<> {
    auto Valid = Detail::ValidatePresentation(Spec, u8"?");
    if (!Valid) return Valid;
    auto Result = Detail::WriteBytes(Writer, u8"io_error(code=", 14);
    if (!Result) return Result;
    Result = Formatter<Foundation::Uint8>{}.Format(
        Writer, Foundation::Cast::Underlying(Value.Code), {});
    if (!Result) return Result;
    Result = Detail::WriteBytes(Writer, u8", operation=", 12);
    if (!Result) return Result;
    Result = Formatter<Foundation::Uint8>{}.Format(
        Writer, Foundation::Cast::Underlying(Value.During), {});
    if (!Result) return Result;
    if (Value.Status.Value()) {
      Result = Detail::WriteBytes(Writer, u8", status=", 9);
      if (!Result) return Result;
      Result = Formatter<UEFI::Status>{}.Format(Writer, Value.Status, {});
      if (!Result) return Result;
    }
    Result = Detail::WriteBytes(Writer, u8", offset=", 9);
    if (!Result) return Result;
    Result = Formatter<Foundation::Uint64>{}.Format(Writer, Value.Offset, {});
    if (!Result) return Result;
    Result = Detail::WriteBytes(Writer, u8", transferred=", 14);
    if (!Result) return Result;
    Result = Formatter<Foundation::Size>{}.Format(Writer, Value.Transferred, {});
    if (!Result) return Result;
    return Detail::WriteBytes(Writer, u8")", 1);
  }
};

template <Foundation::Concepts::Enum T>
struct Formatter<T> {
  [[nodiscard]] auto Format(WriterRef Writer, T Value,
                            const FormatSpec &Spec) const -> Result<> {
    return Formatter<__underlying_type(T)>{}.Format(
        Writer, Foundation::Cast::Underlying(Value), Spec);
  }
};

template <typename T>
struct Formatter<Library::Optional<T>> {
  [[nodiscard]] auto Format(WriterRef Writer,
                            const Library::Optional<T> &Value,
                            const FormatSpec &Spec) const -> Result<> {
    if (!Value) {
      return Detail::WritePadded(Writer, u8"nullopt", Spec);
    }
    return Formatter<T>{}.Format(Writer, Value.Value(), Spec);
  }
};

template <>
struct Formatter<Foundation::NullPtr> {
  [[nodiscard]] auto Format(WriterRef Writer, Foundation::NullPtr,
                            const FormatSpec &Spec) const -> Result<> {
    return Detail::WriteFormattedText(Writer, u8"nullptr", Spec);
  }
};

template <typename T, typename E>
struct Formatter<Library::Expected<T, E>> {
  [[nodiscard]] auto Format(WriterRef Writer,
                            const Library::Expected<T, E> &Value,
                            const FormatSpec &Spec) const -> Result<> {
    auto Valid = Detail::ValidatePresentation(Spec, u8"?");
    if (!Valid) return Valid;
    auto Result = Detail::WriteBytes(
        Writer, Value ? u8"expected(" : u8"unexpected(",
        Value ? 9 : 11);
    if (!Result) return Result;
    FormatSpec Nested{};
    Nested.Debug = Spec.Debug;
    if (Value) Result = Formatter<T>{}.Format(Writer, Value.Value(), Nested);
    else Result = Formatter<E>{}.Format(Writer, Value.Error(), Nested);
    if (!Result) return Result;
    return Detail::WriteBytes(Writer, u8")", 1);
  }
};

template <typename E>
struct Formatter<Library::Expected<Foundation::Void, E>> {
  [[nodiscard]] auto Format(
      WriterRef Writer,
      const Library::Expected<Foundation::Void, E> &Value,
      const FormatSpec &Spec) const -> Result<> {
    auto Valid = Detail::ValidatePresentation(Spec, u8"?");
    if (!Valid) return Valid;
    if (Value) return Detail::WriteBytes(Writer, u8"expected()", 10);
    auto Result = Detail::WriteBytes(Writer, u8"unexpected(", 11);
    if (!Result) return Result;
    FormatSpec Nested{};
    Nested.Debug = Spec.Debug;
    Result = Formatter<E>{}.Format(Writer, Value.Error(), Nested);
    if (!Result) return Result;
    return Detail::WriteBytes(Writer, u8")", 1);
  }
};

namespace Detail {
template <typename T>
struct IsTextRange : Foundation::Traits::False {};
template <typename TChar>
struct IsTextRange<Library::BasicString<TChar>> : Foundation::Traits::True {};
template <typename TChar>
struct IsTextRange<Library::BasicStringView<TChar>>
    : Foundation::Traits::True {};
} // namespace Detail

template <typename T>
concept FormattableRange =
    requires(const T &Value) {
      typename T::ValueType;
      Value.Begin();
      Value.End();
    } && !Detail::IsTextRange<T>::Value;

template <FormattableRange T>
struct Formatter<T> {
  [[nodiscard]] auto Format(WriterRef Writer, const T &Value,
                            const FormatSpec &Spec) const -> Result<> {
    auto Valid = Detail::ValidatePresentation(Spec, u8"?");
    if (!Valid) return Valid;
    auto Result = Detail::WriteBytes(Writer, u8"[", 1);
    if (!Result) return Result;
    Foundation::Bool First{true};
    for (const auto &Entry : Value) {
      if (!First) {
        Result = Detail::WriteBytes(Writer, u8", ", 2);
        if (!Result) return Result;
      }
      First = false;
      FormatSpec ElementSpec{};
      ElementSpec.Debug = Spec.Debug;
      using Element = Foundation::Traits::RemoveCvReferenceType<decltype(Entry)>;
      Result = Formatter<Element>{}.Format(Writer, Entry, ElementSpec);
      if (!Result) return Result;
    }
    return Detail::WriteBytes(Writer, u8"]", 1);
  }
};

namespace Detail {
template <Foundation::Size Index, typename... TValues>
[[nodiscard]] auto FormatTuple(WriterRef Writer,
                               const Library::Tuple<TValues...> &Value,
                               const FormatSpec &Spec) -> Result<> {
  if constexpr (Index == sizeof...(TValues)) {
    return {};
  } else {
    if constexpr (Index != 0) {
      auto Separator = WriteBytes(Writer, u8", ", 2);
      if (!Separator) return Separator;
    }
    const auto &Entry = Library::Get<Index>(Value);
    using Element = Foundation::Traits::RemoveCvReferenceType<decltype(Entry)>;
    FormatSpec ElementSpec{};
    ElementSpec.Debug = Spec.Debug;
    auto Result = Formatter<Element>{}.Format(Writer, Entry, ElementSpec);
    if (!Result) return Result;
    return FormatTuple<Index + 1>(Writer, Value, Spec);
  }
}
} // namespace Detail

template <typename... TValues>
struct Formatter<Library::Tuple<TValues...>> {
  [[nodiscard]] auto Format(WriterRef Writer,
                            const Library::Tuple<TValues...> &Value,
                            const FormatSpec &Spec) const -> Result<> {
    auto Valid = Detail::ValidatePresentation(Spec, u8"?");
    if (!Valid) return Valid;
    auto Result = Detail::WriteBytes(Writer, u8"(", 1);
    if (!Result) return Result;
    Result = Detail::FormatTuple<0>(Writer, Value, Spec);
    if (!Result) return Result;
    return Detail::WriteBytes(Writer, u8")", 1);
  }
};

class HexDump final {
public:
  constexpr explicit HexDump(Library::Span<const Foundation::Byte> Bytes,
                             Foundation::UintPtr Address = 0) noexcept
      : Bytes_(Bytes), Address_(Address) {}
  constexpr auto BytesPerLine(Foundation::Size Value) noexcept -> HexDump & {
    Width_ = Value ? Value : 16;
    return *this;
  }
  constexpr auto ShowAddress(Foundation::Bool Value) noexcept -> HexDump & {
    ShowAddress_ = Value;
    return *this;
  }
  constexpr auto ShowText(Foundation::Bool Value) noexcept -> HexDump & {
    ShowText_ = Value;
    return *this;
  }
  [[nodiscard]] constexpr auto Bytes() const noexcept { return Bytes_; }
  [[nodiscard]] constexpr auto Address() const noexcept { return Address_; }
  [[nodiscard]] constexpr auto Width() const noexcept { return Width_; }
  [[nodiscard]] constexpr auto HasAddress() const noexcept { return ShowAddress_; }
  [[nodiscard]] constexpr auto HasText() const noexcept { return ShowText_; }

private:
  Library::Span<const Foundation::Byte> Bytes_{};
  Foundation::UintPtr Address_{};
  Foundation::Size Width_{16};
  Foundation::Bool ShowAddress_{true};
  Foundation::Bool ShowText_{true};
};

template <>
struct Formatter<HexDump> {
  [[nodiscard]] auto Format(WriterRef Writer, const HexDump &Value,
                            const FormatSpec &Spec) const -> Result<> {
    auto Valid = Detail::ValidatePresentation(Spec, {});
    if (!Valid) return Valid;
    for (Foundation::Size Offset{}; Offset < Value.Bytes().Size();
         Offset += Value.Width()) {
      auto Count = Value.Bytes().Size() - Offset;
      if (Count > Value.Width()) Count = Value.Width();
      Result<> Status{};
      if (Value.HasAddress()) {
        FormatSpec Address{};
        Address.Presentation = u8'X';
        Address.ZeroPad = true;
        Address.Width = sizeof(Foundation::UintPtr) * 2;
        Status = Detail::FormatUnsigned(Writer, Value.Address() + Offset,
                                        false, Address);
        if (!Status) return Status;
        Status = Detail::WriteBytes(Writer, u8"  ", 2);
        if (!Status) return Status;
      }
      for (Foundation::Size Index{}; Index < Value.Width(); ++Index) {
        if (Index < Count) {
          FormatSpec ByteSpec{};
          ByteSpec.Presentation = u8'X';
          ByteSpec.ZeroPad = true;
          ByteSpec.Width = 2;
          Status = Detail::FormatUnsigned(
              Writer, Value.Bytes()[Offset + Index], false, ByteSpec);
        } else {
          Status = Detail::WriteBytes(Writer, u8"  ", 2);
        }
        if (!Status) return Status;
        Status = Detail::WriteBytes(Writer, u8" ", 1);
        if (!Status) return Status;
      }
      if (Value.HasText()) {
        Status = Detail::WriteBytes(Writer, u8" |", 2);
        if (!Status) return Status;
        for (Foundation::Size Index{}; Index < Count; ++Index) {
          const auto Byte = Value.Bytes()[Offset + Index];
          const Foundation::Char8 Character =
              Byte >= 0x20 && Byte <= 0x7E
                  ? Foundation::Cast::Auto<Foundation::Char8>(Byte)
                  : u8'.';
          Status = Detail::WriteBytes(Writer, &Character, 1);
          if (!Status) return Status;
        }
        Status = Detail::WriteBytes(Writer, u8"|", 1);
        if (!Status) return Status;
      }
      if (Offset + Count < Value.Bytes().Size()) {
        Status = Detail::WriteBytes(Writer, u8"\n", 1);
        if (!Status) return Status;
      }
    }
    return {};
  }
};

namespace Detail {
template <Foundation::Size Index = 0, typename TFirst, typename... TRest>
[[nodiscard]] auto FormatArgument(Foundation::Size Wanted, WriterRef Writer,
                                  const FormatSpec &Spec, TFirst &&First,
                                  TRest &&...Rest) -> Result<> {
  if (Wanted == Index) {
    using Type = Foundation::Traits::RemoveCvReferenceType<TFirst>;
    return Formatter<Type>{}.Format(
        Writer, Foundation::Utility::Forward<TFirst>(First), Spec);
  }
  if constexpr (sizeof...(TRest)) {
    return FormatArgument<Index + 1>(
        Wanted, Writer, Spec, Foundation::Utility::Forward<TRest>(Rest)...);
  } else {
    return Failure(
        Error::Semantic(ErrorCode::InvalidFormat, Operation::Format, Wanted));
  }
}
} // namespace Detail

class FormatArgument final {
public:
  template <typename T>
  constexpr explicit FormatArgument(const T &Value) noexcept
      : Object_(&Value), Format_(&FormatValue<
                            Foundation::Traits::RemoveCvReferenceType<T>>) {}

  [[nodiscard]] auto Format(WriterRef Writer, const FormatSpec &Spec) const
      -> Result<> {
    if (!Object_ || !Format_) {
      return Failure(
          Error::Semantic(ErrorCode::InvalidArgument, Operation::Format));
    }
    return Format_(Writer, Object_, Spec);
  }

private:
  template <typename T>
  [[nodiscard]] static auto FormatValue(WriterRef Writer,
                                        const Foundation::Void *Object,
                                        const FormatSpec &Spec) -> Result<> {
    return Formatter<T>{}.Format(
        Writer, *Foundation::Cast::Auto<const T *>(Object), Spec);
  }

  const Foundation::Void *Object_{};
  auto (*Format_)(WriterRef, const Foundation::Void *, const FormatSpec &)
      -> Result<>{};
};

template <typename T>
[[nodiscard]] constexpr auto MakeFormatArgument(const T &Value) noexcept
    -> FormatArgument {
  return FormatArgument{Value};
}

[[nodiscard]] inline auto
VPrint(WriterRef Writer, Library::StringView Format,
       Library::Span<const FormatArgument> Arguments) -> Result<> {
  Foundation::Size ArgumentIndex{};
  Foundation::Size LiteralStart{};
  for (Foundation::Size Index{}; Index < Format.Size(); ++Index) {
    if (Format[Index] != '{' && Format[Index] != '}') continue;
    if (Index + 1 < Format.Size() && Format[Index + 1] == Format[Index]) {
      auto Result = Detail::WriteAscii(Writer, Format.Data() + LiteralStart,
                                       Index - LiteralStart + 1);
      if (!Result) return Result;
      ++Index;
      LiteralStart = Index + 1;
      continue;
    }
    if (Format[Index] == '}') {
      return Failure(
          Error::Semantic(ErrorCode::InvalidFormat, Operation::Format, Index));
    }
    auto Result = Detail::WriteAscii(Writer, Format.Data() + LiteralStart,
                                     Index - LiteralStart);
    if (!Result) return Result;
    const auto FieldStart = Index;
    while (Index < Format.Size() && Format[Index] != '}') ++Index;
    if (Index == Format.Size()) {
      return Failure(Error::Semantic(ErrorCode::InvalidFormat,
                                     Operation::Format, FieldStart));
    }
    auto Field = Format.Substr(FieldStart + 1, Index - FieldStart - 1);
    if (!Field.Empty() && Field[0] == ':') Field.RemovePrefix(1);
    else if (!Field.Empty()) {
      return Failure(Error::Semantic(ErrorCode::InvalidFormat,
                                     Operation::Format, FieldStart));
    }
    if (ArgumentIndex >= Arguments.Size()) {
      return Failure(Error::Semantic(ErrorCode::InvalidFormat,
                                     Operation::Format, ArgumentIndex));
    }
    FormatSpec Spec{};
    Result = Detail::ParseFormatSpec(Field, Spec);
    if (!Result) return Result;
    Result = Arguments[ArgumentIndex++].Format(Writer, Spec);
    if (!Result) return Result;
    LiteralStart = Index + 1;
  }
  if (ArgumentIndex != Arguments.Size()) {
    return Failure(Error::Semantic(ErrorCode::InvalidFormat,
                                   Operation::Format, ArgumentIndex));
  }
  return Detail::WriteAscii(Writer, Format.Data() + LiteralStart,
                            Format.Size() - LiteralStart);
}

template <OutputSink TSink>
[[nodiscard]] auto VPrint(TSink &Sink, Library::StringView Format,
                          Library::Span<const FormatArgument> Arguments)
    -> Result<> {
  return VPrint(WriterRef{Sink}, Format, Arguments);
}

template <OutputSink TSink, typename... TArguments>
[[nodiscard]] auto Print(
    TSink &Sink,
    FormatString<Foundation::Traits::RemoveCvReferenceType<TArguments>...>
        Format,
    TArguments &&...Arguments) -> Result<> {
  WriterRef Writer{Sink};
  const auto Text = Format.View();
  Foundation::Size ArgumentIndex{};
  Foundation::Size LiteralStart{};

  for (Foundation::Size Index{}; Index < Text.Size(); ++Index) {
    if (Text[Index] != '{' && Text[Index] != '}') {
      continue;
    }
    if (Index + 1 < Text.Size() && Text[Index + 1] == Text[Index]) {
      auto Result = Detail::WriteAscii(Writer, Text.Data() + LiteralStart,
                                       Index - LiteralStart + 1);
      if (!Result) return Result;
      ++Index;
      LiteralStart = Index + 1;
      continue;
    }
    if (Text[Index] == '}') {
      return Failure(Error::Semantic(ErrorCode::InvalidFormat,
                                     Operation::Format, Index));
    }

    auto Result = Detail::WriteAscii(Writer, Text.Data() + LiteralStart,
                                     Index - LiteralStart);
    if (!Result) return Result;
    const auto FieldStart = Index;
    while (Index < Text.Size() && Text[Index] != '}') {
      ++Index;
    }
    if (Index == Text.Size()) {
      return Failure(Error::Semantic(ErrorCode::InvalidFormat,
                                     Operation::Format, FieldStart));
    }
    auto Field = Text.Substr(FieldStart + 1, Index - FieldStart - 1);
    if (!Field.Empty() && Field[0] == ':') {
      Field.RemovePrefix(1);
    } else if (!Field.Empty()) {
      return Failure(Error::Semantic(ErrorCode::InvalidFormat,
                                     Operation::Format, FieldStart));
    }
    FormatSpec Spec{};
    Result = Detail::ParseFormatSpec(Field, Spec);
    if (!Result) return Result;
    if constexpr (sizeof...(TArguments)) {
      Result = Detail::FormatArgument(
          ArgumentIndex++, Writer, Spec,
          Foundation::Utility::Forward<TArguments>(Arguments)...);
      if (!Result) return Result;
      LiteralStart = Index + 1;
    } else {
      return Failure(Error::Semantic(ErrorCode::InvalidFormat,
                                     Operation::Format, FieldStart));
    }
  }

  return Detail::WriteAscii(Writer, Text.Data() + LiteralStart,
                            Text.Size() - LiteralStart);
}

template <OutputSink TSink, typename... TArguments>
[[nodiscard]] auto Println(
    TSink &Sink,
    FormatString<Foundation::Traits::RemoveCvReferenceType<TArguments>...>
        Format,
    TArguments &&...Arguments) -> Result<> {
  auto Result = Print(Sink, Format,
                      Foundation::Utility::Forward<TArguments>(Arguments)...);
  if (!Result) {
    return Result;
  }
  const Foundation::Byte LineFeed{'\n'};
  return WriteAll(Sink, {&LineFeed, 1});
}

template <OutputSink TSink, typename... TArguments>
[[nodiscard]] auto FormatTo(
    TSink &Sink,
    FormatString<Foundation::Traits::RemoveCvReferenceType<TArguments>...>
        Format,
    TArguments &&...Arguments) -> Result<> {
  return Print(Sink, Format,
               Foundation::Utility::Forward<TArguments>(Arguments)...);
}

template <typename... TArguments>
[[nodiscard]] auto Format(
    Memory::AllocatorStub Allocator,
    FormatString<Foundation::Traits::RemoveCvReferenceType<TArguments>...>
        FormatText,
    TArguments &&...Arguments) -> Result<Library::U8String> {
  MemorySink Sink{Allocator};
  auto Result = Print(Sink, FormatText,
                      Foundation::Utility::Forward<TArguments>(Arguments)...);
  if (!Result) {
    return Failure(Result.Error());
  }
  Library::U8String Text{Allocator};
  if (!Text.Assign({Foundation::Cast::Auto<const Foundation::Char8 *>(
                        Sink.Buffer().Data()),
                    Sink.Buffer().Size()})) {
    return Failure(
        Error::Semantic(ErrorCode::OutOfMemory, Operation::Allocate));
  }
  return Text;
}
} // namespace UEFIpp::IO
