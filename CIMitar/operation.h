#pragma once
#ifndef CIMITAR_OPERATION_H_INCLUDED
#define CIMITAR_OPERATION_H_INCLUDED
#include "CIMitar.h"

namespace CIMitar
{
	std::vector<Class> GetClass(MI_Session* TheSession, const std::wstring& Namespace, const std::wstring& ClassName, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept;
	std::vector<Class> EnumerateClasses(MI_Session* TheSession, const std::wstring& Namespace, const std::wstring& ClassName, bool NamesOnly, MI_OperationCallbacks* callbacks, OperationFlags* flags, OperationOptions* options) noexcept;
}

#endif CIMITAR_OPERATION_H_INCLUDED
