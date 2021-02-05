#include "guidformatters.hpp"
#include "stringformatters.hpp"
#include <iomanip>
#include <sstream>

std::wstring CIMitar::Formatters::UpperCaseString(const std::wstring& InputString)
{
	std::wstringstream UpperCaser;
	UpperCaser << std::uppercase << InputString;
	return UpperCaser.str();
}

void CIMitar::Formatters::GetGuidFromString(const std::wstring& TextGUID, GUID* Guid)
{
	static_assert(sizeof(unsigned char) == 1, "replace the unsigned char in this function with a data type that is 1 byte on this platform");	// if a char is not precisely 1 byte long, the rest will fail
	constexpr auto GuidCharLength = 32;	// UUIDs are 32 characters in length
	unsigned char GuidBytes[GuidCharLength / 2]{ 0 };	// BYTE count is half the size of the CHAR count because UUIDs use 4 bits per char
	unsigned char Buffer{ 'x' };
	int OutputBytesPosition{ 0 };
	int ValidCharsFound{ 0 };
	for (auto i{ 0 }; TextGUID[i] && ValidCharsFound != GuidCharLength; i++)
	{
		switch (TextGUID[i])
		{
		case L'0': Buffer = static_cast<unsigned char>(0); break;
		case L'1': Buffer = static_cast<unsigned char>(1); break;
		case L'2': Buffer = static_cast<unsigned char>(2); break;
		case L'3': Buffer = static_cast<unsigned char>(3); break;
		case L'4': Buffer = static_cast<unsigned char>(4); break;
		case L'5': Buffer = static_cast<unsigned char>(5); break;
		case L'6': Buffer = static_cast<unsigned char>(6); break;
		case L'7': Buffer = static_cast<unsigned char>(7); break;
		case L'8': Buffer = static_cast<unsigned char>(8); break;
		case L'9': Buffer = static_cast<unsigned char>(9); break;
		case L'A': case L'a': Buffer = static_cast<unsigned char>(0xa); break;
		case L'B': case L'b': Buffer = static_cast<unsigned char>(0xb); break;
		case L'C': case L'c': Buffer = static_cast<unsigned char>(0xc); break;
		case L'D': case L'd': Buffer = static_cast<unsigned char>(0xd); break;
		case L'E': case L'e': Buffer = static_cast<unsigned char>(0xe); break;
		case L'F': case L'f': Buffer = static_cast<unsigned char>(0xf); break;
		default: Buffer = 'x'; break;
		}
		if (Buffer != 'x')
		{
			if (ValidCharsFound & 1)
			{
				GuidBytes[OutputBytesPosition] = GuidBytes[OutputBytesPosition] | Buffer;
				++OutputBytesPosition;
			}
			else
			{
				GuidBytes[OutputBytesPosition] = Buffer << 4;
			}
			++ValidCharsFound;
		}
	}

	Guid->Data1 = (GuidBytes[0] << 24) | (GuidBytes[1] << 16) | (GuidBytes[2] << 8) | GuidBytes[3];
	Guid->Data2 = (GuidBytes[4] << 8) | GuidBytes[5];
	Guid->Data3 = (GuidBytes[6] << 8) | GuidBytes[7];
	for (auto i{ 0 }; i != 8; ++i)
	{
		Guid->Data4[i] = GuidBytes[i + 8];
	}
}

void ClearGuid(GUID* Guid)
{
	Guid->Data1 = Guid->Data2 = Guid->Data3 = 0;
	for (auto i{ 0 }; i != 8; ++i)
	{
		Guid->Data4[i] = 0;
	}
}

std::wstring CIMitar::Formatters::NewStringFromGUID(GUID* Guid, const bool Hyphenate, const bool Brace, const bool Uppercase)
{
	if (Guid == nullptr)	// todo: clean this up
	{
		GUID EmptyGuid{ 0 };
		Guid = &EmptyGuid;
		if (CoCreateGuid(Guid) != S_OK)
			ClearGuid(Guid);
	}
	std::wstring GroupBreak{ (Hyphenate ? L"-" : L"") };
	std::wstringstream GuidStream;
	GuidStream << std::setfill(L'0') << std::hex << (Uppercase ? std::uppercase : std::nouppercase);

	if (Brace) GuidStream << L'{';
	GuidStream << std::setw(8) << Guid->Data1 << GroupBreak;
	for (unsigned long Data : { Guid->Data2, Guid->Data3 })
	{
		GuidStream << std::setw(4) << Data << GroupBreak;
	}

	for (auto i = 0; i != 8; i++)	// GUID.Data4 has 8 members
	{
		GuidStream << std::setw(2) << Guid->Data4[i];
		if (i == 1) GuidStream << GroupBreak;	// if a hyphen is needed, it will be after the 2nd set
	}

	if (Brace) GuidStream << L'}';
	return GuidStream.str();
}
