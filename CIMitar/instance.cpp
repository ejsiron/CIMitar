#include "stdafx.h"
#include "instance.h"

using namespace CIMitar;

Instance::~Instance()
{
	if (TheInstance)
	{
		// Delete: MI_Instance_Clone, MI_Application_NewInstance, MI_Context_NewInstance,
		// MI_Context_NewDynamicInstance, MI_Context_NewParameters, and MI_Utilities_CimErrorFromErrorCode
		if (OnHeap)
			MI_Instance_Delete(TheInstance.get());
		else
			MI_Instance_Destruct(TheInstance.get());
	}
}