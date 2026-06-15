#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/UEFI/Status.hpp>
#include <UEFIpp/UEFI/Types.hpp>
#include <UEFIpp/Protocols/InputKey.hpp>

namespace UEFIpp::Protocols
{
	using InputResetFn = UEFI::StatusValue(*)(class SimpleTextInput* This, Foundation::Bool ExtendedVerification);
	using ReadKeyStrokeFn = UEFI::StatusValue(*)(class SimpleTextInput* This, InputKey* Key);

	class SimpleTextInput
	{
	public:
		InputResetFn Reset;
		ReadKeyStrokeFn ReadKeyStroke;
		UEFI::Event WaitForKey;

		[[nodiscard]] constexpr auto operator<=>(const SimpleTextInput&) const = default;
	};

	static_assert(sizeof(InputKey) == 4);
	static_assert(sizeof(SimpleTextInput) == 24);

	static_assert(Foundation::Traits::IsStandardLayout<InputKey>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<InputKey>::Value);
	static_assert(Foundation::Traits::IsStandardLayout<SimpleTextInput>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<SimpleTextInput>::Value);
}