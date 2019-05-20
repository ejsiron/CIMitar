#pragma once
#include <objbase.h>
#include <string>

namespace CIMitar::Formatters
{
	void NewGuidFromString(const wchar_t* TextGUID, GUID* Guid);
	void NewGuidFromString(const std::wstring& TextGUID, GUID* Guid);
	std::wstring NewStringFromGUID(GUID* Guid, const bool Hyphenate = false, const bool Brace = false, const bool Uppercase = false);
}