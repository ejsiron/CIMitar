#include "appinit.h"

using namespace std;
using namespace CIMitar;

pair<MI_Result, MI_Application> InitializeCIMitar()
{
	pair<MI_Result, MI_Application> Result{ MI_Result::MI_RESULT_OK, MI_Application {0} };
	Result.first = { MI_Application_Initialize(0, L"CIMitar", NULL, &Result.second) };
	return Result;
}
