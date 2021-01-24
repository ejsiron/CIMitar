#pragma once
#ifndef CIMITAR_OPERATION_H_INCLUDED
#define CIMITAR_OPERATION_H_INCLUDED
#include "CIMitar.h"

namespace CIMitar
{
	const bool TestConnection(MI_Session* TheSession) noexcept;
	std::list<Class> GetClass(MI_Session* TheSession, const std::wstring& Namespace, const std::wstring& ClassName, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept;
	std::list<Class> EnumerateClasses(MI_Session* TheSession, const std::wstring& Namespace, const std::wstring& ClassName, bool NamesOnly, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept;
	std::list<Instance> GetAssociatedInstance(MI_Session* TheSession, const std::wstring& Namespace, const MI_Instance* SourceInstance, const std::wstring& AssociatorClassName, const std::wstring& ResultClassName, const std::wstring& Role, const std::wstring& ResultRole, const bool KeysOnly, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept;
	Instance CreateInstance(MI_Session* TheSession, const std::wstring& Namespace, const MI_Instance* SourceInstance, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept;
	std::list<Instance> DeleteInstance(MI_Session* TheSession, const std::wstring& Namespace, const MI_Instance* SourceInstance, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept;
	std::list<Instance> ModifyInstance(MI_Session* TheSession, const std::wstring& Namespace, const MI_Instance* ModifyInstance, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept;
}

#endif CIMITAR_OPERATION_H_INCLUDED
