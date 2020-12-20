#include "appinit.h"

using namespace std;

std::pair<MI_Result, MI_Application> CIMitar::InitializeCIMitar()
{
	pair<MI_Result, MI_Application> Result{ MI_Result::MI_RESULT_OK, MI_Application {0} };
	Result.first = { MI_Application_Initialize(0, L"CIMitar", NULL, &Result.second) };
	return Result;
}
