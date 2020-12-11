#pragma once

#ifndef __cplusplus
#error CIMitar is a C++ wrapper and only works with C++.
#endif

#pragma comment(lib,"mi.lib")

#include <Windows.h>
#include <ctime>
#include <locale>
#include <mi.h> // caution: earlier versions of mi.h did not have a header guard
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#ifndef MI_USE_WCHAR
#error CIMitar only functions with wide characters
#endif

namespace CIMitar
{
	class cimitar_exception
	{
	private:
		const std::wstring moreinformation{};
	public:
		constexpr cimitar_exception(const unsigned int ErrorCode, const int OpCode) :CIMErrorCode(ErrorCode), CIMitarOperationCode(OpCode) {}
		cimitar_exception(const unsigned int ErrorCode, const int OpCode, const std::wstring& MoreInformation)
			:CIMErrorCode(ErrorCode), CIMitarOperationCode(OpCode), moreinformation(MoreInformation)
		{}
		const int CIMErrorCode{ -1 };
		const int CIMitarOperationCode{ -1 };
		const wchar_t* Message() const noexcept;
		const wchar_t* Operation() const noexcept;
		const wchar_t* what() const noexcept { return Message(); }
		const std::wstring& MoreInformation() const noexcept { return moreinformation; }
	};

#pragma region MI type encapsulation
	class Interval
	{
	public:
		unsigned int Days{ 0 };
		unsigned int Hours{ 0 };
		unsigned int Minutes{ 0 };
		unsigned int Seconds{ 0 };
		unsigned int Microseconds{ 0 };
		constexpr Interval() noexcept = default;
		constexpr Interval(MI_Interval* MIInterval) noexcept;
		const MI_Interval& ToMIInterval() const noexcept;
		const MI_Timestamp& ToMITimestamp() const noexcept;
	};

	const bool operator==(const MI_Interval& lhs, const MI_Interval& rhs) noexcept;
	const bool operator==(const Interval& lhs, const Interval& rhs) noexcept;

	class Timestamp
	{
	public:
		unsigned int Year{ 0 };
		unsigned int Month{ 0 };
		unsigned int Day{ 0 };
		unsigned int Hour{ 0 };
		unsigned int Minute{ 0 };
		unsigned int Second{ 0 };
		unsigned int Microseconds{ 0 };
		int UTCOffset{ 0 };
		constexpr Timestamp() noexcept = default;
		constexpr Timestamp(MI_Timestamp* MITimestamp) noexcept;
		const MI_Timestamp& ToMITimestamp() const noexcept;
		const MI_Interval& ToMIInterval() const noexcept;
	};

	const bool operator==(const MI_Timestamp& lhs, const MI_Timestamp& rhs) noexcept;
	const bool operator==(const Timestamp& lhs, const Timestamp& rhs) noexcept;

	tm TMFromCIMTime(const MI_Timestamp& Timestamp) noexcept;
	MI_Timestamp CIMTimeFromTM(const tm& TM) noexcept;

	class UsernamePasswordCreds
	{
	private:
		std::wstring domain{};
		std::wstring username{};
		std::unique_ptr<wchar_t[]> password{ nullptr };	// memory securely wiped at destruction, but not protected in-memory
	public:
		constexpr UsernamePasswordCreds() noexcept {}
		constexpr UsernamePasswordCreds(const MI_UsernamePasswordCreds* Credentials) noexcept;
		constexpr UsernamePasswordCreds(const std::wstring Username, const wchar_t* Password) noexcept;
		constexpr UsernamePasswordCreds(const std::wstring Domain, const std::wstring Username, const wchar_t* Password) noexcept;
		constexpr UsernamePasswordCreds(const UsernamePasswordCreds& copysource) noexcept;
		constexpr UsernamePasswordCreds& operator=(const UsernamePasswordCreds& copysource) noexcept;
		friend const bool operator==(UsernamePasswordCreds& lhs, UsernamePasswordCreds& rhs) noexcept;
		~UsernamePasswordCreds();
		volatile const MI_UsernamePasswordCreds operator()() const noexcept;
	};

