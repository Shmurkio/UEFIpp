#include <SampleApplication/LibrarySamples.hpp>

namespace
{
	enum class SampleError
	{
		Initial,
		Converted
	};
}

namespace SampleApplication
{
	auto RunContainerSamples(
		TestSuite& Tests
	) -> Foundation::Void
	{
		Tests.Begin("Containers");

		Array<Foundation::Int32, 5> Fixed{};
		Fixed.Fill(4);
		Fixed[0] = 1;
		Fixed[2] = 9;

		Tests.Check(
			"Array stores a fixed number of values",
			Fixed.Size() == 5
			&& Fixed.SizeInBytes() == sizeof(Foundation::Int32) * Fixed.Size()
			&& Fixed.Front() == 1
			&& Fixed.Back() == 4
		);

		Tests.Check(
			"Array provides checked access and searching",
			Fixed.At(2)
			&& *Fixed.At(2) == 9
			&& Fixed.At(Fixed.Size()) == nullptr
			&& Fixed.Contains(9)
			&& Fixed.Find(9) == 2
		);

		auto Middle = Fixed.View().Subspan(1, 3);

		Tests.Check(
			"Span creates first, last, and middle views",
			Middle.Size() == 3
			&& Middle.Front() == 4
			&& Middle.Back() == 4
			&& Fixed.View().First(2).Back() == 4
			&& Fixed.View().Last(2).Front() == 4
		);

		Middle.RemovePrefix(1);
		Middle.RemoveSuffix(1);

		Tests.Check(
			"Span trims a view without moving its values",
			Middle.Size() == 1 && Middle.Front() == 9
		);

		constexpr Span<const Foundation::Int32> NullSpan{};

		Tests.Check(
			"A default Span is empty and null",
			NullSpan.Empty() && NullSpan.IsNull()
		);

		Vector<Foundation::Int32> Numbers{};
		auto Built = Numbers.Reserve(12);
		Built = Built && Numbers.PushBack(4);
		Built = Built && Numbers.PushBack(1);
		Built = Built && Numbers.PushBack(3);
		auto* Emplaced = Numbers.EmplaceBack(5);
		Built = Built && Emplaced != nullptr;

		Tests.Check(
			"Vector reserves, pushes, and emplaces values",
			Built
			&& Numbers.Size() == 4
			&& Numbers.Capacity() >= 12
			&& Numbers.Back() == 5
		);

		auto Edited = Built && Numbers.Insert(1, 2);
		Edited = Edited && Numbers.Erase(3);

		Array<Foundation::Int32, 2> Tail{};
		Tail[0] = 7;
		Tail[1] = 6;
		Edited = Edited && Numbers.Append(Tail.View());

		Tests.Check(
			"Vector inserts, erases, and appends ranges",
			Edited
			&& Numbers.Size() == 6
			&& Numbers[0] == 4
			&& Numbers[1] == 2
			&& Numbers.Back() == 6
		);

		const auto Removed = Numbers.RemoveIf([](Foundation::Int32 Value)
			{
				return Value % 2 != 0;
			});

		Numbers.Reverse();
		Numbers.Sort();

		Tests.Check(
			"Vector removes, reverses, and sorts values",
			Removed == 3
			&& Numbers.Size() == 3
			&& Numbers[0] == 2
			&& Numbers[1] == 4
			&& Numbers[2] == 6
		);

		Tests.Check(
			"Vector performs lower-bound and binary searches",
			Numbers.LowerBound(5) == 2
			&& Numbers.BinarySearch(4)
			&& !Numbers.BinarySearch(5)
		);

		Vector<Foundation::Int32> Copied{ Numbers };
		Vector<Foundation::Int32> Moved{ Foundation::Utility::Move(Copied) };

		Tests.Check(
			"Vector copies and moves its elements",
			Moved == Numbers && Copied.Empty()
		);

		auto Resized = Moved.Resize(6, 8);
		Resized = Resized && Moved.PopBack();
		const auto Shrunk = Moved.ShrinkToFit();

		Tests.Check(
			"Vector resizes, pops, and shrinks its allocation",
			Resized
			&& Shrunk
			&& Moved.Size() == 5
			&& Moved.Capacity() == Moved.Size()
			&& Moved.Back() == 8
		);

		Array<Foundation::Int32, 3> Replacement{};
		Replacement[0] = 9;
		Replacement[1] = 8;
		Replacement[2] = 7;

		Vector<Foundation::Int32> Assigned{};
		auto AssignedSuccessfully = Assigned.Assign(Replacement.View());
		Assigned.Swap(Moved);

		Tests.Check(
			"Vector assigns ranges and swaps storage",
			AssignedSuccessfully
			&& Moved.Size() == 3
			&& Moved.Front() == 9
			&& Assigned.Size() == 5
		);

		Moved.Release();

		Tests.Check(
			"Vector releases its allocation",
			Moved.Empty() && Moved.Data() == nullptr && Moved.Capacity() == 0
		);

		Optional<Foundation::Int32> EmptyOptional{};
		Optional<String> Name{ InPlace, "UEFI++" };

		Tests.Check(
			"Optional represents empty and populated values",
			EmptyOptional == NullOpt
			&& EmptyOptional.ValueOr(7) == 7
			&& Name
			&& Name->Size() == 6
		);

		const auto NameLength = Name.Transform([](const String& Value)
			{
				return Value.Size();
			});

		auto Recovered = EmptyOptional.OrElse([]
			{
				return Optional<Foundation::Int32>{ 42 };
			});

		Tests.Check(
			"Optional transforms and recovers values",
			NameLength
			&& NameLength.Value() == 6
			&& Recovered == 42
		);

		Optional<Foundation::Int32> Left{ 1 };
		Optional<Foundation::Int32> Right{ 2 };
		Left.Swap(Right);
		Left.Emplace(9);
		Right.Reset();

		Tests.Check(
			"Optional swaps, emplaces, and resets values",
			Left == 9 && Right == NullOpt
		);

		Expected<Foundation::Int32, SampleError> Success{ 21 };
		Expected<Foundation::Int32, SampleError> Failure
		{
			MakeUnexpected(SampleError::Initial)
		};

		Tests.Check(
			"Expected stores either a value or an error",
			Success
			&& Success.Value() == 21
			&& !Failure
			&& Failure.Error() == SampleError::Initial
			&& Failure.ValueOr(5) == 5
		);

		const auto Doubled = Success.Transform([](Foundation::Int32 Value)
			{
				return Value * 2;
			});

		const auto Chained = Success.AndThen([](Foundation::Int32 Value)
			{
				return Expected<Foundation::Int32, SampleError>{ Value + 1 };
			});

		Tests.Check(
			"Expected transforms and chains successful values",
			Doubled
			&& Doubled.Value() == 42
			&& Chained
			&& Chained.Value() == 22
		);

		const auto ConvertedError = Failure.TransformError([](SampleError)
			{
				return SampleError::Converted;
			});

		const auto RecoveredExpected = Failure.OrElse([](SampleError)
			{
				return Expected<Foundation::Int32, SampleError>{ 84 };
			});

		Tests.Check(
			"Expected transforms and recovers errors",
			!ConvertedError
			&& ConvertedError.Error() == SampleError::Converted
			&& RecoveredExpected
			&& RecoveredExpected.Value() == 84
		);

		Expected<Foundation::Void, SampleError> Completed{};
		Expected<Foundation::Void, SampleError> Broken
		{
			MakeUnexpected(SampleError::Initial)
		};

		const auto CompletionValue = Completed.Transform([]
			{
				return Foundation::Int32{ 7 };
			});

		Tests.Check(
			"Expected<void, E> represents completion or failure",
			Completed
			&& !Broken
			&& Broken.Error() == SampleError::Initial
			&& CompletionValue
			&& CompletionValue.Value() == 7
		);
	}
}