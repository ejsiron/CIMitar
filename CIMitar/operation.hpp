#pragma once
#ifndef CIMITAR_OPERATION_H_INCLUDED
#define CIMITAR_OPERATION_H_INCLUDED
#include "CIMitar.hpp"

namespace CIMitar::Operation
{
	const bool TestConnection(MI_Session* TheSession) noexcept;
	std::list<Class> GetClass(MI_Session* TheSession, const std::wstring& Namespace, const std::wstring& ClassName, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept;
	std::list<Class> EnumerateClasses(MI_Session* TheSession, const std::wstring& Namespace, const std::wstring& ClassName, const bool NamesOnly, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept;
	std::list<Instance> EnumerateInstances(MI_Session* TheSession, const std::wstring& Namespace, const std::wstring& ClassName, const bool KeysOnly, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept;
	std::list<Instance> GetAssociatedInstance(MI_Session* TheSession, const std::wstring& Namespace, const MI_Instance* SourceInstance, const std::wstring& ResultClassName = std::wstring{}, const std::wstring& AssociatorClassName = std::wstring{}, const std::wstring& Role = std::wstring{}, const std::wstring& ResultRole = std::wstring{}, const bool KeysOnly = false, MI_OperationCallbacks* callbacks = nullptr, OperationFlags* flags = nullptr, OperationOptions* options = nullptr) noexcept;
	Instance CreateInstance(MI_Session* TheSession, const std::wstring& Namespace, const MI_Instance* SourceInstance, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept;
	Instance GetInstance(MI_Session* TheSession, const MI_Char* Namespace, const MI_Instance* SourceInstance, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept;
	std::list<Instance> DeleteInstance(MI_Session* TheSession, const std::wstring& Namespace, const MI_Instance* SourceInstance, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept;
	std::list<Instance> ModifyInstance(MI_Session* TheSession, const std::wstring& Namespace, const MI_Instance* ModifyInstance, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept;
	std::list<Instance> QueryInstances(MI_Session* TheSession, const std::wstring& Namespace, const std::wstring Query, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept;
}

#endif CIMITAR_OPERATION_H_INCLUDED
