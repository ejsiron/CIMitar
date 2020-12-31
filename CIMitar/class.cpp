#include "CIMitar.h"

using namespace CIMitar;
using namespace std;

Class::Class(const MI_Class* SourceClass)
{
	if (SourceClass != nullptr)
	{
		miclass = make_unique<MI_Class>(*SourceClass);
	}
}

void Class::swap(Class& CopySource) noexcept
{
	miclass.swap(CopySource.miclass);
}

Class::Class(const Class& CopySource) noexcept
{
	if (CopySource.miclass != nullptr)
	{
		MI_Class* pClonedClass;
		MI_Class_Clone(CopySource.miclass.get(), &pClonedClass);
		if (pClonedClass != nullptr)
		{
			miclass = make_unique<MI_Class>(*pClonedClass);
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
	return IsEmpty() ? wstring{} : miclass->classDecl->name;
}

const std::wstring Class::Namespace() const noexcept
{
	return IsEmpty() ? wstring{} : miclass->namespaceName;
}

const std::wstring Class::ServerName() const noexcept
{
	return IsEmpty() ? wstring{} : miclass->serverName;
}

const std::wstring Class::OwningClassName() const noexcept
{
	return IsEmpty() || IsStatic() || IsDynamic() ? wstring{} : miclass->classDecl->owningClass->classDecl->name;
}

const bool Class::IsEmpty() const noexcept
{
	return miclass == nullptr;
}

const bool Class::IsStatic() const noexcept
{
	return miclass->classDecl->owningClass == NULL;
}

const bool Class::IsDynamic() const noexcept
{
	return reinterpret_cast<int>(miclass->classDecl->owningClass) == -1;
}

const bool Class::IsAssociation() const noexcept
{
	return miclass->classDecl->flags | MI_FLAG_ASSOCIATION;
}

const bool Class::IsIndication() const noexcept
{
	return miclass->classDecl->flags | MI_FLAG_INDICATION;
}

const bool Class::IsAbstract() const noexcept
{
	return miclass->classDecl->flags | MI_FLAG_ABSTRACT;
}

const bool Class::IsTerminal() const noexcept
{
	return miclass->classDecl->flags | MI_FLAG_TERMINAL;
}

Class::~Class()
{
	if (!IsEmpty())
	{
		MI_Class_Delete(miclass.get());
	}
}
