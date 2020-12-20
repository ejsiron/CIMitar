#include "CIMitar.h"
#include "appinit.h"
#include <algorithm>
#include <list>
#include <mutex>
#include <shared_mutex>

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
static shared_mutex ApplicationMutex{};
#pragma endregion Housekeeping

static MI_Session* NewCimSession(const wchar_t* ComputerName, SessionProtocols Protocol)
{
	//MI_Application_NewSession()
}

Session::Session(const wstring& ComputerName)
{
	lock_guard ApplicationAccessGuard(ApplicationMutex);
	if (Sessions.empty())
	{
		auto AppInitResult{ InitializeCIMitar() };
		if (AppInitResult.first == MI_Result::MI_RESULT_OK)
			TheCimApplication = AppInitResult.second;
		else
			TheCimApplication = MI_APPLICATION_NULL;	// this is not enough
	}
	this->ComputerName = ComputerName;
	Sessions.emplace_back(this);
}

Session::~Session()
{
	lock_guard ApplicationAccessGuard(ApplicationMutex);
	Sessions.remove(this);
	if (Sessions.empty() && TheCimApplication.ft != nullptr)
	{
		MI_Application_Close(&TheCimApplication);
		TheCimApplication = MI_APPLICATION_NULL;
	}
}

const bool Session::Close()
{
	// TODO: error checking
	MI_Session_Close(CIMSession, NULL, NULL);
	return true;
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
