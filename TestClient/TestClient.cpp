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
	wcout << "Testing GetClass on Win32_Service\n";
	wcout << "=================================\n";
	auto x = sess.GetClass(L"Win32_Service");
	wcout << L"Class name: " << x.Name() << L'\n';
	wcout << L"Class server name: " << x.ServerName() << L'\n';
	wcout << L"Owning class name: " << x.OwningClassName() << L'\n';
	CIMitar::Instance ErrorInstance{ sess.NewInstance(L"CIM_Error") };
	wcout.flush();

	wcout << "\nTesting GetInstances on Win32_Service" << L'\n';
	wcout << "=======================================\n";
	auto instances{ sess.GetInstances(L"Win32_Service") };
	wcout << L"Instance count: " << instances.size() << L'\n';
	if (instances.size() > 0)
	{
		wcout << L"Showing first instance of " << instances.size() << " instances" << L'\n';
		wcout << instances.front().CimClass().Name() << L'\n';
		wcout << L"Instance properties:\n";
		for (auto const& prop : instances.front().Properties())
		{
			wcout << L"\nProperty: " << prop.Name() << L"\nValue: " << prop.GetValue().String() << L'\n';
		}
	}
	else
	{
		wcout << L"No instances found\n";
	}
	wcout.flush();

	wcout << L"\nTesting QueryInstances on Win32_Service\n";
	wcout << L"=========================================\n";
	wcout << L"Query text: SELECT * FROM Win32_Service WHERE Name = 'w32time'\n";
	instances = sess.QueryInstances(L"SELECT * FROM Win32_Service WHERE Name = 'w32time'");
	wcout << L"Query instance count: " << instances.size() << L'\n';
	if (instances.size() > 0)
	{
		wcout << L"Showing first instance of " << instances.size() << " instances" << L'\n';
		wcout << instances.front().CimClass().Name() << L'\n';
		wcout << L"Instance properties:\n";
		for (auto const& prop : instances.front().Properties())
		{
			wcout << L"\nProperty: " << prop.Name() << L"\nValue: " << prop.GetValue().String() << L'\n';
		}
	}
	else
	{
		wcout << L"No instances found\n";
	}
	wcout.flush();
}
