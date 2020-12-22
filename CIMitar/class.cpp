#include "CIMitar.h"

using namespace CIMitar;

Class::Class(const MI_Class* SourceClass)
{
	name.assign(SourceClass->classDecl->name);
	classnamespace.assign(SourceClass->namespaceName);
	servername.assign(SourceClass->serverName);
	hashcode = SourceClass->classDecl->code;
}