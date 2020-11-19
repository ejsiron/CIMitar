#include "stdafx.h"
#include "connection.h"

using namespace CIMitar;

std::unique_ptr<MI_Application> CIMConnection::TheCimApplication = nullptr;

CIMConnection::CIMConnection(std::wstring& ComputerName)
{
	if (CIMConnection::TheCimApplication == nullptr)
	{
		MI_Instance AppInitError;
		auto AppInitResult = MI_Application_Initialize(0, "CIMitar", &AppInitError, TheCimApplication.get()); //TODO: need to do more error-checking
	}
}
const bool CIMConnection::operator==(const CIMConnection& rhs) const noexcept
{
	return false;
}
