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

#include <iostream>

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

#pragma region Convenience Defaults
Session* DefaultSession;
void CIMitar::SetDefaultSession(Session& NewDefaultSession)
{
	DefaultSession = &NewDefaultSession;
}

static std::wstring DefaultNamespace{ DefaultCIMNamespace };

void CIMitar::SetDefaultNamespace(const std::wstring& Namespace)
{
	DefaultNamespace = Namespace;
}

const std::wstring& CIMitar::GetDefaultNamespace()
{
	return DefaultNamespace;
}
#pragma endregion Convenience Defaults

Session::Session() { std::wcout << L"Constructed\n"; }

Session::~Session()
{
	wcout << L"Entering a destructor lock point for " << this->ComputerName  << endl;
	Close();
	lock_guard ApplicationAccessGuard(ApplicationMutex);
	wcout << L"Before remove, count is " << Sessions.size() << endl;
	Sessions.remove(this);
	wcout << L"After remove, count is " << Sessions.size() << endl;
	if (DefaultSession == this)
	{
		DefaultSession = Sessions.size() ? Sessions.front() : nullptr;
		wcout << L"Moved or deleted default session\n";
	}
	if (Sessions.empty())
	{
		MI_Application_Close(&TheCimApplication);	// TODO: error-checking? nothing can be done
		wcout << L"Closed the app";
	}
	wcout << L"Releasing a destructor lock\n";
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
	ClassOpPack Op{ 1 };
	MI_Session_GetClass(&CIMSession, 0, NULL, Namespace.c_str(), Name.c_str(), NULL, &Op.operation);
	MI_Operation_GetClass(&Op.operation, &Op.pRetrievedClass, &Op.MoreResults, &Op.Result, &Op.pErrorMessage, &Op.pErrorDetails);
	return Class{ Op.pRetrievedClass };	// MI_Session_GetClass should only return one result, and if it doesn't, Op will kill further results when it goes out of scope
}

Session CIMitar::NewSession()
{
	return NewSession(L"");
}

Session CIMitar::NewSession(const std::wstring ComputerName)
{
	return NewSession(ComputerName, SessionOptions{});
}

Session CIMitar::NewSession(const SessionOptions& Options)
{
	return NewSession(L"", Options);
}

Session CIMitar::NewSession(const std::wstring ComputerName, const SessionOptions& Options)
{
	wcout << L"Entering a constructor lock point" << endl;
	lock_guard ApplicationAccessGuard(ApplicationMutex);
	Session newsession{};
	if (Sessions.empty())
	{
		DefaultSession = &newsession;
		auto AppInitResult{ InitializeCIMitar() };
		if (AppInitResult.first == MI_Result::MI_RESULT_OK)
			TheCimApplication = AppInitResult.second;
		else
			TheCimApplication = MI_APPLICATION_NULL;	// TODO: this is not enough
	}
	newsession.ComputerName = ComputerName;
	Sessions.emplace_back(&newsession);
	wcout << L"Leaving constructor\n";
	ApplicationAccessGuard.~lock_guard();
	return std::move(newsession);
}