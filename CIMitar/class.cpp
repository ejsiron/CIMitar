#include "CIMitar.h"

using namespace CIMitar;

Class::Class(MI_ClassDecl* Declaration)
{
	name.assign(Declaration->name);
	
}