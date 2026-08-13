#pragma once

#include <UEFIpp/Architecture/X64/Unwind/Context.hpp>
#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Expected.hpp>
#include <UEFIpp/Library/Containers/Optional.hpp>

namespace UEFIpp::Architecture::X64::Unwind
{
	enum class UnwindError : Foundation::Uint8
	{
		InvalidImage,
		InvalidContext,
		MissingExceptionDirectory,
		InvalidExceptionDirectory,
		InvalidRuntimeFunction,
		InvalidUnwindInfo,
		UnsupportedUnwindVersion,
		UnsupportedUnwindOperation,
		UnsupportedRegister,
		InvalidStackProgress
	};

	struct RuntimeFunctionRange
	{
		Foundation::UintPtr Begin{};
		Foundation::UintPtr End{};
		Foundation::Uint32 BeginRva{};
		Foundation::Uint32 EndRva{};
		Foundation::Uint32 UnwindInfoRva{};

		[[nodiscard]] constexpr auto Size() const -> Foundation::Size
		{
			return End >= Begin
				? Foundation::Cast::Auto<Foundation::Size>(End - Begin)
				: 0;
		}

		[[nodiscard]] constexpr auto Contains(
			Foundation::UintPtr Address
		) const -> Foundation::Bool
		{
			return Address >= Begin && Address < End;
		}
	};

	struct Frame
	{
		Context Caller{};
		Foundation::UintPtr* ReturnAddressSlot{};
	};

	class Unwinder
	{
	public:
		explicit Unwinder(const Foundation::Void* ImageBase);

		[[nodiscard]] auto Valid() const -> Foundation::Bool;
		[[nodiscard]] auto Contains(Foundation::UintPtr Address) const -> Foundation::Bool;
		[[nodiscard]] auto ImageBase() const -> Foundation::UintPtr;
		[[nodiscard]] auto ImageSize() const -> Foundation::Size;
		[[nodiscard]] auto FindRuntimeFunction(
			Foundation::UintPtr Rip
		) const -> Library::Optional<RuntimeFunctionRange>;

		[[nodiscard]] auto VirtualUnwind(
			const Context& Current
		) const -> Library::Expected<Frame, UnwindError>;

	private:
		struct RuntimeFunction;

		[[nodiscard]] auto LookupRuntimeFunction(
			Foundation::UintPtr Rip
		) const -> const RuntimeFunction*;

		[[nodiscard]] auto Register(
			Context& Value,
			Foundation::Uint8 Id
		) const -> Library::Expected<Foundation::UintPtr*, UnwindError>;

		const Foundation::Uint8* Base_{};
		Foundation::Size ImageSize_{};
		const RuntimeFunction* Functions_{};
		Foundation::Size FunctionCount_{};
	};
}