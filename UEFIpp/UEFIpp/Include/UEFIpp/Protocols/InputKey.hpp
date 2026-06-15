#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Protocols
{
	using ScanCodeValue = Foundation::Uint16;

	class ScanCode
	{
	public:
		inline static constexpr ScanCodeValue Null = 0x0000;
		inline static constexpr ScanCodeValue Up = 0x0001;
		inline static constexpr ScanCodeValue Down = 0x0002;
		inline static constexpr ScanCodeValue Right = 0x0003;
		inline static constexpr ScanCodeValue Left = 0x0004;
		inline static constexpr ScanCodeValue Home = 0x0005;
		inline static constexpr ScanCodeValue End = 0x0006;
		inline static constexpr ScanCodeValue Insert = 0x0007;
		inline static constexpr ScanCodeValue Delete = 0x0008;
		inline static constexpr ScanCodeValue PageUp = 0x0009;
		inline static constexpr ScanCodeValue PageDown = 0x000A;
		inline static constexpr ScanCodeValue Function1 = 0x000B;
		inline static constexpr ScanCodeValue Function2 = 0x000C;
		inline static constexpr ScanCodeValue Function3 = 0x000D;
		inline static constexpr ScanCodeValue Function4 = 0x000E;
		inline static constexpr ScanCodeValue Function5 = 0x000F;
		inline static constexpr ScanCodeValue Function6 = 0x0010;
		inline static constexpr ScanCodeValue Function7 = 0x0011;
		inline static constexpr ScanCodeValue Function8 = 0x0012;
		inline static constexpr ScanCodeValue Function9 = 0x0013;
		inline static constexpr ScanCodeValue Function10 = 0x0014;
		inline static constexpr ScanCodeValue Esc = 0x0017;
	};

	class InputKey
	{
	public:
		ScanCodeValue ScanCode;
		Foundation::WChar UnicodeChar;

		[[nodiscard]] constexpr auto HasUnicode() const -> Foundation::Bool
		{
			return UnicodeChar != 0;
		}

		[[nodiscard]] constexpr auto HasScanCode() const -> Foundation::Bool
		{
			return ScanCode != 0;
		}

		[[nodiscard]] constexpr auto Empty() const -> Foundation::Bool
		{
			return ScanCode == 0 && UnicodeChar == 0;
		}

		[[nodiscard]] constexpr auto IsScanCode(ScanCodeValue Value) const -> Foundation::Bool
		{
			return ScanCode == Value;
		}

		[[nodiscard]] constexpr auto IsEscape() const -> Foundation::Bool
		{
			return IsScanCode(Protocols::ScanCode::Esc);
		}

		[[nodiscard]] constexpr auto operator<=>(const InputKey&) const = default;
	};

	static_assert(sizeof(InputKey) == 4);
	static_assert(Foundation::Traits::IsStandardLayout<InputKey>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<InputKey>::Value);
}