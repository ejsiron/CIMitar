// TestClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "..\CIMitar\CIMitar.h"

int main()
{
	CIMitar::Session sess{ CIMitar::NewSession() };
	CIMitar::Session sess2{CIMitar::NewSession(L"svhv2")};
	std::wcout << sess.Connect() << std::endl;
	auto x = sess.GetClass(L"Win32_Service");
	std::wcout << L"Class name: " << x.Name() << std::endl;
	std::wcout << L"Class server name: " << x.ServerName() << std::endl;
	std::wcout << L"Owning class name: " << x.GetOwningClassName() << std::endl;
	std::wcout << sess2.Connect() << std::endl;
	x = sess2.GetClass(L"Win32_Service");
	std::wcout << L"Class name: " << x.Name() << std::endl;
	std::wcout << L"Class server name: " << x.ServerName() << std::endl;
	std::wcout << L"Owning class name: " << x.GetOwningClassName() << std::endl;
	sess.Close();
	//sess2.Close();
	std::cout << "Hello World!\n";
}