	// only compares domain & username to prevent password leaks
	const bool operator==(UsernamePasswordCreds& lhs, UsernamePasswordCreds& rhs) noexcept;
	const bool operator!=(UsernamePasswordCreds& lhs, UsernamePasswordCreds& rhs) noexcept;

	enum class AuthenticationTypes
	{
		DEFAULT,
		NONE,
		DIGEST,
		NEGO_WITH_CREDS,
		NEGO_NO_CREDS,
		BASIC,
		KERBEROS,
		CLIENT_CERTS,
		NTLM,
		CREDSSP,
		ISSUERCERT
	};

	class UserCredentials
	{
	private:
		AuthenticationTypes authenticationtype = AuthenticationTypes::DEFAULT;
		std::variant<UsernamePasswordCreds, std::wstring> credentials;
	public:
		constexpr UserCredentials() noexcept = default;
		constexpr UserCredentials(const MI_UsernamePasswordCreds* Credentials) noexcept : credentials(UsernamePasswordCreds(Credentials)) {}
	};

#pragma endregion

	class Error
	{
	private:
		int CimErrorCode = 0;
		// todo: add Operation tracking
	public:
		const int Code() const noexcept { return CimErrorCode; }
		void Code(const int Code) noexcept { CimErrorCode = Code; }
		const wchar_t* Message() const noexcept;
		static const wchar_t* FindMessage(const int Code) noexcept;
	};

	using ErrorStack = std::vector<Error>;

	class CimBase
	{
	private:
		Error LastError{};
	protected:
		ErrorStack Errors{};
		inline void SetError(const int Code) { LastError.Code(Code); }
	public:
		const Error& GetLastError() { return LastError; }
		virtual ~CimBase() = default;
	};

#pragma region Session
	enum class SessionProtocols { DCOM, WSMAN };
	enum class SessionPrefixOverrides { NONE, HTTP, HTTPS };
	enum class SessionPacketEncodingOptions { DEFAULT, UTF8, UTF16 };
	enum class SessionProxyOptions { AUTO, NONE, IE, WINHTTP };
	enum class SessionErrorModes { NOTIFY, WAIT };

	class SessionOptions
	{
	private:
		template <typename T>
		class SessionOption
		{
		private:
			std::wstring customname;	// deliberately uninitialized since it has limited use
			T value{};
			bool overrideflag{ false };
		public:
			constexpr SessionOption() noexcept {}
			constexpr SessionOption(T Value) noexcept {}
			constexpr SessionOption(T& Value) : value(Value) noexcept {}
			constexpr SessionOption(std::wstring CustomName, T Value) : customname(CustomName), value(Value), overrideflag(true) noexcept {}

			void Reset() noexcept { overrideflag = false; }
			T& get() noexcept { return value; }
			void Set(T& Value) { Value = value; overrideflag = true; }
			void operator=(T& Value) { Set(Value); }
			const bool IsOverridden() const noexcept { return overrideflag; }
		};

