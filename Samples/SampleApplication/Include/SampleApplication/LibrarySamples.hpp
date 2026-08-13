#pragma once

#include <UEFIpp/UEFIpp.hpp>

namespace SampleApplication
{
	class TestSuite
	{
	public:
		auto Begin(StringView Name) -> Foundation::Void;
		auto Check(StringView Description, Foundation::Bool Condition) -> Foundation::Void;

		[[nodiscard]] auto Passed() const -> Foundation::Size;
		[[nodiscard]] auto Failed() const -> Foundation::Size;
		[[nodiscard]] auto AllPassed() const -> Foundation::Bool;

	private:
		Foundation::Size Passed_{};
		Foundation::Size Failed_{};
	};

	auto RunStringSamples(TestSuite& Tests) -> Foundation::Void;
	auto RunContainerSamples(TestSuite& Tests) -> Foundation::Void;
	auto RunFunctionalSamples(TestSuite& Tests) -> Foundation::Void;
	auto RunOwnershipSamples(TestSuite& Tests) -> Foundation::Void;
}
