#pragma once

#include <UEFIpp/Foundation/Types.hpp>

namespace UEFIpp::Foundation
{
	struct SourceLocation
	{
		const Char* File;
		const Char* Function;
		Uint32 Line;

		constexpr SourceLocation(const Char* File, const Char* Function, Uint32 Line) noexcept : File(File), Function(Function), Line(Line)
		{
		}
	};

	class Assertions
	{
	public:
		Assertions() = delete;

		[[noreturn]] static auto Panic([[maybe_unused]] const Char* Message, [[maybe_unused]] SourceLocation Location) noexcept -> Void
		{
			__debugbreak();

			for (;;)
			{
			}
		}

		static auto Assert([[maybe_unused]] Bool Condition, [[maybe_unused]] const Char* Expression, [[maybe_unused]] SourceLocation Location) noexcept -> Void
		{
#if defined(UEFIPP_DEBUG)
			if (!Condition)
			{
				Panic(Expression, Location);
			}
#endif
		}

		static auto Verify(Bool Condition, const Char* Expression, SourceLocation Location) noexcept -> Void
		{
			if (!Condition)
			{
				Panic(Expression, Location);
			}
		}

		[[noreturn]] static auto Unreachable(SourceLocation Location) noexcept -> Void
		{
			Panic("Unreachable code reached", Location);
		}
	};
}

#define UEFIPP_SOURCE_LOCATION() ::UEFIpp::Foundation::SourceLocation { __FILE__, __FUNCTION__, __LINE__ }
#define UEFIPP_ASSERT(Expression) ::UEFIpp::Foundation::Assertions::Assert(static_cast<::UEFIpp::Foundation::Bool>(Expression), #Expression, UEFIPP_SOURCE_LOCATION())
#define UEFIPP_VERIFY(Expression) ::UEFIpp::Foundation::Assertions::Verify(static_cast<::UEFIpp::Foundation::Bool>(Expression), #Expression, UEFIPP_SOURCE_LOCATION())
#define UEFIPP_PANIC(Message) ::UEFIpp::Foundation::Assertions::Panic(Message, UEFIPP_SOURCE_LOCATION())
#define UEFIPP_UNREACHABLE() ::UEFIpp::Foundation::Assertions::Unreachable(UEFIPP_SOURCE_LOCATION())