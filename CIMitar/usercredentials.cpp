#include "CIMitar.h"
#include <algorithm>
#include <cstring>

using namespace CIMitar;

// "volatile" to hopefully prevent the compiler from optimizing this call away during destruction
static volatile void* ObliterateString(std::unique_ptr<wchar_t[]>& DoomedString)
{
	if (DoomedString != nullptr)
	{
		auto StringLength = wcslen(DoomedString.get()) + 1;
		SecureZeroMemory(DoomedString.get(), StringLength * 2);
	}
}

static std::unique_ptr<wchar_t[]> CopyPassword(const wchar_t* Source) noexcept
{
	auto PasswordBufferLength = wcslen(Source) + 1;
	std::unique_ptr<wchar_t[]> Destination{ std::make_unique<wchar_t[]>(PasswordBufferLength) };
	ObliterateString(Destination);
	wcscpy_s(Destination.get(), PasswordBufferLength, Source);
	return Destination;
}

UsernamePasswordCreds::UsernamePasswordCreds(const MI_UsernamePasswordCreds* Credentials) noexcept :
	domain(Credentials->domain),
	username(Credentials->username),
	password(CopyPassword(static_cast<const wchar_t*>(Credentials->password)))
{}

UsernamePasswordCreds::UsernamePasswordCreds(const std::wstring Username, const wchar_t* Password) noexcept :
	domain(std::wstring{}),
	username(Username),
	password(CopyPassword(Password))
{}

constexpr UsernamePasswordCreds::UsernamePasswordCreds(const std::wstring Domain, const std::wstring Username, const wchar_t* Password) noexcept :
	domain(Domain),
	username(Username),
	password(CopyPassword(Password))
{}

constexpr UsernamePasswordCreds::UsernamePasswordCreds(const UsernamePasswordCreds& copysource) noexcept
{
	*this = copysource;
}

constexpr UsernamePasswordCreds& UsernamePasswordCreds::operator=(const UsernamePasswordCreds& copysource) noexcept
{
	this->domain = copysource.domain;
	this->username = copysource.username;
	this->password = CopyPassword(copysource.password.get());
	return *this;
}

UsernamePasswordCreds::~UsernamePasswordCreds()
{
	ObliterateString(password);
}

const bool CIMitar::operator==(UsernamePasswordCreds& lhs, UsernamePasswordCreds& rhs) noexcept
{
	return
		lhs.domain == rhs.domain &&
		lhs.username == rhs.username;
}

const bool CIMitar::operator!=(UsernamePasswordCreds& lhs, UsernamePasswordCreds& rhs) noexcept
{
	return !(lhs == rhs);
}

volatile const MI_UsernamePasswordCreds UsernamePasswordCreds::operator()() const noexcept
{
	return MI_UsernamePasswordCreds{ domain.c_str(), username.c_str(), password.get() };
}
//
//constexpr UserCredentials::UserCredentials(MI_UserCredentials) noexcept
//{
//
//}