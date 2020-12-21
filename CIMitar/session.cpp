#include "CIMitar.h"
#include "appinit.h"
#include "localaddresses.h"
#include "stringformatters.h"
#include <algorithm>
#include <list>
#include <mutex>
#include <regex>
#include <shared_mutex>

using namespace std;
using namespace CIMitar;
using namespace CIMitar::Formatters;
using namespace CIMitar::Infrastructure;

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

Session* DefaultSession{ nullptr };
void CIMitar::SetDefaultSession(Session& NewDefaultSession)
{
	DefaultSession = &NewDefaultSession;
}

Session::Session(const wstring& ComputerName)
{
	Close();
	lock_guard ApplicationAccessGuard(ApplicationMutex);
	if (Sessions.empty())
	{
		DefaultSession = this;
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
	if (!Sessions.empty() && DefaultSession == this)
	{
		DefaultSession = *Sessions.begin();
	}
	if (Sessions.empty() && TheCimApplication.ft != nullptr)
	{
		DefaultSession = nullptr;
		MI_Application_Close(&TheCimApplication);
		TheCimApplication = MI_APPLICATION_NULL;
	}
}

const bool Session::Connect(const SessionProtocols* Protocol)
{
	Close();
	const wchar_t* SelectedProtocol{ Protocol == nullptr ? nullptr : *Protocol == SessionProtocols::DCOM ? L"DCOM" : L"WINRM" };
	wstring LocalNamesFilter{ JoinString(L'|', GetLocalNamesAndIPs()) };
	const bool IsLocal{ regex_match(ComputerName, wregex(LocalNamesFilter)) };
	const wchar_t* TargetName{ ComputerName.empty() || IsLocal ? nullptr : ComputerName.c_str() };
	MI_Result Result{ MI_Application_NewSession(&TheCimApplication, SelectedProtocol, TargetName, NULL, NULL, NULL, &CIMSession) };
	// TODO: error checking and reporting
	return Result	== MI_RESULT_OK;
}

const bool Session::Connect()
{
	return Connect(nullptr);
}

const bool Session::Connect(const SessionProtocols Protocol)
{
	return Connect(&Protocol);
}

const bool Session::Close()
{
	// TODO: error checking
	MI_Session_Close(&CIMSession, NULL, NULL);
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

Class NewClass(ClassDeclaration& Declaration, const std::wstring& Namespace = std::wstring{})
{
	//MI_Class newclass;
	//MI_Result Result{MI_Application_NewClass(&TheCimApplication)}
}