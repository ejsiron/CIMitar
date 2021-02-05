#include "CIMitar.hpp"
#include <array>
#include <string>

static constexpr std::array ActivityMessages
{
	L"Unknown or unspecified operation",
	L"Initializing application-to-management interface connection",
	L"Connecting to the local CIM interface",
	L"Loading the local CIM interface",
	L"Connecting to the target computer",
	L"A test connection failed",
	L"Disconnecting from the target computer",
	L"Verifying the existence of a namespace",
	L"Retrieving an instance from a CIM operation",
	L"Completing a CIM operation",
	L"Loading reference copy of a CIM class",
	L"Cloning a CIM class",
	L"Cloning a CIM instance",
	L"Retrieving an element value",
	L"Setting an element value",
	L"Shutting down the application",
	L"Applying a destination session option"
};

const std::wstring CIMitar::Activity::GetActivity(const Codes Code) noexcept
{
	size_t TranslatedCode{ static_cast<size_t>(Code) };
	if (TranslatedCode < 0 || TranslatedCode >= ActivityMessages.size())
	{
		TranslatedCode = 0;
	}
	return ActivityMessages.at(TranslatedCode);
}
