#include "Serial.hpp"

extern "C" void __outbyte(unsigned short, unsigned char);
extern "C" unsigned char __inbyte(unsigned short);

namespace Serial
{
	OUT_STREAM Out{};

	VOID
	OutInit(
		IN UINT16 Port
	)
	{
		Out.Port = Port;
		Out.Length = 0;
		Out.Base = BASE::Dec;
		Out.Enabled = true;

		Out.Precision = 6;
		Out.HexCase = HEXCASE::Upper;
		Out.Width = 0;
		Out.Fill = '0';
	}
}

static
inline
VOID
PutCRaw(
	IN Serial::OUT_STREAM& Stream,
	IN CHAR C
)
{
	if (!Stream.Enabled)
	{
		return;
	}

	while ((__inbyte(Stream.Port + 5) & 0x20) == 0);
	__outbyte(Stream.Port, C);
}

static
inline
VOID
FlushStream(
	IN Serial::OUT_STREAM& Stream
)
{
	if (!Stream.Enabled || !Stream.Length)
	{
		return;
	}

	for (UINT64 i = 0; i < Stream.Length; i++)
	{
		PutCRaw(Stream, Stream.Buffer[i]);
	}
	

	Stream.Length = 0;
}

static
inline
VOID
PushChar(
	IN Serial::OUT_STREAM& Stream,
	IN CHAR C
)
{
	if (!Stream.Enabled)
	{
		return;
	}

	if (Stream.Length >= Serial::OUT_STREAM::BufferSize)
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
	IN Serial::OUT_STREAM& Stream,
	IN PCSTR String
)
{
	if (!Stream.Enabled || !String)
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
	IN Serial::OUT_STREAM& Stream,
	IN UINT64 Value
)
{
	if (!Stream.Enabled)
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
	IN Serial::HEXCASE Case
)
{
	if (Digit < 10)
	{
		return static_cast<CHAR>('0' + Digit);
	}

	return (Case == Serial::HEXCASE::Upper) ? static_cast<CHAR>('A' + (Digit - 10)) : static_cast<CHAR>('a' + (Digit - 10));
}

static
inline
VOID
PushUnsignedHex(
	IN Serial::OUT_STREAM& Stream,
	IN UINT64 Value
)
{
	if (!Stream.Enabled)
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
	IN Serial::OUT_STREAM& Stream,
	IN UINT64 Value
)
{
	if (Stream.Base == Serial::BASE::Hex)
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
	IN Serial::OUT_STREAM& Stream,
	IN INT64 Value
)
{
	if (!Stream.Enabled)
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
	IN Serial::OUT_STREAM& Stream,
	IN INT64 Value
)
{
	if (Stream.Base == Serial::BASE::Hex)
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
	IN Serial::OUT_STREAM& Stream,
	IN double Value
)
{
	if (!Stream.Enabled)
	{
		return;
	}

	if (Stream.Base == Serial::BASE::Hex)
	{
		Stream.Base = Serial::BASE::Dec;
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

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN PCSTR String
)
{
	PushString(Stream, String);
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN PCCHAR8 String
)
{
	PushString(Stream, reinterpret_cast<PCSTR>(String));
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN Serial::ENDL
)
{
	PushChar(Stream, '\r');
	PushChar(Stream, '\n');
	FlushStream(Stream);
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN UINT8 Value
)
{
	PushUnsignedAuto(Stream, Value);
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN UINT16 Value
)
{
	PushUnsignedAuto(Stream, Value);
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN UINT32 Value
)
{
	PushUnsignedAuto(Stream, Value);
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN UINT64 Value
)
{
	PushUnsignedAuto(Stream, Value);
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN INT8 Value
)
{
	PushSignedAuto(Stream, Value);
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN INT16 Value
)
{
	PushSignedAuto(Stream, (INT64)Value);
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN INT32 Value
)
{
	PushSignedAuto(Stream, (INT64)Value);
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN INT64 Value
)
{
	PushSignedAuto(Stream, Value);
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN Serial::HEX
)
{
	Stream.Base = Serial::BASE::Hex;
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN Serial::DEC
)
{
	Stream.Base = Serial::BASE::Dec;
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN PCVOID Pointer
)
{
	PushUnsignedHex(Stream, reinterpret_cast<UINT64>(Pointer));
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN Serial::UPPER
	)
{
	Stream.HexCase = Serial::HEXCASE::Upper;
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN Serial::LOWER
)
{
	Stream.HexCase = Serial::HEXCASE::Lower;
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN Serial::PREC Precision
)
{
	Stream.Precision = Precision.Digits;
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN Serial::WIDTH Width
)
{
	Stream.Width = Width.Value;
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN Serial::FILL Fill
)
{
	Stream.Fill = Fill.Value;
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN float Value
)
{
	PushFloatDec(Stream, static_cast<double>(Value));
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN double Value
)
{
	PushFloatDec(Stream, Value);
	return Stream;
}

Serial::OUT_STREAM&
operator<<(
	IN Serial::OUT_STREAM& Stream,
	IN CHAR Value
	)
{
	PushChar(Stream, Value);
	return Stream;
}