#include <UEFIpp/Reverse/Scanner.hpp>

namespace UEFIpp::Reverse
{
	auto Scanner::FindForward(const Pattern& Pattern) const -> Library::Optional<Match>
	{
		if (Memory_.Empty() || Pattern.Empty())
		{
			return Library::NullOpt;
		}

		const auto PatternBytes = Pattern.Bytes();

		if (PatternBytes.Size() > Memory_.Size())
		{
			return Library::NullOpt;
		}

		const auto LastOffset = Memory_.Size() - PatternBytes.Size();

		for (Foundation::Size Offset = 0; Offset <= LastOffset; ++Offset)
		{
			Foundation::Bool IsMatch = true;

			for (Foundation::Size Index = 0; Index < PatternBytes.Size(); ++Index)
			{
				if (!PatternBytes[Index].Matches(Memory_[Offset + Index]))
				{
					IsMatch = false;
					break;
				}
			}

			if (IsMatch)
			{
				return Match{ Memory_.Data() + Offset, Offset };
			}
		}

		return Library::NullOpt;
	}

	auto Scanner::FindBackward(const Pattern& Pattern) const -> Library::Optional<Match>
	{
		if (Memory_.Empty() || Pattern.Empty())
		{
			return Library::NullOpt;
		}

		const auto PatternBytes = Pattern.Bytes();

		if (PatternBytes.Size() > Memory_.Size())
		{
			return Library::NullOpt;
		}

		auto Offset = Memory_.Size() - PatternBytes.Size();

		for (;;)
		{
			Foundation::Bool IsMatch = true;

			for (Foundation::Size Index = 0; Index < PatternBytes.Size(); ++Index)
			{
				if (!PatternBytes[Index].Matches(Memory_[Offset + Index]))
				{
					IsMatch = false;
					break;
				}
			}

			if (IsMatch)
			{
				return Match{ Memory_.Data() + Offset, Offset };
			}

			if (Offset == 0)
			{
				break;
			}

			--Offset;
		}

		return {};
	}

	auto Scanner::FindAllForward(const Pattern& Pattern) const -> MatchVector
	{
		MatchVector Matches{};

		if (Memory_.Empty() || Pattern.Empty())
		{
			return Matches;
		}

		const auto PatternBytes = Pattern.Bytes();

		if (PatternBytes.Size() > Memory_.Size())
		{
			return Matches;
		}

		const auto LastOffset = Memory_.Size() - PatternBytes.Size();

		for (Foundation::Size Offset = 0; Offset <= LastOffset; ++Offset)
		{
			Foundation::Bool IsMatch = true;

			for (Foundation::Size Index = 0; Index < PatternBytes.Size(); ++Index)
			{
				if (!PatternBytes[Index].Matches(Memory_[Offset + Index]))
				{
					IsMatch = false;
					break;
				}
			}

			if (IsMatch)
			{
				Matches.PushBack(Match{ Memory_.Data() + Offset, Offset });
			}
		}

		return Matches;
	}

	auto Scanner::FindAllBackward(const Pattern& Pattern) const
		-> MatchVector
	{
		MatchVector Matches{};

		if (Memory_.Empty() || Pattern.Empty())
		{
			return Matches;
		}

		const auto PatternBytes = Pattern.Bytes();

		if (PatternBytes.Size() > Memory_.Size())
		{
			return Matches;
		}

		auto Offset = Memory_.Size() - PatternBytes.Size();

		for (;;)
		{
			Foundation::Bool IsMatch = true;

			for (Foundation::Size Index = 0; Index < PatternBytes.Size(); ++Index)
			{
				if (!PatternBytes[Index].Matches(Memory_[Offset + Index]))
				{
					IsMatch = false;
					break;
				}
			}

			if (IsMatch)
			{
				Matches.PushBack(Match{ Memory_.Data() + Offset, Offset });
			}

			if (Offset == 0)
			{
				break;
			}

			--Offset;
		}

		return Matches;
	}
}