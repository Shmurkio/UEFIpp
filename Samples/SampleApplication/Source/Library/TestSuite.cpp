#include <SampleApplication/LibrarySamples.hpp>

namespace SampleApplication
{
	auto TestSuite::Begin(
		StringView Name
	) -> Foundation::Void
	{
		(void)IO::Println(IO::SystemIO().Console(), "\n-- {} --", Name);
	}

	auto TestSuite::Check(
		StringView Description,
		Foundation::Bool Condition
	) -> Foundation::Void
	{
		if (Condition)
		{
			++Passed_;

			(void)IO::Println(IO::SystemIO().Console(), "  [pass] {}", Description);

			return;
		}

		++Failed_;

		auto& Io = IO::SystemIO();
		(void)IO::Println(Io.Console(), "  [FAIL] {}", Description);
		UEFIPP_LOG(Io.Log(), IO::Severity::Error, "Failed: {}", Description);
	}

	auto TestSuite::Passed() const -> Foundation::Size
	{
		return Passed_;
	}

	auto TestSuite::Failed() const -> Foundation::Size
	{
		return Failed_;
	}

	auto TestSuite::AllPassed() const -> Foundation::Bool
	{
		return Failed_ == 0;
	}
}
