#include "stdafx.h"
#include "connection.h"

namespace CIMitar
{
	CIMConnection::CIMConnection(std::wstring& ComputerName)
	{
		if (TheCIMApplication == nullptr)
		{
			//MI_Instance AppInitError;
			auto AppInitResult = MI_Application_Initialize(0, "CIMitar", NULL, &TheCIMApplication); //TODO: need to do more error-checking
			some a;
		}
	}
	const bool CIMConnection::operator==(const CIMConnection& rhs) const noexcept
	{
		return false;
	}
}