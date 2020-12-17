#include "CIMitar.h"
#include <algorithm>
#include <list>
#include <mutex>

using namespace std;
using namespace CIMitar;

#pragma region Housekeeping
/***************************************************************************
*
* These items must exist but never need to be visible to library consumers
*
***************************************************************************/

static MI_Application TheCimApplication = MI_APPLICATION_NULL;
static list<Session*> Sessions{};
static mutex SessionListMutex{};
static std::set<std::wstring> LocalIPs{};
#pragma endregion Housekeeping

static MI_Session* NewCimSession(const wchar_t* ComputerName, SessionProtocols Protocol)
{
	//MI_Application_NewSession()
}

Session::Session(const wstring& ComputerName)
{
	lock_guard<std::mutex> SessionListAccessGuard(SessionListMutex);
	if (Sessions.empty())
	{
		auto AppInitResult = MI_Application_Initialize(0, L"CIMitar", NULL, &TheCimApplication); //TODO: need to do more error-checking
	}
	this->ComputerName = ComputerName;
	Sessions.emplace_back(this);
}

Session::~Session()
{
	lock_guard<std::mutex> SessionListAccessGuard(SessionListMutex);
	Sessions.remove(this);
	if (Sessions.empty() && TheCimApplication.ft != nullptr)
	{
		OutputDebugString(L"Application deleted");
		MI_Application_Close(&TheCimApplication);
		TheCimApplication = MI_APPLICATION_NULL;
	}
}

const bool Session::Close()
{
	MI_Session_Close(CIMSession, NULL, NULL);
	if (CIMSession == nullptr)
		OutputDebugString(L"Close did it\r\n");
	else
	{
		CIMSession = nullptr;
		OutputDebugString(L"I had to do it\r\n");
	}
}

#pragma region Operators
const bool CIMitar::operator==(const Session& lhs, const Session& rhs) noexcept
{
	return false;
}

const bool CIMitar::operator!=(const Session& lhs, const Session& rhs) noexcept
{
	return !(lhs == rhs);
}
#pragma endregion
