#include "operation.h"
#include <type_traits>
#include <iostream>

using namespace std;
using namespace CIMitar;

class OpWalker
{
private:
	const unsigned int Limit{ 0 };
	int passes{ 0 };
public:
	OpWalker(const unsigned int MaximumResults = 0) noexcept :Limit(MaximumResults) {}
	const bool AcceptMore() noexcept { return Limit == 0 || ++passes != Limit; }
	MI_Boolean MoreResults{ MI_FALSE };
};

template <typename MIType, typename CIMitarType>
class BaseOperationPack
{
public:
	MI_OperationCallbacks callbacks = MI_OPERATIONCALLBACKS_NULL;
	OperationFlags* flags;
	OperationOptions* options;
	MI_Operation operation = MI_OPERATION_NULL;
	MI_Result ResultCode{ MI_Result::MI_RESULT_OK };
	const MI_Char* pErrorMessage{ nullptr };
	const MI_Instance* pErrorDetails{ nullptr };
	const MIType* pItem{ nullptr };
	OpWalker Walker;
	const bool AcceptMore() { return Walker.AcceptMore(); }
	std::vector<CIMitarType> Results{};
	BaseOperationPack() noexcept :Walker(OpWalker()) {};
	BaseOperationPack(const unsigned int MaximumResults) noexcept :Walker(OpWalker(MaximumResults)) {}
	BaseOperationPack(MI_OperationCallbacks& Callbacks) noexcept :callbacks(Callbacks), Walker(OpWalker()) {}
	BaseOperationPack(MI_OperationCallbacks& Callbacks, const unsigned int MaximumResults) noexcept : callbacks(Callbacks), Walker(OpWalker(MaximumResults)) {}
	BaseOperationPack(const BaseOperationPack&) = delete;
	BaseOperationPack& operator=(const BaseOperationPack&) = delete;
	virtual ~BaseOperationPack();
	void Cancel();
};

class ClassOpPack : public BaseOperationPack<MI_Class, CIMitar::Class>
{
public:
	using BaseOperationPack::BaseOperationPack;
};

class EnumerateClassOpPack : public ClassOpPack
{
public:
	using ClassOpPack::ClassOpPack;
};

class InstanceOpPack : public BaseOperationPack<MI_Instance, CIMitar::Instance>
{
public:
	using BaseOperationPack::BaseOperationPack;
};

template <typename ReturnType, typename MIReturnType, typename OperationPack, typename OperationFunction, typename SessionFunction, typename... SessionFunctionArgs>
void ExecuteOperation(
	MI_Session* TheSession, OperationPack& OpPack, OperationFunction OpFunction, SessionFunction SessFunction, SessionFunctionArgs&&... Args
) noexcept
{
	SessFunction((Args)..., &OpPack.operation);
	do
	{
		OpFunction(&OpPack.operation, &OpPack.pItem, &OpPack.Walker.MoreResults, &OpPack.ResultCode, &OpPack.pErrorMessage, &OpPack.pErrorDetails);
		if (OpPack.AcceptMore())
		{
			OpPack.Results.emplace_back(ReturnType{ OpPack.pItem });
		}
	} while (OpPack.Walker.MoreResults);
}

template <typename GetClassFunction, typename... GetClassFunctionArgs>
vector<Class> GetClass(MI_Session* TheSession, GetClassFunction Function, GetClassFunctionArgs&&... Args)
{
	ClassOpPack oppack{};
	ExecuteOperation<Class, MI_Class, ClassOpPack>(TheSession, oppack, MI_Operation_GetClass, Function, forward<GetClassFunctionArgs>(Args)...);
	return oppack.Results;
}

vector<Class> CIMitar::EnumerateClasses(MI_Session* TheSession, const wstring& Namespace, const wstring& ClassName, bool NamesOnly, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept
{
	return GetClass(TheSession, MI_Session_EnumerateClasses, TheSession, 0, nullptr, Namespace.c_str(),
		ClassName.c_str(), NamesOnly, callbacks);
}

vector<Class> CIMitar::GetClass(MI_Session* TheSession, const wstring& Namespace, const wstring& ClassName, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept
{
	return GetClass(TheSession, MI_Session_GetClass, TheSession, 0, nullptr, Namespace.c_str(), ClassName.c_str(), callbacks);
}

//void GetInstance(MI_Session* TheSession, InstanceOpPack& oppack)
//{
//	InstanceOpPack oppack{};
//	ExecuteOperation<Instance, MI_Instance, InstanceOpPack>(TheSession, oppack, MI_Operation_GetInstance);
//}

//vector<Instance> GetAssociatedInstances(MI_Session* TheSession, wstring& Namespace, const MI_Instance* SourceInstance, const wstring& AssociatorClass, const wstring& ResultClass, const wstring& Role, const wstring& ResultRole, const bool KeysOnly, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept
//{
//	InstanceOpPack oppack{};
//	MI_Session_AssociatorInstances(TheSession, 0, 0, Namespace.c_str(), SourceInstance, AssociatorClass.c_str(), ResultClass.c_str(), Role.c_str(), ResultRole.c_str(), KeysOnly, callbacks, &oppack.operation);
//	GetInstance(TheSession, oppack);
//	return oppack.Results;
//}

// wrapped functions that generate operations //
//********************************************//

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

template <typename MIType, typename CIMitarType>
BaseOperationPack<MIType, CIMitarType>::~BaseOperationPack()
{	// TODO: consider error checking -- nothing can be done about a failure
	Cancel();
	MI_Operation_Close(&operation);
}

template <typename MIType, typename CIMitarType>
void BaseOperationPack<MIType, CIMitarType>::Cancel()
{	// TODO: error checking -- nothing can be done about a failure, but probably worth reporting in async ops
	MI_Result CloseResult{ MI_Operation_Cancel(&operation, MI_CancellationReason::MI_REASON_NONE) };
}
