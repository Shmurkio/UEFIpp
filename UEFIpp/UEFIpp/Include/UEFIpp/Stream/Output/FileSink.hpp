#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/FileSystem/File.hpp>

namespace UEFIpp::Stream
{
	class FileOutputSink
	{
	public:
		constexpr FileOutputSink() = default;

		constexpr explicit FileOutputSink(FileSystem::File* File, Foundation::Bool AutoFlush = true) : File_(File), AutoFlush_(AutoFlush)
		{
		}

		[[nodiscard]] constexpr auto Valid() const -> Foundation::Bool
		{
			return File_ && File_->Valid();
		}

		[[nodiscard]] constexpr explicit operator Foundation::Bool() const
		{
			return Valid();
		}

		auto Reset(FileSystem::File* File, Foundation::Bool AutoFlush = true) -> Foundation::Void
		{
			File_ = File;
			AutoFlush_ = AutoFlush;
		}

		auto Write(const Foundation::Char* Data, Foundation::Size Length) -> Foundation::Void
		{
			if (!Valid() || !Data || !Length)
			{
				return;
			}

			if (!File_->Append(Foundation::Cast::Auto<const Foundation::Uint8*>(Data), Foundation::Cast::Auto<Foundation::Uint64>(Length)
			))
			{
				return;
			}

			if (AutoFlush_)
			{
				File_->Flush();
			}
		}

		auto Write(const Foundation::WChar* Data, Foundation::Size Length) -> Foundation::Void
		{
			if (!Valid() || !Data || !Length)
			{
				return;
			}

			for (Foundation::Size Index = 0; Index < Length; ++Index)
			{
				const auto Character = Data[Index];
				const auto Narrow = Character <= 0x7F ? Foundation::Cast::Auto<Foundation::Char>(Character) : Foundation::Char{ '?' };
				Write(&Narrow, 1);
			}
		}

		auto Clear() -> Foundation::Void
		{
			if (!Valid())
			{
				return;
			}

			File_->Handle().SetCurrentPosition(0);
		}

	private:
		FileSystem::File* File_{};
		Foundation::Bool AutoFlush_{ true };
	};
}