#include "CIMitar.h"
#include <cstring>

using namespace CIMitar;
using namespace std;

static volatile class StringWiper
{
public:
	volatile void operator()(wchar_t* TargetString) volatile const noexcept
	{
		wmemset(TargetString, 0, wcslen(TargetString));
	}
};

constexpr UsernamePasswordCreds::UsernamePasswordCreds(const MI_UsernamePasswordCreds* Credentials) noexcept
{
	domain = Credentials->domain;
	username = Credentials->username;
	try
	{
		password = new wchar_t[wcslen(Credentials->password)];
	}
	catch(...)
	{
		password = nullptr;
		return;
	}
	wcscpy(password, (const wchar_t*)Credentials->password);
}

constexpr UsernamePasswordCreds::UsernamePasswordCreds(const std::wstring Username, const wchar_t* Password) noexcept
{

}

UsernamePasswordCreds::~UsernamePasswordCreds()
{
	StringWiper Wiper;
	if (password != nullptr)
	{
		Wiper(password);
		delete[] password;
	}
}