#pragma once

#include <objbase.h>
#include <memory>
#include <string>
#include <vector>

namespace CIMitar::Formatters
{
	template<typename ... Args>
	std::wstring FormatString(const std::wstring& Format, Args ... args)
	{
		size_t size = _snwprintf(nullptr, 0, Format.c_str(), args ...) + sizeof(wchar_t); // +1 is for L'\0'
		std::unique_ptr<wchar_t[]> buf(new wchar_t[size]);
#pragma warning(push)
#pragma warning(suppress:4996)
		_snwprintf(buf.get(), size, Format.c_str(), args ...);
#pragma warning(pop)
		return std::wstring(buf.get(), buf.get() + size - sizeof(wchar_t));	// trim off the L'\0'
	}

	template<typename ... Args>
	std::wstring FormatString(const wchar_t* Format, Args ... args)
	{
		std::wstring ConvertedString = { Format };
		return FormatString(ConvertedString, std::forward<Args>(args)...);
	}

	std::wstring UpperCaseString(const std::wstring& InputString);

	//
	// can throw:
	// * length_error: if final string is too long

	std::wstring JoinString(const std::wstring& Glue, const std::vector<std::wstring>& Parts);
	template <class T>
	std::wstring JoinString(const T Glue, const std::vector<std::wstring>& Parts) { return JoinString(std::wstring{ Glue }, Parts); }

	void GetGuidFromString(const wchar_t* TextGUID, GUID* Guid);
	void GetGuidFromString(const std::wstring& TextGUID, GUID* Guid);
	std::wstring GetStringFromGUID(GUID* Guid, const bool Hyphenate = false, const bool Brace = false, const bool Uppercase = false);
}