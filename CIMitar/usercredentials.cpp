#include "CIMitar.h"
#include <cstring>

using namespace CIMitar;

static volatile wchar_t* ObliterateString(wchar_t* TargetStringAddress)
{
	if (TargetStringAddress != nullptr)
	{
		wmemset(TargetStringAddress, 0, wcslen(TargetStringAddress));
	}
	return TargetStringAddress;
}

// TODO: make more robust
// low priority on the assumption that if the system can't allocate enough bytes for a password that it has bigger problems than
// an inability to feed credentials into an MI session
static const bool CopyPassword(wchar_t* Destination, const wchar_t* Source) noexcept
{
	try
	{
		Destination = new wchar_t[wcslen(Source)];
	}
	catch (...)
	{
		Destination = nullptr;
		return false;
	}
	return wcscpy(Destination, Source);
}

constexpr UsernamePasswordCreds::UsernamePasswordCreds(const MI_UsernamePasswordCreds* Credentials) noexcept
{
	domain = Credentials->domain;
	username = Credentials->username;
	CopyPassword(password, static_cast<const wchar_t*>(Credentials->password));
}

constexpr UsernamePasswordCreds::UsernamePasswordCreds(const std::wstring Username, const wchar_t* Password) noexcept
{
	domain.clear();
	username = Username;
	CopyPassword(password, Password);
}

constexpr UsernamePasswordCreds::UsernamePasswordCreds(const std::wstring Domain, const std::wstring Username, const wchar_t* Password) noexcept
{
	domain = Domain;
	username = Username;
	CopyPassword(password, Password);
}

UsernamePasswordCreds::~UsernamePasswordCreds()
{
	if (ObliterateString(password) != nullptr)
	{
		delete[] password;
	}
}

volatile const MI_UsernamePasswordCreds UsernamePasswordCreds::operator()() const noexcept
{
	return MI_UsernamePasswordCreds{ domain.c_str(), username.c_str(), password };
}
