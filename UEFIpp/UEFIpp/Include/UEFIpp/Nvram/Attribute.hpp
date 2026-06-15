#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>

namespace UEFIpp::Nvram
{
	enum class Attribute : Foundation::Uint32
	{
		None = 0,
		NonVolatile = 0x00000001,
		BootServiceAccess = 0x00000002,
		RuntimeAccess = 0x00000004,
		HardwareErrorRecord = 0x00000008,
		AuthenticatedWriteAccess = 0x00000010,
		TimeBasedAuthenticatedWriteAccess = 0x00000020,
		AppendWrite = 0x00000040,
		EnhancedAuthenticatedAccess = 0x00000080,
	};

	class AttributeMask
	{
	public:
		using ValueType = Foundation::Uint32;

		constexpr AttributeMask() = default;
		constexpr AttributeMask(const AttributeMask&) = default;
		constexpr AttributeMask(AttributeMask&&) noexcept = default;

		constexpr auto operator=(const AttributeMask&) -> AttributeMask & = default;
		constexpr auto operator=(AttributeMask&&) noexcept -> AttributeMask & = default;

		constexpr ~AttributeMask() = default;

		constexpr AttributeMask(Attribute Value) : Value_(ToValue(Value))
		{
		}

		explicit constexpr AttributeMask(ValueType Value) : Value_(Value)
		{
		}

		[[nodiscard]] constexpr auto Value() const -> ValueType
		{
			return Value_;
		}

		[[nodiscard]] constexpr auto Empty() const -> Foundation::Bool
		{
			return Value_ == 0;
		}

		[[nodiscard]] constexpr auto Has(Attribute Value) const -> Foundation::Bool
		{
			return Foundation::Bit::IsSet(Value_, ToValue(Value));
		}

		[[nodiscard]] constexpr auto HasAny(AttributeMask Mask) const -> Foundation::Bool
		{
			return (Value_ & Mask.Value_) != 0;
		}

		[[nodiscard]] constexpr auto HasAll(AttributeMask Mask) const -> Foundation::Bool
		{
			return (Value_ & Mask.Value_) == Mask.Value_;
		}

		[[nodiscard]] constexpr auto Add(Attribute Value) const -> AttributeMask
		{
			return AttributeMask{ Foundation::Bit::Set(Value_, ToValue(Value)) };
		}

		[[nodiscard]] constexpr auto Add(AttributeMask Mask) const -> AttributeMask
		{
			return AttributeMask{ Value_ | Mask.Value_ };
		}

		[[nodiscard]] constexpr auto Remove(Attribute Value) const -> AttributeMask
		{
			return AttributeMask{ Foundation::Bit::Clear(Value_, ToValue(Value)) };
		}

		[[nodiscard]] constexpr auto Remove(AttributeMask Mask) const -> AttributeMask
		{
			return AttributeMask{ Value_ & ~Mask.Value_ };
		}

		[[nodiscard]] constexpr auto Toggle(Attribute Value) const -> AttributeMask
		{
			return AttributeMask{ Foundation::Bit::Toggle(Value_, ToValue(Value)) };
		}

		[[nodiscard]] constexpr auto Toggle(AttributeMask Mask) const -> AttributeMask
		{
			return AttributeMask{ Value_ ^ Mask.Value_ };
		}

		constexpr auto AddInPlace(Attribute Value) -> AttributeMask&
		{
			Value_ = Foundation::Bit::Set(Value_, ToValue(Value));
			return *this;
		}

		constexpr auto RemoveInPlace(Attribute Value) -> AttributeMask&
		{
			Value_ = Foundation::Bit::Clear(Value_, ToValue(Value));
			return *this;
		}

		constexpr auto ToggleInPlace(Attribute Value) -> AttributeMask&
		{
			Value_ = Foundation::Bit::Toggle(Value_, ToValue(Value));
			return *this;
		}

		constexpr auto Clear() -> Foundation::Void
		{
			Value_ = 0;
		}