		SessionOption<std::vector<UserCredentials>> TargetCredentials{};
		SessionOption<std::vector<UserCredentials>> ProxyCredentials{};
		std::map <std::wstring, SessionOption<std::wstring>> CustomStringOptions{};
		std::map <std::wstring, SessionOption<unsigned int>> CustomNumberOptions{};
	public:
		SessionOption<bool> CheckCACert{};
		SessionOption<bool> CheckCertCN{};
		SessionOption<bool> CheckCertRevocation{};
		SessionOption<unsigned int> Port{};
		SessionOption<bool> EncodePortInSPN{};
		SessionOption<SessionPrefixOverrides> PrefixOverride{ SessionPrefixOverrides::NONE };
		SessionOption<MI_DestinationOptions_ImpersonationType> ImpersonationType{ MI_DestinationOptions_ImpersonationType::MI_DestinationOptions_ImpersonationType_Default };
		SessionOption<unsigned int> MaxPacketSizeOverride{};
		SessionOption<SessionPacketEncodingOptions> PacketEncoding{ SessionPacketEncodingOptions::DEFAULT };
		SessionOption<bool> PacketIntegrity{};
		SessionOption<bool> PacketPrivacy{};
		SessionOption<SessionProxyOptions> SessionProxyOption{ SessionProxyOptions::AUTO };
		SessionOption<bool> ProvideMachineName{};
		SessionOption<SessionErrorModes> SessionErrorMode{ SessionErrorModes::NOTIFY };
		SessionOption<Interval> Timeout{};
		SessionOption<std::wstring> OperationLocale{};
		SessionOption<std::wstring> UILocale{};
		SessionOption<SessionProtocols> Protocol{};
		SessionOption<bool> UseHTTPS{};
		void AddCustomOption(std::wstring Name, std::wstring Value);
		void AddCustomOption(std::wstring Name, unsigned int Value);
		void SetCustomOption(std::wstring Name, std::wstring Value);
		void SetCustomOption(std::wstring Name, unsigned int Value);
		void RemoveCustomStringOption(std::wstring Name);
		void RemoveAllCustomStringOptions();
		void RemoveCustomNumberOption(std::wstring Name);
		void RemoveAllCustomNumberOptions();
		void RemoveAllCustomOptions();
		void ResetAllOptions();
	};

	class Session
	{
	private:
		unsigned long long SessionID = 0;
		MI_Session* CIMSession = nullptr;
		static std::map<unsigned long long, std::weak_ptr<Session>, std::greater<unsigned long long>> Sessions;
		// TODO: add tracking mechanism for operations
	public:
		Session(std::wstring& ComputerName);
		virtual ~Session();
		const bool StartLocal(SessionProtocols SessionProtocol = SessionProtocols::DCOM);
		const bool Connect(std::wstring& ComputerName, SessionProtocols SessionProtocol = SessionProtocols::WSMAN);
		void StartLocalAsync(SessionProtocols SessionProtocol = SessionProtocols::DCOM);
		void ConnectAsync(std::wstring& ComputerName, SessionProtocols SessionProtocol = SessionProtocols::WSMAN);
		const bool Reconnect();
		void ReconnectAsync();

		/* Do not call from an asynchronous callback! Use CloseAsync instead! */
		const bool Close();

		/* Do not call from a synchronous method! Use Close instead! */
		void CloseAsync();
	};
	const bool operator==(const Session& lhs, const Session& rhs) noexcept;
	const bool operator!=(const Session& lhs, const Session& rhs) noexcept;
#pragma endregion Session

	class OperationOptions
	{
	private:
		template <typename T>
		class OperationOption
		{

		};
	};

	template <typename SubscriberType, typename ReturnType>
	class CIMOperatorBase
	{
	private:
		std::wstring cimnamespace;
		bool keysonly{ false };
	protected:
		constexpr wchar_t[] QueryLanguage = L"WQL";
		Session session;
		void CIMOperatorBase(Session& OperatingSession, std::wstring& Namespace);
		virtual void ReportError();
		virtual void ReportResult(ReturnType retval);
		virtual void ReportCompletion();
	public:
		const bool IsRunning();
		void Cancel();
	};

#pragma region Initial type work

	enum class CIMType
	{
		Boolean,
		BooleanA,
		Char16,
		Char16A,
		Interval,
		IntervalA,
		Real32,
		Real32A,
		Real64,
		Real64A,
		SInt8,
		SInt8A,
		SInt16,
		SInt16A,
		SInt32,
		SInt32A,
		SInt64,
		SInt64A,
		String,
		StringA,
		Timestamp,
		TimestampA,
		UInt8,
		UInt8A,
		UInt16,
		UInt16A,
		UInt32,
		UInt32A,
		UInt64,
		UInt64A
	};

	const wchar_t* CIMTypeName(CIMType Type) noexcept;

	class CIMValue
	{
	private:
		CIMType type;
	protected:
		CIMValue(const CIMType Type) noexcept : type(Type) {};
	public:
		CIMValue(const CIMValue&) = default;
		CIMValue& operator=(const CIMValue&) = default;
		CIMValue(CIMValue&&) = default;
		CIMValue& operator=(CIMValue&&) = default;
		virtual ~CIMValue() {};
		const CIMType GetType() const noexcept { return type; }
	};

