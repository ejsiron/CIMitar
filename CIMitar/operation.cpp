#include "CIMitar.h"

using namespace std;
using namespace CIMitar;

static std::wstring DefaultNamespace{ DefaultCIMNamespace };

void SetDefaultNamespace(const std::wstring& Namespace)
{
	DefaultNamespace = Namespace;
}

const std::wstring& GetDefaultNamespace()
{
	return DefaultNamespace;
}