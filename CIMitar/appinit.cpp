#include "CIMitar.h"
#include <map>
#include <mutex>
#include <unordered_set>

using namespace std;

MI_Application InitializeCIMitar()
{
	MI_Application TheApplication = MI_APPLICATION_NULL;
	auto InitResult{ MI_Application_Initialize(0, L"CIMitar", NULL, &TheApplication) };
	return TheApplication;
}
