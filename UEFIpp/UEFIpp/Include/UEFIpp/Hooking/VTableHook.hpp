#pragma once

#include <UEFIpp/Hooking/Patch.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Memory/Memory.hpp>

namespace UEFIpp::Hooking
{
	template<typename Signature>
	class VTableHook;

	template<typename Ret, typename... Args>
	class VTableHook<Ret(Args...)>
	{
	public:
		using Function = Ret(*)(Args...);

		constexpr VTableHook() = default;

		VTableHook(Function* Slot, Function Replacement, const WritePatchOptions& Options = {})
		{
			Attach(Slot, Replacement, Options);
		}

		VTableHook(const VTableHook&) = delete;
		auto operator=(const VTableHook&) -> VTableHook & = delete;

		VTableHook(VTableHook&& Other) noexcept
		{
			MoveFrom(Other);
		}

		auto operator=(VTableHook&& Other) noexcept -> VTableHook&
		{
			if (this != &Other)
			{
				Detach();
				MoveFrom(Other);
			}

			return *this;
		}

		~VTableHook()
		{
			Detach();
		}

		[[nodiscard]] auto Attach(Function* Slot, Function Replacement, const WritePatchOptions& Options = {}) -> Foundation::Bool
		{
			if (Attached_ || !Slot || !*Slot || !Replacement || *Slot == Replacement)
			{
				return false;
			}

			const auto NewValue = Replacement;
			if (!Patch_.Apply(Slot, &NewValue, sizeof(NewValue), Options))
			{
				return false;
			}

			Slot_ = Slot;
			Memory::Copy(&Original_, Patch_.OriginalBytes(), sizeof(Original_));
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

		[[nodiscard]] constexpr auto Slot() const -> Function*
		{
			return Slot_;
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
		Function* Slot_{};
		Function Original_{};
		Function Replacement_{};
		Patch Patch_{};
		Foundation::Bool Attached_{};

		auto MoveFrom(VTableHook& Other) -> Foundation::Void
		{
			Slot_ = Other.Slot_;
			Original_ = Other.Original_;
			Replacement_ = Other.Replacement_;
			Patch_ = Foundation::Utility::Move(Other.Patch_);
			Attached_ = Other.Attached_;

			Other.Slot_ = nullptr;
			Other.Original_ = nullptr;
			Other.Replacement_ = nullptr;
			Other.Attached_ = false;
		}
	};

	template<typename Ret, typename... Args>
	class VTableHook<Ret(*)(Args...)> : public VTableHook<Ret(Args...)>
	{
	public:
		using Base = VTableHook<Ret(Args...)>;
		using Base::Base;
	};
}