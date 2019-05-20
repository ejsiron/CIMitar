#pragma once

#ifndef _MI_H_INCLUDED
#define _MI_H_INCLUDED
#include <mi.h>
#endif // _MI_H_INCLUDED

namespace CIMitar
{
	// translates error codes from the MI API
	struct CIMErrors
	{
		static const wchar_t* GetCIMErrorMessage(MI_Result CIMErrorCode) noexcept
		{
			switch (CIMErrorCode)
			{
			case MI_Result::MI_RESULT_OK:
				return L"The operation was successful.";
				break;
			case MI_Result::MI_RESULT_FAILED:
				return L"CIM operation returned an unspecified error.";
				break;
			case MI_Result::MI_RESULT_ACCESS_DENIED:
				return L"Access to the management interface object is denied.";
				break;
			case MI_Result::MI_RESULT_INVALID_NAMESPACE:
				return L"The specified CIM namespace does not exist.";
				break;
			case MI_Result::MI_RESULT_INVALID_PARAMETER:
				return L"An invalid parameter was provided to the CIM operation.";
				break;
			case MI_Result::MI_RESULT_INVALID_CLASS:
				return L"The specified CIM class does not exist.";
				break;
			case MI_Result::MI_RESULT_NOT_FOUND:
				return L"The requested CIM object does not exist.";
				break;
			case MI_Result::MI_RESULT_NOT_SUPPORTED:
				return L"The requested CIM operation is not supported.";
				break;
			case MI_Result::MI_RESULT_CLASS_HAS_CHILDREN:
				return L"Subclasses of the CIM object are preventing this operation.";
				break;
			case MI_Result::MI_RESULT_CLASS_HAS_INSTANCES:
				return L"Child instances of the CIM object are preventing this operation.";
				break;
			case MI_Result::MI_RESULT_INVALID_SUPERCLASS:
				return L"The target CIM superclass does not exist.";
				break;
			case MI_Result::MI_RESULT_ALREADY_EXISTS:
				return L"The CIM object already exists.";
				break;
			case MI_Result::MI_RESULT_NO_SUCH_PROPERTY:
				return L"The CIM object does not implement the requested property.";
				break;
			case MI_Result::MI_RESULT_TYPE_MISMATCH:
				return L"An invalid value type was supplied to a CIM operation.";
				break;
			case MI_Result::MI_RESULT_QUERY_LANGUAGE_NOT_SUPPORTED:
				return L"The specified CIM interface was queried with an unsupported language.";
				break;
			case MI_Result::MI_RESULT_INVALID_QUERY:
				return L"The CIM query is invalid.";
				break;
			case MI_Result::MI_RESULT_METHOD_NOT_AVAILABLE:
				return L"The requested CIM method is unavailable.";
				break;
			case MI_Result::MI_RESULT_METHOD_NOT_FOUND:
				return L"The requested CIM method does not exist.";
				break;
			case MI_Result::MI_RESULT_NAMESPACE_NOT_EMPTY:
				return L"The specified CIM namespace is not empty.";
				break;
			case MI_Result::MI_RESULT_INVALID_ENUMERATION_CONTEXT:
				return L"The target CIM context does not contain the indicated enumeration.";
				break;
			case MI_Result::MI_RESULT_INVALID_OPERATION_TIMEOUT:
				return L"The target CIM server does not support the specified timeout value.";
				break;
			case MI_Result::MI_RESULT_PULL_HAS_BEEN_ABANDONED:
				return L"The CIM server has abandoned the pull operation.";
				break;
			case MI_Result::MI_RESULT_PULL_CANNOT_BE_ABANDONED:
				return L"This CIM pull cannot be abandoned.";
				break;
			case MI_Result::MI_RESULT_FILTERED_ENUMERATION_NOT_SUPPORTED:
				return L"The CIM server does not support filtering the enumeration.";
				break;
			case MI_Result::MI_RESULT_CONTINUATION_ON_ERROR_NOT_SUPPORTED:
				return L"The  CIM server does not allow continuation after an error.";
				break;
			case MI_Result::MI_RESULT_SERVER_LIMITS_EXCEEDED:
				return L"Operation failed because CIM server limits were exceeded.";
				break;
			case MI_Result::MI_RESULT_SERVER_IS_SHUTTING_DOWN:
				return L"Operation failed because CIM server is shutting down.";
				break;
			default:
				return L"Unknown CIM error.";
				break;
			}
		}
	};
}