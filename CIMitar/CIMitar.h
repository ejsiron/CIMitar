#pragma once
#ifndef CIMITAR_CIMITAR_H_INCLUDED
#define CIMITAR_CIMITAR_H_INCLUDED
#ifndef __cplusplus
#error CIMitar is a C++ wrapper and only works with C++.
#endif
#ifdef _MSC_VER
#if _MSVC_LANG < 201703L
#error CIMitar uses C++17 features and requires a capable compiler
#endif
#endif

#include <Windows.h>
#include <ctime>
#include <list>
#include <locale>
#include <mi.h> // caution: earlier versions of mi.h did not have a header guard
#include <map>
#include <memory>
#include <set>
#include <string>
#include <variant>
#include <vector>

#pragma comment(lib,"mi.lib")

#ifndef MI_USE_WCHAR
#error CIMitar only functions with wide characters
#endif

namespace CIMitar
{
	constexpr wchar_t DefaultCIMNamespace[] = { L"root/CIMv2" };

#pragma region Error Handling
	namespace Activity
	{
		// provides error message context
		enum class Codes :size_t
		{
			Unknown,
			ApplicationInitialization,
			LocalSystemInitialization,
			LoadingApplicationInstance,
			Connecting,
			TestingConnection,
			Disconnecting,
			VerifyingNamespace,
			RetrievingInstanceFromOp,
			CompletingOperation,
			LoadingCIMReferenceClass,
			CloningCIMClass,
			CloningCIMInstance,
			RetrievingElementValue,
			SettingElementValue,
			ApplicationShutdown,
			ExtendedOperation,
			ApplyingDestinationOption
		};
		const std::wstring GetActivity(const Codes Code) noexcept;
	}

	class ExtendedError
	{
	private:
		unsigned int ErrorType{ 0 };
		std::wstring OtherErrorType{};
		std::wstring OwningEntity{};
		std::wstring MessageID{};
		std::wstring Message{};
		std::vector<std::wstring> MessageArguments{};
		unsigned int PerceivedSeverity{ 0 };
		unsigned int ProbableCause{ 0 };
		std::wstring ProbableCauseDescription{};
		std::vector<std::wstring> RecommendedActions{};
		std::wstring ErrorSource{};
		unsigned int ErrorSourceFormat = { 0 };
		std::wstring OtherErrorSourceFormat{};
		friend class Error;
	};

	class Error
	{
	private:
		unsigned int cimstatuscode{ 0 };
		std::wstring cimstatuscodedescription{};
		Activity::Codes activitycode{ Activity::Codes::Unknown };
		std::wstring query{};
		std::unique_ptr<ExtendedError> extendederror{ nullptr };
	public:
		Error(unsigned int CIMStatusCode, Activity::Codes ActivityCode) noexcept;
		Error(MI_Instance* ExtendedError, Activity::Codes ActivityCode) noexcept;
		Error(const Error&) noexcept;
		Error operator=(const Error&) noexcept;
		const unsigned int CIMStatusCode() const noexcept;
		const std::wstring CIMStatusCodeDescription() const noexcept;
		const std::wstring Activity() const noexcept;
		const std::wstring Query() const noexcept;
		const std::wstring OtherErrorType() const noexcept;
		const std::wstring OwningEntity() const noexcept;
		const std::wstring MessageID() const noexcept;
		const std::wstring Message() const noexcept;
		const std::vector<std::wstring> MessageArguments() const noexcept;
		const unsigned int PerceivedSeverity() const noexcept;
		const unsigned int ProbableCause() const noexcept;
		const std::wstring ProbableCauseDescription() const noexcept;
		const std::vector<std::wstring> RecommendedActions() const noexcept;
		const std::wstring ErrorSource() const noexcept;
		const unsigned int ErrorSourceFormat() const noexcept;
		const std::wstring OtherErrorSourceFormat() const noexcept;
	};

