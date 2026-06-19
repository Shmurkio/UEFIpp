#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Library
{
	enum class EventResult : Foundation::Uint8
	{
		Continue,
		Stop
	};
}
