// will most likely need to move into CIMitar.h
#pragma once
#pragma (lib, "mi.dll")
#include <mi.h>	// caution: earlier versions of mi.h did not have a header guard
#include <vector>
#include <string>

namespace CIMitar
{
	class CIMConnection sealed
	{
	private:
		CIMConnection();
		static std::vector<std::reference_wrapper<CIMConnection>> Connections;
	public:
		~CIMConnection();
		CIMConnection& Connect();
		CIMConnection& Connect(const std::wstring ComputerName);
		const bool operator==(const CIMConnection& rhs) const noexcept;
	};
}
