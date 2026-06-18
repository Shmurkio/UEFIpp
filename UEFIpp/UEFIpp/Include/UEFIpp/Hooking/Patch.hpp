#pragma once

#include <UEFIpp/Hooking/CodePatch.hpp>

namespace UEFIpp::Hooking
{
	class Patch
	{
	public:
		static constexpr auto MaxSize = Foundation::Size{ 32 };

		constexpr Patch() = default;

		Patch(Foundation::Void* Address, const Foundation::Void* Bytes, Foundation::Size Size, const WritePatchOptions& Options = {})
		{
			Apply(Address, Bytes, Size, Options);
		}

		Patch(const Patch&) = delete;
		auto operator=(const Patch&) -> Patch & = delete;

		Patch(Patch&& Other) noexcept
		{
			MoveFrom(Other);
		}

		auto operator=(Patch&& Other) noexcept -> Patch&
		{
			if (this != &Other)
			{
				Restore();
				MoveFrom(Other);
			}

			return *this;
		}

		~Patch()
		{
			Restore();
		}

		[[nodiscard]] auto Apply(Foundation::Void* Address, const Foundation::Void* Bytes, Foundation::Size Size, const WritePatchOptions& Options = {}) -> Foundation::Bool
		{
			if (Applied_ || !Address || !Bytes || !Size || Size > MaxSize)
			{
				return false;
			}

			if (!CodePatch::Read(Original_, Address, Size))
			{
				return false;
			}

			if (!CodePatch::Write(Address, Bytes, Size, Options))
			{
				Memory::Zero(Original_, MaxSize);
				return false;
			}

			Address_ = Address;
			Size_ = Size;
			Options_ = Options;
			Applied_ = true;

			return true;
		}

		[[nodiscard]] auto Restore() -> Foundation::Bool
		{
			if (!Applied_ || !Address_ || !Size_)
			{
				return false;
			}

			if (!CodePatch::Write(Address_, Original_, Size_, Options_))
			{
				return false;
			}

			Applied_ = false;
			return true;
		}

		[[nodiscard]] constexpr auto IsApplied() const -> Foundation::Bool
		{
			return Applied_;
		}

		[[nodiscard]] constexpr auto Address() const -> Foundation::Void*
		{
			return Address_;
		}

		[[nodiscard]] constexpr auto Size() const -> Foundation::Size
		{
			return Size_;
		}

		[[nodiscard]] constexpr auto OriginalBytes() const -> const Foundation::Uint8*
		{
			return Original_;
		}

	private:
		Foundation::Void* Address_{};
		Foundation::Size Size_{};
		Foundation::Uint8 Original_[MaxSize]{};
		WritePatchOptions Options_{};
		Foundation::Bool Applied_{};

		auto MoveFrom(Patch& Other) -> Foundation::Void
		{
			Address_ = Other.Address_;
			Size_ = Other.Size_;
			Memory::Copy(Original_, Other.Original_, MaxSize);
			Options_ = Other.Options_;
			Applied_ = Other.Applied_;

			Other.Address_ = nullptr;
			Other.Size_ = 0;
			Memory::Zero(Other.Original_, MaxSize);
			Other.Applied_ = false;
		}
	};
}