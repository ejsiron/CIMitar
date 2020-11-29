// will most likely need to move into CIMitar.h
#pragma once
#pragma (lib, "mi.dll")
#include <mi.h>	// caution: earlier versions of mi.h did not have a header guard
#include <list>
#include <map>
#include <string>
#include <memory>

namespace CIMitar
{
	enum class SessionProtocols { DCOM, WSMAN };

	class Session
	{
	private:
		long long SessionID = 0;
		MI_Session* CIMSession = nullptr;
		static std::map<long long, std::reference_wrapper<Session>> Connections;
		// TODO: add tracking mechanism for operations
	public:
		Session(std::wstring& ComputerName);
		virtual ~Session();
		const bool StartLocal();
		const bool StartLocal(SessionProtocols SessionProtocol);
		const bool Connect(std::wstring& ComputerName);
		const bool Connect(std::wstring& ComputerName, SessionProtocols SessionProtocol);
		void StartLocalAsync();
		void ConnectAsync(std::wstring& ComputerName);
		void ConnectAsync(std::wstring& ComputerName, SessionProtocols SessionProtocol);
		const bool Reconnect();
		void ReconnectAsync();

		/* Do not call from an asynchronous callback! Use CloseAsync instead! */
		const bool Close();

		/* Do not call from a synchronous method! Use Close instead! */
		void CloseAsync();
	};
#pragma region Session operators
	const bool operator==(const Session& lhs, const Session& rhs) noexcept;
	const bool operator!=(const Session& lhs, const Session& rhs) noexcept;
#pragma endregion
}
