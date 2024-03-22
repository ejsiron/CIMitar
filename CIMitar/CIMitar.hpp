#pragma once
#ifndef CIMITAR_CIMITAR_H_INCLUDED
#define CIMITAR_CIMITAR_H_INCLUDED
#ifndef __cplusplus
#error CIMitar is a C++ wrapper and only works with C++.
#endif
#ifdef _MSC_VER
#if _MSVC_LANG < 202002L
#error CIMitar uses C++20 features and requires a capable compiler
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
		std::vector <std::variant<std::wstring, unsigned int>> extendederrordata{};	// TODO: nothing pulls this out yet
		std::wstring query{ std::wstring{} };	// TODO: this does not yet populate
		std::unique_ptr<ExtendedError> extendederror{ nullptr };
	public:
		Error(const unsigned int CIMStatusCode, const Activity::Codes ActivityCode, const std::wstring& MoreInformation = L"") noexcept;
		Error(MI_Session* Session, const MI_Instance* ExtendedError, const Activity::Codes ActivityCode, const std::wstring& MoreInformation = L"") noexcept;
		Error(const Error&) noexcept;
		Error operator=(const Error) noexcept;
		void swap(Error&) noexcept;
		const unsigned int CIMStatusCode() const noexcept;
		const std::wstring CIMStatusCodeDescription() const noexcept;
		const std::wstring Activity() const noexcept;
		const std::wstring Query() const noexcept;
		const unsigned int ErrorType() const noexcept;
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
		std::wstring MoreInformation{};
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

	class ManagementType
	{
	public:
		virtual MI_Type GetType() = 0;
		virtual ~ManagementType() {}
	};

	class Timestamp; // forward declaration for Interval conversion

	class Interval :public ManagementType
	{
	public:
		unsigned int Days{ 0 };
		unsigned int Hours{ 0 };
		unsigned int Minutes{ 0 };
		unsigned int Seconds{ 0 };
		unsigned int Microseconds{ 0 };
		Interval() noexcept = default;
		Interval(const Timestamp& timestamp) noexcept;
		Interval(const MI_Datetime& MIDatetime) noexcept;
		Interval(const MI_Interval& MIInterval) noexcept;
		const MI_Interval ToMIInterval() const noexcept;
		const MI_Timestamp ToMITimestamp() const noexcept;
		MI_Type GetType() override { return MI_Type::MI_DATETIME; }
	};

	const bool operator==(const MI_Interval& lhs, const MI_Interval& rhs) noexcept;
	const bool operator==(const Interval& lhs, const Interval& rhs) noexcept;

	class Timestamp :public ManagementType
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
		Timestamp(const Interval&) noexcept;
		Timestamp(const MI_Datetime& MIDatetime) noexcept;
		Timestamp(const MI_Timestamp& MITimestamp) noexcept;
		const MI_Timestamp ToMITimestamp() const noexcept;
		const MI_Interval ToMIInterval() const noexcept;
		MI_Type GetType() override { return MI_Type::MI_DATETIME; }
	};

	const bool operator==(const MI_Timestamp& lhs, const MI_Timestamp& rhs) noexcept;
	const bool operator==(const Timestamp& lhs, const Timestamp& rhs) noexcept;

	tm TMFromCIMTime(const MI_Timestamp& Timestamp) noexcept;
	MI_Timestamp CIMTimeFromTM(const tm& TM) noexcept;

	using DateTime = std::variant <Interval, Timestamp>;

#pragma region Authentication and Credentials
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

	class UserCredentials final
	{
	private:
		std::wstring domain;
		std::wstring username;
		std::vector<char> secret;
		AuthenticationTypes authenticationtype{ AuthenticationTypes::DEFAULT };
	public:
		UserCredentials(const MI_UsernamePasswordCreds* Credentials) noexcept;
		UserCredentials(const std::wstring Username, wchar_t* ClearTextPassword, const bool ClearSource = false) noexcept;
		UserCredentials(const std::wstring Domain, const std::wstring Username, wchar_t* ClearTextPassword, const bool ClearSource = false) noexcept;
		UserCredentials(const UserCredentials& copysource) noexcept;
		UserCredentials& operator=(const UserCredentials& copysource) noexcept;
		friend const bool operator==(UserCredentials& lhs, UserCredentials& rhs) noexcept;
		void GetCredential(MI_UserCredentials* OutCredential) noexcept;
		~UserCredentials() = default;
		bool AddDestinationCredentials(MI_DestinationOptions* DestinationOptions) noexcept;
		bool AddProxyCredentials(MI_DestinationOptions* DestinationOptions) noexcept;
		bool AddDeliveryCredentials(MI_SubscriptionDeliveryOptions* DeliveryOptions) noexcept;
	};

	// only compares domain & username to reduce password leaks
	const bool operator==(UserCredentials& lhs, UserCredentials& rhs) noexcept;
	const bool operator!=(UserCredentials& lhs, UserCredentials& rhs) noexcept;
