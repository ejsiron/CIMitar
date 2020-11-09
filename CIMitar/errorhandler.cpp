#include <string>
#include "CIMitar.h"
#include "errorhandler.h"

void CIMitar::ProcessCIMResult(MI_Result CIMResult, CIMitarOps::OperationCodes CIMitarOperation, const wchar_t* MoreDetails)
{
	if (CIMResult != MI_Result::MI_RESULT_OK)
	{
		if (!MoreDetails) MoreDetails = L"";
		throw cimitar_exception(CIMResult, static_cast<int>(CIMitarOperation), std::wstring{ MoreDetails });
	}
}