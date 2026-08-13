#pragma once

#include <UEFIpp/Library/Functional/FunctionDetail.hpp>

namespace UEFIpp::Library
{
	template<typename>
	class FunctionRef;

	template<typename Return, typename... Args>
	class FunctionRef<Return(Args...)>
	{
	private:
		using InvokeFn = Return(*)(Foundation::Void*, Args...);

	private:
		Foundation::Void* Object_{};
		InvokeFn Invoke_{};

	public:
		constexpr FunctionRef() = default;
		constexpr FunctionRef(Foundation::NullPtr) {}

		template<typename Callable>
		constexpr FunctionRef(Callable& CallableObject) : Object_(&CallableObject), Invoke_([](Foundation::Void* Object, Args... Arguments) -> Return
			{
				return Detail::Invoke<Return, Callable, Args...>(Object, Foundation::Utility::Forward<Args>(Arguments)...);
			})
		{
		}

		[[nodiscard]] constexpr explicit operator Foundation::Bool() const
		{
			return Invoke_ != nullptr;
		}

		Return operator()(Args... Arguments) const
		{
			return Invoke_(Object_, Foundation::Utility::Forward<Args>(Arguments)...);
		}
	};
}
