#pragma once
#include "cimerrors.h"
#include "oplist.h"

namespace CIMitar
{
	void ProcessCIMResult(MI_Result CIMResult, CIMitarOps::OperationCodes CIMitarOperation, const wchar_t* MoreDetails = nullptr);
}
