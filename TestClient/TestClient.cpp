// TestClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "..\CIMitar\CIMitar.h"

int main()
{
	CIMitar::Session sess{};
	//CIMitar::Session sess2{L"svhv2"};
	std::wcout << sess.Connect() << std::endl;
	//std::wcout << sess2.Connect() << std::endl;
	sess.Close();
	//sess2.Close();
	std::cout << "Hello World!\n";
}
