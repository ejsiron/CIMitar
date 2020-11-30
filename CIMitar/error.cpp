#pragma once

#include "stdafx.h"
#include "CIMitar.h"
#include <array>

namespace CIMitar
{
	constexpr wchar_t CIMCodeNotFoundMessage[] = L"No such CIM error code";
	static constexpr std::array<const wchar_t*, 29> ErrorMessages =
	{
		L"The operation was successful.",
		L"CIM operation returned an unspecified error.",
		L"Access to the management interface object is denied.",
		L"The specified CIM namespace does not exist.",
		L"An invalid parameter was provided to the CIM operation.",
		L"The specified CIM class does not exist.",
		L"The requested CIM object does not exist.",
		L"The requested CIM operation is not supported.",
		L"Subclasses of the CIM object are preventing this operation.",
		L"Child instances of the CIM object are preventing this operation.",
		L"The target CIM superclass does not exist.",
		L"The CIM object already exists.",
		L"The CIM object does not implement the requested property.",
		L"An invalid value type was supplied to a CIM operation.",
		L"The specified CIM interface was queried with an unsupported language.",
		L"The CIM query is invalid.",
		L"The requested CIM method is unavailable.",
		L"The requested CIM method does not exist.",
		L"The specified CIM namespace is not empty.",
		L"The target CIM context does not contain the indicated enumeration.",
		L"The target CIM server does not support the specified timeout value.",
		L"The CIM server has abandoned the pull operation.",
		L"This CIM pull cannot be abandoned.",
		L"The CIM server does not support filtering the enumeration.",
		L"The CIM server does not allow continuation after an error.",
		L"Operation failed because CIM server limits were exceeded.",
		L"Operation failed because CIM server is shutting down."
	};

	// translates error codes from the MI API
	const wchar_t* Error::Message() const noexcept
	{
		return FindMessage(CimErrorCode);
	}

	const wchar_t* Error::FindMessage(const int Code) noexcept
	{
		if (Code < 0 || Code >= ErrorMessages.size())
		{
			return ErrorMessages[Code];
		}
		else
		{
			return CIMCodeNotFoundMessage;
		}
	}
}