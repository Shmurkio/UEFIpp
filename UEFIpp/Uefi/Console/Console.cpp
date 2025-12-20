#include "Console.hpp"

namespace Console
{
	OUT_STREAM Out{};

	VOID
	OutInit(
		IN PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL OutProtocol
	)
	{
		Out.Out = OutProtocol;
		Out.Length = 0;
		Out.Base = BASE::Dec;
		Out.Precision = 6;
		Out.HexCase = HEXCASE::Upper;
		Out.Width = 0;
		Out.Fill = '0';
	}
}

static
inline
VOID
FlushStream(
	IN Console::OUT_STREAM& Stream
)
{
	if (!Stream.Out || !Stream.Length)
	{
		return;
	}

	WCHAR BufferW[128];
	UINT64 i = 0;

	while (i < Stream.Length)
	{
		UINT64 j = 0;

		while (i < Stream.Length && j < (sizeof(BufferW) / sizeof(BufferW[0]) - 1))
		{
			BufferW[j++] = static_cast<WCHAR>(static_cast<UINT8>(Stream.Buffer[i++]));
		}

		BufferW[j] = L'\0';
		Stream.Out->OutputString(Stream.Out, BufferW);
	}

	Stream.Length = 0;
}

static
inline
VOID
PushChar(
	IN Console::OUT_STREAM& Stream,
	IN CHAR C
)
{
	if (!Stream.Out)
	{
		return;
	}

	if (Stream.Length >= Console::OUT_STREAM::BufferSize)
	{
		FlushStream(Stream);
	}

	Stream.Buffer[Stream.Length++] = C;

	if (C == '\n')
	{
		FlushStream(Stream);
	}
}

static
inline
VOID
PushString(
	IN Console::OUT_STREAM& Stream,
	IN PCSTR String
)
{
	if (!Stream.Out || !String)
	{
		return;
	}

	while (*String)
	{
		CHAR C = *String++;

		if (C == '\n')
		{
			PushChar(Stream, '\r');
			PushChar(Stream, '\n');
			continue;
		}

		PushChar(Stream, C);
	}
}

static
inline
VOID
PushUnsignedInteger(
	IN Console::OUT_STREAM& Stream,
	IN UINT64 Value
)
{
	if (!Stream.Out)
	{
		return;
	}

	if (!Value)
	{
		PushChar(Stream, '0');
		return;
	}

	CHAR Buffer[20];
	UINT64 Count = 0;


	while (Value)
	{
		UINT64 Digit = Value % 10;
		Buffer[Count++] = static_cast<CHAR>('0' + Digit);
		Value /= 10;
	}

	while (Count > 0)
	{
		PushChar(Stream, Buffer[--Count]);
	}
}

static
inline
CHAR
HexDigit(
	IN UINT8 Digit,
	IN Console::HEXCASE Case
)
{
	if (Digit < 10)
	{
		return static_cast<CHAR>('0' + Digit);
	}

	return (Case == Console::HEXCASE::Upper) ? static_cast<CHAR>('A' + (Digit - 10)) : static_cast<CHAR>('a' + (Digit - 10));
}

static
inline
VOID
PushUnsignedHex(
	IN Console::OUT_STREAM& Stream,
	IN UINT64 Value
)
{
	if (!Stream.Out)
	{
		return;
	}

	CHAR Buffer[16];
	UINT64 Count = 0;

	if (!Value)
	{
		Buffer[Count++] = '0';
	}
	else
	{
		while (Value)
		{
			UINT8 Digit = static_cast<UINT8>(Value & 0xF);
			Buffer[Count++] = HexDigit(Digit, Stream.HexCase);
			Value >>= 4;
		}
	}

	while (Count < Stream.Width)
	{
		PushChar(Stream, Stream.Fill);
		Stream.Width--;
	}

	while (Count > 0)
	{
		PushChar(Stream, Buffer[--Count]);
	}

	Stream.Width = 0;
}

static
inline
VOID
PushUnsignedAuto(
	IN Console::OUT_STREAM& Stream,
	IN UINT64 Value
)
{
	if (Stream.Base == Console::BASE::Hex)
	{
		PushUnsignedHex(Stream, Value);
	}
	else
	{
		PushUnsignedInteger(Stream, Value);
	}
}

