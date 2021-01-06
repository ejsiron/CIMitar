#pragma once

#include "CIMitar.h"
#include "utility.h"
#include <array>

using namespace CIMitar;
using namespace std;

static constexpr array ErrorMessages =
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

const unsigned int Error::CIMStatusCode() const noexcept
{
	return cimstatuscode;
}

const wstring Error::CIMStatusCodeDescription() const noexcept
{
	if (cimstatuscodedescription.size())
	{
		return cimstatuscodedescription;
	}
	else
	{
		size_t TranslatedCode{ static_cast<size_t>(cimstatuscode) };
		if (TranslatedCode >= ErrorMessages.size())
		{	// 1 is the CIM code for unknown/unspecified error
			TranslatedCode = 1;
		}
		return ErrorMessages[static_cast<size_t> (cimstatuscode)];
	}
}

const wstring Error::Activity() const noexcept
{
	return Activity::GetActivity(activitycode);
}

const wstring Error::Query() const noexcept
{
	return query;
}

const wstring Error::OtherErrorType() const noexcept
{
	return Utility::GetValueOrDefault(extendederror.get(), &ExtendedError::OtherErrorType);
}

const wstring Error::OwningEntity() const noexcept
{
	return Utility::GetValueOrDefault(extendederror.get(), &ExtendedError::OwningEntity);
}

const wstring Error::MessageID() const noexcept
{
	return Utility::GetValueOrDefault(extendederror.get(), &ExtendedError::MessageID);
}

const wstring Error::Message() const noexcept
{
	return Utility::GetValueOrDefault(extendederror.get(), &ExtendedError::Message);
}

const vector<wstring> Error::MessageArguments() const noexcept
{
	return Utility::GetValueOrDefault(extendederror.get(), &ExtendedError::MessageArguments);
}

const unsigned int Error::PerceivedSeverity() const noexcept
{
	return Utility::GetValueOrDefault(extendederror.get(), &ExtendedError::PerceivedSeverity);
}

const unsigned int Error::ProbableCause() const noexcept
{
	return Utility::GetValueOrDefault(extendederror.get(), &ExtendedError::ProbableCause);
}

const wstring Error::ProbableCauseDescription() const noexcept
{
	return Utility::GetValueOrDefault(extendederror.get(), &ExtendedError::ProbableCauseDescription);
}

const vector<wstring> Error::RecommendedActions() const noexcept
{
	return Utility::GetValueOrDefault(extendederror.get(), &ExtendedError::RecommendedActions);
}

const wstring Error::ErrorSource() const noexcept
{
	return Utility::GetValueOrDefault(extendederror.get(), &ExtendedError::ErrorSource);
}

const unsigned int Error::ErrorSourceFormat() const noexcept
{
	return Utility::GetValueOrDefault(extendederror.get(), &ExtendedError::ErrorSourceFormat);
}

const wstring Error::OtherErrorSourceFormat() const noexcept
{
	return Utility::GetValueOrDefault(extendederror.get(), &ExtendedError::OtherErrorSourceFormat);
}
