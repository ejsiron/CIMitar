#include "stdafx.h"
#include "CIMitar.h"

using namespace std;
using namespace CIMitar;

static MI_Application* TheCimApplication = nullptr;
static unsigned long long NextIndex = 0;

static MI_Session* NewCimSession(const wchar_t* ComputerName, SessionProtocols Protocol)
{
	MI_Application_NewSession()
}

Session::Session(std::wstring& ComputerName)
{
	if (TheCimApplication == nullptr)
	{
		MI_Instance AppInitError;
		auto AppInitResult = MI_Application_Initialize(0, L"CIMitar", &AppInitError, TheCimApplication); //TODO: need to do more error-checking
		Sessions = std::map<unsigned long long, std::reference_wrapper<Session>, greater<unsigned long long>>();
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
	if (ActiveConnections <= 0 && TheCimApplication != nullptr)
	{
		MI_Application_Close(TheCimApplication);
		delete TheCimApplication;
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
