#include <SampleApplication/LibrarySamples.hpp>

namespace SampleApplication
{
	auto RunFunctionalSamples(
		TestSuite& Tests
	) -> Foundation::Void
	{
		Tests.Begin("Functional");

		auto Multiplier = [](Foundation::Int32 Value)
			{
				return Value * 3;
			};

		FunctionRef<Foundation::Int32(Foundation::Int32)> Reference{ Multiplier };
		FunctionRef<Foundation::Int32(Foundation::Int32)> EmptyReference{};

		Tests.Check(
			"FunctionRef invokes a non-owning callable",
			Reference && Reference(7) == 21 && !EmptyReference
		);

		Function<Foundation::Int32(Foundation::Int32)> Increment
		{
			[Offset = Foundation::Int32{ 2 }](Foundation::Int32 Value)
			{
				return Value + Offset;
			}
		};

		Function<Foundation::Int32(Foundation::Int32)> Copied{ Increment };
		Function<Foundation::Int32(Foundation::Int32)> Moved
		{
			Foundation::Utility::Move(Copied)
		};

		Tests.Check(
			"Function owns, copies, and moves a callable",
			Increment
			&& Increment(5) == 7
			&& Moved
			&& Moved(8) == 10
			&& !Copied
		);

		Array<Foundation::Uint64, 8> LargeCapture{};
		LargeCapture.Fill(1);
		LargeCapture.Back() = 9;

		Function<Foundation::Uint64(Foundation::Uint64)> LargeFunction
		{
			[LargeCapture](Foundation::Uint64 Value)
			{
				return Value + LargeCapture.Back();
			}
		};

		Tests.Check(
			"Function stores callables larger than its inline buffer",
			LargeFunction && LargeFunction(11) == 20
		);

		Increment.Reset();

		Tests.Check(
			"Function resets to an empty state",
			Increment.Empty() && !Increment
		);

		UniquePtr<Foundation::Int32> Offset
		{
			new Foundation::Int32{ 4 }
		};

		MoveOnlyFunction<Foundation::Int32(Foundation::Int32)> AddOffset
		{
			[Offset = Foundation::Utility::Move(Offset)](Foundation::Int32 Value)
			{
				return Offset ? Value + *Offset : Value;
			}
		};

		MoveOnlyFunction<Foundation::Int32(Foundation::Int32)> MovedOnly
		{
			Foundation::Utility::Move(AddOffset)
		};

		Tests.Check(
			"MoveOnlyFunction owns a move-only callable",
			!Offset
			&& !AddOffset
			&& MovedOnly
			&& MovedOnly(6) == 10
		);

		MovedOnly.Reset();

		Tests.Check(
			"MoveOnlyFunction resets to an empty state",
			MovedOnly.Empty() && !MovedOnly
		);

		Event<Foundation::Int32> Changed{};
		Foundation::Int32 PersistentTotal{};
		Foundation::Int32 OnceTotal{};
		Foundation::Size OnceCalls{};

		const auto PersistentConnection = Changed.Subscribe(
			[&](Foundation::Int32 Value)
			{
				PersistentTotal += Value;
			}
		);

		const auto OnceConnection = Changed.SubscribeOnce(
			[&](Foundation::Int32 Value)
			{
				OnceTotal += Value;
				++OnceCalls;
			}
		);

		Tests.Check(
			"Event creates inspectable connections",
			PersistentConnection.Valid()
			&& OnceConnection.Valid()
			&& PersistentConnection != OnceConnection
			&& Changed.Contains(PersistentConnection)
			&& Changed.Size() == 2
		);

		Changed.Emit(3);
		Changed(2);

		Tests.Check(
			"Event emits persistent and one-shot callbacks",
			PersistentTotal == 5
			&& OnceTotal == 3
			&& OnceCalls == 1
			&& !Changed.Contains(OnceConnection)
		);

		const auto Disabled = Changed.Disable(PersistentConnection);
		Changed.Emit(9);
		const auto Enabled = Changed.Enable(PersistentConnection);
		Changed.Emit(1);

		Tests.Check(
			"Event disables and re-enables callbacks",
			Disabled && Enabled && PersistentTotal == 6
		);

		Foundation::Int32 ScopedTotal{};

		{
			Event<Foundation::Int32>::Handler ScopedHandler
			{
				[&](Foundation::Int32 Value)
				{
					ScopedTotal += Value;
				}
			};

			const auto Connection = Changed += Foundation::Utility::Move(ScopedHandler);
			ScopedEventConnection<Event<Foundation::Int32>> Scoped{ Changed, Connection };

			Tests.Check(
				"ScopedEventConnection owns an active subscription",
				Scoped.Connected()
				&& Scoped.Connection() == Connection
			);

			Changed.Emit(2);
		}

		Changed.Emit(2);

		Tests.Check(
			"ScopedEventConnection disconnects when leaving scope",
			ScopedTotal == 2 && Changed.Size() == 1
		);

		auto ConnectionCopy = PersistentConnection;
		ConnectionCopy.Reset();

		Tests.Check(
			"EventConnection resets independently of the event",
			!ConnectionCopy && PersistentConnection
		);

		const auto Disconnected = Changed -= PersistentConnection;

		Tests.Check(
			"Event disconnects callbacks and becomes empty",
			Disconnected && Changed.Empty()
		);

		StopEvent<Foundation::Int32> Filter{};
		Foundation::Size FirstCalls{};
		Foundation::Size StopCalls{};
		Foundation::Size LastCalls{};

		const auto FirstConnection = Filter.SubscribeOnce(
			[&](Foundation::Int32)
			{
				++FirstCalls;
				return EventResult::Continue;
			}
		);

		const auto StopConnection = Filter.Subscribe(
			[&](Foundation::Int32 Value)
			{
				++StopCalls;
				return Value >= 5 ? EventResult::Stop : EventResult::Continue;
			}
		);

		const auto LastConnection = Filter.Subscribe(
			[&](Foundation::Int32)
			{
				++LastCalls;
				return EventResult::Continue;
			}
		);

		const auto Stopped = Filter.Emit(7);
		const auto Continued = Filter(2);

		Tests.Check(
			"StopEvent stops propagation and removes one-shot callbacks",
			FirstConnection
			&& StopConnection
			&& LastConnection
			&& Stopped == EventResult::Stop
			&& Continued == EventResult::Continue
			&& FirstCalls == 1
			&& StopCalls == 2
			&& LastCalls == 1
		);

		const auto RemovedStop = Filter.Disconnect(StopConnection);
		Filter.Clear();

		Tests.Check(
			"StopEvent disconnects and clears callbacks",
			RemovedStop && Filter.Empty()
		);
	}
}
