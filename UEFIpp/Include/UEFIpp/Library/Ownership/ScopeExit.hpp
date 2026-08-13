#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Library
{
	template<typename TFunction>
	concept ScopeExitFunction = requires(TFunction& Function)
	{
		Function();
	};

	template<ScopeExitFunction TFunction>
	class ScopeExit
	{
	public:
		explicit constexpr ScopeExit(TFunction Function) noexcept :
			Function_(Foundation::Utility::Move(Function))
		{
		}

		ScopeExit(const ScopeExit&) = delete;
		auto operator=(const ScopeExit&) -> ScopeExit& = delete;
		auto operator=(ScopeExit&&) -> ScopeExit& = delete;

		constexpr ScopeExit(ScopeExit&& Other) noexcept :
			Function_(Foundation::Utility::Move(Other.Function_)),
			Active_(Other.Active_)
		{
			Other.Release();
		}

		constexpr ~ScopeExit() noexcept
		{
			if (Active_)
			{
				(void)Function_();
			}
		}

		constexpr auto Release() noexcept -> Foundation::Void
		{
			Active_ = false;
		}

		[[nodiscard]] constexpr auto Active() const noexcept -> Foundation::Bool
		{
			return Active_;
		}

	private:
		[[no_unique_address]] TFunction Function_;
		Foundation::Bool Active_{ true };
	};

	template<typename TFunction>
	ScopeExit(TFunction) -> ScopeExit<TFunction>;
}
