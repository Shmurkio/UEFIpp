#include <UEFIpp/UEFIpp.hpp>
#include <SampleApplication/LibrarySamples.hpp>

namespace
{
	[[nodiscard]] auto IoFailure(
		const IO::Error& Error
	) -> UEFI::MainResult
	{
		return MakeUnexpected(
			Error.Status.Failed()
				? Error.Status.Code()
				: UEFI::StatusCode::DeviceError
		);
	}
}

[[nodiscard]] auto Main(
	const Vector<U8String>& Args
) -> UEFI::MainResult
{
	auto& Io = IO::SystemIO();

	if (auto Cleared = Io.ConsoleTransport().ClearTerminal(); !Cleared)
	{
		return IoFailure(Cleared.Error());
	}

	auto Header = IO::Out(Io.Console())
		<< "SampleApplication started" << IO::Newline
		<< "UEFI++ Library sample suite" << IO::Newline
		<< "Command-line arguments: " << Args.Size()
		<< IO::Newline << IO::Newline << IO::Flush;
	if (!Header)
	{
		return IoFailure(Header.Error());
	}

	auto Started = Io.Log().Log<IO::Severity::Info>(
		UEFIPP_SOURCE_LOCATION(), "Library sample suite started");
	if (!Started)
	{
		return IoFailure(Started.Error());
	}

	if (!Args.Empty())
	{
		auto ArgumentHeader = IO::Out(Io.Console())
			<< "Arguments:" << IO::Newline;
		if (!ArgumentHeader)
		{
			return IoFailure(ArgumentHeader.Error());
		}

		for (const auto& Arg : Args)
		{
			auto Argument = IO::Out(Io.Console())
				<< "  " << Arg << IO::Newline;
			if (!Argument)
			{
				return IoFailure(Argument.Error());
			}
		}
	}

	SampleApplication::TestSuite Tests{};
	SampleApplication::RunStringSamples(Tests);
	SampleApplication::RunContainerSamples(Tests);
	SampleApplication::RunFunctionalSamples(Tests);
	SampleApplication::RunOwnershipSamples(Tests);
	SampleApplication::RunIoSamples(Tests);

	auto Summary = IO::Out(Io.Console())
		<< IO::Newline
		<< "Passed: " << Tests.Passed() << IO::Newline
		<< "Failed: " << Tests.Failed() << IO::Newline;
	if (!Summary)
	{
		return IoFailure(Summary.Error());
	}

	if (!Tests.AllPassed())
	{
		UEFIPP_LOG(Io.Log(), IO::Severity::Error,
			"Library sample suite failed: {} passed, {} failed",
			Tests.Passed(), Tests.Failed());
		return MakeUnexpected(UEFI::StatusCode::Aborted);
	}

	auto CompletedOutput = IO::Out(Io.Console())
		<< "All Library samples passed" << IO::Newline << IO::Flush;
	if (!CompletedOutput)
	{
		return IoFailure(CompletedOutput.Error());
	}

	auto Completed = Io.Log().Log<IO::Severity::Info>(
		UEFIPP_SOURCE_LOCATION(), "Library sample suite completed");
	if (!Completed)
	{
		return IoFailure(Completed.Error());
	}

	return {};
}
