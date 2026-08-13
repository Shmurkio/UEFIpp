#include <SampleApplication/LibrarySamples.hpp>

namespace SampleApplication
{
	auto RunStringSamples(
		TestSuite& Tests
	) -> Foundation::Void
	{
		Tests.Begin("Strings");

		constexpr StringView View{ "UEFI++ Library" };

		Tests.Check(
			"StringView exposes size, front, and back",
			View.Size() == 14 && View.Front() == 'U' && View.Back() == 'y'
		);

		Tests.Check(
			"StringView searches characters and text",
			View.Find('+') == 4
			&& View.Find("Library") == 7
			&& View.FindLast('+') == 5
			&& View.Contains("EFI")
		);

		Tests.Check(
			"StringView recognizes prefixes and suffixes",
			View.StartsWith("UEFI")
			&& View.StartsWith('U')
			&& View.EndsWith("Library")
			&& View.EndsWith('y')
		);

		auto Trimmed = View;
		Trimmed.RemovePrefix(7);
		Trimmed.RemoveSuffix(4);

		Tests.Check(
			"StringView creates and trims non-owning views",
			Trimmed == "Lib" && View.Substr(7) == "Library"
		);

		const auto CharacterSpan = View.AsSpan();

		Tests.Check(
			"StringView converts to a read-only Span",
			CharacterSpan.Size() == View.Size()
			&& CharacterSpan.Front() == View.Front()
			&& CharacterSpan.Back() == View.Back()
		);

		Tests.Check(
			"StringView compares lexicographically",
			StringView{ "alpha" } < StringView{ "beta" }
			&& View.Compare("UEFI++ Library") == 0
		);

		constexpr StringView EmptyView{};

		Tests.Check(
			"A default StringView is empty and null",
			EmptyView.Empty() && EmptyView.IsNull()
		);

		String Text{ "UEFI" };
		auto MutationSucceeded = Text.Reserve(32);
		MutationSucceeded = MutationSucceeded && Text.Append("++");
		MutationSucceeded = MutationSucceeded && Text.PushBack('!');

		Tests.Check(
			"String reserves, appends, and remains null terminated",
			MutationSucceeded
			&& Text == "UEFI++!"
			&& Text.CStr()[Text.Size()] == '\0'
		);

		Text.PopBack();
		Text += " Library";

		Tests.Check(
			"String supports mutation and searching",
			Text == "UEFI++ Library"
			&& Text.Contains("Library")
			&& Text.FindLast('+') == 5
		);

		const auto Word = Text.Substr(7, 7);

		Tests.Check(
			"String creates owned substrings",
			Word == "Library" && Word.Size() == 7
		);

		String Copy{ Text };
		String Moved{ Foundation::Utility::Move(Copy) };

		Tests.Check(
			"String copies and moves its storage",
			Moved == Text && Copy.Empty()
		);

		String Assigned{};
		auto AssignedSuccessfully = Assigned.Assign("firmware");
		const auto* SecondCharacter = Assigned.At(1);

		Tests.Check(
			"String assigns and provides checked access",
			AssignedSuccessfully
			&& SecondCharacter
			&& *SecondCharacter == 'i'
			&& Assigned.At(Assigned.Size()) == nullptr
		);

		auto ResizedSuccessfully = Assigned.Resize(10, '.');

		Tests.Check(
			"String resizes with a fill character",
			ResizedSuccessfully && Assigned == "firmware.."
		);

		Assigned.Clear();

		Tests.Check(
			"String clears without losing its valid C string",
			Assigned.Empty() && Assigned.CStr()[0] == '\0'
		);

		const WideString Wide{ L"wide" };
		const U8String Utf8{ u8"utf8" };
		const U16String Utf16{ u"utf16" };
		const U32String Utf32{ U"utf32" };

		Tests.Check(
			"Wide and Unicode string aliases use the same interface",
			Wide.View() == WideStringView{ L"wide" }
			&& Utf8.View() == U8StringView{ u8"utf8" }
			&& Utf16.View() == U16StringView{ u"utf16" }
			&& Utf32.View() == U32StringView{ U"utf32" }
		);
	}
}
