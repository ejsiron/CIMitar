#include "CIMitar.h"
#include <map>
#include <mutex>
#include <unordered_set>
#include <netioapi.h>

using namespace std;

static HANDLE AddressChangedHandle{};
static map<NET_IFINDEX, vector<wstring>> InterfaceIPs;
mutex IPListLock{};
VOID WINAPI IPInterfaceChanged(PVOID CallerContext, PMIB_IPINTERFACE_ROW InterfaceRow, MIB_NOTIFICATION_TYPE NotificationType);
void LoadAllAdapters();
void RecordAdapterIPs(const NET_IFINDEX);

MI_Application InitializeCIMitar()
{
	MI_Application TheApplication = MI_APPLICATION_NULL;
	auto InitResult{ MI_Application_Initialize(0, L"CIMitar", NULL, &TheApplication) };
	auto RegistrationResult{ NotifyIpInterfaceChange(AF_UNSPEC, IPInterfaceChanged, &TheApplication, TRUE, &AddressChangedHandle) };
}

VOID WINAPI IPInterfaceChanged(PVOID CallerContext, PMIB_IPINTERFACE_ROW InterfaceRow, MIB_NOTIFICATION_TYPE NotificationType)
{
	InterfaceRow->InterfaceIndex;
}

void LoadAllAdapters()
{
	InterfaceIPs.clear();
	PMIB_IPINTERFACE_TABLE pInterfaceTable{NULL};
	std::unordered_set<NET_IFINDEX> FoundIndexes{};
	if (GetIpInterfaceTable(AF_UNSPEC, &pInterfaceTable) == NO_ERROR)
	{
		for (auto i{ 0 }; i < pInterfaceTable->NumEntries; ++i)
		{
			FoundIndexes.insert(pInterfaceTable->Table[i].InterfaceIndex);
		}
	}
	FreeMibTable(pInterfaceTable);
	RecordAdapterIPs(FoundIndexes);
}

void RecordAdapterIPs(const unordered_set<NET_IFINDEX>& AdapterIndexes)
{
	lock_guard<mutex> ListGuard{IPListLock};
	for (auto const Index : AdapterIndexes)
	{
		InterfaceIPs.erase(Index);

	}
}