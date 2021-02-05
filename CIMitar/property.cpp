#include "CIMitar.hpp"
#include "value.hpp"

using namespace CIMitar;
using namespace std;

const wstring& Property::Name() const noexcept
{
	return name;
}

const bool Property::IsNull() const noexcept
{
	return memoryflag && MI_FLAG_NULL;
}

const bool Property::IsArray() const noexcept
{
	return cimvalue.IsArray();
}

Property::Property(const MI_Char* Name, MI_Value& Value, MI_Type Type, MI_Uint32 MemoryFlag) noexcept :
	name(Name), cimvalue(Value, Type, MI_FLAG_NULL& MemoryFlag), cimtype(CIMTypeIDTranslator(Type)), memoryflag(MemoryFlag)
{}

const Value& Property::GetValue() const noexcept
{
	return cimvalue;
}

const CIMTypes Property::GetType() const noexcept
{
	return cimtype;
}
