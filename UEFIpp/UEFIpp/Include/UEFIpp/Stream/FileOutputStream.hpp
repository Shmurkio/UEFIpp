#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/FileSystem/File.hpp>
#include <UEFIpp/Stream/Out/Out.hpp>

namespace UEFIpp::Stream
{
	class FileOutputStream
	{
	public:
		FileOutputStream() = default;

		explicit FileOutputStream(const FileSystem::Path& Path, FileSystem::FileOpenMode Mode = FileSystem::FileOpenMode::CreateReadWrite, Foundation::Bool AutoFlush = false)
		{
			Open(Path, Mode, AutoFlush);
		}

		~FileOutputStream()
		{
			Flush();
			Close();
		}

		auto Open(const FileSystem::Path& Path, FileSystem::FileOpenMode Mode = FileSystem::FileOpenMode::CreateReadWrite, Foundation::Bool AutoFlush = false) -> Foundation::Bool
		{
			Close();

			if (!File_.Open(Path, Mode))
			{
				Sink_.Reset(nullptr);
				return false;
			}

			Sink_.Reset(&File_, AutoFlush);

			return true;
		}

		auto Close() -> Foundation::Bool
		{
			Sink_.Reset(nullptr);
			return File_.Close();
		}

		auto Flush() -> Foundation::Bool
		{
			return File_.Flush();
		}

		[[nodiscard]] auto Valid() const -> Foundation::Bool
		{
			return File_.Valid();
		}

		template<typename TValue>
		auto operator<<(const TValue& Value) -> FileOutputStream&
		{
			Output_ << Value;
			return *this;
		}

	private:
		FileSystem::File File_{};
		FileOutputSink Sink_{ &File_ };
		Output<FileOutputSink> Output_{ Sink_ };
	};
}