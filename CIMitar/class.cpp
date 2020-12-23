#include "CIMitar.h"

using namespace CIMitar;

Class::Class(const MI_Class* SourceClass)
{
	if (SourceClass == nullptr)
	{
		empty = true;
	}
	else
	{
		name.assign(SourceClass->classDecl->name);
		classnamespace.assign(SourceClass->namespaceName);
		servername.assign(SourceClass->serverName);
		hashcode = SourceClass->classDecl->code;
		if (SourceClass->classDecl->owningClass == NULL)
		{
			isstatic = true;
		}
		else if (reinterpret_cast<int>(SourceClass->classDecl->owningClass) == -1)
		{
			isdynamic = true;
		}
		else
		{
			ownerclassname.assign(SourceClass->classDecl->owningClass->classDecl->name);
		}
	}
}