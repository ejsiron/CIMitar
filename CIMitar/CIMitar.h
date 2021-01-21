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
		std::wstring moreinformation{};
		std::vector <std::variant<std::wstring, unsigned int>> extendederrordata{};
		std::unique_ptr<ExtendedError> extendederror{ nullptr };
	public:
		Error(const unsigned int CIMStatusCode, const Activity::Codes ActivityCode, const std::wstring& MoreInformation = L"") noexcept;
		Error(const MI_Instance* ExtendedError, const Activity::Codes ActivityCode, const std::wstring& MoreInformation = L"") noexcept;
		Error(const Error&) noexcept;
		Error operator=(const Error) noexcept;
		void swap(Error&);
		const unsigned int CIMStatusCode() const noexcept;
		const std::wstring CIMStatusCodeDescription() const noexcept;
		const std::wstring Activity() const noexcept;
		const std::wstring MoreInformation() const noexcept;
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
		Interval() noexcept = default;
		Interval(const MI_Datetime& MIDatetime) noexcept;
		Interval(const MI_Interval& MIInterval) noexcept;
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
		Timestamp() noexcept = default;
		Timestamp(const MI_Datetime& MIDatetime) noexcept;
		Timestamp(const MI_Timestamp& MITimestamp) noexcept;
		const MI_Timestamp ToMITimestamp() const noexcept;
		const MI_Interval ToMIInterval() const noexcept;
	};

	const bool operator==(const MI_Timestamp& lhs, const MI_Timestamp& rhs) noexcept;
	const bool operator==(const Timestamp& lhs, const Timestamp& rhs) noexcept;

	tm TMFromCIMTime(const MI_Timestamp& Timestamp) noexcept;
	MI_Timestamp CIMTimeFromTM(const tm& TM) noexcept;

	using DateTime = std::variant <Interval, Timestamp>;

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

	//forward declaration for use in class Instance
	class PropertyDeclaration;

	class Instance
	{
	private:
		std::unique_ptr<MI_Instance> ciminstance{ nullptr };
		static Instance Clone(const MI_Instance& SourceInstance) noexcept;
		friend class Session;
	public:
		Instance(const MI_Instance*) noexcept;
		Instance(const Instance&) noexcept;
		Instance operator=(const Instance&) noexcept;
		void swap(Instance& CopySource) noexcept;
		virtual ~Instance();
		static Instance Empty() noexcept;
		const std::wstring ServerName() const noexcept;
		const std::wstring Namespace() const noexcept;
		unsigned int ElementCount() noexcept;
		std::list<PropertyDeclaration> Properties{};
		const bool Refresh() noexcept;
	};
	void swap(Instance& lhs, Instance& rhs) noexcept;

	// forward declaration for use in class Session
	class Class;

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

	enum class CIMTypes
	{
		Boolean,
		BooleanA,
		Char16,
		Char16A,
		DateTime,
		DateTimeA,
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
		UInt8,
		UInt8A,
		UInt16,
		UInt16A,
		UInt32,
		UInt32A,
		UInt64,
		UInt64A
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

	class Value
	{
	private:
		std::variant<
			wchar_t,
			unsigned int,
			int,
			unsigned long long,
			long long,
			float,
			double,
			CIMitar::Instance,
			CIMitar::Interval,
			CIMitar::Timestamp,
			std::wstring,
			std::vector<bool>,
			std::vector<wchar_t>,
			std::vector<unsigned int>,
			std::vector<int>,
			std::vector<unsigned long long>,
			std::vector<long long>,
			std::vector<float>,
			std::vector<double>,
			std::vector<CIMitar::Interval>,
			std::vector<CIMitar::Timestamp>,
			std::vector<std::wstring>,
			std::vector<CIMitar::Instance>
		>
			cimvalue;
		CIMTypes cimtype;
		bool isarray{ false };
	public:
		Value(MI_Value& Val, const MI_Type Type) noexcept;
		const bool IsArray() const noexcept;
		const bool Boolean() const noexcept;
		const std::vector<bool> BooleanA() const noexcept;
		const wchar_t Char16() const noexcept;
		const std::vector<wchar_t> Char16A() const noexcept;
		const DateTime DateTime() const noexcept;
		const std::vector<CIMitar::DateTime> DateTimeA() const noexcept;
		const Instance Instance() const noexcept;
		const std::vector<CIMitar::Instance> InstanceA() const noexcept;
		const float Real32() const noexcept;
		const std::vector<float> Real32A() const noexcept;
		const double Real64() const noexcept;
		const std::vector<double> Real64A() const noexcept;
		const std::wstring String() const noexcept;
		const std::vector<std::wstring> StringA() const noexcept;
		const int SignedInt() const noexcept;
		const std::vector<int> SignedIntA() const noexcept;
		const long SignedInt64() const noexcept;
		const std::vector<long> SignedInt64A() const noexcept;
		const unsigned int UnsignedInt() const noexcept;
		const std::vector<unsigned int> UnsignedIntA() const noexcept;
		const unsigned long UnsignedInt64() const noexcept;
		const std::vector<unsigned long> UnsignedInt64A() const noexcept;
	};
}

#endif CIMITAR_CIMITAR_H_INCLUDED
