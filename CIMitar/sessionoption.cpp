#include "CIMitar.h"

using namespace CIMitar;
using namespace std;

void SessionOptions::AddCustomOption(wstring Name, wstring Value)
{
	CustomStringOptions[Name] = Value;
}

void SessionOptions::AddCustomOption(wstring Name, unsigned int Value)
{
	CustomNumberOptions[Name] = Value;
}

void SessionOptions::SetCustomOption(wstring Name, wstring Value)
{
	AddCustomOption(Name, Value);
}

void SessionOptions::SetCustomOption(wstring Name, unsigned int Value)
{
	AddCustomOption(Name, Value);
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

void SessionOptions::RemoveCustomStringOption(wstring Name)
{
	RemoveCustomOption(Name, CustomStringOptions);
}

void SessionOptions::RemoveAllCustomStringOptions()
{
	CustomStringOptions.clear();
}

void SessionOptions::RemoveCustomNumberOption(wstring Name)
{
	RemoveCustomOption(Name, CustomNumberOptions);
}

void SessionOptions::RemoveAllCustomNumberOptions()
{
	CustomNumberOptions.clear();
}

void SessionOptions::RemoveAllCustomOptions()
{
	RemoveAllCustomStringOptions();
	RemoveAllCustomStringOptions();
}

void SessionOptions::ResetAllOptions()
{
	*this = SessionOptions{};
}