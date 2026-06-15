#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::UEFI
{
	class DevicePathProtocol
	{
	public:
		Foundation::Uint8 Type;
		Foundation::Uint8 SubType;
		Foundation::Uint8 Length[2];

		[[nodiscard]] constexpr auto Size() const -> Foundation::Uint16
		{
			return static_cast<Foundation::Uint16>(static_cast<Foundation::Uint16>(Length[0]) | static_cast<Foundation::Uint16>(Length[1]) << 8);
		}

		[[nodiscard]] constexpr auto operator<=>(const DevicePathProtocol&) const = default;
	};

	static_assert(sizeof(DevicePathProtocol) == 4);
	static_assert(Foundation::Traits::IsStandardLayout<DevicePathProtocol>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<DevicePathProtocol>::Value);

	inline constexpr auto DevicePathTypeHardware = Foundation::Uint8{ 0x01 };
	inline constexpr auto DevicePathTypeAcpi = Foundation::Uint8{ 0x02 };
	inline constexpr auto DevicePathTypeMessaging = Foundation::Uint8{ 0x03 };
	inline constexpr auto DevicePathTypeMedia = Foundation::Uint8{ 0x04 };
	inline constexpr auto DevicePathTypeBiosBootSpecification = Foundation::Uint8{ 0x05 };
	inline constexpr auto DevicePathTypeEnd = Foundation::Uint8{ 0x7F };

	inline constexpr auto DevicePathSubTypeEndInstance = Foundation::Uint8{ 0x01 };
	inline constexpr auto DevicePathSubTypeEndEntire = Foundation::Uint8{ 0xFF };

	[[nodiscard]] constexpr auto IsEndDevicePath(const DevicePathProtocol& DevicePath) -> Foundation::Bool
	{
		return DevicePath.Type == DevicePathTypeEnd;
	}

	[[nodiscard]] constexpr auto IsEndEntireDevicePath(const DevicePathProtocol& DevicePath) -> Foundation::Bool
	{
		return DevicePath.Type == DevicePathTypeEnd &&
			DevicePath.SubType == DevicePathSubTypeEndEntire;
	}

	[[nodiscard]] constexpr auto IsEndInstanceDevicePath(const DevicePathProtocol& DevicePath) -> Foundation::Bool
	{
		return DevicePath.Type == DevicePathTypeEnd && DevicePath.SubType == DevicePathSubTypeEndInstance;
	}
}