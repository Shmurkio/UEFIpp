#pragma once

#include <UEFIpp/Foundation/Foundation.hpp>
#include <UEFIpp/Library/String/String.hpp>
#include <UEFIpp/Library/String/StringView.hpp>

namespace UEFIpp::Network
{
    enum class FtpEntryType : Foundation::Uint8
    {
        Unknown,
        File,
        Directory,
        SymbolicLink
    };

    class FtpEntry
    {
    public:
        Library::String Name{};
        Library::String Raw{};
        Foundation::Uint64 Size{};
        FtpEntryType Type{ FtpEntryType::Unknown };

    public:
        [[nodiscard]] auto Empty() const -> Foundation::Bool
        {
            return Name.Empty();
        }

        [[nodiscard]] constexpr auto IsFile() const -> Foundation::Bool
        {
            return Type == FtpEntryType::File;
        }

        [[nodiscard]] constexpr auto IsDirectory() const -> Foundation::Bool
        {
            return Type == FtpEntryType::Directory;
        }

        [[nodiscard]] constexpr auto IsSymbolicLink() const -> Foundation::Bool
        {
            return Type == FtpEntryType::SymbolicLink;
        }
    };
}