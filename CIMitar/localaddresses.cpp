#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <algorithm>
#include <iterator>
#include <vector>
#include "localaddresses.h"

#pragma comment(lib, "iphlpapi.lib") // *UnicastIpAddress* and *Mib* functions
#pragma comment (lib, "Ws2_32.lib")	// INetNtop function

using namespace std;

void GetLocalNames(std::set<std::wstring>& Addresses) noexcept
{
	try
	{
		DWORD BufferSize{ 0 };
		vector<wchar_t> Buffer{};
		for (auto NameFormat : {
			COMPUTER_NAME_FORMAT::ComputerNamePhysicalNetBIOS,
			COMPUTER_NAME_FORMAT::ComputerNamePhysicalDnsHostname,
			COMPUTER_NAME_FORMAT::ComputerNamePhysicalDnsFullyQualified
			})
		{
			BufferSize = 0;
			BOOL GetNameResult{ 0 };
			do
			{
				Buffer.resize(BufferSize);
				GetNameResult = GetComputerNameEx(NameFormat, Buffer.data(), &BufferSize);
				if (GetNameResult == 0) GetNameResult = GetLastError();
			} while (GetNameResult == ERROR_MORE_DATA);
			if (!Buffer.empty() && Buffer[0] != 0)
			{
				wstring Name{};
				Name.resize(BufferSize);
				transform(Buffer.cbegin(), Buffer.cend(), back_inserter(Name), [](wchar_t c)
					{ return tolower(static_cast<unsigned char>(c)); }
				);
				Addresses.insert(Name);
			}
		}
	}
	catch (...)
	{
		// as used in this program, this routine isn't important enough to care if this succeeds, just keep it from crashing
	}
}

void GetLocalIPs(std::set<std::wstring>& Addresses) noexcept
{
	constexpr size_t MaxAddressTextLength{ 46 };	// to hold IPv4 or IPv6
	wchar_t szAddress[MaxAddressTextLength]{ 0 };
	PMIB_UNICASTIPADDRESS_TABLE pAddressTable{ nullptr };
	try
	{
		auto Result = GetUnicastIpAddressTable(AF_UNSPEC, &pAddressTable);
		if (Result == NO_ERROR)
		{
			for (unsigned long i{ 0 }; i < pAddressTable->NumEntries; ++i)
			{
				auto& CurrentProtocol = pAddressTable->Table[i].Address.si_family;
				auto& CurrentAdapterIndex = pAddressTable->Table[i].InterfaceIndex;
				auto& RawAddress = pAddressTable->Table[i].Address;
				InetNtop(CurrentProtocol, &RawAddress, szAddress, MaxAddressTextLength);
				Addresses.insert(szAddress);
			}
			FreeMibTable(pAddressTable);
		}
	}
	catch (...)
	{
		// as used in this program, this routine isn't important enough to care if this succeeds, just keep it from crashing
	}
}

const std::set<std::wstring> CIMitar::Infrastructure::GetLocalNamesAndIPs() noexcept
{
	std::set<std::wstring> Addresses;	// could throw but if there isn't enough memory for an empty set then the system is in trouble anyway
	GetLocalNames(Addresses);
	GetLocalIPs(Addresses);
	return Addresses;
}
