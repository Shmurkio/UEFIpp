#include <UEFIpp/CRT/String.hpp>
#include <UEFIpp/Memory/Memory.hpp>

#pragma function(memcpy)
#pragma function(memmove)
#pragma function(memset)
#pragma function(memcmp)
#pragma function(strlen)
#pragma function(wcslen)
#pragma function(strcpy)
#pragma function(strncpy)
#pragma function(strcmp)
#pragma function(strncmp)
#pragma function(strcat)
#pragma function(memchr)
#pragma function(wcscpy)
#pragma function(wcsncpy)
#pragma function(wcscmp)
#pragma function(wcsncmp)

extern "C"
{
	void* memcpy(void* Destination, const void* Source, std::size_t Count)
	{
		return UEFIpp::Memory::Copy(Destination, Source, Count);
	}

	void* memmove(void* Destination, const void* Source, std::size_t Count)
	{
		return UEFIpp::Memory::Move(Destination, Source, Count);
	}

	void* memset(void* Destination, int Value, std::size_t Count)
	{
		return UEFIpp::Memory::Set(Destination, UEFIpp::Foundation::Cast::Auto<UEFIpp::Foundation::Uint8>(Value), Count);
	}

	int memcmp(const void* Left, const void* Right, std::size_t Count)
	{
		return UEFIpp::Memory::Compare(Left, Right, Count);
	}

	std::size_t strlen(const char* String)
	{
		if (!String)
		{
			return 0;
		}

		std::size_t Length = 0;

		while (String[Length])
		{
			++Length;
		}

		return Length;
	}

	std::size_t wcslen(const wchar_t* String)
	{
		if (!String)
		{
			return 0;
		}

		std::size_t Length = 0;

		while (String[Length])
		{
			++Length;
		}

		return Length;
	}

	char* strcpy(char* Destination, const char* Source)
	{
		if (!Destination || !Source)
		{
			return Destination;
		}

		char* Result = Destination;

		while ((*Destination++ = *Source++) != '\0')
		{
		}

		return Result;
	}

	char* strncpy(char* Destination, const char* Source, std::size_t Count)
	{
		if (!Destination || !Source)
		{
			return Destination;
		}

		std::size_t i = 0;

		for (; i < Count && Source[i]; ++i)
		{
			Destination[i] = Source[i];
		}

		for (; i < Count; ++i)
		{
			Destination[i] = '\0';
		}

		return Destination;
	}

	int strcmp(const char* Left, const char* Right)
	{
		if (Left == Right)
		{
			return 0;
		}

		if (!Left)
		{
			return -1;
		}

		if (!Right)
		{
			return 1;
		}

		while (*Left && (*Left == *Right))
		{
			++Left;
			++Right;
		}

		return UEFIpp::Foundation::Cast::Auto<unsigned char>(*Left) - UEFIpp::Foundation::Cast::Auto<unsigned char>(*Right);
	}

	int strncmp(const char* Left, const char* Right, std::size_t Count)
	{
		if (!Count)
		{
			return 0;
		}

		if (Left == Right)
		{
			return 0;
		}

		if (!Left)
		{
			return -1;
		}

		if (!Right)
		{
			return 1;
		}

		for (std::size_t i = 0; i < Count; ++i)
		{
			const auto L = UEFIpp::Foundation::Cast::Auto<unsigned char>(Left[i]);
			const auto R = UEFIpp::Foundation::Cast::Auto<unsigned char>(Right[i]);

			if (L != R)
			{
				return L - R;
			}

			if (L == '\0')
			{
				return 0;
			}
		}

		return 0;
	}
	
	void* memchr(const void* Buffer, int Character, std::size_t Count)
	{
		if (!Buffer)
		{
			return nullptr;
		}

		const auto* Bytes = UEFIpp::Foundation::Cast::Auto<const unsigned char*>(Buffer);
		const auto Value = UEFIpp::Foundation::Cast::Auto<unsigned char>(Character);

		for (std::size_t i = 0; i < Count; ++i)
		{
			if (Bytes[i] == Value)
			{
				return UEFIpp::Foundation::Cast::Auto<void*>(&Bytes[i]);
			}
		}

		return nullptr;
	}

	char* strcat(char* Destination, const char* Source)
	{
		if (!Destination || !Source)
		{
			return Destination;
		}

		auto* End = Destination + strlen(Destination);
		strcpy(End, Source);

		return Destination;
	}

	char* strncat(char* Destination, const char* Source, std::size_t Count)
	{
		if (!Destination || !Source)
		{
			return Destination;
		}

		auto* End = Destination + strlen(Destination);
		std::size_t i = 0;

		for (; i < Count && Source[i]; ++i)
		{
			End[i] = Source[i];
		}

		End[i] = '\0';

		return Destination;
	}

	wchar_t* wcscpy(wchar_t* Destination, const wchar_t* Source)
	{
		if (!Destination || !Source)
		{
			return Destination;
		}

		auto* Result = Destination;

		while ((*Destination++ = *Source++) != L'\0')
		{
		}

		return Result;
	}

	wchar_t* wcsncpy(wchar_t* Destination, const wchar_t* Source, std::size_t Count)
	{
		if (!Destination || !Source)
		{
			return Destination;
		}

		std::size_t i = 0;

		for (; i < Count && Source[i]; ++i)
		{
			Destination[i] = Source[i];
		}

		for (; i < Count; ++i)
		{
			Destination[i] = L'\0';
		}

		return Destination;
	}

	int wcscmp(const wchar_t* Left, const wchar_t* Right)
	{
		if (Left == Right)
		{
			return 0;
		}

		if (!Left)
		{
			return -1;
		}

		if (!Right)
		{
			return 1;
		}

		while (*Left && (*Left == *Right))
		{
			++Left;
			++Right;
		}

		return UEFIpp::Foundation::Cast::Auto<int>(*Left) - UEFIpp::Foundation::Cast::Auto<int>(*Right);
	}

	int wcsncmp(const wchar_t* Left, const wchar_t* Right, std::size_t Count)
	{
		if (!Count)
		{
			return 0;
		}

		if (Left == Right)
		{
			return 0;
		}

		if (!Left)
		{
			return -1;
		}

		if (!Right)
		{
			return 1;
		}

		for (std::size_t i = 0; i < Count; ++i)
		{
			if (Left[i] != Right[i])
			{
				return UEFIpp::Foundation::Cast::Auto<int>(Left[i]) - UEFIpp::Foundation::Cast::Auto<int>(Right[i]);
			}

			if (Left[i] == L'\0')
			{
				return 0;
			}
		}

		return 0;
	}
}

