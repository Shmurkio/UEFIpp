#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/String/StringView.hpp>

namespace UEFIpp::Architecture::X64::InstructionSet
{
	enum class MachineMode : Foundation::Uint8
	{
		Long64,
		LongCompat32,
		LongCompat16,
		Legacy32,
		Legacy16,
		Real16
	};

	enum class RegisterId : Foundation::Uint16
	{
		None = 0,
		Al = 1,
		Cl = 2,
		Dl = 3,
		Bl = 4,
		Ah = 5,
		Ch = 6,
		Dh = 7,
		Bh = 8,
		Spl = 9,
		Bpl = 10,
		Sil = 11,
		Dil = 12,
		R8b = 13,
		R9b = 14,
		R10b = 15,
		R11b = 16,
		R12b = 17,
		R13b = 18,
		R14b = 19,
		R15b = 20,
		Ax = 21,
		Cx = 22,
		Dx = 23,
		Bx = 24,
		Sp = 25,
		Bp = 26,
		Si = 27,
		Di = 28,
		R8w = 29,
		R9w = 30,
		R10w = 31,
		R11w = 32,
		R12w = 33,
		R13w = 34,
		R14w = 35,
		R15w = 36,
		Eax = 37,
		Ecx = 38,
		Edx = 39,
		Ebx = 40,
		Esp = 41,
		Ebp = 42,
		Esi = 43,
		Edi = 44,
		R8d = 45,
		R9d = 46,
		R10d = 47,
		R11d = 48,
		R12d = 49,
		R13d = 50,
		R14d = 51,
		R15d = 52,
		Rax = 53,
		Rcx = 54,
		Rdx = 55,
		Rbx = 56,
		Rsp = 57,
		Rbp = 58,
		Rsi = 59,
		Rdi = 60,
		R8 = 61,
		R9 = 62,
		R10 = 63,
		R11 = 64,
		R12 = 65,
		R13 = 66,
		R14 = 67,
		R15 = 68,
		St0 = 69,
		St1 = 70,
		St2 = 71,
		St3 = 72,
		St4 = 73,
		St5 = 74,
		St6 = 75,
		St7 = 76,
		X87control = 77,
		X87status = 78,
		X87tag = 79,
		Mm0 = 80,
		Mm1 = 81,
		Mm2 = 82,
		Mm3 = 83,
		Mm4 = 84,
		Mm5 = 85,
		Mm6 = 86,
		Mm7 = 87,
		Xmm0 = 88,
		Xmm1 = 89,
		Xmm2 = 90,
		Xmm3 = 91,
		Xmm4 = 92,
		Xmm5 = 93,
		Xmm6 = 94,
		Xmm7 = 95,
		Xmm8 = 96,
		Xmm9 = 97,
		Xmm10 = 98,
		Xmm11 = 99,
		Xmm12 = 100,
		Xmm13 = 101,
		Xmm14 = 102,
		Xmm15 = 103,
		Xmm16 = 104,
		Xmm17 = 105,
		Xmm18 = 106,
		Xmm19 = 107,
		Xmm20 = 108,
		Xmm21 = 109,
		Xmm22 = 110,
		Xmm23 = 111,
		Xmm24 = 112,
		Xmm25 = 113,
		Xmm26 = 114,
		Xmm27 = 115,
		Xmm28 = 116,
		Xmm29 = 117,
		Xmm30 = 118,
		Xmm31 = 119,
		Ymm0 = 120,
		Ymm1 = 121,
		Ymm2 = 122,
		Ymm3 = 123,
		Ymm4 = 124,
		Ymm5 = 125,
		Ymm6 = 126,
		Ymm7 = 127,
		Ymm8 = 128,
		Ymm9 = 129,
		Ymm10 = 130,
		Ymm11 = 131,
		Ymm12 = 132,
		Ymm13 = 133,
		Ymm14 = 134,
		Ymm15 = 135,
		Ymm16 = 136,
		Ymm17 = 137,
		Ymm18 = 138,
		Ymm19 = 139,
		Ymm20 = 140,
		Ymm21 = 141,
		Ymm22 = 142,
		Ymm23 = 143,
		Ymm24 = 144,
		Ymm25 = 145,
		Ymm26 = 146,
		Ymm27 = 147,
		Ymm28 = 148,
		Ymm29 = 149,
		Ymm30 = 150,
		Ymm31 = 151,
		Zmm0 = 152,
		Zmm1 = 153,
		Zmm2 = 154,
		Zmm3 = 155,
		Zmm4 = 156,
		Zmm5 = 157,
		Zmm6 = 158,
		Zmm7 = 159,
		Zmm8 = 160,
		Zmm9 = 161,
		Zmm10 = 162,
		Zmm11 = 163,
		Zmm12 = 164,
		Zmm13 = 165,
		Zmm14 = 166,
		Zmm15 = 167,
		Zmm16 = 168,
		Zmm17 = 169,
		Zmm18 = 170,
		Zmm19 = 171,
		Zmm20 = 172,
		Zmm21 = 173,
		Zmm22 = 174,
		Zmm23 = 175,
		Zmm24 = 176,
		Zmm25 = 177,
		Zmm26 = 178,
		Zmm27 = 179,
		Zmm28 = 180,
		Zmm29 = 181,
		Zmm30 = 182,
		Zmm31 = 183,
		Tmm0 = 184,
		Tmm1 = 185,
		Tmm2 = 186,
		Tmm3 = 187,
		Tmm4 = 188,
		Tmm5 = 189,
		Tmm6 = 190,
		Tmm7 = 191,
		Flags = 192,
		Eflags = 193,
		Rflags = 194,
		Ip = 195,
		Eip = 196,
		Rip = 197,
		Es = 198,
		Cs = 199,
		Ss = 200,
		Ds = 201,
		Fs = 202,
		Gs = 203,
		Gdtr = 204,
		Ldtr = 205,
		Idtr = 206,
		Tr = 207,
		Tr0 = 208,
		Tr1 = 209,
		Tr2 = 210,
		Tr3 = 211,
		Tr4 = 212,
		Tr5 = 213,
		Tr6 = 214,
		Tr7 = 215,
		Cr0 = 216,
		Cr1 = 217,
		Cr2 = 218,
		Cr3 = 219,
		Cr4 = 220,
		Cr5 = 221,
		Cr6 = 222,
		Cr7 = 223,
		Cr8 = 224,
		Cr9 = 225,
		Cr10 = 226,
		Cr11 = 227,
		Cr12 = 228,
		Cr13 = 229,
		Cr14 = 230,
		Cr15 = 231,
		Dr0 = 232,
		Dr1 = 233,
		Dr2 = 234,
		Dr3 = 235,
		Dr4 = 236,
		Dr5 = 237,
		Dr6 = 238,
		Dr7 = 239,
		Dr8 = 240,
		Dr9 = 241,
		Dr10 = 242,
		Dr11 = 243,
		Dr12 = 244,
		Dr13 = 245,
		Dr14 = 246,
		Dr15 = 247,
		K0 = 248,
		K1 = 249,
		K2 = 250,
		K3 = 251,
		K4 = 252,
		K5 = 253,
		K6 = 254,
		K7 = 255,
		Bnd0 = 256,
		Bnd1 = 257,
		Bnd2 = 258,
		Bnd3 = 259,
		Bndcfg = 260,
		Bndstatus = 261,
		Mxcsr = 262,
		Pkru = 263,
		Xcr0 = 264,
		Uif = 265
	};

