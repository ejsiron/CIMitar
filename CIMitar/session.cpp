#include "stdafx.h"
#include "CIMitar.h"

using namespace std;
using namespace CIMitar;

static MI_Application TheCimApplication = MI_APPLICATION_NULL;
static unsigned long long NextIndex = 0;
std::map<unsigned long long, std::weak_ptr<Session>, std::greater<unsigned long long>> Session::Sessions
{ std::map<unsigned long long, std::weak_ptr<Session>, std::greater<unsigned long long>>() };

static MI_Session* NewCimSession(const wchar_t* ComputerName, SessionProtocols Protocol)
{
	//MI_Application_NewSession()
}

static void CreateSession(const wchar_t* ComputerName)
{
	 
}

Session::Session(std::wstring& ComputerName)
{
	if (Sessions.empty())
	{
		//std::vector<MI_Instance*> AppInitErrors(1);
		MI_Instance* AppInitErrors;
		auto AppInitResult = MI_Application_Initialize(0, L"CIMitar", &AppInitErrors, &TheCimApplication); //TODO: need to do more error-checking
		NextIndex = 0;
	}
	else
	{
		++NextIndex;
	}
	Sessions.emplace(NextIndex, this);
}

Session::~Session()
{
	auto loc = Sessions.find(SessionID);
	if (loc != Sessions.end())
	{
		Sessions.erase(loc);
	}

	if (Sessions.empty() && TheCimApplication.ft != nullptr)
	{
		OutputDebugString(L"Application deleted");
		MI_Application_Close(&TheCimApplication);
		TheCimApplication = MI_APPLICATION_NULL;
	}
}

const bool Session::Connect()
{

}

const bool Session::Connect(wstring& ComputerName)
{

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
