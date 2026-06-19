#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Protocols/SimpleTextInputEx.hpp>

namespace UEFIpp::Stream
{
	template<typename TSource>
	class Input
	{
	public:
		constexpr Input() = default;

		constexpr explicit Input(TSource Source) : Source_(Source)
		{
		}

		auto SetSource(TSource Source) -> Foundation::Void
		{
			Source_ = Source;
		}

		[[nodiscard]] auto ReadKey() -> Protocols::KeyData
		{
			return Source_.ReadKey();
		}

		[[nodiscard]] auto ReadKeyEcho() -> Protocols::KeyData
		{
			return Source_.ReadKeyEcho();
		}

		[[nodiscard]] auto GetKey() -> Protocols::KeyData
		{
			return ReadKey();
		}

		auto GetKey(Protocols::KeyData& Key) -> Input&
		{
			Key = ReadKey();
			return *this;
		}

		[[nodiscard]] auto ReadCharacter() -> Library::String::ValueType
		{
			for (;;)
			{
				const auto Key = ReadKey();

				if (Key.HasUnicode())
				{
					return Foundation::Cast::Auto<Library::String::ValueType>(Key.Key.UnicodeChar);
				}
			}
		}

		[[nodiscard]] auto ReadCharacterEcho() -> Library::String::ValueType
		{
			for (;;)
			{
				const auto Key = ReadKeyEcho();

				if (Key.HasUnicode())
				{
					return Foundation::Cast::Auto<Library::String::ValueType>(Key.Key.UnicodeChar);
				}
			}
		}

		[[nodiscard]] auto Get() -> Library::String::ValueType
		{
			return ReadCharacter();
		}

		auto Get(Library::String::ValueType& Character) -> Input&
		{
			Character = ReadCharacter();
			return *this;
		}

		auto ReadLine(Library::String& String) -> Foundation::Void
		{
			String.Clear();

			for (;;)
			{
				const auto Character = ReadCharacterEcho();

				if (Character == '\r' || Character == '\n')
				{
					break;
				}

				if (Character == '\b')
				{
					if (!String.Empty())
					{
						String.PopBack();
					}

					continue;
				}

				String.PushBack(Character);
			}
		}

		[[nodiscard]] constexpr auto Source() -> TSource&
		{
			return Source_;
		}

		[[nodiscard]] constexpr auto Source() const -> const TSource&
		{
			return Source_;
		}

	private:
		TSource Source_{};
	};

	template<typename TSource>
	auto operator>>(Input<TSource>& Stream, Protocols::KeyData& Key) -> Input<TSource>&
	{
		Key = Stream.ReadKey();
		return Stream;
	}

	template<typename TSource>
	auto operator>>(Input<TSource>& Stream, Library::String::ValueType& Character) -> Input<TSource>&
	{
		Character = Stream.ReadCharacter();
		return Stream;
	}

	template<typename TSource>
	auto operator>>(Input<TSource>& Stream, Library::String& String) -> Input<TSource>&
	{
		Stream.ReadLine(String);
		return Stream;
	}
}