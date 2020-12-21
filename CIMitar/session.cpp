#include "CIMitar.h"
#include "appinit.h"
#include "localaddresses.h"
#include "operation.h"
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

std::unique_ptr<Session> DefaultSession{ nullptr };
void CIMitar::SetDefaultSession(Session& NewDefaultSession)
{
	DefaultSession = make_unique<Session>(NewDefaultSession);
}

Session::Session(const wstring& ComputerName)
{
	Close();
	lock_guard ApplicationAccessGuard(ApplicationMutex);
	if (Sessions.empty())
	{
		DefaultSession = make_unique<Session>(this);
		auto AppInitResult{ InitializeCIMitar() };
		if (AppInitResult.first == MI_Result::MI_RESULT_OK)
			TheCimApplication = AppInitResult.second;
		else
			TheCimApplication = MI_APPLICATION_NULL;	// TODO: this is not enough
	}
	this->ComputerName = ComputerName;
	Sessions.emplace_back(this);
}

Session::~Session()
{
	lock_guard ApplicationAccessGuard(ApplicationMutex);
	Sessions.remove(this);
	if (!Sessions.empty() && DefaultSession.get() == this)
	{
		DefaultSession = make_unique<Session>(Sessions.begin());
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
	return Result == MI_RESULT_OK;
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

Class Session::GetClass(const std::wstring& Name) { return GetClass(GetDefaultNamespace(), Name); }
Class Session::GetClass(const std::wstring& Namespace, const std::wstring& Name)
{
	ClassOpPack Op{};
	MI_Session_GetClass(&CIMSession, 0, NULL, Namespace.c_str(), Name.c_str(), NULL, &Op.operation);
	MI_Operation_GetClass(&Op.operation, &Op.pRetrievedClass, &Op.MoreResults, &Op.Result, &Op.pErrorMessage, &Op.pErrorDetails);
	return Class (Op.pRetrievedClass);	// MI_Session_GetClass should only return one result, and if it doesn't, Op will kill further results when it goes out of scope
}