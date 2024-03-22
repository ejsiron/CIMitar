#include "CIMitar.hpp"

using namespace CIMitar;
using namespace std;

using OpCodes = CIMitar::Activity::Codes;

static void ProcessResult(MI_Result Result, ErrorStack& ErrorLog, OpCodes OpCode, wstring& OptionName) noexcept
{
	if (Result != MI_RESULT_OK)
	{
		ErrorLog.emplace_back(Error{ static_cast<const unsigned int>(Result), OpCode, wstring{ L"Applying: " + OptionName } });
	}
}

static void ClearDestinationOptions(MI_DestinationOptions* Options) noexcept
{
	if (Options != nullptr)
	{
		MI_DestinationOptions_Delete(Options);
	}
}

template <typename OptionFunction, typename MIOptionContainer, typename... OptionArgs>
void ApplyOption(ErrorStack& ErrorLog, OpCodes OpCode, wstring OptionName, OptionFunction Function, MIOptionContainer* Options, OptionArgs... Args)
{
	MI_Result OptionApplicationResult{ Function(Options, (Args)...) };
	ProcessResult(OptionApplicationResult, ErrorLog, OpCode, OptionName);
}

SessionOptions::SessionOptions()
{
	Reset();
}

void SessionOptions::Reset()
{
	if (DestinationOptions)
	{
		ClearDestinationOptions(DestinationOptions.get());
	}
	MI_DestinationOptions NewOptions = MI_DESTINATIONOPTIONS_NULL;
	DestinationOptions.reset(MI_DestinationOptions_New());
}

void SessionOptions::ApplyOptions(MI_DestinationOptions* Options, ErrorStack& SessionErrors) noexcept
{
	SessionErrors.clear();
	if (Options != nullptr)
	{
		for (auto const& StringOption : CustomStringOptions)
		{
			ApplyOption(SessionErrors, OpCodes::ApplyingDestinationOption, StringOption.first, MI_DestinationOptions_SetString, Options, StringOption.first.c_str(), StringOption.second.Get().c_str());
		}

		for (auto const& NumericOption : CustomNumberOptions)
		{
			ApplyOption(SessionErrors, OpCodes::ApplyingDestinationOption, NumericOption.first, MI_DestinationOptions_SetNumber, Options, NumericOption.first.c_str(), NumericOption.second.Get());
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

template<typename T>
static void RemoveCustomOption(wstring Name, map<wstring, T>& OptionList)
{
	auto location = OptionList.find(Name);
	if (location != OptionList.end())
	{
		OptionList.erase(location);
	}
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