		[[nodiscard]] explicit constexpr operator Foundation::Bool() const
		{
			return !Empty();
		}

		[[nodiscard]] friend constexpr auto operator==(AttributeMask, AttributeMask) -> Foundation::Bool = default;

	private:
		[[nodiscard]] static constexpr auto ToValue(Attribute Value) -> ValueType
		{
			return Foundation::Cast::Auto<ValueType>(Value);
		}

	private:
		ValueType Value_{};
	};

	[[nodiscard]] constexpr auto ToMask(Attribute Value) -> AttributeMask
	{
		return AttributeMask{ Value };
	}

	[[nodiscard]] constexpr auto ToValue(AttributeMask Mask) -> AttributeMask::ValueType
	{
		return Mask.Value();
	}

	[[nodiscard]] constexpr auto operator|(Attribute Left, Attribute Right) -> AttributeMask
	{
		return AttributeMask{ Left }.Add(Right);
	}

	[[nodiscard]] constexpr auto operator|(AttributeMask Left, Attribute Right) -> AttributeMask
	{
		return Left.Add(Right);
	}

	[[nodiscard]] constexpr auto operator|(Attribute Left, AttributeMask Right) -> AttributeMask
	{
		return Right.Add(Left);
	}

	[[nodiscard]] constexpr auto operator|(AttributeMask Left, AttributeMask Right) -> AttributeMask
	{
		return Left.Add(Right);
	}

	[[nodiscard]] constexpr auto operator&(AttributeMask Left, AttributeMask Right) -> AttributeMask
	{
		return AttributeMask{ Left.Value() & Right.Value() };
	}

	[[nodiscard]] constexpr auto operator&(AttributeMask Left, Attribute Right) -> AttributeMask
	{
		return AttributeMask{ Left.Value() & ToValue(AttributeMask{ Right }) };
	}

	[[nodiscard]] constexpr auto operator~(AttributeMask Mask) -> AttributeMask
	{
		return AttributeMask{ ~Mask.Value() };
	}

	[[nodiscard]] constexpr auto operator~(Attribute Value) -> AttributeMask
	{
		return ~AttributeMask{ Value };
	}

	constexpr auto operator|=(AttributeMask& Left, Attribute Right) -> AttributeMask&
	{
		Left = Left | Right;
		return Left;
	}

	constexpr auto operator|=(AttributeMask& Left, AttributeMask Right) -> AttributeMask&
	{
		Left = Left | Right;
		return Left;
	}

	constexpr auto operator&=(AttributeMask& Left, AttributeMask Right) -> AttributeMask&
	{
		Left = Left & Right;
		return Left;
	}

	inline constexpr AttributeMask NoAttributes{};
	inline constexpr AttributeMask NonVolatile{ Attribute::NonVolatile };
	inline constexpr AttributeMask BootServiceAccess{ Attribute::BootServiceAccess };
	inline constexpr AttributeMask RuntimeAccess{ Attribute::RuntimeAccess };
	inline constexpr AttributeMask HardwareErrorRecord{ Attribute::HardwareErrorRecord };
	inline constexpr AttributeMask AuthenticatedWriteAccess{ Attribute::AuthenticatedWriteAccess };
	inline constexpr AttributeMask TimeBasedAuthenticatedWriteAccess{ Attribute::TimeBasedAuthenticatedWriteAccess };
	inline constexpr AttributeMask AppendWrite{ Attribute::AppendWrite };
	inline constexpr AttributeMask EnhancedAuthenticatedAccess{ Attribute::EnhancedAuthenticatedAccess };

	inline constexpr AttributeMask DefaultAttributes = NonVolatile | BootServiceAccess | RuntimeAccess;
	inline constexpr AttributeMask VolatileBootServiceAttributes = BootServiceAccess;
	inline constexpr AttributeMask RuntimeVolatileAttributes = BootServiceAccess | RuntimeAccess;
}