	class CIMBoolean : public CIMValue
	{
	public:
		bool Value;
		CIMBoolean(const bool Val = true) noexcept : CIMValue(CIMType::Boolean), Value(Val) {}
		CIMBoolean(const MI_Boolean Val = MI_TRUE) noexcept : CIMValue(CIMType::Boolean), Value(Val == MI_TRUE) {}
		CIMBoolean(const CIMBoolean&) = default;
		CIMBoolean& operator=(const CIMBoolean&) = default;
		CIMBoolean(CIMBoolean&&) = default;
		CIMBoolean& operator=(CIMBoolean&&) = default;
		operator bool() const noexcept { return Value; }
	};

	class CIMBooleanA : public CIMValue
	{
	public:
		std::vector<bool> Value;
		CIMBooleanA(const MI_BooleanA* Val) noexcept;
		CIMBooleanA(std::vector<bool> Val) noexcept :CIMValue(CIMType::BooleanA), Value(std::move(Val)) {}
		CIMBooleanA(const CIMBooleanA&) = default;
		CIMBooleanA& operator=(const CIMBooleanA&) = default;
		CIMBooleanA(CIMBooleanA&&) = default;
		CIMBooleanA& operator=(CIMBooleanA&&) = default;
		operator std::vector<bool>() const noexcept { return Value; }
	};

	class CIMChar16 : public CIMValue
	{
	public:
		wchar_t Value;
		CIMChar16(const MI_Char16 Val) noexcept : CIMValue(CIMType::Char16), Value(Val) {}
		CIMChar16(wchar_t Val) noexcept : CIMValue(CIMType::Char16), Value(Val) {}
		CIMChar16(const CIMChar16&) = default;
		CIMChar16& operator=(const CIMChar16&) = default;
		CIMChar16(CIMChar16&&) = default;
		CIMChar16& operator=(CIMChar16&&) = default;
		operator wchar_t() const noexcept { return Value; }
	};

	class CIMChar16A : public CIMValue
	{
	public:
		std::vector<wchar_t> Value;
		CIMChar16A(const MI_Char16A* Val) noexcept;
		CIMChar16A(std::vector<wchar_t> Val) noexcept : CIMValue(CIMType::Char16A), Value(std::move(Val)) {}
		CIMChar16A(const CIMChar16A&) = default;
		CIMChar16A& operator=(const CIMChar16A&) = default;
		CIMChar16A(CIMChar16A&&) = default;
		CIMChar16A& operator=(CIMChar16A&&) = default;
		operator std::vector<wchar_t>() const noexcept { return Value; }
	};

	class CIMInterval : public CIMValue
	{
	public:
		MI_Interval Value;
		CIMInterval(const MI_Datetime Val) noexcept : CIMValue(CIMType::Interval), Value(Val.u.interval) {}
		CIMInterval(const CIMInterval&) = default;
		CIMInterval& operator=(const CIMInterval&) = default;
		CIMInterval(CIMInterval&&) = default;
		CIMInterval& operator=(CIMInterval&&) = default;
		operator MI_Interval() const noexcept { return Value; }
	};

	class CIMIntervalA : public CIMValue
	{
	public:
		std::vector<MI_Interval> Value;
		CIMIntervalA(const MI_DatetimeA* Val) noexcept;
		CIMIntervalA(const CIMIntervalA&) = default;
		CIMIntervalA& operator=(const CIMIntervalA&) = default;
		CIMIntervalA(CIMIntervalA&&) = default;
		CIMIntervalA& operator=(CIMIntervalA&&) = default;
		operator std::vector<MI_Interval>() const noexcept { return Value; }
	};

	class CIMReal32 : public CIMValue
	{
	public:
		float Value;
		CIMReal32(const float Val) noexcept :CIMValue(CIMType::Real32), Value(Val) {}
		CIMReal32(const CIMReal32&) = default;
		CIMReal32& operator=(const CIMReal32&) = default;
		CIMReal32(CIMReal32&&) = default;
		CIMReal32& operator=(CIMReal32&&) = default;
		operator float() const noexcept { return Value; }
	};

