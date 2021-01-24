#include "CIMitar.h"

using namespace CIMitar;
using namespace std;

static unique_ptr<MI_Instance> CloneInstance(const MI_Instance* SourceInstance) noexcept
{
	MI_Instance* ClonedInstance;
	MI_Instance_Clone(SourceInstance, &ClonedInstance);
	return unique_ptr<MI_Instance>{ClonedInstance};
}

Instance::Instance(const MI_Instance* SourceInstance, const bool Destruct) noexcept : destruct(Destruct)
{
	if (SourceInstance != nullptr)
	{
		ciminstance = unique_ptr<MI_Instance>(CloneInstance(SourceInstance));
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
		destruct = CopySource.destruct;
		ciminstance = unique_ptr<MI_Instance>(CloneInstance(CopySource.ciminstance.get()));
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
	std::swap(destruct, CopySource.destruct);
	ciminstance.swap(CopySource.ciminstance);
}

Instance Instance::Empty() noexcept
{
	return Instance{ nullptr };
}

const std::wstring Instance::ServerName() const noexcept
{
	return ciminstance->serverName ? ciminstance->serverName : L"unknown";
}

const std::wstring Instance::Namespace() const noexcept
{
	return ciminstance->nameSpace ? ciminstance->nameSpace : DefaultCIMNamespace;
}

unsigned int Instance::ElementCount() noexcept
{
	MI_Uint32 count{ 0 };
	MI_Instance_GetElementCount(ciminstance.get(), &count);
	return count;
}

const std::list<Property> Instance::Properties() noexcept
{
	if (!properties.size())
	{
		auto PropertyCount{ ElementCount() };
		if (PropertyCount)
		{
			for (unsigned int i{ 0 }; i < PropertyCount; ++i)
			{
				const MI_Char* InPropertyName;
				MI_Value InValue;
				MI_Type InType;
				MI_Uint32 InFlags;
				if (MI_RESULT_OK == MI_Instance_GetElementAt(ciminstance.get(), i, &InPropertyName, &InValue, &InType, &InFlags))
				{
					properties.emplace_back(Property{ InPropertyName, InValue, InType, InFlags });
				}
			}
		}
	}
	return properties;
}

Instance::~Instance()
{
	if (ciminstance)
	{
		MI_Instance* rawinstance = ciminstance.release();
		/***************************************************************/
		// MI_Instance_Delete when created with : MI_Instance_Clone, MI_Application_NewInstance, MI_Context_NewInstance, MI_Context_NewDynamicInstance, MI_Context_NewParameters, and MI_Utilities_CimErrorFromErrorCode
		// MI_Instance_Destruct for all other creation methods
		/***************************************************************/
		destruct ? MI_Instance_Destruct(rawinstance) : MI_Instance_Delete(rawinstance);
	}
}

void CIMitar::swap(Instance& lhs, Instance& rhs) noexcept
{
	lhs.swap(rhs);
}