char* strchr(const char* String, int Character)
{
	if (!String)
	{
		return nullptr;
	}

	const auto Value = UEFIpp::Foundation::Cast::Auto<char>(Character);

	for (;; ++String)
	{
		if (*String == Value)
		{
			return UEFIpp::Foundation::Cast::Auto<char*>(String);
		}

		if (*String == '\0')
		{
			return nullptr;
		}
	}
}

char* strrchr(const char* String, int Character)
{
	if (!String)
	{
		return nullptr;
	}

	const auto Value = UEFIpp::Foundation::Cast::Auto<char>(Character);
	const char* Result = nullptr;

	for (;; ++String)
	{
		if (*String == Value)
		{
			Result = String;
		}

		if (*String == '\0')
		{
			return UEFIpp::Foundation::Cast::Auto<char*>(Result);
		}
	}
}

char* strstr(const char* String, const char* Substring)
{
	if (!String || !Substring)
	{
		return nullptr;
	}

	if (!*Substring)
	{
		return UEFIpp::Foundation::Cast::Auto<char*>(String);
	}

	const auto Length = strlen(Substring);

	for (; *String; ++String)
	{
		if (!strncmp(String, Substring, Length))
		{
			return UEFIpp::Foundation::Cast::Auto<char*>(String);
		}
	}

	return nullptr;
}

wchar_t* wcschr(const wchar_t* String, wchar_t Character)
{
	if (!String)
	{
		return nullptr;
	}

	for (;; ++String)
	{
		if (*String == Character)
		{
			return UEFIpp::Foundation::Cast::Auto<wchar_t*>(String);
		}

		if (*String == L'\0')
		{
			return nullptr;
		}
	}
}