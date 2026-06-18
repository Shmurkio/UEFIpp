#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/FileSystem/File.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Memory/Memory.hpp>

namespace UEFIpp::Stream
{
	class FileInputStream
	{
	public:
		FileInputStream() = default;

		explicit FileInputStream(const FileSystem::Path& Path, FileSystem::FileOpenMode Mode = FileSystem::FileOpenMode::Read)
		{
			Open(Path, Mode);
		}

		FileInputStream(const FileInputStream&) = delete;
		auto operator=(const FileInputStream&) -> FileInputStream & = delete;

		FileInputStream(FileInputStream&& Other) noexcept
		{
			*this = Foundation::Utility::Move(Other);
		}

		auto operator=(FileInputStream&& Other) noexcept -> FileInputStream&
		{
			if (this == &Other)
			{
				return *this;
			}

			Close();

			File_ = Foundation::Utility::Move(Other.File_);
			Buffer_ = Foundation::Utility::Move(Other.Buffer_);
			Position_ = Other.Position_;
			Open_ = Other.Open_;

			Other.Position_ = 0;
			Other.Open_ = false;

			return *this;
		}

		~FileInputStream()
		{
			Close();
		}

		auto Open(const FileSystem::Path& Path, FileSystem::FileOpenMode Mode = FileSystem::FileOpenMode::Read) -> Foundation::Bool
		{
			Close();

			if (!File_.Open(Path, Mode))
			{
				return false;
			}

			if (!File_.Read(Buffer_))
			{
				File_.Close();
				Buffer_.Clear();
				return false;
			}

			Position_ = 0;
			Open_ = true;

			return true;
		}

		auto Close() -> Foundation::Bool
		{
			Buffer_.Clear();
			Position_ = 0;
			Open_ = false;
			return File_.Close();
		}

		[[nodiscard]] auto Valid() const -> Foundation::Bool
		{
			return Open_ && File_.Valid();
		}

		[[nodiscard]] explicit operator Foundation::Bool() const
		{
			return Valid();
		}

		[[nodiscard]] auto Empty() const -> Foundation::Bool
		{
			return Buffer_.Empty();
		}

		[[nodiscard]] auto Eof() const -> Foundation::Bool
		{
			return Position_ >= Buffer_.Size();
		}

		[[nodiscard]] auto Size() const -> Foundation::Uint64
		{
			return Foundation::Cast::Auto<Foundation::Uint64>(Buffer_.Size());
		}

		[[nodiscard]] auto Tell() const -> Foundation::Uint64
		{
			return Foundation::Cast::Auto<Foundation::Uint64>(Position_);
		}

		auto Seek(Foundation::Uint64 Position) -> Foundation::Bool
		{
			if (!Valid() || Position > Size())
			{
				return false;
			}

			Position_ = Foundation::Cast::Auto<Foundation::Size>(Position);
			return true;
		}

		auto Rewind() -> Foundation::Void
		{
			Position_ = 0;
		}

		[[nodiscard]] auto Peek(Foundation::Char& Character) const -> Foundation::Bool
		{
			if (!Valid() || Eof())
			{
				return false;
			}

			Character = Foundation::Cast::Auto<Foundation::Char>(Buffer_.Data()[Position_]);
			return true;
		}

		auto Get(Foundation::Char& Character) -> Foundation::Bool
		{
			if (!Peek(Character))
			{
				return false;
			}

			++Position_;
			return true;
		}

		auto Read(Foundation::Uint8* Data, Foundation::Uint64 Length, Foundation::Uint64& BytesRead) -> Foundation::Bool
		{
			BytesRead = 0;

			if (!Valid() || (!Data && Length != 0))
			{
				return false;
			}

			if (Length == 0 || Eof())
			{
				return true;
			}

			const auto Remaining = Buffer_.Size() - Position_;
			const auto Wanted = Foundation::Cast::Auto<Foundation::Size>(Length);
			const auto Count = Wanted < Remaining ? Wanted : Remaining;

			Memory::Copy(Data, Buffer_.Data() + Position_, Count);

			Position_ += Count;
			BytesRead = Foundation::Cast::Auto<Foundation::Uint64>(Count);

			return true;
		}

		auto ReadLine(Library::String& Line) -> Foundation::Bool
		{
			Line.Clear();

			if (!Valid() || Eof())
			{
				return false;
			}

			for (;;)
			{
				Foundation::Char Character{};

				if (!Get(Character))
				{
					break;
				}

				if (Character == '\n')
				{
					break;
				}

				if (Character == '\r')
				{
					Foundation::Char Next{};

					if (Peek(Next) && Next == '\n')
					{
						++Position_;
					}

					break;
				}

				Line.PushBack(Character);
			}

			return true;
		}

		auto ReadToken(Library::String& Token) -> Foundation::Bool
		{
			Token.Clear();

			if (!Valid())
			{
				return false;
			}

			SkipWhitespace();

			if (Eof())
			{
				return false;
			}

			for (;;)
			{
				Foundation::Char Character{};

				if (!Peek(Character) || IsWhitespace(Character))
				{
					break;
				}

				++Position_;
				Token.PushBack(Character);
			}

			return !Token.Empty();
		}

		[[nodiscard]] auto File() -> FileSystem::File&
		{
			return File_;
		}

		[[nodiscard]] auto File() const -> const FileSystem::File&
		{
			return File_;
		}

		[[nodiscard]] auto Buffer() const -> const Library::Vector<Foundation::Uint8>&
		{
			return Buffer_;
		}

		auto operator>>(Library::String& String) -> FileInputStream&
		{
			ReadToken(String);
			return *this;
		}

		auto operator>>(Foundation::Char& Character) -> FileInputStream&
		{
			Get(Character);
			return *this;
		}

	private:
		[[nodiscard]] static constexpr auto IsWhitespace(Foundation::Char Character) -> Foundation::Bool
		{
			return Character == ' ' || Character == '\t' || Character == '\r' || Character == '\n';
		}

		auto SkipWhitespace() -> Foundation::Void
		{
			while (!Eof())
			{
				const auto Character = Foundation::Cast::Auto<Foundation::Char>(Buffer_.Data()[Position_]);

				if (!IsWhitespace(Character))
				{
					break;
				}

				++Position_;
			}
		}

	private:
		FileSystem::File File_{};
		Library::Vector<Foundation::Uint8> Buffer_{};
		Foundation::Size Position_{};
		Foundation::Bool Open_{};
	};

	[[nodiscard]] inline auto GetLine(FileInputStream& Stream, Library::String& Line) -> Foundation::Bool
	{
		return Stream.ReadLine(Line);
	}
}