	class CIMReal32A : public CIMValue
	{
	public:
		std::vector<float> Value;
		CIMReal32A(const MI_Real32A* Val) noexcept;
		CIMReal32A(const CIMReal32A&) = default;
		CIMReal32A& operator=(const CIMReal32A&) = default;
		CIMReal32A(CIMReal32A&&) = default;
		CIMReal32A& operator=(CIMReal32A&&) = default;
		operator std::vector<float>() const noexcept { return Value; }
	};

	class CIMReal64 : public CIMValue
	{
	public:
		long double Value;
		CIMReal64(const long double Val) noexcept :CIMValue(CIMType::Real64), Value(Val) {}
		CIMReal64(const CIMReal64&) = default;
		CIMReal64& operator=(const CIMReal64&) = default;
		CIMReal64(CIMReal64&&) = default;
		CIMReal64& operator=(CIMReal64&&) = default;
		operator long double() const noexcept { return Value; }
	};

	class CIMReal64A : public CIMValue
	{
	public:
		std::vector<long double> Value;
		CIMReal64A(const MI_Real64A* Val) noexcept;
		CIMReal64A(const CIMReal64A&) = default;
		CIMReal64A& operator=(const CIMReal64A&) = default;
		CIMReal64A(CIMReal64A&&) = default;
		CIMReal64A& operator=(CIMReal64A&&) = default;
		operator std::vector<long double>() const noexcept { return Value; }
	};

	class CIMSInt8 : public CIMValue
	{
	public:
		int Value;
		explicit CIMSInt8(const MI_Sint8 Val) noexcept : CIMValue(CIMType::SInt8), Value(Val) {}
		CIMSInt8(const int Val) noexcept :CIMValue(CIMType::SInt8), Value(Val) {}
		CIMSInt8(const CIMSInt8&) = default;
		CIMSInt8& operator=(CIMSInt8&) = default;
		CIMSInt8(CIMSInt8&&) = default;
		CIMSInt8& operator=(CIMSInt8&&) = default;
		operator int() const noexcept { return Value; }
	};

	class CIMSInt8A :public CIMValue
	{
	public:
		std::vector<int> Value;
		CIMSInt8A(const MI_Sint8A* Val) noexcept;
		CIMSInt8A(const CIMSInt8A&) = default;
		CIMSInt8A& operator=(const CIMSInt8A&) = default;
		CIMSInt8A(CIMSInt8A&&) = default;
		CIMSInt8A& operator=(CIMSInt8A&&) = default;
		operator std::vector<int>() const noexcept { return Value; }
	};

	class CIMSInt16 :public CIMValue
	{
	public:
		int Value;
		CIMSInt16(const MI_Sint16 Val) noexcept :CIMValue(CIMType::SInt16), Value(Val) {}
		CIMSInt16(const CIMSInt16&) = default;
		CIMSInt16& operator=(const CIMSInt16&) = default;
		CIMSInt16(CIMSInt16&&) = default;
		CIMSInt16& operator=(CIMSInt16&&) = default;
		operator int() const noexcept { return Value; }
	};

	class CIMSInt16A :public CIMValue
	{
	public:
		std::vector<int> Value;
		CIMSInt16A(const MI_Sint16A* Val) noexcept;
		CIMSInt16A(const CIMSInt16A&) = default;
		CIMSInt16A& operator=(const CIMSInt16A&) = default;
		CIMSInt16A(CIMSInt16A&&) = default;
		CIMSInt16A& operator=(CIMSInt16A&&) = default;
		operator std::vector<int>() const noexcept { return Value; }
	};

	class CIMSInt32 : public CIMValue
	{
	public:
		int Value;
		CIMSInt32(const MI_Sint32 Val) noexcept :CIMValue(CIMType::SInt32), Value(Val) {};
		CIMSInt32(const CIMSInt32&) = default;
		CIMSInt32& operator=(const CIMSInt32&) = default;
		CIMSInt32(CIMSInt32&&) = default;
		CIMSInt32& operator=(CIMSInt32&&) = default;
		operator int() const noexcept { return Value; }
	};

