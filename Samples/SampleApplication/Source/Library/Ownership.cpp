#include <SampleApplication/LibrarySamples.hpp>

namespace
{
	struct Widget
	{
		Foundation::Int32 Value{};
	};

	struct CountingDelete
	{
		Foundation::Size* Count{};

		auto operator()(
			Widget* Pointer
		) const noexcept -> Foundation::Void
		{
			if (Count)
			{
				++*Count;
			}

			delete Pointer;
		}
	};

	struct ResourceCloser
	{
		Foundation::Size* Count{};
		Foundation::Int32* Last{};

		auto operator()(
			Foundation::Int32& Resource
		) const noexcept -> Foundation::Void
		{
			if (Count)
			{
				++*Count;
			}

			if (Last)
			{
				*Last = Resource;
			}
		}
	};
}

namespace SampleApplication
{
	auto RunOwnershipSamples(
		TestSuite& Tests
	) -> Foundation::Void
	{
		Tests.Begin("Ownership");

		UniquePtr<Widget> First
		{
			new Widget{ 21 }
		};

		UniquePtr<Widget> Second
		{
			new Widget{ 7 }
		};

		Tests.Check(
			"UniquePtr owns and dereferences one object",
			First
			&& First->Value == 21
			&& (*First).Value == 21
		);

		auto* Released = First.Release();

		Tests.Check(
			"UniquePtr releases ownership without deleting",
			!First && Released && Released->Value == 21
		);

		First.Reset(Released);
		Swap(First, Second);

		Tests.Check(
			"UniquePtr resets and swaps owned objects",
			First
			&& Second
			&& First->Value == 7
			&& Second->Value == 21
		);

		UniquePtr<Foundation::Int32[]> Values
		{
			new Foundation::Int32[3]{ 2, 4, 6 }
		};

		Tests.Check(
			"UniquePtr<T[]> owns and indexes arrays",
			Values && Values[0] == 2 && Values[1] == 4 && Values[2] == 6
		);

		Foundation::Size DeleteCount{};

		{
			UniquePtr<Widget, CountingDelete> Counted
			{
				new Widget{ 1 },
				CountingDelete{ &DeleteCount }
			};

			Counted.Reset(new Widget{ 2 });

			Tests.Check(
				"UniquePtr invokes a custom deleter when reset",
				Counted
				&& Counted->Value == 2
				&& DeleteCount == 1
				&& Counted.GetDeleter().Count == &DeleteCount
			);
		}

		Tests.Check(
			"UniquePtr invokes its deleter at scope exit",
			DeleteCount == 2
		);

		Foundation::Size CloseCount{};
		Foundation::Int32 LastClosed{};

		{
			ResourceCloser Close{ &CloseCount, &LastClosed };
			UniqueResource Resource{ Foundation::Int32{ 12 }, Close };

			Tests.Check(
				"UniqueResource owns an arbitrary resource",
				Resource
				&& Resource.OwnsResource()
				&& Resource.Get() == 12
			);

			Resource.Reset(24);

			Tests.Check(
				"UniqueResource closes the old resource when reset",
				Resource.Get() == 24
				&& CloseCount == 1
				&& LastClosed == 12
			);

			const auto ReleasedResource = Resource.Release();
			Resource.Reset(30);

			UniqueResource Other{ Foundation::Int32{ 40 }, Close };
			Swap(Resource, Other);

			Tests.Check(
				"UniqueResource releases and swaps resources",
				ReleasedResource == 24
				&& Resource.Get() == 40
				&& Other.Get() == 30
			);
		}

		Tests.Check(
			"UniqueResource closes every owned resource at scope exit",
			CloseCount == 3
		);

		Foundation::Size ExitCount{};

		{
			ScopeExit Guard
			{
				[&]
				{
					++ExitCount;
				}
			};

			Tests.Check(
				"ScopeExit begins active",
				Guard.Active()
			);
		}

		Tests.Check(
			"ScopeExit invokes its function when leaving scope",
			ExitCount == 1
		);

		{
			ScopeExit Guard
			{
				[&]
				{
					++ExitCount;
				}
			};

			Guard.Release();

			Tests.Check(
				"ScopeExit can release its function",
				!Guard.Active()
			);
		}

		Tests.Check(
			"A released ScopeExit performs no work",
			ExitCount == 1
		);

		{
			ScopeExit Original
			{
				[&]
				{
					++ExitCount;
				}
			};

			ScopeExit Moved{ Foundation::Utility::Move(Original) };

			Tests.Check(
				"ScopeExit transfers activity when moved",
				!Original.Active() && Moved.Active()
			);
		}

		Tests.Check(
			"A moved ScopeExit invokes its function once",
			ExitCount == 2
		);
	}
}