	struct ErrorData
	{
		unsigned int CIMStatusCode{ 0 };
		std::wstring CIMStatusCodeDescription{};
		std::wstring Activity{};
		std::wstring Query{};
		unsigned int ErrorType{ 0 };
		std::wstring OtherErrorType{};
		std::wstring OwningEntity{};
		std::wstring MessageID{};
		std::wstring Message{};
		std::vector<std::wstring> MessageArguments{};
		unsigned int PerceivedSeverity{ 0 };
		unsigned int ProbableCause{ 0 };
		std::wstring ProbableCauseDescription{};
		std::vector<std::wstring> RecommendedActions{};
		std::wstring ErrorSource{};
		unsigned int ErrorSourceFormat = { 0 };
		std::wstring OtherErrorSourceFormat{};
	};

	ErrorData ConvertErrorToPlainData(Error& SourceError) noexcept;
	using ErrorStack = std::vector<Error>;
#pragma endregion Error Handling

	enum class CallbackModes
	{
		Report,
		Inquire,
		Ignore
	};

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
		const MI_Interval ToMIInterval() const noexcept;
		const MI_Timestamp ToMITimestamp() const noexcept;
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
		const MI_Timestamp ToMITimestamp() const noexcept;
		const MI_Interval ToMIInterval() const noexcept;
	};

	const bool operator==(const MI_Timestamp& lhs, const MI_Timestamp& rhs) noexcept;
	const bool operator==(const Timestamp& lhs, const Timestamp& rhs) noexcept;

	tm TMFromCIMTime(const MI_Timestamp& Timestamp) noexcept;
	MI_Timestamp CIMTimeFromTM(const tm& TM) noexcept;

	class UsernamePasswordCreds
	{
	private:
		std::wstring domain;
		std::wstring username;
		std::unique_ptr<wchar_t[]> password;	// memory securely wiped at destruction, but not protected in-memory
		friend class UserCredentials;
	public:
		UsernamePasswordCreds(const MI_UsernamePasswordCreds* Credentials) noexcept;
		UsernamePasswordCreds(const std::wstring Username, const wchar_t* Password) noexcept;
		UsernamePasswordCreds(const std::wstring Domain, const std::wstring Username, const wchar_t* Password) noexcept;
		UsernamePasswordCreds(const UsernamePasswordCreds& copysource) noexcept;
		UsernamePasswordCreds& operator=(const UsernamePasswordCreds& copysource) noexcept;
		friend const bool operator==(UsernamePasswordCreds& lhs, UsernamePasswordCreds& rhs) noexcept;
		~UsernamePasswordCreds();
	};

	// only compares domain & username to reduce password leaks
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

	enum class CredentialApplicationModes
	{
		DestinationMode,
		ProxyMode,
		SubscriptionDeliveryMode
	};

	class UserCredentials
	{
	private:
		AuthenticationTypes authenticationtype{ AuthenticationTypes::DEFAULT };
		std::variant<std::wstring, UsernamePasswordCreds> credentials;
	public:
		UserCredentials(MI_UserCredentials* Credentials) noexcept;
		UserCredentials(const MI_UsernamePasswordCreds* Credentials) noexcept : credentials(std::move(UsernamePasswordCreds(Credentials))) {}
		UserCredentials(const std::wstring& CertificateThumbprint) noexcept : credentials(CertificateThumbprint) {}
		UserCredentials(const UsernamePasswordCreds& Credentials) noexcept :credentials(Credentials) {}
		MI_Result ApplyCredential(void* TargetOption, CredentialApplicationModes ApplicationMode) noexcept;
	};

	enum class SessionProtocols { DCOM, WSMAN };
	enum class SessionPrefixOverrides { NONE, HTTP, HTTPS };
	enum class SessionPacketEncodingOptions { DEFAULT, UTF8, UTF16 };
	enum class SessionProxyOptions { AUTO, NONE, IE, WINHTTP };
	enum class SessionErrorModes { NOTIFY, WAIT };

	template <typename T>
	class Option
	{
	protected:
		T value;
#pragma warning(push)
#pragma warning(disable: 26495)
		Option() noexcept {}
#pragma warning(pop)
		Option(T Value) noexcept :value(Value) {}
	public:
		const T& get() const noexcept { return value; }
		virtual void Set(T Value) { value = Value; }
		void operator=(T& Value) { Set(Value); }
	};

	// TODO: investigate templating an application function that takes the *Set* function and translates parameters

	template <typename T>
	class ListedOption :public Option<T>
	{
	private:
		std::set<const void*>& optionlist;
	public:
		void swap(ListedOption& SwapSource) noexcept
		{
			std::swap(this->value, SwapSource.value);
			std::swap(optionlist, SwapSource.optionlist);
		}
		ListedOption(std::set<const void*>& OwningOptionList) noexcept :optionlist(OwningOptionList) {}
		ListedOption(const ListedOption& CopySource)noexcept :optionlist(CopySource.optionlist) { this->value = CopySource.value; }
		ListedOption& operator=(ListedOption CopySource) noexcept
		{
			if (this != &CopySource)
			{
				swap(CopySource);
			}
			return *this;
		}
		void Reset() noexcept { optionlist.erase(this); }
		void Set(T Value) override { this->value = Value; optionlist.insert(this); }
		const bool IsOverridden() const noexcept
		{
			auto found{ optionlist.find(this) };
			return found != optionlist.end();
		}
	};
	template <typename T>
	void swap(ListedOption<T>& lhs, ListedOption<T>& rhs)
	{
		lhs.swap(rhs);
	}

	template <typename T>
	class CustomSessionOption :public Option<T>
	{
	private:
		std::wstring customname{};
	public:
		CustomSessionOption() noexcept {}
		CustomSessionOption(std::wstring CustomName, T Value) :customname(CustomName), Option<T>(Value) {}
		void SetName(std::wstring& NewName) { customname = NewName; }
	};

	class SessionOptions
	{
	private:
		std::set<const void*> overriddenoptions{};
		std::map <std::wstring, CustomSessionOption<unsigned int>> CustomNumberOptions{};
		std::map <std::wstring, CustomSessionOption<std::wstring>> CustomStringOptions{};
		std::vector<UserCredentials> TargetCredentials{};
		std::vector<UserCredentials> ProxyCredentials{};
		const bool HasCustomOptions() const noexcept;
		void ApplyOptions(MI_DestinationOptions* Options, ErrorStack& SessionErrors) noexcept;
		friend class Session;
	public:
		SessionOptions() noexcept {}
		SessionOptions(const SessionOptions&) noexcept = default;
		SessionOptions& operator=(const SessionOptions&) noexcept = default;
		SessionOptions(SessionOptions&&) noexcept = default;
		SessionOptions& operator=(SessionOptions&&) noexcept = default;
		ListedOption<bool> CheckCACert{ overriddenoptions };
		ListedOption<bool> CheckCertCN{ overriddenoptions };
		ListedOption<bool> CheckCertRevocation{ overriddenoptions };
		ListedOption<unsigned int> Port{ overriddenoptions };
		ListedOption<bool> EncodePortInSPN{ overriddenoptions };
		ListedOption<SessionPrefixOverrides> PrefixOverride{ overriddenoptions };
		ListedOption<MI_DestinationOptions_ImpersonationType> ImpersonationType{ overriddenoptions };
		ListedOption<unsigned int> MaxPacketSizeOverride{ overriddenoptions };
		ListedOption<SessionPacketEncodingOptions> PacketEncoding{ overriddenoptions };
		ListedOption<bool> PacketIntegrity{ overriddenoptions };
		ListedOption<bool> PacketPrivacy{ overriddenoptions };
		ListedOption<SessionProxyOptions> SessionProxyOption{ overriddenoptions };
		ListedOption<bool> ProvideMachineName{ overriddenoptions };
		ListedOption<SessionErrorModes> SessionErrorMode{ overriddenoptions };
		ListedOption<Interval> Timeout{ overriddenoptions };
		ListedOption<std::wstring> OperationLocale{ overriddenoptions };
		ListedOption<std::wstring> UILocale{ overriddenoptions };
		ListedOption<SessionProtocols> Protocol{ overriddenoptions };
		ListedOption<bool> UseHTTPS{ overriddenoptions };
		void AddTargetCredentials(const UserCredentials& Credentials) noexcept;
		void AddTargetCredentials(const MI_UsernamePasswordCreds* Credentials) noexcept;
		void AddProxyCredentials(const UserCredentials& Credentials) noexcept;
		void AddProxyCredentials(const MI_UsernamePasswordCreds* Credentials) noexcept;
		void ClearTargetCredentials() noexcept;
		void ClearProxyCredentials() noexcept;
		void AddCustom(std::wstring Name, std::wstring Value);
		void AddCustom(std::wstring Name, unsigned int Value);
		void SetCustom(std::wstring Name, std::wstring Value);
		void SetCustom(std::wstring Name, unsigned int Value);
		void RemoveCustomString(std::wstring Name);
		void RemoveAllCustomStrings();
		void RemoveCustomNumber(std::wstring Name);
		void RemoveAllCustomNumbers();
		void RemoveAllCustom();
		void ResetAll();
	};

	// forward declarations for use in class Session
	class Class;
	class Instance;

	class Session
	{
	private:
		std::shared_ptr<MI_Session> TheSession;
		std::wstring ComputerName;
		std::vector<MI_Result> ApplyCustomOptions(std::variant<MI_OperationOptions*, MI_DestinationOptions*> OptionPack) noexcept;
		const bool Connect(const SessionProtocols* Protocol);
		Session();
		ErrorStack lasterrors{};
	public:
		void swap(Session& SwapSource) noexcept;
		Session(const Session&) noexcept;
		Session& operator=(Session) noexcept;
		Session(Session&&) noexcept = default;
		Session& operator=(Session&&) noexcept = default;
		virtual ~Session();
		static Session NullSession;
		SessionOptions Options{};
		ErrorStack LastErrors() noexcept;
		const bool Connect();
		const bool Connect(const SessionProtocols Protocol);
		const bool Close();
		const bool TestConnection() noexcept;
		Class GetClass(const std::wstring& Name) noexcept;
		Class GetClass(const std::wstring& ClassName, const std::wstring& Name) noexcept;
		std::vector<Class> GetClasses(const std::wstring& Namespace, const bool NameOnly) noexcept;
		std::vector<Class> GetClasses(const std::wstring& Namespace, const std::wstring& SourceClassName, const bool NameOnly) noexcept;
		std::vector<Class> GetClasses(const Class& SourceClass, const bool NameOnly = false) noexcept;
		Instance NewInstance(const std::wstring& ClassName) noexcept;
		Instance NewInstance(const std::wstring& Namespace, const std::wstring& ClassName) noexcept;
		Instance NewInstance(const Instance& SourceInstance) noexcept;

		friend Session NewSession();
		friend Session NewSession(const std::wstring ComputerName);
		friend Session NewSession(const SessionOptions& Options);
		friend Session NewSession(const std::wstring ComputerName, const SessionOptions& Options);
		friend Session& GetDefaultSession() noexcept;
	};
	void swap(Session& lhs, Session& rhs) noexcept;
	const bool operator==(const Session& lhs, const Session& rhs) noexcept;
	const bool operator!=(const Session& lhs, const Session& rhs) noexcept;

	enum class OperatorMessageChannels
	{
		Warning,
		Verbose,
		Debug
	};

	enum class OperationRTTIFlags :int
	{
		Default = MI_OPERATIONFLAGS_DEFAULT_RTTI,
		None = MI_OPERATIONFLAGS_NO_RTTI,
		Basic = MI_OPERATIONFLAGS_BASIC_RTTI,
		Standard = MI_OPERATIONFLAGS_STANDARD_RTTI,
		Full = MI_OPERATIONFLAGS_FULL_RTTI
	};

	enum class OperationPolymorphismFlags :int
	{
		Deep = 0,	// default
		Shallow = MI_OPERATIONFLAGS_POLYMORPHISM_SHALLOW,
		DeepBasePropertiesOnly = MI_OPERATIONFLAGS_POLYMORPHISM_DEEP_BASE_PROPS_ONLY
	};

	class OperationFlags
	{
	public:
		bool ManualAckResults{ false };
		bool LocalizedQualifiers{ false };
		bool UseExpensiveProperties{ false };
		bool ReportOperationStarted{ false };
		OperationRTTIFlags RTTI{ OperationRTTIFlags::Default };
		OperationPolymorphismFlags Polymorphism{ OperationPolymorphismFlags::Deep };
	};

	template <typename T>
	class CustomOperationOption :public CustomSessionOption<T>
	{
	private:
		OperationFlags flags{};
	public:
		CustomOperationOption() noexcept {}
		CustomOperationOption(std::wstring CustomName, T Value) :CustomSessionOption<T>(CustomName, Value) {}
		CustomOperationOption(std::wstring CustomName, T Value, OperationFlags Flags) :flags(Flags), CustomSessionOption<T>(CustomName, Value) {}
		void SetFlags(OperationFlags Flags) { flags = Flags; }
	};

	template <typename T>
	class ClientTypedCustomOperationOption : public CustomSessionOption<T>
	{
	private:
		MI_Type cimtype{ MI_Type::MI_BOOLEAN };
		bool mustcomply{ false };
	public:
		ClientTypedCustomOperationOption() {}
		ClientTypedCustomOperationOption(std::wstring CustomName, T Value, const MI_Type Type) :cimtype(Type), CustomSessionOption<T>(CustomName, Value) {}
		ClientTypedCustomOperationOption(std::wstring CustomName, T Value, const MI_Type Type, const bool MustComply) :cimtype(Type), mustcomply(MustComply), CustomSessionOption<T>(CustomName, Value) {}
	};

	class OperationOptions
	{
	private:
		std::set<const void*> overriddenoptions;
	public:
		ListedOption<bool> EnableWarningMessageChannel{ overriddenoptions };
		ListedOption<bool> EnableVerboseMessageChannel{ overriddenoptions };
		ListedOption<bool> EnableDebugMessageChannel{ overriddenoptions };
		ListedOption<bool> PromptUserMode{ overriddenoptions };
		ListedOption<std::pair<bool, bool>> PromptUserRegularMode{ overriddenoptions };
		ListedOption<bool> PromptClientOnError{ overriddenoptions };
		ListedOption<std::pair<bool, bool>> Force32Bit{ overriddenoptions };
		ListedOption<std::wstring> ResourceURI{ overriddenoptions };
		ListedOption<std::wstring> ResourceURIPrefix{ overriddenoptions };
		ListedOption<Interval> Timeout{ overriddenoptions };
		ListedOption<bool> ProvideMachineID{ overriddenoptions };
	};

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
		MI_Timestamp Value{ 0 };
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

	class Qualifier
	{
	private:
		std::wstring name{};
		unsigned int type{ 0 };
		unsigned int flavor{ 0 };
		void* value{ nullptr };
		Qualifier(const MI_Qualifier* SourceQualifier);
		friend class Class;
	public:
		const std::wstring Name() const noexcept { return name; }
		const bool EnableOverrideSet() const noexcept { return flavor | MI_FLAG_ENABLEOVERRIDE; }
		const bool DisableOverrideSet() const noexcept { return flavor | MI_FLAG_DISABLEOVERRIDE; }
		const bool Restricted() const noexcept { return flavor | MI_FLAG_RESTRICTED; }
		const bool Inheritable() const noexcept { return flavor | MI_FLAG_TOSUBCLASS; }
		const bool Translatable() const noexcept { return flavor | MI_FLAG_TRANSLATABLE; }
	};

	class SchemaDecl
	{
	public:
		std::list<Qualifier> Qualifiers;
		// Classes;
	};
	class FeatureDeclaration
	{
	public:
		std::wstring Name{};
		unsigned int flags{ 0 };
		unsigned int hash{ 0 };
		std::list<Qualifier> Qualifiers{};
	};

	class ParameterDeclaration : FeatureDeclaration
	{
	public:
		unsigned int type;
		std::wstring ClassName{};
		unsigned int subscript;
		unsigned int offset;
	};

	class PropertyDeclaration : ParameterDeclaration
	{
	public:
		std::wstring OriginAncestor{};
		std::wstring PropagatorAncestor{};
		void* value;
	};

	class Instance
	{
	private:
		std::unique_ptr<MI_Instance> ciminstance{ nullptr };
		static Instance Clone(const MI_Instance& SourceInstance) noexcept;
		friend class Session;
	public:
		Instance(const Instance&) noexcept;
		Instance(const MI_Instance*) noexcept;
		Instance operator=(const Instance&) noexcept;
		void swap(Instance& CopySource) noexcept;
		virtual ~Instance();
		const std::wstring ServerName() const noexcept;
		const std::wstring Namespace() const noexcept;
		unsigned int ElementCount() noexcept;
		std::list<PropertyDeclaration> Properties{};
		const bool Refresh() noexcept;
	};
	void swap(Instance& lhs, Instance& rhs) noexcept;

	class Class
	{
	private:
		std::unique_ptr<MI_Class> cimclass{ nullptr };
		//Class(MI_ClassDecl* Declaration) noexcept;
		friend class Session;
	public:
		Class(const MI_Class* SourceClass) noexcept;
		void swap(Class&) noexcept;
		Class(const Class&) noexcept;
		Class& operator=(Class) noexcept;
		virtual ~Class();
		const std::wstring Name() const noexcept;
		const std::wstring Namespace() const noexcept;
		const std::wstring ServerName() const noexcept;
		const std::wstring OwningClassName() const noexcept;
		const bool IsEmpty() const noexcept;
		const bool IsStatic() const noexcept;
		const bool IsDynamic() const noexcept;
		const bool IsAssociation() const noexcept;
		const bool IsIndication() const noexcept;
		const bool IsAbstract() const noexcept;
		const bool IsTerminal() const noexcept;
		// qualifiers
		// properties
		// methods
		// schema
	};
	void swap(Class&, Class&) noexcept;

	Session NewSession();
	Session NewSession(const std::wstring ComputerName);
	Session NewSession(const SessionOptions& Options);
	Session NewSession(const std::wstring ComputerName, const SessionOptions& Options);

	void SetDefaultNamespace(const std::wstring& Namespace);
	const std::wstring& GetDefaultNamespace();
	inline void ResetNamespace() { SetDefaultNamespace(DefaultCIMNamespace); }
	void SetDefaultSession(Session& NewDefaultSession) noexcept;
	Session& GetDefaultSession() noexcept;
	Instance NewInstance(const std::wstring& ClassName) noexcept;
	Instance NewInstance(const std::wstring& Namespace, const std::wstring& ClassName) noexcept;
	Instance NewInstance(Class& SourceClass);	// this will make a client-side-only instance; add an overload that accepts properties
	std::list<Instance> GetInstance(std::wstring ClassName) noexcept;
	std::list<Instance> GetInstance(std::wstring Namespace, std::wstring ClassName) noexcept;
	std::list<Instance> GetInstance(std::wstring ClassName, std::wstring Query) noexcept;
	std::list<Instance> GetInstance(std::wstring Namespace, std::wstring ClassName, std::wstring Query) noexcept;
	const bool RemoveInstance(Instance& Instance);
	const bool RemoveInstance(const std::wstring& Query);
	const bool RemoveInstance(Session& Session, const std::wstring& Query);
	std::list<Class> ListClasses(const std::wstring& Namespace = GetDefaultNamespace());
	std::list<Class> ListClasses(const Session& Session, const std::wstring& Namespace = GetDefaultNamespace());

	// TODO: ListClasses with parameter pack
	Class GetClass(const std::wstring& Name);
	Class GetClass(const std::wstring& Namespace, const std::wstring& Name);
	// TODO: GetClass with parameter pack
	const bool InvokeMethod(Class& Class, const std::wstring& MethodName, std::map<std::wstring, void*> Arguments);
	const bool InvokeMethod(Instance& Instance, const std::wstring& MethodName, std::map<std::wstring, void*> Arguments);
	const bool InvokeMethod(Session& Session, Class& Class, const std::wstring& MethodName, std::map<std::wstring, void*> Arguments);
	const bool InvokeMethod(Session& Session, Instance& Instance, const std::wstring& MethodName, std::map<std::wstring, void*> Arguments);
	// TODO: invoke parameter packs
	const bool SubscribeEvent(const std::wstring& Query);
	const bool SubscribeEvent(const std::wstring& Namespace, const std::wstring& Query);
	const bool SubscribeEvent(Session& Session, const std::wstring& Query);
	const bool SubscribeEvent(Session& Session, const std::wstring& Namespace, const std::wstring& Query);
	// TODO: subscribe parameter packs
	const bool TestConnection();
	const bool TestConnection(Session& Session);
	// TODO: test with callbacks
}

#endif CIMITAR_CIMITAR_H_INCLUDED
