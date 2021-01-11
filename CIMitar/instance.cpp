#include "CIMitar.h"

using namespace CIMitar;
using namespace std;

static unique_ptr<MI_Instance> CloneInstance(const MI_Instance* SourceInstance) noexcept
{
	MI_Instance* ClonedInstance;
	MI_Instance_Clone(SourceInstance, &ClonedInstance);
	return make_unique<MI_Instance>(*ClonedInstance);
}

Instance::Instance(const MI_Instance* SourceInstance) noexcept
{
	if (SourceInstance != nullptr)
	{
		ciminstance = make_unique<MI_Instance>(CloneInstance(SourceInstance));
	}
}

Instance Instance::Clone(const MI_Instance& SourceInstance) noexcept
{
	return Instance(&SourceInstance);
}

Instance::Instance(const Instance& CopySource) noexcept
{
	if (CopySource.ciminstance != nullptr)
	{
		ciminstance = CloneInstance(CopySource.ciminstance.get());
	}
}

Instance Instance::operator=(const Instance& CopySource) noexcept
{
	if (this != &CopySource)
	{
		*this = CopySource;
	}
	return *this;
}

void Instance::swap(Instance& CopySource) noexcept
{
	ciminstance.swap(CopySource.ciminstance);
}

const std::wstring Instance::ServerName() const noexcept
{
	return ciminstance->serverName ? ciminstance->serverName : L"";
}

const std::wstring Instance::Namespace() const noexcept
{
	return ciminstance->nameSpace ? ciminstance->nameSpace : L"";
}

unsigned int Instance::ElementCount() noexcept
{
	MI_Uint32 count{ 0 };
	MI_Instance_GetElementCount(ciminstance.get(), &count);
	return count;
}

Instance::~Instance()
{
	if (ciminstance)
	{
		/***************************************************************/
		// MI_Instance_Delete when created with : MI_Instance_Clone, MI_Application_NewInstance, MI_Context_NewInstance, MI_Context_NewDynamicInstance, MI_Context_NewParameters, and MI_Utilities_CimErrorFromErrorCode
		// MI_Instance_Destruct for all other creation methods
		// this class only uses MI_Instance_Clone
		/***************************************************************/
		MI_Instance_Delete(ciminstance.get());
	}
}
