#include "CIMitar.h"

using namespace CIMitar;
using namespace std;

const bool WithOptions::HasCustomOptions() const noexcept
{
	return CustomStringOptions.size() && CustomNumberOptions.size();
}

std::vector<MI_Result> WithOptions::ApplyCustomOptions(variant<MI_OperationOptions*, MI_DestinationOptions*> OptionPack) noexcept
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

void WithOptions::AddCustom(wstring Name, wstring Value)
{
	CustomStringOptions[Name] = CustomOption(Name, Value);
}

void WithOptions::AddCustom(wstring Name, unsigned int Value)
{
	CustomNumberOptions[Name] = CustomOption(Name, Value);
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