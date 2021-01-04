#include "CIMitar.h"
#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

using namespace std;
using namespace CIMitar;

const map<AuthenticationTypes, wstring> AuthTypeMap = {
	{AuthenticationTypes::DEFAULT, MI_AUTH_TYPE_DEFAULT},
	{AuthenticationTypes::NONE, MI_AUTH_TYPE_NONE},
	{AuthenticationTypes::DIGEST, MI_AUTH_TYPE_DIGEST},
	{AuthenticationTypes::NEGO_WITH_CREDS, MI_AUTH_TYPE_NEGO_WITH_CREDS},
	{AuthenticationTypes::NEGO_NO_CREDS, MI_AUTH_TYPE_NEGO_NO_CREDS},
	{AuthenticationTypes::BASIC, MI_AUTH_TYPE_BASIC},
	{AuthenticationTypes::KERBEROS, MI_AUTH_TYPE_KERBEROS},
	{AuthenticationTypes::CLIENT_CERTS, MI_AUTH_TYPE_CLIENT_CERTS},
	{AuthenticationTypes::NTLM, MI_AUTH_TYPE_NTLM}
};

// "volatile" to hopefully prevent the compiler from optimizing this call away during destruction
static volatile void* ObliterateString(std::unique_ptr<wchar_t[]>& DoomedString)
{
	if (DoomedString != nullptr)
	{
		auto StringLength = wcslen(DoomedString.get()) + 1;
		SecureZeroMemory(DoomedString.get(), StringLength * 2);
	}
	return &DoomedString;
}

static std::unique_ptr<wchar_t[]> CopyPassword(const wchar_t* Source) noexcept
{
	auto PasswordBufferLength = wcslen(Source) + 1;
	std::unique_ptr<wchar_t[]> Destination{ std::make_unique<wchar_t[]>(PasswordBufferLength) };
	ObliterateString(Destination);
	wcscpy_s(Destination.get(), PasswordBufferLength, Source);
	return Destination;
}

UserCredentials::UserCredentials(MI_UserCredentials* Credentials) noexcept
{
	for (auto const& AuthType : AuthTypeMap)
	{
		if (!AuthType.second.compare(Credentials->authenticationType))
		{
			authenticationtype = AuthType.first;
		}
	}
	if (authenticationtype == AuthenticationTypes::CLIENT_CERTS)
	{
		credentials = Credentials->credentials.certificateThumbprint;
	}
	else
	{
		credentials = UsernamePasswordCreds(&Credentials->credentials.usernamePassword);
	}
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

UsernamePasswordCreds::UsernamePasswordCreds(const std::wstring Domain, const std::wstring Username, const wchar_t* Password) noexcept :
	domain(Domain),
	username(Username),
	password(CopyPassword(Password))
{}

UsernamePasswordCreds::UsernamePasswordCreds(const UsernamePasswordCreds& copysource) noexcept
{
	*this = copysource;
}

UsernamePasswordCreds& UsernamePasswordCreds::operator=(const UsernamePasswordCreds& copysource) noexcept
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

MI_Result UserCredentials::ApplyCredential(void* TargetOption, CredentialApplicationModes ApplicationMode) noexcept
{
	MI_UserCredentials uc{};
	switch (authenticationtype)
	{
	case AuthenticationTypes::BASIC:
		uc.authenticationType = MI_AUTH_TYPE_BASIC;
		break;
	case AuthenticationTypes::CLIENT_CERTS:
		uc.authenticationType = MI_AUTH_TYPE_CLIENT_CERTS;
		break;
	case AuthenticationTypes::CREDSSP:
		uc.authenticationType = MI_AUTH_TYPE_CREDSSP;
		break;
	case AuthenticationTypes::DIGEST:
		uc.authenticationType = MI_AUTH_TYPE_DIGEST;
		break;
	case AuthenticationTypes::ISSUERCERT:
		uc.authenticationType = MI_AUTH_TYPE_ISSUER_CERT;
		break;
	case AuthenticationTypes::KERBEROS:
		uc.authenticationType = MI_AUTH_TYPE_KERBEROS;
		break;
	case AuthenticationTypes::NEGO_NO_CREDS:
		uc.authenticationType = MI_AUTH_TYPE_NEGO_NO_CREDS;
		break;
	case AuthenticationTypes::NEGO_WITH_CREDS:
		uc.authenticationType = MI_AUTH_TYPE_NEGO_WITH_CREDS;
		break;
	case AuthenticationTypes::NONE:
		uc.authenticationType = MI_AUTH_TYPE_NONE;
		break;
	case AuthenticationTypes::NTLM:
		uc.authenticationType = MI_AUTH_TYPE_NTLM;
		break;
	default:
		uc.authenticationType = MI_AUTH_TYPE_DEFAULT;
		break;
	}
	try
	{
		switch (credentials.index())
		{
		case 0:
			uc.credentials.certificateThumbprint = get<wstring>(credentials).c_str();
			break;
		case 1:
			uc.credentials.usernamePassword.domain = get<UsernamePasswordCreds>(credentials).domain.c_str();
			uc.credentials.usernamePassword.username = get<UsernamePasswordCreds>(credentials).username.c_str();
			uc.credentials.usernamePassword.password = get<UsernamePasswordCreds>(credentials).password.get();
			break;
		}
	}
	catch (...)
	{
		// if anything goes wrong, allow the MI API to deal with an empty credential pack
	}
	switch (ApplicationMode)
	{
	case CredentialApplicationModes::DestinationMode:
		return MI_DestinationOptions_AddDestinationCredentials(static_cast<MI_DestinationOptions*>(TargetOption), &uc);
	case CredentialApplicationModes::ProxyMode:
		return MI_DestinationOptions_AddProxyCredentials(static_cast<MI_DestinationOptions*>(TargetOption), &uc);
	case CredentialApplicationModes::SubscriptionDeliveryMode:
		return MI_SubscriptionDeliveryOptions_AddDeliveryCredentials(static_cast<MI_SubscriptionDeliveryOptions*>(TargetOption), &uc);
	default:
		return MI_RESULT_INVALID_PARAMETER;
	}
}
