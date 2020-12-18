#pragma once
#include <set>
#include <string>

namespace CIMitar::Infrastructure
{
	const std::set<std::wstring> GetLocalNamesAndIPs() noexcept;
}
