#include "stdafx.h"
#include "CIMitar.h"
#include "cimerrors.h"
#include "oplist.h"

namespace CIMitar
{
	const wchar_t* cimitar_exception::Message() const noexcept
	{
		return CIMErrors::GetCIMErrorMessage(static_cast<MI_Result>(CIMErrorCode));
	}

	const wchar_t* cimitar_exception::Operation() const noexcept
	{
		return CIMitarOps::GetOperationMessage(static_cast<CIMitarOps::OperationCodes>(CIMitarOperationCode));
	}
}
