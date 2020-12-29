#include "CIMitar.h"

using namespace CIMitar;
using namespace std;

const bool SessionOptions::HasCustomOptions() const noexcept
{
	return CustomStringOptions.size() && CustomNumberOptions.size();
}

std::vector<MI_Result> Session::ApplyCustomOptions(variant<MI_OperationOptions*, MI_DestinationOptions*> OptionPack) noexcept
{
	std::vector<MI_Result> Results{};
	if (auto pOptions{ std::get_if<MI_OperationOptions*>(&OptionPack) })
	{
		/*for (auto NumericOption : this->CustomNumberOptions)
		{
			Results.emplace_back(MI_OperationOptions_SetNumber())
		}*/
	}
	else if (auto pOptions{ std::get_if<MI_DestinationOptions*>(&OptionPack) })
	{
		/*for (auto const& NumericOption : this->CustomNumberOptions)
		{
			Results.emplace_back(MI_DestinationOptions_SetNumber(*pOptions, NumericOption.first.c_str(), NumericOption.second.value));
		}
		for (auto const& StringOption : this->CustomStringOptions)
		{
			Results.emplace_back(MI_DestinationOptions_SetString(*pOptions, StringOption.first.c_str(), StringOption.second.value.c_str()));
		}*/
	}
	else
	{
		Results.push_back(MI_RESULT_INVALID_PARAMETER);
	}
	return Results;
}

void SessionOptions::AddCustom(wstring Name, wstring Value)
{
	CustomStringOptions[Name] = CustomSessionOption{ Name, Value };
}

void SessionOptions::AddCustom(wstring Name, unsigned int Value)
{
	CustomNumberOptions[Name] = CustomSessionOption{ Name, Value };
}

void SessionOptions::SetCustom(wstring Name, wstring Value)
{
	AddCustom(Name, Value);
}

void SessionOptions::SetCustom(wstring Name, unsigned int Value)
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

void SessionOptions::RemoveCustomString(wstring Name)
{
	RemoveCustomOption(Name, CustomStringOptions);
}

void SessionOptions::RemoveAllCustomStrings()
{
	CustomStringOptions.clear();
}

void SessionOptions::RemoveCustomNumber(wstring Name)
{
	RemoveCustomOption(Name, CustomNumberOptions);
}

void SessionOptions::RemoveAllCustomNumbers()
{
	CustomNumberOptions.clear();
}

void SessionOptions::RemoveAllCustom()
{
	RemoveAllCustomNumbers();
	RemoveAllCustomStrings();
}

void SessionOptions::ResetAll()
{
	*this = SessionOptions();
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