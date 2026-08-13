#include <UEFIpp/FileSystem/File.hpp>

#include <UEFIpp/Memory/Memory.hpp>
#include <UEFIpp/Protocols/Access.hpp>
#include <UEFIpp/Protocols/LoadedImage.hpp>
#include <UEFIpp/Protocols/SimpleFileSystem.hpp>
#include <UEFIpp/Protocols/Traits.hpp>
#include <UEFIpp/Text/Encoding.hpp>
#include <UEFIpp/UEFI/Context.hpp>

namespace UEFIpp::FileSystem
{
	namespace
	{
		inline constexpr UEFI::Guid FileInfoGuid{ 0x09576e92, 0x6d3f, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } };

		struct RawTime
		{
			Foundation::Uint16 Year;
			Foundation::Uint8 Month;
			Foundation::Uint8 Day;
			Foundation::Uint8 Hour;
			Foundation::Uint8 Minute;
			Foundation::Uint8 Second;
			Foundation::Uint8 Pad1;
			Foundation::Uint32 Nanosecond;
			Foundation::Int16 TimeZone;
			Foundation::Uint8 Daylight;
			Foundation::Uint8 Pad2;
		};

		struct RawFileInfo
		{
			Foundation::Uint64 Size;
			Foundation::Uint64 FileSize;
			Foundation::Uint64 PhysicalSize;
			RawTime CreateTime;
			RawTime LastAccessTime;
			RawTime ModificationTime;
			Foundation::Uint64 Attribute;
			Foundation::WChar FileName[1];
		};

		[[nodiscard]] auto ToFileTime(const RawTime& Time) -> FileSystem::Time
		{
			return FileSystem::Time{ Time.Year, Time.Month, Time.Day, Time.Hour, Time.Minute, Time.Second, Time.Nanosecond };
		}

		[[nodiscard]] auto ToOpenMask(FileOpenMode Mode) -> Foundation::Uint64
		{
			return Foundation::Cast::Auto<Foundation::Uint64>(Mode);
		}

		[[nodiscard]] auto CreateAttributesFor(FileOpenMode Mode) -> Foundation::Uint64
		{
			if (Foundation::Bit::HasAny(ToOpenMask(Mode), Protocols::ToMask(Protocols::FileOpenMode::Create)))
			{
				return Protocols::ToMask(Protocols::FileAttribute::Archive);
			}

			return 0;
		}

		[[nodiscard]] auto CurrentFileSystemHandle(
			Memory::AllocatorStub Allocator
		) -> UEFI::Handle
		{
			Protocols::Access Access{ &UEFI::Context::BootServices(), Allocator };
			auto LoadedImage = Access.Handle<Protocols::LoadedImage>(UEFI::Context::ImageHandle());

			if (!LoadedImage || !*LoadedImage)
			{
				return nullptr;
			}

			return (*LoadedImage)->DeviceHandle;
		}

		[[nodiscard]] auto QueryInfo(
			Protocols::File& Handle,
			const Path& Path,
			FileInfo& Info,
			Memory::AllocatorStub Allocator
		) -> Foundation::Bool
		{
			Foundation::UintN InfoSize{};
			auto Guid = FileInfoGuid;
			auto Status = Handle.QueryInfo(Guid, nullptr, InfoSize);

			if (Status != UEFI::StatusCode::BufferTooSmall || !InfoSize)
			{
				return false;
			}

			auto* RawBuffer = Allocator
				? Allocator.AllocateStorage<Foundation::Uint8>(InfoSize)
				: new Foundation::Uint8[InfoSize];

			if (!RawBuffer)
			{
				return false;
			}

			auto* Buffer = Foundation::Cast::Auto<RawFileInfo*>(RawBuffer);

			Guid = FileInfoGuid;
			Status = Handle.QueryInfo(Guid, Buffer, InfoSize);

			if (UEFI::IsError(Status))
			{
				if (Allocator)
				{
					UEFIPP_VERIFY(Allocator.Free(RawBuffer));
				}
				else
				{
					delete[] RawBuffer;
				}

				return false;
			}

			Info = FileInfo{
				Path,
				Buffer->FileSize,
				Buffer->PhysicalSize,
				ToFileTime(Buffer->CreateTime),
				ToFileTime(Buffer->ModificationTime),
				ToFileTime(Buffer->LastAccessTime),
				FileAttributes{ Buffer->Attribute },
				Allocator
			};

			if (Allocator)
			{
				UEFIPP_VERIFY(Allocator.Free(RawBuffer));
			}
			else
			{
				delete[] RawBuffer;
			}

			return true;
		}

