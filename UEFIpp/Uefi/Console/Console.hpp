#pragma once

#include "../Uefi.hpp"

namespace Console
{
    //
    // Output stream.
    //
    enum class BASE : UINT8
    {
        Dec = 10,
		Hex = 16,
    };

    struct PREC
    {
        UINT8 Digits;
    };

    enum class HEXCASE : UINT8
    {
        Upper,
        Lower
    };

    struct WIDTH
    {
        UINT8 Value;
    };

    struct FILL
    {
        CHAR Value;
    };

    struct OUT_STREAM
    {
        PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL Out;
        static constexpr UINT64 BufferSize = 512;
        CHAR Buffer[BufferSize];
        UINT64 Length;
        BASE Base;
        UINT8 Precision;
        HEXCASE HexCase;
        UINT8 Width;
        CHAR Fill;
    };

    extern OUT_STREAM Out;

    struct ENDL {};
    inline constexpr ENDL Endl{};

    struct HEX {};
    inline constexpr HEX Hex{};

    struct DEC {};
    inline constexpr DEC Dec{};

    struct CLEAR {};
    inline constexpr CLEAR Clear{};

    struct UPPER {};
    inline constexpr UPPER Upper{};

    struct LOWER {};
    inline constexpr LOWER Lower{};

    VOID
    OutInit(
        IN PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL OutputProtocol
    );

    inline
    constexpr
    PREC
    Prec(
        IN UINT8 Digits
    )
    {
        return PREC{ Digits };
    }

    inline
    constexpr
    WIDTH
    Width(
        IN UINT8 Width
    )
    {
        return WIDTH{ Width };
    }

    inline
    constexpr
    FILL
    Fill(
        IN CHAR Fill
    )
    {
        return FILL{ Fill };
    }
}

// Write a null-terminated ASCII string to the stream.
// Newlines ('\n') are translated to CRLF ("\r\n") automatically.
Console::OUT_STREAM& operator<<(Console::OUT_STREAM& Stream, PCSTR String);

// Write a single ASCII character to the console stream.
Console::OUT_STREAM& operator<<(Console::OUT_STREAM& Stream, CHAR Value);

// Insert an end-of-line into the stream.
// Emits CRLF, flushes the internal buffer, and forces output.
Console::OUT_STREAM& operator<<(Console::OUT_STREAM& Stream, Console::ENDL);

// Write an unsigned 8-bit integer to the stream.
// Output base (decimal or hexadecimal) depends on current stream state.
Console::OUT_STREAM& operator<<(Console::OUT_STREAM& Stream, UINT8 Value);

// Write an unsigned 16-bit integer to the stream.
// Output base (decimal or hexadecimal) depends on current stream state.
Console::OUT_STREAM& operator<<(Console::OUT_STREAM& Stream, UINT16 Value);

// Write an unsigned 32-bit integer to the stream.
// Output base (decimal or hexadecimal) depends on current stream state.
Console::OUT_STREAM& operator<<(Console::OUT_STREAM& Stream, UINT32 Value);

// Write an unsigned 64-bit integer to the stream.
// Output base (decimal or hexadecimal) depends on current stream state.
Console::OUT_STREAM& operator<<(Console::OUT_STREAM& Stream, UINT64 Value);

// Write a signed 8-bit integer to the stream.
// Negative values are prefixed with '-' in decimal mode.
// In hexadecimal mode, the value is printed as unsigned.
Console::OUT_STREAM& operator<<(Console::OUT_STREAM& Stream, INT8 Value);

// Write a signed 16-bit integer to the stream.
// Negative values are prefixed with '-' in decimal mode.
// In hexadecimal mode, the value is printed as unsigned.
Console::OUT_STREAM& operator<<(Console::OUT_STREAM& Stream, INT16 Value);

// Write a signed 32-bit integer to the stream.
// Negative values are prefixed with '-' in decimal mode.
// In hexadecimal mode, the value is printed as unsigned.
Console::OUT_STREAM& operator<<(Console::OUT_STREAM& Stream, INT32 Value);

// Write a signed 64-bit integer to the stream.
// Negative values are prefixed with '-' in decimal mode.
// In hexadecimal mode, the value is printed as unsigned.
Console::OUT_STREAM& operator<<(Console::OUT_STREAM& Stream, INT64 Value);

// Switch the stream to hexadecimal integer output mode.
// Affects all subsequent integer output.
Console::OUT_STREAM& operator<<(Console::OUT_STREAM& Stream, Console::HEX);

// Switch the stream to decimal integer output mode.
// Affects all subsequent integer output.
Console::OUT_STREAM& operator<<(Console::OUT_STREAM& Stream, Console::DEC);

// Write a pointer value to the stream in hexadecimal form.
// The pointer is reinterpreted as a 64-bit unsigned integer.
Console::OUT_STREAM& operator<<(Console::OUT_STREAM& Stream, PCVOID Pointer);

// Clear the console screen.
// Flushes any buffered output before clearing.
Console::OUT_STREAM& operator<<(Console::OUT_STREAM& Stream, Console::CLEAR);

// Set the number of digits printed after the decimal point  for floating-point output.
// Affects all subsequent float/double output.
Console::OUT_STREAM& operator<<(Console::OUT_STREAM& Stream, Console::PREC Precision);

// Write a single-precision floating-point value to the stream.
// Output uses fixed-point decimal formatting with the current precision.
Console::OUT_STREAM&operator<<(Console::OUT_STREAM& Stream, float Value);

// Write a double-precision floating-point value to the stream.
// Output uses fixed-point decimal formatting with the current precision.
Console::OUT_STREAM& operator<<(Console::OUT_STREAM& Stream, double Value);

// Switch hexadecimal digit output to uppercase ('A'–'F').
// Affects all subsequent hexadecimal integer and pointer output.
Console::OUT_STREAM&operator<<(Console::OUT_STREAM& Stream, Console::UPPER);

// Switch hexadecimal digit output to lowercase ('a'–'f').
// Affects all subsequent hexadecimal integer and pointer output.
Console::OUT_STREAM&operator<<(Console::OUT_STREAM& Stream, Console::LOWER);

// Set the minimum field width for the next formatted output.
// If the formatted value is shorter than the specified width,
// it will be padded using the current fill character.
// The width setting is automatically reset after a single use.
Console::OUT_STREAM&operator<<(Console::OUT_STREAM& Stream, Console::WIDTH Width);

// Set the padding character used when field width exceeds the value length.
// The fill character remains active until changed explicitly.
Console::OUT_STREAM& operator<<(Console::OUT_STREAM& Stream, Console::FILL Fill);