	enum class RegisterClass : Foundation::Uint8
	{
		Invalid,
		GeneralPurpose8,
		GeneralPurpose16,
		GeneralPurpose32,
		GeneralPurpose64,
		FloatingPoint,
		Mmx,
		Xmm,
		Ymm,
		Zmm,
		Tile,
		Flags,
		InstructionPointer,
		Segment,
		Test,
		Control,
		Debug,
		Mask,
		Bound,
		Other
	};

	class Register
	{
	public:
		constexpr Register() noexcept = default;

		constexpr explicit Register(
			RegisterId Id
		) noexcept :
			Id_(Id)
		{
		}

		[[nodiscard]] constexpr auto Id() const noexcept
			-> RegisterId
		{
			return Id_;
		}

		[[nodiscard]] constexpr auto IsValid() const noexcept
			-> Foundation::Bool
		{
			return Id_ != RegisterId::None;
		}

		[[nodiscard]] auto Class() const noexcept
			-> RegisterClass;

		[[nodiscard]] auto Width(
			MachineMode Mode = MachineMode::Long64
		) const noexcept -> Foundation::Uint16;

		[[nodiscard]] auto Root(
			MachineMode Mode = MachineMode::Long64
		) const noexcept -> Register;

		[[nodiscard]] auto Name() const noexcept
			-> Library::StringView;

