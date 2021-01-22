#include "CIMitar.h"

using namespace CIMitar;

const bool Property::IsNull() const noexcept
{
	return MemoryFlag && MI_FLAG_NULL;
}

const bool Property::IsArray() const noexcept
{
	return CimValue.IsArray();
}
