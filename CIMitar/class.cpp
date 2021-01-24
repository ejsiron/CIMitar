#include "CIMitar.h"

using namespace CIMitar;
using namespace std;

Class::Class(const MI_Class* SourceClass) noexcept
{
	if (SourceClass != nullptr)
	{
		MI_Class* ClonedClass;
		if (MI_RESULT_OK == MI_Class_Clone(SourceClass, &ClonedClass))
		{
			cimclass = make_unique<MI_Class>(*ClonedClass);
		}
	}
}

void Class::swap(Class& CopySource) noexcept
{
	cimclass.swap(CopySource.cimclass);
}

Class::Class(const Class& CopySource) noexcept
{
	if (CopySource.cimclass != nullptr)
	{
		MI_Class* pClonedClass;
		MI_Class_Clone(CopySource.cimclass.get(), &pClonedClass);
		if (pClonedClass != nullptr)
		{
			cimclass = make_unique<MI_Class>(*pClonedClass);
		}
	}
}

Class& Class::operator=(Class CopySource) noexcept
{
	if (this != &CopySource)
	{
		swap(CopySource);
	}
	return *this;
}

const std::wstring Class::Name() const noexcept
{
	return IsEmpty() ? wstring{} : cimclass->classDecl->name;
}

const std::wstring Class::Namespace() const noexcept
{
	return IsEmpty() ? wstring{} : cimclass->namespaceName;
}

const std::wstring Class::ServerName() const noexcept
{
	return IsEmpty() ? wstring{} : cimclass->serverName;
}

const std::wstring Class::OwningClassName() const noexcept
{
	return IsEmpty() || IsStatic() || IsDynamic() ? wstring{} : cimclass->classDecl->owningClass->classDecl->name;
}

const bool Class::IsEmpty() const noexcept
{
	return cimclass == nullptr;
}

const bool Class::IsStatic() const noexcept
{
	return cimclass->classDecl->owningClass == NULL;
}

const bool Class::IsDynamic() const noexcept
{
	return reinterpret_cast<int>(cimclass->classDecl->owningClass) == -1;
}

const bool Class::IsAssociation() const noexcept
{
	return cimclass->classDecl->flags | MI_FLAG_ASSOCIATION;
}

const bool Class::IsIndication() const noexcept
{
	return cimclass->classDecl->flags | MI_FLAG_INDICATION;
}

const bool Class::IsAbstract() const noexcept
{
	return cimclass->classDecl->flags | MI_FLAG_ABSTRACT;
}

const bool Class::IsTerminal() const noexcept
{
	return cimclass->classDecl->flags | MI_FLAG_TERMINAL;
}

Class::~Class()
{
	if (!IsEmpty())
	{
		MI_Class* rawclass = cimclass.release();
		MI_Class_Delete(rawclass);
	}
}
