#include "CIMitar.h"
#include "value.h"

using namespace CIMitar;

const bool Property::IsNull() const noexcept
{
	return memoryflag && MI_FLAG_NULL;
}

const bool Property::IsArray() const noexcept
{
	return cimvalue.IsArray();
}

Property::Property(const MI_Char* Name, MI_Value& Value, MI_Type Type, MI_Uint32 MemoryFlag) noexcept :
	name(Name), cimvalue(Value, Type), cimtype(CIMTypeIDTranslator(Type)), memoryflag(MemoryFlag)
{}
