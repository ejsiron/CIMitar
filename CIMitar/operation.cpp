#include "CIMitar.h"
#include "operation.h"

using namespace std;
using namespace CIMitar;

const bool OpWalker::operator()()
{
	return more && !(Limit == 0 || ++passes != Limit);
}

BaseOperationPack::~BaseOperationPack()
{	// TODO: consider error checking -- nothing can be done about a failure
	Cancel();
	MI_Operation_Close(&operation);
}

void BaseOperationPack::Cancel()
{	// TODO: consider error checking -- nothing can be done about a failure, but probably worth reporting in async ops
	MI_Result CloseResult{ MI_Operation_Cancel(&operation, MI_CancellationReason::MI_REASON_NONE) };
}
