#pragma once
#pragma comment(lib, "mi.dll")
#include <mi.h>	// caution: earlier versions of mi.h did not have a header guard
#include <string>

namespace CIMitar
{
	class Element
	{
	private:
		std::wstring Name;
		MI_Value Value;
		MI_Type Type;
		MI_Uint32 MemoryFlag;
	public:
		Element(const MI_Char* Name, MI_Value* Value, MI_Type Type, MI_Uint32 MemoryFlag);
		Element(std::wstring& Name, MI_Value& Value, MI_Type Type, MI_Uint32 MemoryFlag);
		~Element();
		const bool IsNull() { return MemoryFlag && MI_FLAG_NULL; }
		const bool IsArray() { return Type && MI_ARRAY; }
	};
}
