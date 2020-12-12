#include "CIMitar.h"

using namespace CIMitar;
using namespace std;

void WithOptions::AddCustom(wstring Name, wstring Value)
{
	CustomStringOptions[Name] = Value;
}

void WithOptions::AddCustom(wstring Name, unsigned int Value)
{
	CustomNumberOptions[Name] = Value;
}

void WithOptions::SetCustom(wstring Name, wstring Value)
{
	AddCustom(Name, Value);
}

void WithOptions::SetCustom(wstring Name, unsigned int Value)
{
	AddCustom(Name, Value);
}

template<typename T>
static void RemoveCustomOption(wstring Name, map<wstring, T>& OptionList)
{
	auto location = OptionList.find(Name);
	if (location != OptionList.end())
	{
		OptionList.erase(location);
	}
}

void WithOptions::RemoveCustomString(wstring Name)
{
	RemoveCustomOption(Name, CustomStringOptions);
}

void WithOptions::RemoveAllCustomStrings()
{
	CustomStringOptions.clear();
}

void WithOptions::RemoveCustomNumber(wstring Name)
{
	RemoveCustomOption(Name, CustomNumberOptions);
}

void WithOptions::RemoveAllCustomNumbers()
{
	CustomNumberOptions.clear();
}

void WithOptions::RemoveAllCustom()
{
	RemoveAllCustomNumbers();
	RemoveAllCustomStrings();
}

void WithOptions::ResetAll()
{
	*this = WithOptions{};
}

void SessionOptions::AddTargetCredentials(const UserCredentials& Credentials) noexcept
{
	TargetCredentials.push_back(Credentials);
}

void SessionOptions::AddTargetCredentials(const MI_UsernamePasswordCreds* Credentials) noexcept
{
	TargetCredentials.push_back(std::move(UserCredentials{ Credentials }));
}

void SessionOptions::AddProxyCredentials(const UserCredentials& Credentials) noexcept
{
	ProxyCredentials.push_back(Credentials);
}

void SessionOptions::AddProxyCredentials(const MI_UsernamePasswordCreds* Credentials) noexcept
{
	ProxyCredentials.push_back(std::move(Credentials));
}

void SessionOptions::ClearTargetCredentials() noexcept
{
	TargetCredentials.clear();
}

void SessionOptions::ClearProxyCredentials() noexcept
{
	ProxyCredentials.clear();
}