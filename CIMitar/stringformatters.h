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

	constexpr size_t MinItemsForJoinOptimization{ 50 };	// arbitrary number to decide if JoinString pre-allocation calculations are worth it

	template <typename Container>
	std::wstring JoinString(const std::wstring& Glue, const Container& Parts)
	{
		std::wstring JoinedString{};
		if (Parts.size())
		{
			if (Parts.size() >= MinItemsForJoinOptimization)
			{
				size_t FinalStringLength{ Glue.size() * (Parts.size() - 1) };
				for (auto const& Part : Parts)
				{
					FinalStringLength += Part.size();
				}
				JoinedString.resize(FinalStringLength);
			}
			JoinedString = *Parts.cbegin();
			if (JoinedString.size() > 1)
			{
				for (auto Part{ ++Parts.cbegin() }; Part != Parts.cend(); ++Part)
				{
					JoinedString += Glue + *Part;
				}
			}
		}
		return JoinedString;
	}

	template <typename T, typename Container>
	std::wstring JoinString(const T& Glue, const Container& Parts)
	{
		return JoinString(std::wstring{ Glue }, Parts);
	}

	void GetGuidFromString(const wchar_t* TextGUID, GUID* Guid);
	void GetGuidFromString(const std::wstring& TextGUID, GUID* Guid);
	std::wstring GetStringFromGUID(GUID* Guid, const bool Hyphenate = false, const bool Brace = false, const bool Uppercase = false);
}