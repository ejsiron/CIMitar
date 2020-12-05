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
static std::unique_ptr<wchar_t[]> CopyPassword(const wchar_t* Source) noexcept
{
	auto PasswordBufferLength = wcslen(Source) + 1;
	std::unique_ptr<wchar_t[]> Destination{ std::make_unique<wchar_t[]>(PasswordBufferLength) };
	ObliterateString(Destination.get());
	wcscpy_s(Destination.get(), PasswordBufferLength, Source);
	return Destination;
}

constexpr UsernamePasswordCreds::UsernamePasswordCreds(const MI_UsernamePasswordCreds* Credentials) noexcept
{
	domain = Credentials->domain;
	username = Credentials->username;
	password = CopyPassword(static_cast<const wchar_t*>(Credentials->password));
}

constexpr UsernamePasswordCreds::UsernamePasswordCreds(const std::wstring Username, const wchar_t* Password) noexcept
{
	domain.clear();
	username = Username;
	password = CopyPassword(Password);
}

constexpr UsernamePasswordCreds::UsernamePasswordCreds(const std::wstring Domain, const std::wstring Username, const wchar_t* Password) noexcept
{
	domain = Domain;
	username = Username;
	password = CopyPassword(Password);
}

UsernamePasswordCreds::~UsernamePasswordCreds()
{
	ObliterateString(password.get());
}

volatile const MI_UsernamePasswordCreds UsernamePasswordCreds::operator()() const noexcept
{
	return MI_UsernamePasswordCreds{ domain.c_str(), username.c_str(), password.get() };
}
