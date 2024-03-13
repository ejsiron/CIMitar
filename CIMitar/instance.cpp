#include "CIMitar.hpp"
#include "operation.hpp"

using namespace CIMitar;
using namespace std;

static void DestroyDeleter(MI_Instance* instance)
{
	if (instance)
	{
		MI_Instance_Delete(instance);
	}
}

static void DestructDeleter(MI_Instance* instance)
{
	if (instance)
	{
		MI_Instance_Destruct(instance);
	}
}

static unique_ptr<MI_Instance, void(*)(MI_Instance*)> CloneInstance(const MI_Instance* SourceInstance) noexcept
{
	MI_Instance* ClonedInstance;
	MI_Instance_Clone(SourceInstance, &ClonedInstance);
	return unique_ptr<MI_Instance, void(*)(MI_Instance*)>{ClonedInstance, DestructDeleter};
}

Instance::Instance(const MI_Instance* SourceInstance, MI_Session* Owner) noexcept : owner(Owner)
{
	if (SourceInstance != nullptr)
	{
		ciminstance = CloneInstance(SourceInstance);
		owner = Owner;
	}
}

Instance Instance::Clone(const MI_Instance& SourceInstance, MI_Session* Owner) noexcept
{
	return Instance(&SourceInstance, Owner);
}

Instance::Instance(const Instance& CopySource) noexcept
{
	if (CopySource.ciminstance != nullptr)
	{
		owner = CopySource.owner;
		ciminstance = CloneInstance(CopySource.ciminstance.get());
	}
}

Instance Instance::operator=(const Instance& CopySource) noexcept
{
	Instance tmp{ CopySource };
	swap(tmp);
	return *this;
}

void Instance::swap(Instance& SwapSource) noexcept
{
	ciminstance.swap(SwapSource.ciminstance);
	std::swap(owner, SwapSource.owner);
	properties.clear();	// swaps are done more for copy-swap idiom than actual swapping, so it's ultimately cheaper to rebuild the properties as necessary than to swap lists each time
	SwapSource.properties.clear();
}

Instance Instance::Empty() noexcept
{
	return Instance{ nullptr, nullptr };
}

const std::wstring Instance::ServerName() const noexcept
{
	return ciminstance->serverName ? ciminstance->serverName : L"unknown";
}

const std::wstring Instance::Namespace() const noexcept
{
	return ciminstance->nameSpace ? ciminstance->nameSpace : DefaultCIMNamespace;
}

const unsigned int Instance::ElementCount() noexcept
{
	MI_Uint32 count{ 0 };
	MI_Instance_GetElementCount(ciminstance.get(), &count);
	return count;
}

const Class& Instance::CimClass() noexcept
{
	if (cimclass == nullptr)
	{
		MI_Class* instanceclass{ nullptr };
		MI_Instance_GetClass(ciminstance.get(), &instanceclass);
		cimclass = make_unique<Class>(Class{ instanceclass });
	}
	return *cimclass;
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

const bool Instance::Refresh() noexcept
{
	if (owner && ciminstance && ciminstance->nameSpace)
	{
		auto ReturnedInstance{ Operation::GetInstance(owner, ciminstance->nameSpace, ciminstance.get(), nullptr, nullptr, nullptr) };
		if (ReturnedInstance.ciminstance)
		{
			swap(ReturnedInstance);
			return true;
		}
	}
	return false;
}

void CIMitar::swap(Instance& lhs, Instance& rhs) noexcept
{
	lhs.swap(rhs);
}

Instance CIMitar::NewInstance(const std::wstring& ClassName) noexcept
{
	return NewInstance(DefaultCIMNamespace, ClassName);
}

Instance CIMitar::NewInstance(const std::wstring& Namespace, const std::wstring& ClassName) noexcept
{
	return GetDefaultSession().NewInstance(Namespace, ClassName);
}

std::list<Instance> CIMitar::GetInstance(std::wstring ClassName) noexcept
{
	return GetDefaultSession().GetInstances(ClassName);
}

std::list<Instance> CIMitar::GetInstance(std::wstring Namespace, std::wstring ClassName) noexcept
{
	return GetDefaultSession().GetInstances(Namespace, ClassName);
}

