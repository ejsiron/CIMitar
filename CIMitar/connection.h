// will most likely need to move into CIMitar.h
#pragma once
#pragma (lib, "mi.dll")
#include <mi.h>	// caution: earlier versions of mi.h did not have a header guard
#include <vector>
#include <string>
#include <memory>

namespace CIMitar
{
	class CIMConnection sealed
	{
	private:
		CIMConnection(std::wstring& ComputerName);
		static std::unique_ptr<MI_Application> TheCimApplication;
		static std::vector<std::reference_wrapper<CIMConnection>> Connections;
	public:
		~CIMConnection();
		CIMConnection& Connect();	// local connection
		CIMConnection& Connect(const std::wstring& ComputerName);
		const bool operator==(const CIMConnection& rhs) const noexcept;
	};
}
