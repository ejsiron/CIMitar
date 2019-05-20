#pragma once
#pragma comment(lib, "mi.lib")
#ifndef _MI_H_INCLUDED
#define _MI_H_INCLUDED
#include <mi.h>
#endif // _MI_H_INCLUDED
#include <map>
#include <string>

namespace CIMitar
{
	class CIMApplication sealed
	{
	public:
		struct SessionRecord
		{
			MI_Session* Session;
			size_t RefCount;
		};
	private:
		static bool IsInitialized;
		static MI_Application* Application;
		static size_t RefCount;
		static MI_Session* LocalSession;
		static std::wstring LocalComputerNameFilter;
		static std::map<std::wstring, SessionRecord> SessionPool;
	public:
		CIMApplication();
		CIMApplication(const CIMApplication&) = delete;
		CIMApplication(CIMApplication&&) = delete;
		CIMApplication operator=(const CIMApplication&) = delete;
		~CIMApplication();
		MI_Application* GetMIApplication();
		MI_Session* NewSession(const wchar_t* ComputerName = nullptr);
		static const bool IsSessionLocal(const MI_Session* Session);
		void RemoveSession(const wchar_t* ComputerName = nullptr);
	};
}
