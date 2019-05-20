#include "stdafx.h"
#include "cimapplication.h"
#include "CIMitarUtility.h"
#include "formatguid.h"
#include "formatstring.h"

using namespace std;
using namespace CIMitar;
using Ops = CIMitarOps::OperationCodes;

bool CIMApplication::IsInitialized = false;
MI_Application* CIMApplication::Application = new MI_Application;
size_t CIMApplication::RefCount = 0;
MI_Session* CIMApplication::LocalSession{ nullptr };
wstring CIMApplication::LocalComputerNameFilter{};
map<wstring, CIMApplication::SessionRecord> CIMApplication::SessionPool{ map<wstring, CIMApplication::SessionRecord>() };

MI_Session* NewSession(MI_Application* Application, const std::wstring& ComputerName)
{
	const wchar_t* TargetName{ nullptr };
	if (ComputerName.size() != 0)
	{
		TargetName = ComputerName.c_str();
	}
	MI_Session* NewSession = new MI_Session;
	MI_Result ConnectResult;
	ConnectResult = MI_Application_NewSession(Application, NULL, TargetName, NULL, NULL, NULL, NewSession);
	ProcessCIMResult(ConnectResult, Ops::Connecting, TargetName);
	return NewSession;
}

void CloseSession(MI_Session* Session) noexcept
{
	if (Session)
	{
		if (Session->reserved2)
			MI_Session_Close(Session, NULL, NULL);
		delete Session;
	}
}

MI_Session* InitializeApplication(MI_Application* Application, std::wstring& LocalComputerNameFilter) noexcept
{
	auto LocalSession = NewSession(Application, std::wstring(L""));
	auto ComputerSystem = CIMInstance::GetFirstInstance(LocalSession, Strings::ClassNameComputerSystem);
	auto LocalComputerName = ComputerSystem.GetStringElement(Strings::ElementNameName);
	auto LocalComputerDomain = ComputerSystem.GetStringElement(Strings::ElementNameDomain);
	auto IPAddresses{ vector<wstring>() };
	auto NetworkInformation{ CIMInstance::GetInstances(LocalSession, Strings::ClassNameNetworkAdapterConfiguration) };
	for (auto NetworkObject : NetworkInformation)
	{
		auto NetworkObjectIPAddresses = NetworkObject.GetStringArrayElement(Strings::ElementNameIPAddress);
		IPAddresses.insert(IPAddresses.end(), NetworkObjectIPAddresses.begin(), NetworkObjectIPAddresses.end());
	}
	LocalComputerNameFilter = CreateComputerNameFilter(LocalComputerName, LocalComputerDomain, IPAddresses);
	return LocalSession;
}

CIMApplication::SessionRecord NewSessionRecord(MI_Application* Application, std::wstring& ComputerName)
{
	CIMApplication::SessionRecord NewSessionRecord;
	auto TargetComputerName{ ESUtility::UpperCaseString(ComputerName) };
	NewSessionRecord.Session = NewSession(Application, TargetComputerName);
	return NewSessionRecord;
}

CIMApplication::CIMApplication()
{
	if (!IsInitialized)
	{
		std::wstring ApplicationName{ ESUtility::NewStringFromGUID(nullptr, false, false, true) };
		MI_Result ApplicationInitializationResult;
		ApplicationInitializationResult = MI_Application_Initialize(0, ApplicationName.c_str(), NULL, Application);
		ProcessCIMResult(ApplicationInitializationResult, Ops::ApplicationInitialization);
		LocalSession = InitializeApplication(Application, LocalComputerNameFilter);
		IsInitialized = true;
	}
	RefCount++;
}

CIMApplication::~CIMApplication()
{
	RefCount--;
	if (!RefCount)
	{
		CloseSession(LocalSession);
		MI_Application_Close(Application);
		delete CIMApplication::Application;
		IsInitialized = false;
	}
}

MI_Application* CIMApplication::GetMIApplication()
{
	return Application;
}

MI_Session* AddPooledSession(MI_Application* Application, std::wstring& ComputerName, map<wstring, CIMApplication::SessionRecord>& SessionPool)
{
	MI_Session* AddedSession{ nullptr };
	auto TargetComputerName{ ESUtility::UpperCaseString(ComputerName) };
	if (SessionPool[TargetComputerName].RefCount)
		AddedSession = SessionPool[TargetComputerName].Session;
	else
	{
		AddedSession = NewSession(Application, TargetComputerName);
		SessionPool[TargetComputerName].Session = AddedSession;
	}
	++SessionPool[TargetComputerName].RefCount;
	return AddedSession;
}

MI_Session* CIMApplication::NewSession(const wchar_t* ComputerName)
{
	std::wstring TargetComputerName{ ComputerName };
	if (ComputerName == nullptr || ComputerName[0] == 0 || MatchesComputerName(ComputerName, LocalComputerNameFilter))
		return LocalSession;
	else
		return AddPooledSession(Application, TargetComputerName, SessionPool);
}

void RemovePooledSession(std::wstring& ComputerName, map<wstring, CIMApplication::SessionRecord>& SessionPool)
{
	auto TargetComputerName{ ESUtility::UpperCaseString(ComputerName) };
	auto TargetRecord{ SessionPool[TargetComputerName] };
	switch (TargetRecord.RefCount)
	{
	case 1:
		CloseSession(TargetRecord.Session);
	case 0:	// note the intentional fall-through from case 1; we want to remove any record with 1 or 0 refcounts
		SessionPool.erase(ComputerName);
		break;
	default:	// any RefCount number greater than 1
		--SessionPool[ComputerName].RefCount;
	}
}

const bool CIMApplication::IsSessionLocal(const MI_Session* Session)
{
	return Session == LocalSession;
}

void CIMApplication::RemoveSession(const wchar_t* ComputerName)
{
	std::wstring TargetComputerName{ ComputerName };
	if (!MatchesComputerName(ComputerName, LocalComputerNameFilter))
		RemovePooledSession(TargetComputerName, SessionPool);
}