static
inline
VOID
PushSignedInteger(
	IN Console::OUT_STREAM& Stream,
	IN INT64 Value
)
{
	if (!Stream.Out)
	{
		return;
	}

	if (Value < 0)
	{
		PushChar(Stream, '-');
		UINT64 Abs = static_cast<UINT64>(-(Value + 1)) + 1;
		PushUnsignedInteger(Stream, Abs);
	}
	else
	{
		PushUnsignedInteger(Stream, static_cast<UINT64>(Value));
	}
}

static
inline
VOID
PushSignedAuto(
	IN Console::OUT_STREAM& Stream,
	IN INT64 Value
)
{
	if (Stream.Base == Console::BASE::Hex)
	{
		PushUnsignedHex(Stream, static_cast<UINT64>(Value));
	}
	else
	{
		PushSignedInteger(Stream, Value);
	}
}

static
inline
UINT64
Pow10U64(
	IN UINT8 Exp
)
{
	UINT64 p = 1;
	while (Exp--)
	{
		p *= 10;
	}
	return p;
}

static
inline
VOID
PushFloatDec(
	IN Console::OUT_STREAM& Stream,
	IN double Value
)
{
	if (!Stream.Out)
	{
		return;
	}

	if (Stream.Base == Console::BASE::Hex)
	{
		Stream.Base = Console::BASE::Dec;
	}

	if (Value < 0.0)
	{
		PushChar(Stream, '-');
		Value = -Value;
	}

	UINT8  Precision = Stream.Precision;
	UINT64 Scale = Pow10U64(Precision);

	double ScaledD = Value * static_cast<double>(Scale) + 0.5;
	UINT64 Scaled = static_cast<UINT64>(ScaledD);

	UINT64 IPart = (Precision == 0) ? Scaled : (Scaled / Scale);
	UINT64 FPart = (Precision == 0) ? 0 : (Scaled % Scale);

	PushUnsignedInteger(Stream, IPart);

	if (Precision == 0)
	{
		return;
	}

	PushChar(Stream, '.');

	UINT64 Div = Scale / 10;

	while (Div > 0)
	{
		if (FPart < Div)
		{
			PushChar(Stream, '0');
		}
		else
		{
			break;
		}
		Div /= 10;
	}

	PushUnsignedInteger(Stream, FPart);
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN PCSTR String
)
{
	PushString(Stream, String);
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN Console::ENDL
)
{
	PushChar(Stream, '\r');
	PushChar(Stream, '\n');
	FlushStream(Stream);
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN UINT8 Value
)
{
	PushUnsignedAuto(Stream, Value);
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN UINT16 Value
)
{
	PushUnsignedAuto(Stream, Value);
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN UINT32 Value
)
{
	PushUnsignedAuto(Stream, Value);
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN UINT64 Value
)
{
	PushUnsignedAuto(Stream, Value);
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN INT8 Value
)
{
	PushSignedAuto(Stream, Value);
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN INT16 Value
)
{
	PushSignedAuto(Stream, (INT64)Value);
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN INT32 Value
)
{
	PushSignedAuto(Stream, (INT64)Value);
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN INT64 Value
)
{
	PushSignedAuto(Stream, Value);
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN Console::HEX
)
{
	Stream.Base = Console::BASE::Hex;
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN Console::DEC
)
{
	Stream.Base = Console::BASE::Dec;
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN PCVOID Pointer
)
{
	PushUnsignedHex(Stream, reinterpret_cast<UINT64>(Pointer));
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN Console::CLEAR
)
{
	if (!Stream.Out)
	{
		return Stream;
	}

	FlushStream(Stream);
	Stream.Out->ClearScreen(Stream.Out);

	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN Console::PREC Precision
	)
{
	Stream.Precision = Precision.Digits;
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN float Value
	)
{
	PushFloatDec(Stream, static_cast<double>(Value));
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN double Value
	)
{
	PushFloatDec(Stream, Value);
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN Console::UPPER
)
{
	Stream.HexCase = Console::HEXCASE::Upper;
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN Console::LOWER
)
{
	Stream.HexCase = Console::HEXCASE::Lower;
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN Console::WIDTH Width
)
{
	Stream.Width = Width.Value;
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN Console::FILL Fill
)
{
	Stream.Fill = Fill.Value;
	return Stream;
}

Console::OUT_STREAM&
operator<<(
	IN Console::OUT_STREAM& Stream,
	IN CHAR Value
)
{
	PushChar(Stream, Value);
	return Stream;
}