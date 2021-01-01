#include "CIMitar.h"
#include "operation.h"
#include <type_traits>

using namespace std;
using namespace CIMitar;

// TODO: rework, must always go until MoreResults == MI_FALSE, even if canceled
template <typename ReturnType, typename MIReturnType, typename OperationPack>
std::vector<ReturnType> ExecuteOperation(MI_Session* TheSession, OperationPack& OpPack,
	MI_Result OperationFunction(MI_Operation*, const MIReturnType**, MI_Boolean*, MI_Result*, const MI_Char**, const MI_Instance**)
) noexcept
{
	std::vector<ReturnType> RTs{};
	do
	{
		OperationFunction(&OpPack.operation, &OpPack.pItem, &OpPack.MoreResults, &OpPack.Result, &Op.pErrorMessage, &Op.pErrorDetails);
		RTs.emplace_back(ReturnType{ Op.pItem });
	} while (Op.More());
	return RTs;
}

vector<Class> GetClass(MI_Session* TheSession, ClassOpPack& oppack)
{
	return ExecuteOperation<Class, MI_Class, ClassOpPack>(TheSession, oppack, MI_Operation_GetClass);
}

vector<Class> EnumerateClasses(MI_Session* TheSession, wstring& Namespace, wstring& ClassName, const bool NamesOnly, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept
{
	ClassOpPack oppack{};
	MI_Session_EnumerateClasses(TheSession, 0, 0, Namespace.c_str(), ClassName.c_str(), NamesOnly, callbacks, &oppack.operation);
	return GetClass(TheSession, oppack);
}

vector<Class> GetClass(MI_Session* TheSession, wstring& Namespace, wstring& ClassName, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept
{
	ClassOpPack oppack{};
	MI_Session_GetClass(TheSession, 0, 0, Namespace.c_str(), ClassName.c_str(), callbacks, &oppack.operation);
	return GetClass(TheSession, oppack);
}

// wrapped functions that generate operations //
//********************************************//

//MI_INLINE void MI_Session_AssociatorInstances(
//	MI_Session* session,
//	MI_Uint32             flags,
//	MI_OperationOptions* options,
//	const MI_Char* namespaceName,
//	const MI_Instance* instanceKey,
//	const MI_Char* assocClass,
//	const MI_Char* resultClass,
//	const MI_Char* role,
//	const MI_Char* resultRole,
//	MI_Boolean            keysOnly,
//	MI_OperationCallbacks* callbacks,
//	MI_Operation* operation
//);

//MI_INLINE void MI_Session_CreateInstance(
//	MI_Session* session,
//	MI_Uint32             flags,
//	MI_OperationOptions* options,
//	const MI_Char* namespaceName,
//	const MI_Instance* inboundInstance,
//	MI_OperationCallbacks* callbacks,
//	MI_Operation* operation
//);

//MI_INLINE void MI_Session_DeleteInstance(
//MI_Session* session,
//MI_Uint32             flags,
//MI_OperationOptions* options,
//const MI_Char* namespaceName,
//const MI_Instance* inboundInstance,
//MI_OperationCallbacks* callbacks,
//MI_Operation* operation
//);

//MI_INLINE void MI_Session_EnumerateInstances(
//	MI_Session* session,
//	MI_Uint32             flags,
//	MI_OperationOptions* options,
//	const MI_Char* namespaceName,
//	const MI_Char* className,
//	MI_Boolean            keysOnly,
//	MI_OperationCallbacks* callbacks,
//	MI_Operation* operation
//);

//MI_INLINE void MI_Session_GetInstance(
//	MI_Session* session,
//	MI_Uint32             flags,
//	MI_OperationOptions* options,
//	const MI_Char* namespaceName,
//	const MI_Instance* inboundInstance,
//	MI_OperationCallbacks* callbacks,
//	MI_Operation* operation
//);

//MI_INLINE void MI_Session_Invoke(
//	MI_Session* session,
//	MI_Uint32             flags,
//	MI_OperationOptions* options,
//	const MI_Char* namespaceName,
//	const MI_Char* className,
//	const MI_Char* methodName,
//	const MI_Instance* inboundInstance,
//	const MI_Instance* inboundProperties,
//	MI_OperationCallbacks* callbacks,
//	MI_Operation* operation
//);

//MI_INLINE void MI_Session_ModifyInstance(
//	MI_Session* session,
//	MI_Uint32             flags,
//	MI_OperationOptions* options,
//	const MI_Char* namespaceName,
//	const MI_Instance* inboundInstance,
//	MI_OperationCallbacks* callbacks,
//	MI_Operation* operation
//);

//MI_INLINE void MI_Session_QueryInstances(
//	MI_Session* session,
//	MI_Uint32             flags,
//	MI_OperationOptions* options,
//	const MI_Char* namespaceName,
//	const MI_Char* queryDialect,
//	const MI_Char* queryExpression,
//	MI_OperationCallbacks* callbacks,
//	MI_Operation* operation
//);

//MI_INLINE void MI_Session_ReferenceInstances(
//	MI_Session* session,
//	MI_Uint32             flags,
//	MI_OperationOptions* options,
//	const MI_Char* namespaceName,
//	const MI_Instance* instanceKey,
//	const MI_Char* resultClass,
//	const MI_Char* role,
//	MI_Boolean            keysOnly,
//	MI_OperationCallbacks* callbacks,
//	MI_Operation* operation
//);
//
//MI_INLINE void MI_Session_Subscribe(
//	MI_Session* session,
//	MI_Uint32                            flags,
//	MI_OperationOptions* options,
//	const MI_Char* namespaceName,
//	const MI_Char* queryDialect,
//	const MI_Char* queryExpression,
//	const MI_SubscriptionDeliveryOptions* deliverOptions,
//	MI_OperationCallbacks* callbacks,
//	MI_Operation* operation
//);

//MI_INLINE void MI_Session_TestConnection(
//	MI_Session* session,
//	MI_Uint32             flags,
//	MI_OperationCallbacks* callbacks,
//	MI_Operation* operation
//);

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
{	// TODO: error checking -- nothing can be done about a failure, but probably worth reporting in async ops
	MI_Result CloseResult{ MI_Operation_Cancel(&operation, MI_CancellationReason::MI_REASON_NONE) };
}
