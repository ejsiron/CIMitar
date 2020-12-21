#include "CIMitar.h"
#include "operation.h"

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

BaseOperationPack::~BaseOperationPack()
{	// TODO: consider error checking -- nothing can be done about a failure
	Cancel();
	MI_Operation_Close(&operation);
}

void BaseOperationPack::Cancel()
{	// TODO: consider error checking -- nothing can be done about a failure, but probably worth reporting in async ops
	MI_Operation_Cancel(&operation, MI_CancellationReason::MI_REASON_NONE);
}
