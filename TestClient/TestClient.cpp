// TestClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <fstream>
#include <iostream>
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
	wofstream outfile;
	outfile.open("c:\\temp\\bin.bin", ios::out | ios::trunc);
	outfile.close();
	for (auto& svc : instances)
	{
		counter++;
		wcout << counter << L" of " << instances.size() << endl;
		wcout << svc.CimClass().Name() << endl;
		for (auto const& prop : svc.Properties())
		{
			try
			{
				outfile.open("c:\\temp\\bin.bin", ios::out | ios::binary | ios::app);

				if (//prop.Name() == L"Name" ||
					prop.Name() == L"Description")
					outfile << L"Property: " << prop.Name() << L" | size: " << prop.GetValue().String().size() << L" | Value: " << prop.GetValue().String() << endl;
				outfile.flush();
				outfile.close();
			}
			catch (...)
			{
				wcout << L"error" << endl;
			}
		}
		//wcout.flush();
	}
}
