#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/FileSystem/File.hpp>
#include <UEFIpp/Stream/Output/Output.hpp>

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

		[[nodiscard]] auto Write(const Foundation::Void* Data, Foundation::Size Size) -> Foundation::Bool
		{
			if (!Data && Size != 0)
			{
				return false;
			}

			return File_.Write(Foundation::Cast::Auto<const Foundation::Uint8*>(Data), Foundation::Cast::Auto<Foundation::Uint64>(Size));
		}

		[[nodiscard]] auto Write(Library::Span<const Foundation::Byte> Data) -> Foundation::Bool
		{
			return Write(Data.Data(), Data.SizeInBytes());
		}

		[[nodiscard]] auto Write(const Library::Vector<Foundation::Byte>& Data) -> Foundation::Bool
		{
			return Write({ Data.Data(), Data.Size() });
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