		[[nodiscard]] constexpr auto operator==(
			const Register&
		) const noexcept -> Foundation::Bool = default;

	private:
		RegisterId Id_{ RegisterId::None };
	};

	inline constexpr auto RegisterCount =
		static_cast<Foundation::Size>(266);

	namespace Registers
	{
		inline constexpr Register None{ RegisterId::None };
		inline constexpr Register Al{ RegisterId::Al };
		inline constexpr Register Cl{ RegisterId::Cl };
		inline constexpr Register Dl{ RegisterId::Dl };
		inline constexpr Register Bl{ RegisterId::Bl };
		inline constexpr Register Ah{ RegisterId::Ah };
		inline constexpr Register Ch{ RegisterId::Ch };
		inline constexpr Register Dh{ RegisterId::Dh };
		inline constexpr Register Bh{ RegisterId::Bh };
		inline constexpr Register Spl{ RegisterId::Spl };
		inline constexpr Register Bpl{ RegisterId::Bpl };
		inline constexpr Register Sil{ RegisterId::Sil };
		inline constexpr Register Dil{ RegisterId::Dil };
		inline constexpr Register R8b{ RegisterId::R8b };
		inline constexpr Register R9b{ RegisterId::R9b };
		inline constexpr Register R10b{ RegisterId::R10b };
		inline constexpr Register R11b{ RegisterId::R11b };
		inline constexpr Register R12b{ RegisterId::R12b };
		inline constexpr Register R13b{ RegisterId::R13b };
		inline constexpr Register R14b{ RegisterId::R14b };
		inline constexpr Register R15b{ RegisterId::R15b };
		inline constexpr Register Ax{ RegisterId::Ax };
		inline constexpr Register Cx{ RegisterId::Cx };
		inline constexpr Register Dx{ RegisterId::Dx };
		inline constexpr Register Bx{ RegisterId::Bx };
		inline constexpr Register Sp{ RegisterId::Sp };
		inline constexpr Register Bp{ RegisterId::Bp };
		inline constexpr Register Si{ RegisterId::Si };
		inline constexpr Register Di{ RegisterId::Di };
		inline constexpr Register R8w{ RegisterId::R8w };
		inline constexpr Register R9w{ RegisterId::R9w };
		inline constexpr Register R10w{ RegisterId::R10w };
		inline constexpr Register R11w{ RegisterId::R11w };
		inline constexpr Register R12w{ RegisterId::R12w };
		inline constexpr Register R13w{ RegisterId::R13w };
		inline constexpr Register R14w{ RegisterId::R14w };
		inline constexpr Register R15w{ RegisterId::R15w };
		inline constexpr Register Eax{ RegisterId::Eax };
		inline constexpr Register Ecx{ RegisterId::Ecx };
		inline constexpr Register Edx{ RegisterId::Edx };
		inline constexpr Register Ebx{ RegisterId::Ebx };
		inline constexpr Register Esp{ RegisterId::Esp };
		inline constexpr Register Ebp{ RegisterId::Ebp };
		inline constexpr Register Esi{ RegisterId::Esi };
		inline constexpr Register Edi{ RegisterId::Edi };
		inline constexpr Register R8d{ RegisterId::R8d };
		inline constexpr Register R9d{ RegisterId::R9d };
		inline constexpr Register R10d{ RegisterId::R10d };
		inline constexpr Register R11d{ RegisterId::R11d };
		inline constexpr Register R12d{ RegisterId::R12d };
		inline constexpr Register R13d{ RegisterId::R13d };
		inline constexpr Register R14d{ RegisterId::R14d };
		inline constexpr Register R15d{ RegisterId::R15d };
		inline constexpr Register Rax{ RegisterId::Rax };
		inline constexpr Register Rcx{ RegisterId::Rcx };
		inline constexpr Register Rdx{ RegisterId::Rdx };
		inline constexpr Register Rbx{ RegisterId::Rbx };
		inline constexpr Register Rsp{ RegisterId::Rsp };
		inline constexpr Register Rbp{ RegisterId::Rbp };
		inline constexpr Register Rsi{ RegisterId::Rsi };
		inline constexpr Register Rdi{ RegisterId::Rdi };
		inline constexpr Register R8{ RegisterId::R8 };
		inline constexpr Register R9{ RegisterId::R9 };
		inline constexpr Register R10{ RegisterId::R10 };
		inline constexpr Register R11{ RegisterId::R11 };
		inline constexpr Register R12{ RegisterId::R12 };
		inline constexpr Register R13{ RegisterId::R13 };
		inline constexpr Register R14{ RegisterId::R14 };
		inline constexpr Register R15{ RegisterId::R15 };
		inline constexpr Register St0{ RegisterId::St0 };
		inline constexpr Register St1{ RegisterId::St1 };
		inline constexpr Register St2{ RegisterId::St2 };
		inline constexpr Register St3{ RegisterId::St3 };
		inline constexpr Register St4{ RegisterId::St4 };
		inline constexpr Register St5{ RegisterId::St5 };
		inline constexpr Register St6{ RegisterId::St6 };
		inline constexpr Register St7{ RegisterId::St7 };
		inline constexpr Register X87control{ RegisterId::X87control };
		inline constexpr Register X87status{ RegisterId::X87status };
		inline constexpr Register X87tag{ RegisterId::X87tag };
		inline constexpr Register Mm0{ RegisterId::Mm0 };
		inline constexpr Register Mm1{ RegisterId::Mm1 };
		inline constexpr Register Mm2{ RegisterId::Mm2 };
		inline constexpr Register Mm3{ RegisterId::Mm3 };
		inline constexpr Register Mm4{ RegisterId::Mm4 };
		inline constexpr Register Mm5{ RegisterId::Mm5 };
		inline constexpr Register Mm6{ RegisterId::Mm6 };
		inline constexpr Register Mm7{ RegisterId::Mm7 };
		inline constexpr Register Xmm0{ RegisterId::Xmm0 };
		inline constexpr Register Xmm1{ RegisterId::Xmm1 };
		inline constexpr Register Xmm2{ RegisterId::Xmm2 };
		inline constexpr Register Xmm3{ RegisterId::Xmm3 };
		inline constexpr Register Xmm4{ RegisterId::Xmm4 };
		inline constexpr Register Xmm5{ RegisterId::Xmm5 };
		inline constexpr Register Xmm6{ RegisterId::Xmm6 };
		inline constexpr Register Xmm7{ RegisterId::Xmm7 };
		inline constexpr Register Xmm8{ RegisterId::Xmm8 };
		inline constexpr Register Xmm9{ RegisterId::Xmm9 };
		inline constexpr Register Xmm10{ RegisterId::Xmm10 };
		inline constexpr Register Xmm11{ RegisterId::Xmm11 };
		inline constexpr Register Xmm12{ RegisterId::Xmm12 };
		inline constexpr Register Xmm13{ RegisterId::Xmm13 };
		inline constexpr Register Xmm14{ RegisterId::Xmm14 };
		inline constexpr Register Xmm15{ RegisterId::Xmm15 };
		inline constexpr Register Xmm16{ RegisterId::Xmm16 };
		inline constexpr Register Xmm17{ RegisterId::Xmm17 };
		inline constexpr Register Xmm18{ RegisterId::Xmm18 };
		inline constexpr Register Xmm19{ RegisterId::Xmm19 };
		inline constexpr Register Xmm20{ RegisterId::Xmm20 };
		inline constexpr Register Xmm21{ RegisterId::Xmm21 };
		inline constexpr Register Xmm22{ RegisterId::Xmm22 };
		inline constexpr Register Xmm23{ RegisterId::Xmm23 };
		inline constexpr Register Xmm24{ RegisterId::Xmm24 };
		inline constexpr Register Xmm25{ RegisterId::Xmm25 };
		inline constexpr Register Xmm26{ RegisterId::Xmm26 };
		inline constexpr Register Xmm27{ RegisterId::Xmm27 };
		inline constexpr Register Xmm28{ RegisterId::Xmm28 };
		inline constexpr Register Xmm29{ RegisterId::Xmm29 };
		inline constexpr Register Xmm30{ RegisterId::Xmm30 };
		inline constexpr Register Xmm31{ RegisterId::Xmm31 };
		inline constexpr Register Ymm0{ RegisterId::Ymm0 };
		inline constexpr Register Ymm1{ RegisterId::Ymm1 };
		inline constexpr Register Ymm2{ RegisterId::Ymm2 };
		inline constexpr Register Ymm3{ RegisterId::Ymm3 };
		inline constexpr Register Ymm4{ RegisterId::Ymm4 };
		inline constexpr Register Ymm5{ RegisterId::Ymm5 };
		inline constexpr Register Ymm6{ RegisterId::Ymm6 };
		inline constexpr Register Ymm7{ RegisterId::Ymm7 };
		inline constexpr Register Ymm8{ RegisterId::Ymm8 };
		inline constexpr Register Ymm9{ RegisterId::Ymm9 };
		inline constexpr Register Ymm10{ RegisterId::Ymm10 };
		inline constexpr Register Ymm11{ RegisterId::Ymm11 };
		inline constexpr Register Ymm12{ RegisterId::Ymm12 };
		inline constexpr Register Ymm13{ RegisterId::Ymm13 };
		inline constexpr Register Ymm14{ RegisterId::Ymm14 };
		inline constexpr Register Ymm15{ RegisterId::Ymm15 };
		inline constexpr Register Ymm16{ RegisterId::Ymm16 };
		inline constexpr Register Ymm17{ RegisterId::Ymm17 };
		inline constexpr Register Ymm18{ RegisterId::Ymm18 };
		inline constexpr Register Ymm19{ RegisterId::Ymm19 };
		inline constexpr Register Ymm20{ RegisterId::Ymm20 };
		inline constexpr Register Ymm21{ RegisterId::Ymm21 };
		inline constexpr Register Ymm22{ RegisterId::Ymm22 };
		inline constexpr Register Ymm23{ RegisterId::Ymm23 };
		inline constexpr Register Ymm24{ RegisterId::Ymm24 };
		inline constexpr Register Ymm25{ RegisterId::Ymm25 };
		inline constexpr Register Ymm26{ RegisterId::Ymm26 };
		inline constexpr Register Ymm27{ RegisterId::Ymm27 };
		inline constexpr Register Ymm28{ RegisterId::Ymm28 };
		inline constexpr Register Ymm29{ RegisterId::Ymm29 };
		inline constexpr Register Ymm30{ RegisterId::Ymm30 };
		inline constexpr Register Ymm31{ RegisterId::Ymm31 };
		inline constexpr Register Zmm0{ RegisterId::Zmm0 };
		inline constexpr Register Zmm1{ RegisterId::Zmm1 };
		inline constexpr Register Zmm2{ RegisterId::Zmm2 };
		inline constexpr Register Zmm3{ RegisterId::Zmm3 };
		inline constexpr Register Zmm4{ RegisterId::Zmm4 };
		inline constexpr Register Zmm5{ RegisterId::Zmm5 };
		inline constexpr Register Zmm6{ RegisterId::Zmm6 };
		inline constexpr Register Zmm7{ RegisterId::Zmm7 };
		inline constexpr Register Zmm8{ RegisterId::Zmm8 };
		inline constexpr Register Zmm9{ RegisterId::Zmm9 };
		inline constexpr Register Zmm10{ RegisterId::Zmm10 };
		inline constexpr Register Zmm11{ RegisterId::Zmm11 };
		inline constexpr Register Zmm12{ RegisterId::Zmm12 };
		inline constexpr Register Zmm13{ RegisterId::Zmm13 };
		inline constexpr Register Zmm14{ RegisterId::Zmm14 };
		inline constexpr Register Zmm15{ RegisterId::Zmm15 };
		inline constexpr Register Zmm16{ RegisterId::Zmm16 };
		inline constexpr Register Zmm17{ RegisterId::Zmm17 };
		inline constexpr Register Zmm18{ RegisterId::Zmm18 };
		inline constexpr Register Zmm19{ RegisterId::Zmm19 };
		inline constexpr Register Zmm20{ RegisterId::Zmm20 };
		inline constexpr Register Zmm21{ RegisterId::Zmm21 };
		inline constexpr Register Zmm22{ RegisterId::Zmm22 };
		inline constexpr Register Zmm23{ RegisterId::Zmm23 };
		inline constexpr Register Zmm24{ RegisterId::Zmm24 };
		inline constexpr Register Zmm25{ RegisterId::Zmm25 };
		inline constexpr Register Zmm26{ RegisterId::Zmm26 };
		inline constexpr Register Zmm27{ RegisterId::Zmm27 };
		inline constexpr Register Zmm28{ RegisterId::Zmm28 };
		inline constexpr Register Zmm29{ RegisterId::Zmm29 };
		inline constexpr Register Zmm30{ RegisterId::Zmm30 };
		inline constexpr Register Zmm31{ RegisterId::Zmm31 };
		inline constexpr Register Tmm0{ RegisterId::Tmm0 };
		inline constexpr Register Tmm1{ RegisterId::Tmm1 };
		inline constexpr Register Tmm2{ RegisterId::Tmm2 };
		inline constexpr Register Tmm3{ RegisterId::Tmm3 };
		inline constexpr Register Tmm4{ RegisterId::Tmm4 };
		inline constexpr Register Tmm5{ RegisterId::Tmm5 };
		inline constexpr Register Tmm6{ RegisterId::Tmm6 };
		inline constexpr Register Tmm7{ RegisterId::Tmm7 };
		inline constexpr Register Flags{ RegisterId::Flags };
		inline constexpr Register Eflags{ RegisterId::Eflags };
		inline constexpr Register Rflags{ RegisterId::Rflags };
		inline constexpr Register Ip{ RegisterId::Ip };
		inline constexpr Register Eip{ RegisterId::Eip };
		inline constexpr Register Rip{ RegisterId::Rip };
		inline constexpr Register Es{ RegisterId::Es };
		inline constexpr Register Cs{ RegisterId::Cs };
		inline constexpr Register Ss{ RegisterId::Ss };
		inline constexpr Register Ds{ RegisterId::Ds };
		inline constexpr Register Fs{ RegisterId::Fs };
		inline constexpr Register Gs{ RegisterId::Gs };
		inline constexpr Register Gdtr{ RegisterId::Gdtr };
		inline constexpr Register Ldtr{ RegisterId::Ldtr };
		inline constexpr Register Idtr{ RegisterId::Idtr };
		inline constexpr Register Tr{ RegisterId::Tr };
		inline constexpr Register Tr0{ RegisterId::Tr0 };
		inline constexpr Register Tr1{ RegisterId::Tr1 };
		inline constexpr Register Tr2{ RegisterId::Tr2 };
		inline constexpr Register Tr3{ RegisterId::Tr3 };
		inline constexpr Register Tr4{ RegisterId::Tr4 };
		inline constexpr Register Tr5{ RegisterId::Tr5 };
		inline constexpr Register Tr6{ RegisterId::Tr6 };
		inline constexpr Register Tr7{ RegisterId::Tr7 };
		inline constexpr Register Cr0{ RegisterId::Cr0 };
		inline constexpr Register Cr1{ RegisterId::Cr1 };
		inline constexpr Register Cr2{ RegisterId::Cr2 };
		inline constexpr Register Cr3{ RegisterId::Cr3 };
		inline constexpr Register Cr4{ RegisterId::Cr4 };
		inline constexpr Register Cr5{ RegisterId::Cr5 };
		inline constexpr Register Cr6{ RegisterId::Cr6 };
		inline constexpr Register Cr7{ RegisterId::Cr7 };
		inline constexpr Register Cr8{ RegisterId::Cr8 };
		inline constexpr Register Cr9{ RegisterId::Cr9 };
		inline constexpr Register Cr10{ RegisterId::Cr10 };
		inline constexpr Register Cr11{ RegisterId::Cr11 };
		inline constexpr Register Cr12{ RegisterId::Cr12 };
		inline constexpr Register Cr13{ RegisterId::Cr13 };
		inline constexpr Register Cr14{ RegisterId::Cr14 };
		inline constexpr Register Cr15{ RegisterId::Cr15 };
		inline constexpr Register Dr0{ RegisterId::Dr0 };
		inline constexpr Register Dr1{ RegisterId::Dr1 };
		inline constexpr Register Dr2{ RegisterId::Dr2 };
		inline constexpr Register Dr3{ RegisterId::Dr3 };
		inline constexpr Register Dr4{ RegisterId::Dr4 };
		inline constexpr Register Dr5{ RegisterId::Dr5 };
		inline constexpr Register Dr6{ RegisterId::Dr6 };
		inline constexpr Register Dr7{ RegisterId::Dr7 };
		inline constexpr Register Dr8{ RegisterId::Dr8 };
		inline constexpr Register Dr9{ RegisterId::Dr9 };
		inline constexpr Register Dr10{ RegisterId::Dr10 };
		inline constexpr Register Dr11{ RegisterId::Dr11 };
		inline constexpr Register Dr12{ RegisterId::Dr12 };
		inline constexpr Register Dr13{ RegisterId::Dr13 };
		inline constexpr Register Dr14{ RegisterId::Dr14 };
		inline constexpr Register Dr15{ RegisterId::Dr15 };
		inline constexpr Register K0{ RegisterId::K0 };
		inline constexpr Register K1{ RegisterId::K1 };
		inline constexpr Register K2{ RegisterId::K2 };
		inline constexpr Register K3{ RegisterId::K3 };
		inline constexpr Register K4{ RegisterId::K4 };
		inline constexpr Register K5{ RegisterId::K5 };
		inline constexpr Register K6{ RegisterId::K6 };
		inline constexpr Register K7{ RegisterId::K7 };
		inline constexpr Register Bnd0{ RegisterId::Bnd0 };
		inline constexpr Register Bnd1{ RegisterId::Bnd1 };
		inline constexpr Register Bnd2{ RegisterId::Bnd2 };
		inline constexpr Register Bnd3{ RegisterId::Bnd3 };
		inline constexpr Register Bndcfg{ RegisterId::Bndcfg };
		inline constexpr Register Bndstatus{ RegisterId::Bndstatus };
		inline constexpr Register Mxcsr{ RegisterId::Mxcsr };
		inline constexpr Register Pkru{ RegisterId::Pkru };
		inline constexpr Register Xcr0{ RegisterId::Xcr0 };
		inline constexpr Register Uif{ RegisterId::Uif };
	}
}
