#pragma once

#include <UEFIpp/CRT/StdDef.hpp>

extern "C"
{
    void* memcpy(void* Destination, const void* Source, std::size_t Count);
    void* memmove(void* Destination, const void* Source, std::size_t Count);
    void* memset(void* Destination, int Value, std::size_t Count);
    int memcmp(const void* Left, const void* Right, std::size_t Count);

    std::size_t strlen(const char* String);
    std::size_t wcslen(const wchar_t* String);

    char* strcpy(char* Destination, const char* Source);
    char* strncpy(char* Destination, const char* Source, std::size_t Count);
    int strcmp(const char* Left, const char* Right);
    int strncmp(const char* Left, const char* Right, std::size_t Count);

    char* strcat(char* Destination, const char* Source);
    char* strncat(char* Destination, const char* Source, std::size_t Count);

    void* memchr(const void* Buffer, int Character, std::size_t Count);

    wchar_t* wcscpy(wchar_t* Destination, const wchar_t* Source);
    wchar_t* wcsncpy(wchar_t* Destination, const wchar_t* Source, std::size_t Count);
    int wcscmp(const wchar_t* Left, const wchar_t* Right);
    int wcsncmp(const wchar_t* Left, const wchar_t* Right, std::size_t Count);
}

char* strchr(const char* String, int Character);
char* strrchr(const char* String, int Character);
char* strstr(const char* String, const char* Substring);
wchar_t* wcschr(const wchar_t* String, wchar_t Character);