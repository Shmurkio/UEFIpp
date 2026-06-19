#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Foundation/Utility.hpp>
#include <UEFIpp/Foundation/Traits.hpp>

namespace UEFIpp::Library
{
	namespace Detail
	{
		template<typename Return, typename Callable, typename... Args>
		static Return Invoke(Foundation::Void* Object, Args... Arguments)
		{
			if constexpr (Foundation::Traits::IsVoid<Return>::Value)
			{
				(*static_cast<Callable*>(Object))(Foundation::Utility::Forward<Args>(Arguments)...);
			}
			else
			{
				return (*static_cast<Callable*>(Object))(Foundation::Utility::Forward<Args>(Arguments)...);
			}
		}

		template<typename Return, typename Callable, typename... Args>
		static Return InvokeConst(const Foundation::Void* Object, Args... Arguments)
		{
			if constexpr (Foundation::Traits::IsVoid<Return>::Value)
			{
				(*static_cast<const Callable*>(Object))(Foundation::Utility::Forward<Args>(Arguments)...);
			}
			else
			{
				return (*static_cast<const Callable*>(Object))(Foundation::Utility::Forward<Args>(Arguments)...);
			}
		}
	}
}
