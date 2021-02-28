// TestClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <iostream>
#include <locale>

#include "..\CIMitar\CIMitar.hpp"
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
	auto instances{ sess.GetInstances(L"Win32_Service") };
	wcout << L"Count: " << instances.size() << endl;
	size_t counter{ 0 };
	for (auto& svc : instances)
	{
		counter++;
		wcout << counter << L" of " << instances.size() << endl;
		wcout << svc.CimClass().Name() << endl;
		for (auto const& prop : svc.Properties())
		{
			if (prop.Name() == L"Name" || prop.Name() == L"Description")
			{
				wcout << L"Property: " << prop.Name() << L" | Value: " << prop.GetValue().String() << endl;
			}
		}
		wcout.flush();
	}
}
