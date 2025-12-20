#pragma once

#include "../Uefi.hpp"

namespace Serial
{
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
        static constexpr UINT64 BufferSize = 512;
        CHAR Buffer[BufferSize];
        UINT64 Length;
        BASE Base;
        UINT8 Precision;
        HEXCASE HexCase;
        UINT8 Width;
        CHAR Fill;
        UINT16 Port;
        BOOLEAN Enabled;
    };

    extern OUT_STREAM Out;

    struct ENDL {};
    inline constexpr ENDL Endl{};

    struct HEX {};
    inline constexpr HEX Hex{};

    struct DEC {};
    inline constexpr DEC Dec{};

    struct UPPER {};
    inline constexpr UPPER Upper{};

    struct LOWER {};
    inline constexpr LOWER Lower{};

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

	VOID
	OutInit(
		IN UINT16 Port
	);
}

Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, PCSTR String);
Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, Serial::ENDL);
Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, Serial::HEX);
Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, Serial::DEC);

Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, UINT8 Value);
Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, UINT16 Value);
Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, UINT32 Value);
Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, UINT64 Value);

Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, INT8 Value);
Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, INT16 Value);
Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, INT32 Value);
Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, INT64 Value);

Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, PCVOID Pointer);

Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, Serial::PREC Precision);

Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, Serial::WIDTH Width);
Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, Serial::FILL Fill);

Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, Serial::UPPER);
Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, Serial::LOWER);

Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, float Value);
Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, double Value);

Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, CHAR Value);