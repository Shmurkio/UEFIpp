#include <SampleApplication/LibrarySamples.hpp>

namespace SampleApplication
{
	auto TestSuite::Begin(
		StringView Name
	) -> Foundation::Void
	{
		Stream::Out::Console
			<< Stream::Endl
			<< "-- " << Name << " --" << Stream::Endl;
	}

	auto TestSuite::Check(
		StringView Description,
		Foundation::Bool Condition
	) -> Foundation::Void
	{
		if (Condition)
		{
			++Passed_;

			Stream::Out::Console
				<< "  [pass] " << Description << Stream::Endl;

			return;
		}

		++Failed_;

		Stream::Out::Console
			<< "  [FAIL] " << Description << Stream::Endl;

		Stream::Out::Serial
			<< Trace() << "Failed: " << Description << Stream::Endl;
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
