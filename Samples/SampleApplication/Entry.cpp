#include <UEFIpp/UEFIpp.hpp>
#include <SampleApplication/LibrarySamples.hpp>

[[nodiscard]] auto Main(
	const Vector<String>& Args
) -> UEFI::MainResult
{
	Stream::Out::Console
		<< Stream::ClearScreen
		<< "SampleApplication started" << Stream::Endl
		<< "UEFI++ Library sample suite" << Stream::Endl
		<< "Command-line arguments: " << Args.Size() << Stream::Endl << Stream::Endl;

	Stream::Out::Serial
		<< Trace() << "Library sample suite started" << Stream::Endl;

	if (!Args.Empty())
	{
		Stream::Out::Console
			<< "Arguments:" << Stream::Endl;

		for (const auto& Arg : Args)
		{
			Stream::Out::Console
				<< "  " << Arg << Stream::Endl;
		}
	}

	SampleApplication::TestSuite Tests{};

	SampleApplication::RunStringSamples(Tests);
	SampleApplication::RunContainerSamples(Tests);
	SampleApplication::RunFunctionalSamples(Tests);
	SampleApplication::RunOwnershipSamples(Tests);

	Stream::Out::Console
		<< Stream::Endl
		<< "Passed: " << Tests.Passed() << Stream::Endl
		<< "Failed: " << Tests.Failed() << Stream::Endl;

	if (!Tests.AllPassed())
	{
		Stream::Out::Serial
			<< Trace() << "Library sample suite failed" << Stream::Endl;

		return MakeUnexpected(UEFI::StatusCode::Aborted);
	}

	Stream::Out::Console
		<< "All Library samples passed" << Stream::Endl;

	Stream::Out::Serial
		<< Trace() << "Library sample suite completed" << Stream::Endl;

	return {};
}