	class CIMSInt32A : public CIMValue
	{
	public:
		std::vector<int> Value;
		CIMSInt32A(const MI_Sint32A* Val) noexcept;
		CIMSInt32A(const CIMSInt32A&) = default;
		CIMSInt32A& operator=(const CIMSInt32A&) = default;
		CIMSInt32A(CIMSInt32A&&) = default;
		CIMSInt32A& operator=(CIMSInt32A&&) = default;
		operator std::vector<int>() const noexcept { return Value; }
	};

	class CIMSInt64 : public CIMValue
	{
	public:
		long long Value;
		CIMSInt64(const long long Val) noexcept :CIMValue(CIMType::SInt64), Value(Val) {};
		CIMSInt64(const CIMSInt64&) = default;
		CIMSInt64& operator=(const CIMSInt64&) = default;
		CIMSInt64(CIMSInt64&&) = default;
		CIMSInt64& operator=(CIMSInt64&&) = default;
		operator long long() const noexcept { return Value; }
	};

	class CIMSInt64A : public CIMValue
	{
	public:
		std::vector<long long> Value;
		CIMSInt64A(const MI_Sint64A* Val) noexcept;
		CIMSInt64A(const CIMSInt64A&) = default;
		CIMSInt64A& operator=(const CIMSInt64A&) = default;
		CIMSInt64A(CIMSInt64A&&) = default;
		CIMSInt64A& operator=(CIMSInt64A&&) = default;
		operator std::vector<long long >() const noexcept { return Value; }
	};

	class CIMString : public CIMValue
	{
	public:
		std::wstring Value;
		CIMString(const MI_Char* Val) noexcept;
		CIMString(const CIMString&) = default;
		CIMString& operator=(const CIMString&) = default;
		CIMString(CIMString&&) = default;
		operator std::wstring() const noexcept { return Value; }
	};

	class CIMStringA : public CIMValue
	{
	public:
		std::vector<std::wstring> Value;
		CIMStringA(const MI_StringA* Val) noexcept;
		CIMStringA(const CIMStringA&) = default;
		CIMStringA& operator=(const CIMStringA&) = default;
		CIMStringA(CIMStringA&&) = default;
		operator std::vector<std::wstring>() const noexcept { return Value; }
	};

	class CIMTimestamp : public CIMValue
	{
	public:
		MI_Timestamp Value;
		CIMTimestamp() noexcept : CIMValue(CIMType::Timestamp) {}
		CIMTimestamp(MI_Datetime Val) noexcept : CIMValue(CIMType::Timestamp), Value(std::move(Val.u.timestamp)) {}
		CIMTimestamp(MI_Timestamp Val) noexcept : CIMValue(CIMType::Timestamp), Value(std::move(Val)) {}
		CIMTimestamp(const CIMTimestamp&) = default;
		CIMTimestamp& operator=(const CIMTimestamp&) = default;
		CIMTimestamp(CIMTimestamp&&) = default;
		CIMTimestamp& operator=(CIMTimestamp&&) = default;
		operator MI_Timestamp() const noexcept { return Value; }
	};

	class CIMTimestampA : public CIMValue
	{
	public:
		std::vector<MI_Timestamp> Value;
		CIMTimestampA(const MI_DatetimeA*) noexcept;
		CIMTimestampA(const CIMTimestampA&) = default;
		CIMTimestampA& operator=(const CIMTimestampA&) = default;
		CIMTimestampA(CIMTimestampA&&) = default;
		CIMTimestampA& operator=(CIMTimestampA&&) = default;
		operator std::vector<MI_Timestamp>() const noexcept { return Value; }
	};

