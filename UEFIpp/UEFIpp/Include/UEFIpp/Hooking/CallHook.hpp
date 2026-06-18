#pragma once

#include <UEFIpp/Hooking/Patch.hpp>
#include <UEFIpp/Hooking/X64/RelativeCall.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Hooking
{
	template<typename Signature>
	class CallHook;

	template<typename Ret, typename... Args>
	class CallHook<Ret(Args...)>
	{
	public:
		using Function = Ret(*)(Args...);

		static constexpr auto PatchSize = X64::RelativeCall::Size;

		constexpr CallHook() = default;

		CallHook(Foundation::Void* CallInstruction, Function Replacement, const WritePatchOptions& Options = {})
		{
			Attach(CallInstruction, Replacement, Options);
		}

		CallHook(const CallHook&) = delete;
		auto operator=(const CallHook&) -> CallHook & = delete;

		CallHook(CallHook&& Other) noexcept
		{
			MoveFrom(Other);
		}

		auto operator=(CallHook&& Other) noexcept -> CallHook&
		{
			if (this != &Other)
			{
				Detach();
				MoveFrom(Other);
			}

			return *this;
		}

		~CallHook()
		{
			Detach();
		}

		[[nodiscard]] auto Attach(Foundation::Void* CallInstruction, Function Replacement, const WritePatchOptions& Options = {}) -> Foundation::Bool
		{
			if (Attached_ || !CallInstruction || !Replacement || !X64::RelativeCall::IsCall(CallInstruction))
			{
				return false;
			}

			const auto OriginalTarget = X64::RelativeCall::Target(CallInstruction);
			if (!OriginalTarget)
			{
				return false;
			}

			Foundation::Uint8 Call[PatchSize]{};
			const auto Source = Foundation::Cast::Auto<Foundation::UintPtr>(CallInstruction);
			const auto Destination = Foundation::Cast::Auto<Foundation::UintPtr>(Replacement);

			if (!X64::RelativeCall::Make(Call, Source, Destination))
			{
				return false;
			}

			if (!Patch_.Apply(CallInstruction, Call, PatchSize, Options))
			{
				return false;
			}

			CallInstruction_ = CallInstruction;
			Original_ = Foundation::Cast::Auto<Function>(OriginalTarget);
			Replacement_ = Replacement;
			Attached_ = true;

			return true;
		}

		[[nodiscard]] auto Detach() -> Foundation::Bool
		{
			if (!Attached_)
			{
				return false;
			}

			if (!Patch_.Restore())
			{
				return false;
			}

			Attached_ = false;
			return true;
		}

		[[nodiscard]] constexpr auto IsAttached() const -> Foundation::Bool
		{
			return Attached_;
		}

		[[nodiscard]] constexpr auto CallInstruction() const -> Foundation::Void*
		{
			return CallInstruction_;
		}

		[[nodiscard]] constexpr auto Original() const -> Function
		{
			return Original_;
		}

		[[nodiscard]] constexpr auto Replacement() const -> Function
		{
			return Replacement_;
		}

		auto CallOriginal(Args... Arguments) -> Ret
		{
			if (!Original_)
			{
				if constexpr (!Foundation::Traits::IsVoid<Ret>::Value)
				{
					return Ret{};
				}
				else
				{
					return;
				}
			}

			if constexpr (Foundation::Traits::IsVoid<Ret>::Value)
			{
				Original_(Arguments...);
				return;
			}
			else
			{
				return Original_(Arguments...);
			}
		}

	private:
		Foundation::Void* CallInstruction_{};
		Function Original_{};
		Function Replacement_{};
		Patch Patch_{};
		Foundation::Bool Attached_{};

		auto MoveFrom(CallHook& Other) -> Foundation::Void
		{
			CallInstruction_ = Other.CallInstruction_;
			Original_ = Other.Original_;
			Replacement_ = Other.Replacement_;
			Patch_ = Foundation::Utility::Move(Other.Patch_);
			Attached_ = Other.Attached_;

			Other.CallInstruction_ = nullptr;
			Other.Original_ = nullptr;
			Other.Replacement_ = nullptr;
			Other.Attached_ = false;
		}
	};

	template<typename Ret, typename... Args>
	class CallHook<Ret(*)(Args...)> : public CallHook<Ret(Args...)>
	{
	public:
		using Base = CallHook<Ret(Args...)>;
		using Base::Base;
	};
}