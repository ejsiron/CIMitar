#pragma once
#ifndef CIMITAR_OPERATION_H_INCLUDED
#define CIMITAR_OPERATION_H_INCLUDED
#include <mi.h>	// caution: earlier versions of mi.h did not have a header guard

class OpWalker
{
private:
	MI_Boolean* more{ nullptr };
	const unsigned int Limit{ 0 };
	int passes{ 0 };
public:
	OpWalker(MI_Boolean* MoreResults, const unsigned int MaximumResults = 0) noexcept :more(MoreResults), Limit(MaximumResults) {}
	const bool operator()();
};

class BaseOperationPack
{
private:
	MI_OperationCallbacks callbacks = MI_OPERATIONCALLBACKS_NULL;
public:
	MI_Operation operation = MI_OPERATION_NULL;
	MI_Result Result{ MI_Result::MI_RESULT_OK };
	MI_Boolean MoreResults{ MI_TRUE };
	const MI_Char* pErrorMessage{ nullptr };
	const MI_Instance* pErrorDetails{ nullptr };
	OpWalker More;
	BaseOperationPack() noexcept :More(OpWalker(&MoreResults)) {};
	BaseOperationPack(const unsigned int MaximumResults) noexcept :More(OpWalker(&MoreResults, MaximumResults)) {}
	BaseOperationPack(MI_OperationCallbacks& Callbacks) noexcept :callbacks(Callbacks), More(OpWalker(&MoreResults)) {}
	BaseOperationPack(MI_OperationCallbacks& Callbacks, const unsigned int MaximumResults) noexcept : callbacks(Callbacks), More(OpWalker(&MoreResults, MaximumResults)) {}
	BaseOperationPack(const BaseOperationPack&) = delete;
	BaseOperationPack& operator=(const BaseOperationPack&) = delete;
	virtual ~BaseOperationPack();
	void Cancel();
};

class ClassOpPack : public BaseOperationPack
{
public:
	using BaseOperationPack::BaseOperationPack;
	const MI_Class* pRetrievedClass{ nullptr };
};

class InstanceOpPack : public BaseOperationPack
{
	using BaseOperationPack::BaseOperationPack;
	const MI_Instance* pRetrievedInstance{ nullptr };
};

#endif CIMITAR_OPERATION_H_INCLUDED
