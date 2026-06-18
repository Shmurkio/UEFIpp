#pragma once

#include <UEFIpp/Hooking/Patch.hpp>
#include <UEFIpp/Hooking/X64/AbsoluteJump.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Memory/Memory.hpp>

namespace UEFIpp::Hooking
{
	template<typename Signature>
	class Detour;

	template<typename Ret, typename... Args>
	class Detour<Ret(Args...)>
	{
	public:
		using Function = Ret(*)(Args...);

		static constexpr auto PatchSize = X64::AbsoluteJump::Size;

		constexpr Detour() = default;

		Detour(Function Target, Function Replacement, const WritePatchOptions& Options = {})
		{
			Attach(Target, Replacement, Options);
		}

		Detour(const Detour&) = delete;
		auto operator=(const Detour&) -> Detour & = delete;

		Detour(Detour&& Other) noexcept
		{
			MoveFrom(Other);
		}

		auto operator=(Detour&& Other) noexcept -> Detour&
		{
			if (this != &Other)
			{
				Detach();
				MoveFrom(Other);
			}

			return *this;
		}

		~Detour()
		{
			Detach();
		}

		[[nodiscard]] auto Attach(Function Target, Function Replacement, const WritePatchOptions& Options = {}) -> Foundation::Bool
		{
			if (Attached_ || !Target || !Replacement || Target == Replacement)
			{
				return false;
			}

			Foundation::Uint8 Jump[PatchSize]{};
			X64::AbsoluteJump::Make(Jump, Foundation::Cast::Auto<Foundation::UintPtr>(Replacement));

			if (!Patch_.Apply(Foundation::Cast::Auto<Foundation::Void*>(Target), Jump, PatchSize, Options))
			{
				return false;
			}

			Target_ = Target;
			Replacement_ = Replacement;
			Options_ = Options;
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

		[[nodiscard]] constexpr auto Target() const -> Function
		{
			return Target_;
		}

		[[nodiscard]] constexpr auto Replacement() const -> Function
		{
			return Replacement_;
		}

		auto CallOriginal(Args... Arguments) -> Ret
		{
			if (!Target_)
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

			const auto WasAttached = Attached_;

			if (WasAttached && !Detach())
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
				Target_(Arguments...);

				if (WasAttached)
				{
					Reattach();
				}

				return;
			}
			else
			{
				auto Result = Target_(Arguments...);

				if (WasAttached)
				{
					Reattach();
				}

				return Result;
			}
		}

	private:
		Function Target_{};
		Function Replacement_{};
		Patch Patch_{};
		WritePatchOptions Options_{};
		Foundation::Bool Attached_{};

		[[nodiscard]] auto Reattach() -> Foundation::Bool
		{
			Foundation::Uint8 Jump[PatchSize]{};
			X64::AbsoluteJump::Make(Jump, Foundation::Cast::Auto<Foundation::UintPtr>(Replacement_));

			if (!Patch_.Apply(Foundation::Cast::Auto<Foundation::Void*>(Target_), Jump, PatchSize, Options_))
			{
				Attached_ = false;
				return false;
			}

			Attached_ = true;
			return true;
		}

		auto MoveFrom(Detour& Other) -> Foundation::Void
		{
			Target_ = Other.Target_;
			Replacement_ = Other.Replacement_;
			Patch_ = Foundation::Utility::Move(Other.Patch_);
			Options_ = Other.Options_;
			Attached_ = Other.Attached_;

			Other.Target_ = nullptr;
			Other.Replacement_ = nullptr;
			Other.Attached_ = false;
		}
	};

	template<typename Ret, typename... Args>
	class Detour<Ret(*)(Args...)> : public Detour<Ret(Args...)>
	{
	public:
		using Base = Detour<Ret(Args...)>;
		using Base::Base;
	};
}