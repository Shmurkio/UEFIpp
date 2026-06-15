#include <UEFIpp/Reverse/Pattern.hpp>

namespace UEFIpp::Reverse
{
	namespace
	{
		[[nodiscard]] auto IsSpace(char Character) -> Foundation::Bool
		{
			return Character == ' ' || Character == '\t' || Character == '\r' || Character == '\n';
		}

		[[nodiscard]] auto IsHex(char Character) -> Foundation::Bool
		{
			return (Character >= '0' && Character <= '9') || (Character >= 'A' && Character <= 'F') || (Character >= 'a' && Character <= 'f');
		}

		[[nodiscard]] auto HexToNibble(char Character) -> Foundation::Uint8
		{
			if (Character >= '0' && Character <= '9')
			{
				return static_cast<Foundation::Uint8>(Character - '0');
			}

			if (Character >= 'A' && Character <= 'F')
			{
				return static_cast<Foundation::Uint8>(Character - 'A' + 10);
			}

			return static_cast<Foundation::Uint8>(Character - 'a' + 10);
		}
	}

	auto Pattern::Compile(StringViewType Text) -> Library::Optional<Pattern>
	{
		StorageType Bytes{};

		for (Foundation::Size Index = 0; Index < Text.Size();)
		{
			while (Index < Text.Size() && IsSpace(Text[Index]))
			{
				++Index;
			}

			if (Index >= Text.Size())
			{
				break;
			}

			if (Text[Index] == '?')
			{
				++Index;

				if (Index < Text.Size() && Text[Index] == '?')
				{
					++Index;
				}

				Bytes.PushBack(PatternByte::Wildcard());
				continue;
			}

			if (Index + 1 >= Text.Size())
			{
				return {};
			}

			if (!IsHex(Text[Index]) || !IsHex(Text[Index + 1]))
			{
				return {};
			}

			const auto Byte = static_cast<Foundation::Uint8>((HexToNibble(Text[Index]) << 4) | HexToNibble(Text[Index + 1]));

			Bytes.PushBack(PatternByte::Value(Byte));
			Index += 2;
		}

		if (Bytes.Empty())
		{
			return {};
		}

		return Pattern{ Foundation::Utility::Move(Bytes) };
	}
}