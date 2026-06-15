#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/Containers/Optional.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Library/String/StringView.hpp>

#include <UEFIpp/Reverse/Match.hpp>
#include <UEFIpp/Reverse/Pattern.hpp>

namespace UEFIpp::Reverse
{
	enum class ScanDirection
	{
		Forward,
		Backward
	};

	class Scanner
	{
	public:
		using Byte = Foundation::Uint8;
		using ByteSpan = Library::Span<const Byte>;
		using StringViewType = Library::StringView;
		using MatchType = Match;
		using MatchVector = Library::Vector<MatchType>;

	public:
		Scanner() = default;

		explicit Scanner(ByteSpan Memory) : Memory_(Memory)
		{
		}

		Scanner(const Foundation::Void* Base, Foundation::Size Size) : Memory_(static_cast<const Byte*>(Base), Size)
		{
		}

		~Scanner() = default;

		Scanner(const Scanner&) = default;
		Scanner(Scanner&&) noexcept = default;

		auto operator=(const Scanner&) -> Scanner & = default;
		auto operator=(Scanner&&) noexcept -> Scanner & = default;

	public:
		[[nodiscard]] auto Find(const Pattern& Pattern) const -> Library::Optional<Match>
		{
			return FindForward(Pattern);
		}

		[[nodiscard]] auto Find(StringViewType Text) const -> Library::Optional<Match>
		{
			return FindForward(Text);
		}

		[[nodiscard]] auto Find(const Pattern& Pattern, ScanDirection Direction) const -> Library::Optional<Match>
		{
			return Direction == ScanDirection::Forward ? FindForward(Pattern) : FindBackward(Pattern);
		}

		[[nodiscard]] auto Find(StringViewType Text, ScanDirection Direction) const -> Library::Optional<Match>
		{
			return Direction == ScanDirection::Forward ? FindForward(Text) : FindBackward(Text);
		}

		[[nodiscard]] auto FindForward(const Pattern& Pattern) const -> Library::Optional<Match>;

		[[nodiscard]] auto FindBackward(const Pattern& Pattern) const -> Library::Optional<Match>;

		[[nodiscard]] auto FindForward(StringViewType Text) const -> Library::Optional<Match>
		{
			auto Compiled = Pattern::Compile(Text);

			if (!Compiled)
			{
				return {};
			}

			return FindForward(Compiled.Value());
		}

		[[nodiscard]] auto FindBackward(StringViewType Text) const -> Library::Optional<Match>
		{
			auto Compiled = Pattern::Compile(Text);

			if (!Compiled)
			{
				return {};
			}

			return FindBackward(Compiled.Value());
		}

		[[nodiscard]] auto FindAll(const Pattern& Pattern) const -> MatchVector
		{
			return FindAllForward(Pattern);
		}

		[[nodiscard]] auto FindAll(StringViewType Text) const -> MatchVector
		{
			return FindAllForward(Text);
		}

		[[nodiscard]] auto FindAll(const Pattern& Pattern, ScanDirection Direction) const -> MatchVector
		{
			return Direction == ScanDirection::Forward ? FindAllForward(Pattern) : FindAllBackward(Pattern);
		}

		[[nodiscard]] auto FindAll(StringViewType Text, ScanDirection Direction) const -> MatchVector
		{
			return Direction == ScanDirection::Forward ? FindAllForward(Text) : FindAllBackward(Text);
		}

		[[nodiscard]] auto FindAllForward(const Pattern& Pattern) const -> MatchVector;

		[[nodiscard]] auto FindAllBackward(const Pattern& Pattern) const -> MatchVector;

		[[nodiscard]] auto FindAllForward(StringViewType Text) const -> MatchVector
		{
			auto Compiled = Pattern::Compile(Text);

			if (!Compiled)
			{
				return {};
			}

			return FindAllForward(Compiled.Value());
		}

		[[nodiscard]] auto FindAllBackward(StringViewType Text) const -> MatchVector
		{
			auto Compiled = Pattern::Compile(Text);

			if (!Compiled)
			{
				return {};
			}

			return FindAllBackward(Compiled.Value());
		}

	public:
		[[nodiscard]] auto Memory() const -> ByteSpan
		{
			return Memory_;
		}

		[[nodiscard]] auto Empty() const -> Foundation::Bool
		{
			return Memory_.Empty();
		}

		[[nodiscard]] auto Size() const -> Foundation::Size
		{
			return Memory_.Size();
		}

		[[nodiscard]] auto Data() const -> const Byte*
		{
			return Memory_.Data();
		}

	private:
		ByteSpan Memory_{};
	};
}