#pragma endregion

	enum class SessionProtocols { DCOM, WSMAN };
	enum class SessionPrefixOverrides { HTTP, HTTPS };
	enum class SessionPacketEncodingOptions { DEFAULT, UTF8, UTF16 };
	enum class SessionProxyTypes { AUTO, NONE, IE, WINHTTP };
	enum class SessionErrorModes { NOTIFY, WAIT };

	class OptionPackBase
	{
	public:
		OptionPackBase() = default;
		OptionPackBase(const OptionPackBase&) = default;
		OptionPackBase& operator=(const OptionPackBase&) = default;
		OptionPackBase(OptionPackBase&&) = default;
		OptionPackBase& operator=(OptionPackBase&&) = default;
		virtual ~OptionPackBase() = default;
		virtual void SetCustomOption(const std::wstring& OptionName, const int CustomValue) = 0;
		virtual void SetCustomOption(const std::wstring& OptionName, const std::wstring CustomValue) = 0;
		virtual void RemoveCustomStringOption(const std::wstring& OptionName) = 0;
		virtual void RemoveCustomNumberOption(const std::wstring& OptionName) = 0;
		virtual void Reset() = 0;
	};

	class SessionOptions: public OptionPackBase
	{
	private:
		std::unique_ptr<MI_DestinationOptions> DestinationOptions{ nullptr };
		std::map <std::wstring, unsigned int> CustomNumberOptions{};
		std::map <std::wstring, std::wstring> CustomStringOptions{};
		friend class Session;
	public:
		SessionOptions();
		SessionOptions(const SessionOptions&) = delete;
		SessionOptions& operator=(const SessionOptions&) = delete;
		SessionOptions(SessionOptions&&) noexcept = default;
		SessionOptions& operator=(SessionOptions&&) noexcept = default;
		virtual ~SessionOptions();
		void SetCheckCACert(bool Check);
		void SetCheckCertCN(bool Check);
		void SetCheckCertRevocation(bool Check);
		void SetConnectionPort(unsigned int Port);
		void SetEncodePortInSPN(bool Encode);
		void SetHttpUrlPrefix(SessionPrefixOverrides Prefix);
		void SetImpersonationType(MI_DestinationOptions_ImpersonationType Impersonation);
		void SetMaxPacketSize(unsigned int Size);
		void SetPacketEncoding(SessionPacketEncodingOptions Encoding);
		void SetPacketIntegrity(bool Integrity);
		void SetPacketPrivacy(bool Privacy);
		void SetSessionProxyType(SessionProxyTypes ProxyType);
		void SetSessionErrorMode(SessionErrorModes ErrorMode);
		void SetTimeout(Interval Timeout);
		void SetDataLocale(std::wstring Locale);
		void SetUILocale(std::wstring Locale);
		void SetSessionProtocol(SessionProtocols Protocol);
		void SetUseHTTPS(bool Use);
		void AddTargetCredentials(const UserCredentials& Credentials) noexcept;
		void AddTargetCredentials(const MI_UsernamePasswordCreds* Credentials) noexcept;
		void AddProxyCredentials(const UserCredentials& Credentials) noexcept;
		void AddProxyCredentials(const MI_UsernamePasswordCreds* Credentials) noexcept;
		virtual void SetCustomOption(const std::wstring& OptionName, const int CustomValue) override;
		virtual void SetCustomOption(const std::wstring& OptionName, const std::wstring CustomValue) override;
		virtual void RemoveCustomStringOption(const std::wstring& OptionName) override;
		virtual void RemoveCustomNumberOption(const std::wstring& OptionName) override;
		virtual void Reset() override;
	};

	//forward declarations for use in Class and Instance
	class Property;
	class PropertyDeclaration;

	class Class
	{
	private:
		std::unique_ptr<MI_Class> cimclass{ nullptr };
		friend class Session;
	public:
		Class(const MI_Class* SourceClass) noexcept;
		void swap(Class&) noexcept;
		Class(const Class&) noexcept;
		Class& operator=(Class) noexcept;
		~Class();
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

	class Instance
	{
	private:
		std::unique_ptr<MI_Instance, void(*)(MI_Instance*)> ciminstance{ nullptr, nullptr };
		static Instance Clone(const MI_Instance& SourceInstance, MI_Session* Owner) noexcept;
		std::list<Property> properties{};
		MI_Session* owner{ nullptr };	// TODO: convert to weak_ptr
		std::unique_ptr<Class> cimclass{ nullptr };
		friend class Session;
	public:
		Instance(const MI_Instance*, MI_Session* Owner = nullptr) noexcept;
		Instance(const Instance&) noexcept;
		Instance operator=(const Instance&) noexcept;
		void swap(Instance& CopySource) noexcept;
		static Instance Empty() noexcept;
		const std::wstring ServerName() const noexcept;
		const std::wstring Namespace() const noexcept;
		const Class& CimClass() noexcept;
		const unsigned int ElementCount() noexcept;
		const std::list<Property> Properties() noexcept;
		const bool Refresh() noexcept;
	};
	void swap(Instance& lhs, Instance& rhs) noexcept;

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
		//SessionOptions Options{};
		const ErrorStack LastErrors() noexcept;
		const bool Connect();
		const bool Connect(const SessionProtocols Protocol);
		const bool Close();
		const bool TestConnection() noexcept;
		Class GetClass(const std::wstring& Name) noexcept;
		Class GetClass(const std::wstring& ClassName, const std::wstring& Name) noexcept;
		std::list<Class> GetClasses(const std::wstring& Namespace, const bool NameOnly) noexcept;
		std::list<Class> GetClasses(const std::wstring& Namespace, const std::wstring& SourceClassName, const bool NameOnly) noexcept;
		std::list<Class> GetClasses(const Class& SourceClass, const bool NameOnly = false) noexcept;
		std::list<Instance> GetInstances(const std::wstring& ClassName, const bool KeysOnly = false) noexcept;
		std::list<Instance> GetInstances(const std::wstring& Namespace, const std::wstring& ClassName, const bool KeysOnly = false) noexcept;
		std::list<Instance> GetInstances(const Class& SourceClass, const bool KeysOnly = false) noexcept;
		Instance NewInstance(const std::wstring& ClassName) noexcept;
		Instance NewInstance(const std::wstring& Namespace, const std::wstring& ClassName) noexcept;
		Instance NewInstance(const Instance& SourceInstance) noexcept;
		std::list<Instance> QueryInstances(const std::wstring& Query);
		std::list<Instance> QueryInstances(const std::wstring& Namespace, const std::wstring& Query);
		std::list<Instance> GetAssociatedInstances(const Instance& SourceInstance, const bool KeysOnly = false);
		std::list<Instance> GetAssociatedInstances(const Instance& SourceInstance, const std::wstring& ResultClass, const bool KeysOnly = false);
		std::list<Instance> GetAssociatedInstances(const Instance& SourceInstance, const std::wstring& AssociationClass, const std::wstring& ResultClass, const std::wstring& Role, const std::wstring& ResultRole, const bool KeysOnly = false);

		friend Session NewSession();
		friend Session NewSession(const std::wstring ComputerName);
		//friend Session NewSession(const SessionOptions& Options);
		//friend Session NewSession(const std::wstring ComputerName, const SessionOptions& Options);
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

	/*class OperationOptions
	{
	private:
		std::set<const void*> overriddenoptions;
	public:
		NativeOption<bool> EnableWarningMessageChannel{ overriddenoptions };
		NativeOption<bool> EnableVerboseMessageChannel{ overriddenoptions };
		NativeOption<bool> EnableDebugMessageChannel{ overriddenoptions };
		NativeOption<bool> PromptUserMode{ overriddenoptions };
		NativeOption<std::pair<bool, bool>> PromptUserRegularMode{ overriddenoptions };
		NativeOption<bool> PromptClientOnError{ overriddenoptions };
		NativeOption<std::pair<bool, bool>> Force32Bit{ overriddenoptions };
		NativeOption<std::wstring> ResourceURI{ overriddenoptions };
		NativeOption<std::wstring> ResourceURIPrefix{ overriddenoptions };
		NativeOption<Interval> Timeout{ overriddenoptions };
		NativeOption<bool> ProvideMachineID{ overriddenoptions };
	};
	*/
	enum class CIMTypes
	{
		Boolean,
		BooleanA,
		Char16,
		Char16A,
		DateTime,
		DateTimeA,
		Instance,
		InstanceA,
		Real32,
		Real32A,
		Real64,
		Real64A,
		Reference,
		ReferenceA,
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

	Session NewSession();
	Session NewSession(const std::wstring ComputerName);
	//Session NewSession(const SessionOptions& Options);
	//Session NewSession(const std::wstring ComputerName, const SessionOptions& Options);

	using cimvaluevariant = std::variant<
		int,
		unsigned int,
		long long,
		unsigned long long,
		float,
		double,
		wchar_t,
		std::wstring,
		CIMitar::Instance,
		CIMitar::Interval,
		CIMitar::Timestamp,
		std::vector<int>,
		std::vector<unsigned int>,
		std::vector<long long>,
		std::vector<unsigned long long>,
		std::vector<float>,
		std::vector<double>,
		std::vector<wchar_t>,
		std::vector<std::wstring>,
		std::vector<CIMitar::Instance>,
		std::vector<CIMitar::Interval>,
		std::vector<CIMitar::Timestamp>
	> ;

	class Value
	{
	private:
		cimvaluevariant cimvalue;
		CIMTypes cimtype;
		bool isarray{ false };
		bool isempty{ true };
	public:
		Value(MI_Value& Val, const MI_Type Type, const bool Empty) noexcept;
		const bool IsArray() const noexcept;
		const bool IsEmpty() const noexcept;
		const bool Boolean() const noexcept;
		const std::vector<bool> BooleanA() const noexcept;
		const wchar_t Char16() const noexcept;
		const std::vector<wchar_t> Char16A() const noexcept;
		const Interval Interval() const noexcept;
		const std::vector<CIMitar::Interval> IntervalA() const noexcept;
		const Timestamp Timestamp() const noexcept;
		const std::vector<CIMitar::Timestamp> TimestampA() const noexcept;
		const Instance Instance() const noexcept;
		const std::vector<CIMitar::Instance> InstanceA() const noexcept;
		const float Real32() const noexcept;
		const std::vector<float> Real32A() const noexcept;
		const double Real64() const noexcept;
		const std::vector<double> Real64A() const noexcept;
		const std::wstring String() const;
		const std::vector<std::wstring> StringA() const noexcept;
		const int SignedInt() const noexcept;
		const std::vector<int> SignedIntA() const noexcept;
		const long long SignedInt64() const noexcept;
		const std::vector<long long> SignedInt64A() const noexcept;
		const unsigned int UnsignedInt() const noexcept;
		const std::vector<unsigned int> UnsignedIntA() const noexcept;
		const unsigned long long UnsignedInt64() const noexcept;
		const std::vector<unsigned long long> UnsignedInt64A() const noexcept;
	};

	class Property
	{
	private:
		std::wstring name{};
		Value cimvalue;
		CIMTypes cimtype{ CIMTypes::Boolean };
		unsigned int memoryflag{ 0 };
	public:
		Property(const MI_Char* Name, MI_Value& Value, MI_Type Type, MI_Uint32 MemoryFlag) noexcept;
		const std::wstring& Name() const noexcept;
		const bool IsNull() const noexcept;
		const bool IsArray() const noexcept;
		const Value& GetValue() const noexcept;
		const CIMTypes GetType() const noexcept;
	};

	/* Stand-alone functions that work from the default session */
	void SetDefaultNamespace(const std::wstring& Namespace);
	const std::wstring& GetDefaultNamespace();
	inline void ResetNamespace() { SetDefaultNamespace(DefaultCIMNamespace); }
	void SetDefaultSession(Session& NewDefaultSession) noexcept;
	Session& GetDefaultSession() noexcept;
	Instance NewInstance(const std::wstring& ClassName) noexcept;
	Instance NewInstance(const std::wstring& Namespace, const std::wstring& ClassName) noexcept;
	Instance NewInstance(Class& SourceClass);	// TODO: this will make a client-side-only instance; add an overload that accepts properties
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
