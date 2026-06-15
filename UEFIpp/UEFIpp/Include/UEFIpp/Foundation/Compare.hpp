#pragma once

#include <UEFIpp/Foundation/Types.hpp>
#include <compare>

namespace UEFIpp::Foundation
{
	enum class Ordering : Int8
	{
		Less = -1,
		Equal = 0,
		Greater = 1
	};
}