		[[nodiscard]] auto OpenRoot(
			UEFI::Handle FsHandle,
			Protocols::File*& Root,
			Memory::AllocatorStub Allocator
		) -> Foundation::Bool
		{
			if (!FsHandle)
			{
				return false;
			}

			Protocols::Access Access{ &UEFI::Context::BootServices(), Allocator };
			auto FileSystem = Access.Handle<Protocols::SimpleFileSystem>(FsHandle);

			if (!FileSystem || !*FileSystem)
			{
				return false;
			}

			const auto Status = (*FileSystem)->OpenRoot(Root);

			return UEFI::IsSuccess(Status) && Root;
		}
	}

	File::File(File&& Other) noexcept
	{
		*this = Foundation::Utility::Move(Other);
	}

	File::~File()
	{
		(void)Close();
	}

	auto File::operator=(File&& Other) noexcept -> File&
	{
		if (this == &Other)
		{
			return *this;
		}

		(void)Close();

		Handle_ = Other.Handle_;
		Allocator_ = Other.Allocator_;
		Info_ = Foundation::Utility::Move(Other.Info_);

		Other.Handle_ = nullptr;
		Other.Allocator_.Reset();
		Other.Info_ = FileInfo{};

		return *this;
	}

	auto File::Open(const Path& Path, FileOpenMode Mode) -> Foundation::Bool
	{
		return Open(Path, CurrentFileSystemHandle(Allocator_), Mode);
	}

	auto File::Open(const Path& FilePath, UEFI::Handle FsHandle, FileOpenMode Mode) -> Foundation::Bool
	{
		if (!Close())
		{
			return false;
		}

		if (FilePath.Empty() || !FsHandle)
		{
			return false;
		}

		Protocols::File* Root{};

		if (!OpenRoot(FsHandle, Root, Allocator_))
		{
			return false;
		}

		Path NormalizedPath{ FilePath.View(), Allocator_ };
		NormalizedPath.Normalize();

		auto WidePath = Text::Encoding::ToWideAscii(
			NormalizedPath.View(),
			Allocator_
		);
		Protocols::File* FileHandle{};

		const auto Status = Root->OpenFile(FileHandle, WidePath.CStr(), ToOpenMask(Mode), CreateAttributesFor(Mode));

		(void)Root->CloseFile();

		if (UEFI::IsError(Status) || !FileHandle)
		{
			return false;
		}

		FileInfo NewInfo{ Allocator_ };

		if (!QueryInfo(*FileHandle, NormalizedPath, NewInfo, Allocator_))
		{
			(void)FileHandle->CloseFile();
			return false;
		}

		Handle_ = FileHandle;
		FileSystemHandle_ = FsHandle;
		Info_ = Foundation::Utility::Move(NewInfo);

		return true;
	}

	auto File::Valid() const -> Foundation::Bool
	{
		return Handle_ != nullptr;
	}

	auto File::Handle() const -> Protocols::File&
	{
		UEFIPP_ASSERT(Handle_ != nullptr);
		return *Handle_;
	}

	auto File::Info() const -> const FileInfo&
	{
		return Info_;
	}

	auto File::Close() -> Foundation::Bool
	{
		if (!Handle_)
		{
			Info_ = FileInfo{ Allocator_ };
			return true;
		}

		auto* OldHandle = Handle_;
		Handle_ = nullptr;
		Info_ = FileInfo{ Allocator_ };

		const auto Status = OldHandle->CloseFile();
		return UEFI::IsSuccess(Status);
	}

	auto File::Delete() -> Foundation::Bool
	{
		if (!Handle_)
		{
			return false;
		}

		auto* OldHandle = Handle_;
		Handle_ = nullptr;
		Info_ = FileInfo{ Allocator_ };

		const auto Status = OldHandle->DeleteFile();
		return !UEFI::IsError(Status);
	}

	auto File::Flush() -> Foundation::Bool
	{
		if (!Handle_)
		{
			return false;
		}

		const auto Status = Handle_->FlushFile();
		return !UEFI::IsError(Status);
	}

	auto File::Read(Library::Vector<Foundation::Uint8>& Buffer) -> Foundation::Bool
	{
		if (!Handle_ || Info_.IsDirectory())
		{
			return false;
		}

		const auto FileSize = Foundation::Cast::Auto<Foundation::Size>(Info_.Size());

		if (!Buffer.Resize(FileSize))
		{
			return false;
		}

		if (FileSize == 0)
		{
			return true;
		}

		if (UEFI::IsError(Handle_->SetCurrentPosition(0)))
		{
			Buffer.Clear();
			return false;
		}

		auto ReadSize = Foundation::Cast::Auto<Foundation::UintN>(FileSize);
		const auto Status = Handle_->ReadFile(Buffer.Data(), ReadSize);

		if (UEFI::IsError(Status) || ReadSize != FileSize)
		{
			Buffer.Clear();
			return false;
		}

		return true;
	}

	auto File::Read(Foundation::Uint8*& Buffer, Foundation::Uint64& BufferSize) -> Foundation::Bool
	{
		Buffer = nullptr;
		BufferSize = 0;

		if (!Handle_ || Info_.IsDirectory())
		{
			return false;
		}

		BufferSize = Info_.Size();

		if (BufferSize == 0)
		{
			return true;
		}

		Buffer = Allocator_
			? Allocator_.AllocateStorage<Foundation::Uint8>(
				Foundation::Cast::Auto<Foundation::Size>(BufferSize)
			)
			: new Foundation::Uint8[BufferSize];

		if (!Buffer)
		{
			BufferSize = 0;
			return false;
		}

		if (UEFI::IsError(Handle_->SetCurrentPosition(0)))
		{
			UEFIPP_VERIFY(FreeReadBuffer(Buffer));
			BufferSize = 0;
			return false;
		}

		auto ReadSize = Foundation::Cast::Auto<Foundation::UintN>(BufferSize);
		const auto Status = Handle_->ReadFile(Buffer, ReadSize);

		if (UEFI::IsError(Status) || ReadSize != BufferSize)
		{
			UEFIPP_VERIFY(FreeReadBuffer(Buffer));
			BufferSize = 0;
			return false;
		}

		return true;
	}

	auto File::FreeReadBuffer(Foundation::Uint8*& Buffer) const -> Foundation::Bool
	{
		if (!Buffer)
		{
			return true;
		}

		if (Allocator_)
		{
			if (!Allocator_.Free(Buffer))
			{
				return false;
			}
		}
		else
		{
			delete[] Buffer;
		}

		Buffer = nullptr;
		return true;
	}

	auto File::Write(const Library::Vector<Foundation::Uint8>& Buffer) -> Foundation::Bool
	{
		return Write(Buffer.Data(), Buffer.Size());
	}

	auto File::Write(const Foundation::Uint8* Buffer, Foundation::Uint64 BufferSize) -> Foundation::Bool
	{
		if (!Handle_ || Info_.IsDirectory() || (!Buffer && BufferSize != 0))
		{
			return false;
		}

		if (UEFI::IsError(Handle_->SetCurrentPosition(0)))
		{
			return false;
		}

		if (BufferSize == 0)
		{
			return true;
		}

		auto WriteSize = Foundation::Cast::Auto<Foundation::UintN>(BufferSize);
		const auto Status = Handle_->WriteFile(Buffer, WriteSize);

		if (UEFI::IsError(Status) || WriteSize != BufferSize)
		{
			return false;
		}

		return QueryInfo(*Handle_, Info_.Path(), Info_, Allocator_);
	}

	auto File::Write(const Library::Vector<Foundation::Uint8>& Buffer, Foundation::Uint64 Position) -> Foundation::Bool
	{
		return Write(Buffer.Data(), Buffer.Size(), Position);
	}

	auto File::Write(const Foundation::Uint8* Buffer, Foundation::Uint64 BufferSize, Foundation::Uint64 Position) -> Foundation::Bool
	{
		if (!Handle_ || Info_.IsDirectory() || (!Buffer && BufferSize != 0))
		{
			return false;
		}

		if (UEFI::IsError(Handle_->SetCurrentPosition(Position)))
		{
			return false;
		}

		if (BufferSize == 0)
		{
			return true;
		}

		auto WriteSize = Foundation::Cast::Auto<Foundation::UintN>(BufferSize);
		const auto Status = Handle_->WriteFile(Buffer, WriteSize);

		if (UEFI::IsError(Status) || WriteSize != BufferSize)
		{
			return false;
		}

		return QueryInfo(*Handle_, Info_.Path(), Info_, Allocator_);
	}

	auto File::Append(const Library::Vector<Foundation::Uint8>& Buffer) -> Foundation::Bool
	{
		return Append(Buffer.Data(), Buffer.Size());
	}

	auto File::Append(const Foundation::Uint8* Buffer, Foundation::Uint64 BufferSize) -> Foundation::Bool
	{
		if (!Handle_ || Info_.IsDirectory() || (!Buffer && BufferSize != 0))
		{
			return false;
		}

		if (UEFI::IsError(Handle_->SetCurrentPosition(Foundation::Uint64(-1))))
		{
			return false;
		}

		if (BufferSize == 0)
		{
			return true;
		}

		auto WriteSize = Foundation::Cast::Auto<Foundation::UintN>(BufferSize);
		const auto Status = Handle_->WriteFile(Buffer, WriteSize);

		if (UEFI::IsError(Status) || WriteSize != BufferSize)
		{
			return false;
		}

		return QueryInfo(*Handle_, Info_.Path(), Info_, Allocator_);
	}

	auto File::Position(Foundation::Uint64& Position) -> Foundation::Bool
	{
		if (!Handle_)
		{
			return false;
		}

		const auto Status = Handle_->CurrentPosition(Position);

		return UEFI::IsSuccess(Status);
	}

	auto File::OpenAny(const Path& Path, FileOpenMode Mode) -> Foundation::Bool
	{
		if (!Close())
		{
			return false;
		}

		if (Path.Empty())
		{
			return false;
		}

		// First try current filesystem.
		if (Open(Path, CurrentFileSystemHandle(Allocator_), Mode))
		{
			return true;
		}

		Protocols::Access Access{ &UEFI::Context::BootServices() };

		auto Handles = Access.LocateHandles<Protocols::SimpleFileSystem>();

		if (!Handles)
		{
			return false;
		}

		for (const auto FsHandle : Handles.Value())
		{
			if (FsHandle == CurrentFileSystemHandle(Allocator_))
			{
				continue;
			}

			if (Open(Path, FsHandle, Mode))
			{
				return true;
			}
		}

		return false;
	}

	[[nodiscard]] auto File::FileSystemHandle() const -> UEFI::Handle
	{
		return FileSystemHandle_;
	}
}
