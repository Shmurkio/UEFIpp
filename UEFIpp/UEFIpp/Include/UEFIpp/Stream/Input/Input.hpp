#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Library/Functional/Event.hpp>
#include <UEFIpp/Protocols/SimpleTextInputEx.hpp>

namespace UEFIpp::Stream
{
	template<typename TSource>
	class Input
	{
	public:
		Library::Event<Protocols::KeyData> OnKey{};
		Library::Event<Library::String::ValueType> OnCharacter{};
		Library::Event<Library::StringView> OnLine{};

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
			auto Key = Source_.ReadKey();
			OnKey.Emit(Key);

			return Key;
		}

		[[nodiscard]] auto ReadKeyEcho() -> Protocols::KeyData
		{
			auto Key = Source_.ReadKeyEcho();

			OnKey.Emit(Key);

			return Key;
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
					const auto Character = Foundation::Cast::Auto<Library::String::ValueType>(Key.Key.UnicodeChar);
					OnCharacter.Emit(Character);
					return Character;
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
					const auto Character = Foundation::Cast::Auto<Library::String::ValueType>(Key.Key.UnicodeChar);
					OnCharacter.Emit(Character);
					return Character;
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

			OnLine.Emit(String.View());
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