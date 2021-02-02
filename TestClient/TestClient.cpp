// TestClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "..\CIMitar\CIMitar.h"
using namespace std;

int main()
{
	CIMitar::Session sess{ CIMitar::NewSession() };
	sess.Connect();
	auto x = sess.GetClass(L"Win32_Service");
	wcout << L"Class name: " << x.Name() << std::endl;
	wcout << L"Class server name: " << x.ServerName() << std::endl;
	wcout << L"Owning class name: " << x.OwningClassName() << std::endl;
	CIMitar::Instance ErrorInstance{ sess.NewInstance(L"CIM_Error") };
	for (auto& svc : sess.GetInstances(L"Win32_Service"))
	{
		wcout << svc.CimClass().Name() << endl;
		for (auto const& prop : svc.Properties())
		{
			if (prop.Name() == L"Name")
			{
				wcout << prop.GetValue().String() << endl;
			}
		}
	}

	sess.TestConnection();
}
