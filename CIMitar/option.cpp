#include "CIMitar.h"

using namespace CIMitar;
using namespace std;

using OpCodes = CIMitar::Activity::Codes;

void ProcessResult(MI_Result Result, ErrorStack& ErrorLog, OpCodes OpCode, wstring& OptionName) noexcept
{
	if (Result != MI_RESULT_OK)
	{
		ErrorLog.emplace_back(Error(static_cast<const unsigned int>(Result), static_cast<const int>(OpCode), wstring{ L"Applying: " + OptionName }));
	}
}

template <typename OptionFunction, typename MIOptionContainer, typename... OptionArgs>
void ApplyOption(ErrorStack& ErrorLog, OpCodes OpCode, wstring OptionName, OptionFunction Function, MIOptionContainer* Options, OptionArgs... Args)
{
	MI_Result OptionApplicationResult{ Function(Options, (Args)...) };
	ProcessResult(OptionApplicationResult, ErrorLog, OpCode, OptionName);
}

const bool SessionOptions::HasCustomOptions() const noexcept
{
	return CustomStringOptions.size() && CustomNumberOptions.size();
}

void SessionOptions::ApplyOptions(MI_DestinationOptions* Options, ErrorStack& SessionErrors) noexcept
{
	SessionErrors.clear();
	if (Options != nullptr)
	{
		for (auto const& StringOption : CustomStringOptions)
		{
			ApplyOption(SessionErrors, OpCodes::ApplyingDestinationOption, StringOption.first, MI_DestinationOptions_SetString, Options, StringOption.first.c_str(), StringOption.second.get().c_str());
		}

		for (auto const& NumericOption : CustomNumberOptions)
		{
			ApplyOption(SessionErrors, OpCodes::ApplyingDestinationOption, NumericOption.first, MI_DestinationOptions_SetNumber, Options, NumericOption.first.c_str(), NumericOption.second.get());
		}

		//for (auto const& TargetCredential : TargetCredentials)
		//{
		//	auto CredApplier{ UserCredentials::ApplyCredential };
		//	ApplyOption(SessionErrors, OpCodes::ApplyingDestinationOption, L"destination credential", TargetCredential.*CredApplier, Options, )
		//}
	}
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
