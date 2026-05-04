#pragma once

#include <cstdint>
#include "../String/String.hpp"
#include "../Vector/Vector.hpp"

class File
{
public:
    typedef struct _FILE_TIME
    {
        uint16_t Year = 0;
        uint8_t Month = 0;
        uint8_t Day = 0;
        uint8_t Hour = 0;
        uint8_t Minute = 0;
        uint8_t Second = 0;
        uint32_t Nanosecond = 0;
        int16_t TimeZone = 0;
        uint8_t Daylight = 0;
    } FILE_TIME, *PFILE_TIME;

private:
    String Path_{};
    String Name_{};
    Vector<uint8_t> Buffer_{};
    uint64_t PhysicalSize_ = 0;
    uint64_t Attribute_ = 0;
    FILE_TIME CreateTime_{};
    FILE_TIME LastAccessTime_{};
    FILE_TIME ModificationTime_{};
    void* FsHandle_ = nullptr;
    bool Loaded_ = false;

public:
    File() = default;
    ~File();

    File(const File&) = delete;
    auto operator=(const File&) -> File & = delete;

    File(File&& Other) noexcept;
    auto operator=(File&& Other) noexcept -> File&;

    auto Load(const String& FilePath) -> bool;
    auto LoadFromFileSystem(void* FsHandle, const String& FilePath) -> bool;
    auto Unload() -> bool;
    auto Reload() -> bool;

    auto Save() const -> bool;
    auto SaveAs(const String& FilePath) const -> bool;

    static auto Write(const String& FilePath, const void* FileBuffer, uint64_t FileSize) -> bool;
    static auto Write(const String& FilePath, const Vector<uint8_t>& FileBuffer) -> bool;

    auto Delete() -> bool;
    static auto Delete(const String& FilePath) -> bool;

    auto Loaded() const -> bool;
    auto Empty() const -> bool;

    auto Path() const -> const String&;
    auto Name() const -> const String&;
    auto Buffer() -> Vector<uint8_t>&;
    auto Buffer() const -> const Vector<uint8_t>&;
    auto Data() -> void*;
    auto Data() const -> const void*;
    auto Bytes() -> uint8_t*;
    auto Bytes() const -> const uint8_t*;
    auto Size() const -> uint64_t;
    auto PhysicalSize() const -> uint64_t;
    auto Attribute() const -> uint64_t;
    auto FileSystemHandle() const -> void*;

    auto CreateTime() const -> const FILE_TIME&;
    auto LastAccessTime() const -> const FILE_TIME&;
    auto ModificationTime() const -> const FILE_TIME&;

    auto IsDirectory() const -> bool;
    auto IsReadOnly() const -> bool;
    auto IsHidden() const -> bool;
    auto IsSystem() const -> bool;
    auto IsReserved() const -> bool;
    auto IsArchive() const -> bool;
};