#pragma once
#ifndef CIMITAR_OPERATION_H_INCLUDED
#define CIMITAR_OPERATION_H_INCLUDED
#include <mi.h>	// caution: earlier versions of mi.h did not have a header guard

class BaseOperationPack
{
private:
	MI_OperationCallbacks callbacks = MI_OPERATIONCALLBACKS_NULL;
public:
	MI_Operation operation = MI_OPERATION_NULL;
	MI_Result Result{ MI_Result::MI_RESULT_OK };
	MI_Boolean MoreResults{ MI_TRUE };
	MI_Char* pErrorMessage{ nullptr };
	MI_Instance* pErrorDetails{ nullptr };
	BaseOperationPack() noexcept = default;
	BaseOperationPack(MI_OperationCallbacks& Callbacks) noexcept :callbacks(Callbacks) {}
	BaseOperationPack(const BaseOperationPack&) = delete;
	BaseOperationPack& operator=(const BaseOperationPack&) = delete;
	void Cancel();
};

class ClassOpPack : public BaseOperationPack
{
public:
	ClassOpPack() noexcept = default;
	ClassOpPack(MI_OperationCallbacks& Callbacks) noexcept :BaseOperationPack(Callbacks) {}
	MI_Class* pRetrievedClass{ nullptr };
};

#endif CIMITAR_OPERATION_H_INCLUDED