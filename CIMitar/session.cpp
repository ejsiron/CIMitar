#include "stdafx.h"
#include "session.h"

using namespace std;
using namespace CIMitar;

static MI_Application* TheCimApplication = nullptr;

Session::Session(std::wstring& ComputerName)
{
	if (TheCimApplication == nullptr)
	{
		MI_Instance AppInitError;
		auto AppInitResult = MI_Application_Initialize(0, L"CIMitar", &AppInitError, TheCimApplication); //TODO: need to do more error-checking
	}
	Connections.emplace_back();
}

Session::~Session()
{
	--ActiveConnections;
	if (ActiveConnections <= 0 && TheCimApplication != nullptr)
	{
		MI_Application_Close(TheCimApplication);
		delete TheCimApplication;
		ActiveConnections = 0;	// logically, connections can't dip below 0, but it's cheap protection
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

const bool CIMitar::operator==(const Session& lhs, const Session& rhs) noexcept
{
	return false;
}

const bool CIMitar::operator!=(const Session& lhs, const Session& rhs) noexcept
{
	return !(lhs == rhs);
}