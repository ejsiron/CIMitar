// TestClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "..\CIMitar\CIMitar.h"

int main()
{
	CIMitar::Session sess{};
	CIMitar::Session sess2{};
	sess.Connect();
	sess.Close();
	sess2.Close();
	std::cout << "Hello World!\n";
}
