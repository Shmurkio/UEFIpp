#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Protocols/File.hpp>
#include <UEFIpp/UEFI/Status.hpp>

namespace UEFIpp::Protocols
{
	class SimpleFileSystem;

	using SimpleFileSystemOpenVolumeFn = UEFI::StatusCode(*)(SimpleFileSystem* This, File** Root);

	class SimpleFileSystem
	{
	public:
		Foundation::Uint64 Revision;
		SimpleFileSystemOpenVolumeFn OpenVolume;

		[[nodiscard]] auto OpenRoot(File*& Root) -> UEFI::StatusCode
		{
			return OpenVolume(this, &Root);
		}
	};

	static_assert(sizeof(SimpleFileSystem) == 16);
	static_assert(Foundation::Traits::IsStandardLayout<SimpleFileSystem>::Value);
	static_assert(Foundation::Traits::IsTriviallyCopyable<SimpleFileSystem>::Value);
}