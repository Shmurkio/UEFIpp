#pragma once

namespace UEFIpp::Foundation
{
	using Void = void;

	using Int8 = signed char;
	using Int16 = short;
	using Int32 = int;
	using Int64 = long long;

	using Uint8 = unsigned char;
	using Uint16 = unsigned short;
	using Uint32 = unsigned int;
	using Uint64 = unsigned long long;

	using Float32 = float;
	using Float64 = double;

	using Float = Float32;
	using Double = Float64;

	using Char = char;

	using Char8 = char8_t;
	using Char16 = char16_t;
	using Char32 = char32_t;

	using WChar = wchar_t;

	using Byte = Uint8;

	using IntPtr = Int64;
	using UintPtr = Uint64;

	using IntN = IntPtr;
	using UintN = UintPtr;

	using Size = UintN;
	using PtrDiff = IntN;

	using Bool = bool;

	static_assert(sizeof(Int8) == 1);
	static_assert(sizeof(Int16) == 2);
	static_assert(sizeof(Int32) == 4);
	static_assert(sizeof(Int64) == 8);

	static_assert(sizeof(Uint8) == 1);
	static_assert(sizeof(Uint16) == 2);
	static_assert(sizeof(Uint32) == 4);
	static_assert(sizeof(Uint64) == 8);

	static_assert(sizeof(Float32) == 4);
	static_assert(sizeof(Float64) == 8);

	static_assert(sizeof(Char8) == 1);
	static_assert(sizeof(Char16) == 2);
	static_assert(sizeof(Char32) == 4);

	static_assert(sizeof(IntPtr) == sizeof(void*));
	static_assert(sizeof(UintPtr) == sizeof(void*));
}