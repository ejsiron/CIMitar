#include "CIMitar.hpp"
#include <algorithm>
#include <cstring>
#include <wincrypt.h>
#include <memory>
#include <string>
#include <vector>

#pragma comment(lib, "crypt32.lib")

using namespace std;
using namespace CIMitar;

class CryptMemoryLocker
{
private:
	/*
	* This critical section is initialized on first access but not destroyed until the process exits.
	* Ordinarily, this is considered bad practice. In this case, this is the only copy that will ever
	* exist. The overhead of using refcounting or a singleton pattern is not worth the effort.
	* Windows will clean up the critical section when the process exits.
	*/
	static CRITICAL_SECTION PasswordCriticalSection;
	static PVOID* Lock(DWORD Length)
	{
		if (PasswordCriticalSection.OwningThread == 0)
		{
			InitializeCriticalSectionAndSpinCount(&PasswordCriticalSection, 0x4000);
		}
		EnterCriticalSection(&PasswordCriticalSection);
		PVOID locked = VirtualAlloc(NULL, Length, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE | PAGE_TARGETS_INVALID);
		VirtualLock(locked, Length);	// todo: capture errors from GetLastError()
		return &locked;
	}
	static void* Unlock(PVOID* Locked, DWORD Length)
	{
		VirtualUnlock(Locked, Length);
		VirtualFree(Locked, 0, MEM_RELEASE);
		LeaveCriticalSection(&PasswordCriticalSection);
	}

public:
	static vector<char> GetCipherText(wchar_t* ClearText)
	{
		DWORD EncryptionPadding{ 0 };
		DWORD ClearTextLength = (wcslen(ClearText) + 1) * sizeof(wchar_t);
		DWORD EncryptedLength{ ClearTextLength };
		if (EncryptionPadding = ClearTextLength % CRYPTPROTECTMEMORY_BLOCK_SIZE)
		{
			EncryptedLength += CRYPTPROTECTMEMORY_BLOCK_SIZE - EncryptionPadding;
		}
	}
};
CRITICAL_SECTION CryptMemoryLocker::PasswordCriticalSection = { 0 };

const std::map<AuthenticationTypes, std::wstring> AuthTypeMap = {
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

// "volatile" to hopefully prevent the compiler from optimizing this call away
static volatile void* ZeroString(wchar_t* DoomedString)
{
	if (DoomedString != nullptr)
	{
		auto StringLength = wcslen(DoomedString) + 1;
		SecureZeroMemory(DoomedString, StringLength * sizeof(wchar_t));
	}
	return &DoomedString;
}

static pair<DWORD, vector<char>> EncryptSecret(const wchar_t* ClearText) noexcept
{

	PVOID secret = VirtualAlloc(NULL, EncryptedLength, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE | PAGE_TARGETS_INVALID);
	// todo: nullptr check
	VirtualLock(secret, EncryptedLength);
	// todo: VirtualLock returns bool, errors in GetLastError()
	memcpy_s(secret, EncryptedLength, ClearText, ClearTextLength);
	CryptProtectMemory(secret, EncryptedLength, CRYPTPROTECTMEMORY_SAME_PROCESS);
	DWORD EncryptionStatus = GetLastError();
	wstring EncryptedSecret{
		EncryptionStatus == ERROR_SUCCESS ? static_cast<wchar_t*>(secret)
		: L""
	};
	VirtualUnlock(secret, EncryptedLength);
	VirtualFree(secret, 0, MEM_RELEASE);
}

static std::unique_ptr<wchar_t[]> CopyPassword(const wchar_t* Source) noexcept
{
	auto PasswordBufferLength = wcslen(Source) + 1;
	std::unique_ptr<wchar_t[]> Destination{ std::make_unique<wchar_t[]>(PasswordBufferLength) };
	ZeroString(Destination.get());
	wcscpy_s(Destination.get(), PasswordBufferLength, Source);
	return Destination;
}

UserCredentials::UserCredentials(const std::wstring Username, wchar_t* Password, const bool ClearSource) noexcept
{
	UserCredentials::UserCredentials(std::wstring{}, Username, Password, ClearSource);
}

UserCredentials::UserCredentials(const std::wstring Domain, const std::wstring Username, wchar_t* ClearTextPassword, const bool ClearSource) noexcept
{
	domain = Domain;
	username = Username;
	if (PasswordCriticalSection.OwningThread == 0)
	{
		InitializeCriticalSectionAndSpinCount(&PasswordCriticalSection, 0x4000);
	}
	EnterCriticalSection(&PasswordCriticalSection);
	DWORD EncryptionPadding{ 0 };
	//wchar_t* EncryptedPassword{ nullptr };
	DWORD ClearTextLength = (wcslen(ClearTextPassword) + 1) * sizeof(wchar_t);
	DWORD EncryptedLength{ ClearTextLength };
	if (EncryptionPadding = ClearTextLength % CRYPTPROTECTMEMORY_BLOCK_SIZE)
	{
		EncryptedLength += CRYPTPROTECTMEMORY_BLOCK_SIZE - EncryptionPadding;
	}
	PVOID EncryptedText = VirtualAlloc(NULL, EncryptedLength, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE | PAGE_TARGETS_INVALID);
	// todo: nullptr check
	VirtualLock(EncryptedText, EncryptedLength);
	// todo: VirtualLock returns bool, errors in GetLastError()
	memcpy_s(EncryptedText, EncryptedLength, ClearTextPassword, ClearTextLength);
	CryptProtectMemory(EncryptedText, EncryptedLength, CRYPTPROTECTMEMORY_SAME_PROCESS);
	DWORD EncryptionStatus = GetLastError();	// todo: deal with any error code from CryptProtectMemory
	secret =
		EncryptionStatus == ERROR_SUCCESS ? static_cast<wchar_t*>(EncryptedText)
		: L""
		;
	VirtualUnlock(EncryptedText, EncryptedLength);
	VirtualFree(EncryptedText, 0, MEM_RELEASE);
	if (ClearSource)
	{
		ZeroString(ClearTextPassword);
	}
	LeaveCriticalSection(&PasswordCriticalSection);
}

template<typename OptionType, typename AddCredentialFunction>
void UserCredentials::AddCredentials(UserCredentials& Credentials, OptionType* TargetOption, AddCredentialFunction AddFunction) noexcept
{
	EnterCriticalSection(&PasswordCriticalSection);
	MI_UserCredentials* credentials{ static_cast<MI_UserCredentials*>(
		VirtualAlloc(NULL, sizeof(MI_UserCredentials), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE | PAGE_TARGETS_INVALID)
		) };
	//TODO check nullptr, GetLastError() has details
	if (VirtualLock(credentials, sizeof(MI_UserCredentials)))
	{
		//TODO credentials.authenticationType = MI_AUTH_TYPE_DEFAULT;
		credentials->authenticationType = MI_AUTH_TYPE_BASIC;
		credentials->credentials.usernamePassword.domain = Credentials.domain.c_str();
		credentials->credentials.usernamePassword.username = Credentials.username.c_str();
		credentials->credentials.usernamePassword.password = Credentials.password.get();
		AddFunction(TargetOption, credentials);
		VirtualUnlock(credentials, sizeof(MI_UserCredentials));
	}
	// else log error, GetLastError() has details
	VirtualFree(credentials, 0, MEM_RELEASE);
	LeaveCriticalSection(&PasswordCriticalSection);
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
		;
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
