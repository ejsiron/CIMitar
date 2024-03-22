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
	wcout << L"Testing connection to local machine\n";
	wcout << L"===================================\n";
	wcout << L"Test connection result: " << boolalpha << sess.TestConnection() << L'\n';
	wcout << "\nTesting GetClass on Win32_Service\n";
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

	if (instances.size())
	{
		wcout << L"\nTesting GetAssociatedInstance on Win32_Service\n";
		wcout << L"================================================\n";
		//auto assocInstances{ sess.GetAssociatedInstances(instances.front(), L"CIM_Dependency", L"Antecedent") };
		auto assocInstances{ sess.GetAssociatedInstances(instances.front()) };
		wcout << L"Associated instance count: " << assocInstances.size() << L'\n';
		if (assocInstances.size() > 0)
		{
			wcout << L"Showing first instance of " << assocInstances.size() << " associated instances" << L'\n';
			wcout << assocInstances.front().CimClass().Name() << L'\n';
			wcout << L"Instance properties:\n";
			for (auto const& prop : assocInstances.front().Properties())
			{
				wcout << L"\nProperty: " << prop.Name() << L"\nValue: " << prop.GetValue().String() << L'\n';
			}
		}
		else
		{
			wcout << L"No associated instances found\n";
		}
	}
	else
	{
		wcout << "No instances to test GetAssociatedInstance\n";
	}
	wcout.flush();
}
