#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <map>
#include <mutex>
#include <thread>
#include "localaddresses.h"	// if Windows is included before winsock2, bad things happen

#pragma comment(lib, "iphlpapi.lib") // *UnicastIpAddress* and *Mib* functions
#pragma comment (lib, "Ws2_32.lib")	// INetNtop function

using namespace std;
using namespace CIMitar::Infrastructure;

size_t LocalAddresses::refcount{ 0 };
set<wstring> LocalAddresses::Addresses{};

static constexpr size_t MaxAddressTextLength{ 46 };	// to hold IPv4 or IPv6
static std::mutex AddressesMutex{};
static HANDLE ChangeListenerHandle{ 0 };

void ModifyAddressTable(const PMIB_UNICASTIPADDRESS_TABLE pAddressTable, set<wstring>* Addresses, const bool Delete = false) noexcept
{
	wchar_t szAddress[MaxAddressTextLength]{ 0 };
	std::lock_guard AddressesGuard(AddressesMutex);
	for (unsigned long i{ 0 }; i < pAddressTable->NumEntries; ++i)
	{
		auto& CurrentProtocol = pAddressTable->Table[i].Address.si_family;
		auto& CurrentAdapterIndex = pAddressTable->Table[i].InterfaceIndex;
		auto& RawAddress = pAddressTable->Table[i].Address;
		InetNtop(CurrentProtocol, &RawAddress, szAddress, MaxAddressTextLength);
		if (Delete)
		{
			Addresses->insert(szAddress);
		}
		else
		{
			Addresses->erase(szAddress);
		}
	}
}

void GetAllLocalAddresses(set<wstring>* Addresses)
{
	PMIB_UNICASTIPADDRESS_TABLE pAddressTable{ nullptr };
	auto Result = GetUnicastIpAddressTable(AF_UNSPEC, &pAddressTable);
	if (Result == NO_ERROR)
	{
		ModifyAddressTable(pAddressTable, Addresses);
		FreeMibTable(pAddressTable);
	}
}

VOID WINAPI IPAddressChanged(PVOID CallerContext, PMIB_UNICASTIPADDRESS_ROW pChangedIPRow, MIB_NOTIFICATION_TYPE NotificationType)
{
	MIB_UNICASTIPADDRESS_TABLE EntryTable;	// do not run FreeMibTable() on this
	bool DeleteFlag{ false };
	switch (NotificationType)
	{
	case MIB_NOTIFICATION_TYPE::MibDeleteInstance:
		DeleteFlag = true;
		[[fallthrough]];
	case MIB_NOTIFICATION_TYPE::MibAddInstance:
		EntryTable = { 1, *pChangedIPRow };
		ModifyAddressTable(&EntryTable, static_cast<set<wstring>*>(CallerContext), DeleteFlag);
		break;
	default:
		break;
	}
}

LocalAddresses::LocalAddresses() noexcept
{
	++refcount;
	GetAllLocalAddresses(&Addresses);
	if (ChangeListenerHandle == 0)
	{
		if (NotifyUnicastIpAddressChange(AF_UNSPEC, IPAddressChanged, &Addresses, FALSE, &ChangeListenerHandle) != NO_ERROR)
		{
			ChangeListenerHandle = 0;	// this component is not important enough in this app to warrant worrying about this
		}
	}
}

LocalAddresses::LocalAddresses(const LocalAddresses& source) noexcept
{
	refcount++;
}

LocalAddresses& LocalAddresses::operator=(const LocalAddresses& source) noexcept
{
	if (&source != this) refcount++;
	return *this;
}

LocalAddresses::~LocalAddresses()
{
	--refcount;
	if (!refcount)
	{
		if (ChangeListenerHandle)
		{
			CancelMibChangeNotify2(ChangeListenerHandle);
			ChangeListenerHandle = 0;
		}
		std::lock_guard AddressesGuard(AddressesMutex);
		Addresses.clear();
	}
}

const set<wstring> LocalAddresses::operator()() const noexcept
{
	lock_guard AddressesGuard(AddressesMutex);
	return Addresses;	// return a copy so that caller locking is unnecessary and casting away constness doesn't hurt anything
}
