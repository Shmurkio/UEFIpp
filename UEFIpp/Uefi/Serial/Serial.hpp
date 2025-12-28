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
Serial::OUT_STREAM& operator<<(Serial::OUT_STREAM& Stream, PCCHAR8 String);
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

namespace UTF8
{
    //
    // Uppercase Greek.
    //
    constexpr PCCHAR8 Alpha = u8"Α";
    constexpr PCCHAR8 Beta = u8"Β";
    constexpr PCCHAR8 Gamma = u8"Γ";
    constexpr PCCHAR8 Delta = u8"Δ";
    constexpr PCCHAR8 Epsilon = u8"Ε";
    constexpr PCCHAR8 Zeta = u8"Ζ";
    constexpr PCCHAR8 Eta = u8"Η";
    constexpr PCCHAR8 Theta = u8"Θ";
    constexpr PCCHAR8 Iota = u8"Ι";
    constexpr PCCHAR8 Kappa = u8"Κ";
    constexpr PCCHAR8 Lambda = u8"Λ";
    constexpr PCCHAR8 Mu = u8"Μ";
    constexpr PCCHAR8 Nu = u8"Ν";
    constexpr PCCHAR8 Xi = u8"Ξ";
    constexpr PCCHAR8 Omicron = u8"Ο";
    constexpr PCCHAR8 Pi = u8"Π";
    constexpr PCCHAR8 Rho = u8"Ρ";
    constexpr PCCHAR8 Sigma = u8"Σ";
    constexpr PCCHAR8 Tau = u8"Τ";
    constexpr PCCHAR8 Upsilon = u8"Υ";
    constexpr PCCHAR8 Phi = u8"Φ";
    constexpr PCCHAR8 Chi = u8"Χ";
    constexpr PCCHAR8 Psi = u8"Ψ";
    constexpr PCCHAR8 Omega = u8"Ω";

    //
    // Lowercase Greek.
    //
    constexpr PCCHAR8 alpha = u8"α";
    constexpr PCCHAR8 beta = u8"β";
    constexpr PCCHAR8 gamma = u8"γ";
    constexpr PCCHAR8 delta = u8"δ";
    constexpr PCCHAR8 epsilon = u8"ε";
    constexpr PCCHAR8 zeta = u8"ζ";
    constexpr PCCHAR8 eta = u8"η";
    constexpr PCCHAR8 theta = u8"θ";
    constexpr PCCHAR8 iota = u8"ι";
    constexpr PCCHAR8 kappa = u8"κ";
    constexpr PCCHAR8 lambda = u8"λ";
    constexpr PCCHAR8 mu = u8"μ";
    constexpr PCCHAR8 nu = u8"ν";
    constexpr PCCHAR8 xi = u8"ξ";
    constexpr PCCHAR8 omicron = u8"ο";
    constexpr PCCHAR8 pi = u8"π";
    constexpr PCCHAR8 rho = u8"ρ";
    constexpr PCCHAR8 sigma = u8"σ";
    constexpr PCCHAR8 tau = u8"τ";
    constexpr PCCHAR8 upsilon = u8"υ";
    constexpr PCCHAR8 phi = u8"φ";
    constexpr PCCHAR8 chi = u8"χ";
    constexpr PCCHAR8 psi = u8"ψ";
    constexpr PCCHAR8 omega = u8"ω";

    //
    // Arrows / transitions.
    //
    constexpr PCCHAR8 Arrow = u8"→";
    constexpr PCCHAR8 LongArrow = u8"⟶";
    constexpr PCCHAR8 DoubleArrow = u8"⇒";
    constexpr PCCHAR8 DoubleArrowS = u8"⇒*";
    constexpr PCCHAR8 LeftArrow = u8"←";
    constexpr PCCHAR8 BiArrow = u8"↔";

    //
    // Acceptance / logic.
    //
    constexpr PCCHAR8 Accepts = u8"⊢";
    constexpr PCCHAR8 Rejects = u8"⊬";
    constexpr PCCHAR8 True = u8"⊤";
    constexpr PCCHAR8 False = u8"⊥";

    //
    // Sets / relations.
    //
    constexpr PCCHAR8 Subset = u8"⊂";
    constexpr PCCHAR8 SubsetEq = u8"⊆";
    constexpr PCCHAR8 Superset = u8"⊃";
    constexpr PCCHAR8 SupersetEq = u8"⊇";
    constexpr PCCHAR8 UnionSet = u8"∪";
    constexpr PCCHAR8 Intersect = u8"∩";
    constexpr PCCHAR8 SetMinus = u8"∖";
    constexpr PCCHAR8 PowerSet = u8"𝒫";
    constexpr PCCHAR8 StateSet = u8"Q";
    constexpr PCCHAR8 ElementOf = u8"∈";
    constexpr PCCHAR8 NotElementOf = u8"∉";
    constexpr PCCHAR8 EmptySet = u8"∅";
    constexpr PCCHAR8 Cartesian = u8"×";
    constexpr PCCHAR8 Equal = u8"=";
    constexpr PCCHAR8 NotEqual = u8"≠";
    constexpr PCCHAR8 Equivalent = u8"≡";

    //
    // Subscripts / superscripts.
    //
    constexpr PCCHAR8 Sub0 = u8"₀";
    constexpr PCCHAR8 Sub1 = u8"₁";
    constexpr PCCHAR8 Sub2 = u8"₂";
    constexpr PCCHAR8 Sub3 = u8"₃";
    constexpr PCCHAR8 Sub4 = u8"₄";
    constexpr PCCHAR8 Sub5 = u8"₅";
    constexpr PCCHAR8 Sub6 = u8"₆";
    constexpr PCCHAR8 Sub7 = u8"₇";
    constexpr PCCHAR8 Sub8 = u8"₈";
    constexpr PCCHAR8 Sub9 = u8"₉";
    constexpr PCCHAR8 SupStar = u8"⁎";
    constexpr PCCHAR8 SupPlus = u8"⁺";

    //
    // Regex operators.
    //
    constexpr PCCHAR8 Union = u8"|";
    constexpr PCCHAR8 Concat = u8"·";
    constexpr PCCHAR8 Star = u8"*";
    constexpr PCCHAR8 Plus = u8"+";
    constexpr PCCHAR8 Optional = u8"?";

    //
    // Set formatting.
    //
    constexpr PCCHAR8 LBrace = u8"{";
    constexpr PCCHAR8 RBrace = u8"}";
    constexpr PCCHAR8 LParen = u8"(";
    constexpr PCCHAR8 RParen = u8")";
    constexpr PCCHAR8 Comma = u8",";

    //
    // Definitions / signatures.
    //
    constexpr PCCHAR8 Colon = u8":";

    //
    // Formatting / separators.
    //
    constexpr PCCHAR8 Slash = u8"/";
    constexpr PCCHAR8 Backslash = u8"\\";
}