#include "CIMitar.h"

using namespace CIMitar;
using namespace std;

Instance::Instance(const MI_Instance* SourceInstance, const bool CreatedOnHeap) noexcept
{
	if (SourceInstance != nullptr)
	{
		ciminstance = make_unique<MI_Instance>(*SourceInstance);
	}
}

const std::wstring Instance::ServerName() const noexcept
{
	return ciminstance->serverName;
}

const std::wstring Instance::Namespace() const noexcept
{
	return ciminstance->nameSpace;
}

Instance::~Instance()
{
	if (ciminstance)
	{
		// Delete: MI_Instance_Clone, MI_Application_NewInstance, MI_Context_NewInstance,
		// MI_Context_NewDynamicInstance, MI_Context_NewParameters, and MI_Utilities_CimErrorFromErrorCode
		if (onheap)
			MI_Instance_Delete(ciminstance.get());
		else
			MI_Instance_Destruct(ciminstance.get());
	}
}
