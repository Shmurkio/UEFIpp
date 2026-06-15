#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::UEFI
{
	class Guid
	{
	public:
		constexpr Guid() = default;

		constexpr Guid(Foundation::Uint32 Data1, Foundation::Uint16 Data2, Foundation::Uint16 Data3, const Foundation::Uint8(&Data4)[8]) : Data1(Data1), Data2(Data2), Data3(Data3), Data4{ Data4[0], Data4[1], Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7] }
		{
		}

		constexpr Guid(Foundation::Uint32 Data1, Foundation::Uint16 Data2, Foundation::Uint16 Data3, Foundation::Uint8 D0, Foundation::Uint8 D1, Foundation::Uint8 D2, Foundation::Uint8 D3, Foundation::Uint8 D4, Foundation::Uint8 D5, Foundation::Uint8 D6, Foundation::Uint8 D7) : Data1(Data1), Data2(Data2), Data3(Data3), Data4{ D0, D1, D2, D3, D4, D5, D6, D7 }
		{
		}

		[[nodiscard]] constexpr auto IsValid() const -> Foundation::Bool
		{
			return *this != Guid{};
		}

		[[nodiscard]] constexpr auto Data1Value() const -> Foundation::Uint32
		{
			return Data1;
		}

		[[nodiscard]] constexpr auto Data2Value() const -> Foundation::Uint16
		{
			return Data2;
		}

		[[nodiscard]] constexpr auto Data3Value() const -> Foundation::Uint16
		{
			return Data3;
		}

		[[nodiscard]] constexpr auto Data4Value() const -> const Foundation::Uint8*
		{
			return Data4;
		}

		[[nodiscard]] constexpr auto operator<=>(const Guid&) const = default;

	public:
		Foundation::Uint32 Data1{};
		Foundation::Uint16 Data2{};
		Foundation::Uint16 Data3{};
		Foundation::Uint8 Data4[8]{};
	};

	static_assert(sizeof(Guid) == 16);
	static_assert(Foundation::Traits::IsTriviallyCopyable<Guid>::Value);
	static_assert(Foundation::Traits::IsStandardLayout<Guid>::Value);
}