	class CIMUInt8 :public CIMValue
	{
	public:
		unsigned int Value;
		CIMUInt8(const MI_Uint8 Val) noexcept :CIMValue(CIMType::UInt8), Value(Val) {}
		CIMUInt8(const CIMUInt8&) = default;
		CIMUInt8& operator=(const CIMUInt8&) = default;
		CIMUInt8(CIMUInt8&&) = default;
		CIMUInt8& operator=(CIMUInt8&&) = default;
		operator unsigned int() const noexcept { return Value; }
	};

	class CIMUInt8A :public CIMValue
	{
	public:
		std::vector<unsigned int> Value;
		CIMUInt8A(const MI_Uint8A* Val) noexcept;
		CIMUInt8A(const CIMUInt8A&) = default;
		CIMUInt8A& operator=(const CIMUInt8A&) = default;
		CIMUInt8A(CIMUInt8A&&) = default;
		CIMUInt8A& operator=(CIMUInt8A&&) = default;
		operator std::vector<unsigned int>() const noexcept { return Value; }
	};

	class CIMUInt16 :public CIMValue
	{
	public:
		unsigned int Value;
		CIMUInt16(const MI_Uint16 Val) noexcept :CIMValue(CIMType::UInt8), Value(Val) {}
		CIMUInt16(const CIMUInt16&) = default;
		CIMUInt16& operator=(const CIMUInt16&) = default;
		CIMUInt16(CIMUInt16&&) = default;
		CIMUInt16& operator=(CIMUInt16&&) = default;
		operator unsigned int() const noexcept { return Value; }
	};

	class CIMUInt16A :public CIMValue
	{
	public:
		std::vector<unsigned int> Value;
		CIMUInt16A(const MI_Uint16A* Val) noexcept;
		CIMUInt16A(const CIMUInt16A&) = default;
		CIMUInt16A& operator=(const CIMUInt16A&) = default;
		CIMUInt16A(CIMUInt16A&&) = default;
		CIMUInt16A& operator=(CIMUInt16A&&) = default;
		operator std::vector<unsigned int>() const noexcept { return Value; }
	};

	class CIMUInt32 : public CIMValue
	{
	public:
		unsigned int Value;
		CIMUInt32(const unsigned int Val) noexcept :CIMValue(CIMType::UInt32), Value(Val) {}
		CIMUInt32(const CIMUInt32&) = default;
		CIMUInt32& operator=(const CIMUInt32&) = default;
		CIMUInt32(CIMUInt32&&) = default;
		CIMUInt32& operator=(CIMUInt32&&) = default;
		operator unsigned int() const noexcept { return Value; }
	};

	class CIMUInt32A : public CIMValue
	{
	public:
		std::vector<unsigned int> Value;
		CIMUInt32A(const MI_Uint32A* Val) noexcept;
		CIMUInt32A(const CIMUInt32A&) = default;
		CIMUInt32A& operator=(const CIMUInt32A&) = default;
		CIMUInt32A(CIMUInt32A&&) = default;
		CIMUInt32A& operator=(CIMUInt32A&&) = default;
		operator std::vector<unsigned int>() const noexcept { return Value; }
	};

	class CIMUInt64 : public CIMValue
	{
	public:
		unsigned long long Value;
		CIMUInt64(const long long Val) noexcept :CIMValue(CIMType::UInt64), Value(Val) {}
		CIMUInt64(const CIMUInt64&) = default;
		CIMUInt64& operator=(const CIMUInt64&) = default;
		CIMUInt64(CIMUInt64&&) = default;
		CIMUInt64& operator=(CIMUInt64&&) = default;
		operator unsigned long long() const noexcept { return Value; }
	};

	class CIMUInt64A : public CIMValue
	{
	public:
		std::vector<unsigned long long> Value;
		CIMUInt64A(const MI_Uint64A* Val) noexcept;
		CIMUInt64A(const CIMUInt64A&) = default;
		CIMUInt64A& operator=(const CIMUInt64A&) = default;
		CIMUInt64A(CIMUInt64A&&) = default;
		CIMUInt64A& operator=(CIMUInt64A&&) = default;
		operator std::vector<unsigned long long>() const noexcept { return Value; }
	};
#pragma endregion Initial type work
}