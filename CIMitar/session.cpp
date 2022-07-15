#include "CIMitar.hpp"
#include "appinit.hpp"
#include "localaddresses.hpp"
#include "operation.hpp"
#include "stringformatters.hpp"
#include <mutex>
#include <regex>
#include <shared_mutex>

using namespace std;
using namespace CIMitar;
using namespace CIMitar::Formatters;
using namespace CIMitar::Infrastructure;
using namespace std::regex_constants;

constexpr wchar_t QueryLanguage[]{ L"WQL" };
static MI_Application TheCimApplication = MI_APPLICATION_NULL;
static map<MI_Session*, Session*> Sessions{};
static mutex SessionListMutex{};
static shared_mutex ApplicationMutex{};

#pragma region Convenience Defaults
Session* DefaultSession;
Session Session::NullSession = Session();
void CIMitar::SetDefaultSession(Session& NewDefaultSession) noexcept
{
	DefaultSession = &NewDefaultSession;
}

Session& CIMitar::GetDefaultSession() noexcept
{
	if (DefaultSession == nullptr)
	{
		return Session::NullSession;
	}
	else
	{
		return *DefaultSession;
	}
}

static std::wstring DefaultNamespace{ DefaultCIMNamespace };

void CIMitar::SetDefaultNamespace(const std::wstring& Namespace)
{
	DefaultNamespace = Namespace;
}

const std::wstring& CIMitar::GetDefaultNamespace()
{
	return DefaultNamespace;
}
#pragma endregion Convenience Defaults

void Session::swap(Session& SwapSource) noexcept
{
	TheSession.swap(SwapSource.TheSession);
	ComputerName.swap(SwapSource.ComputerName);
}

void CIMitar::swap(Session& lhs, Session& rhs) noexcept
{
	lhs.swap(rhs);
}

void SessionDeleter(MI_Session* DoomedSession)
{
	lock_guard ApplicationAccessGuard(ApplicationMutex);
	if (DoomedSession->ft)
	{
		MI_Session_Close(DoomedSession, NULL, NULL);
	}
	Sessions.erase(DoomedSession);
	if (Sessions.empty())
	{
		MI_Application_Close(&TheCimApplication);	// TODO: error-checking? nothing can be done
	}
}

Session::Session()
{
	TheSession = shared_ptr<MI_Session>(new MI_Session, SessionDeleter);
	*TheSession = MI_SESSION_NULL;
}

Session::Session(const Session& CopySource) noexcept
{
	TheSession = CopySource.TheSession;
	ComputerName = CopySource.ComputerName;
}

Session& Session::operator=(Session CopySource) noexcept
{
	if (this != &CopySource)
	{
		swap(CopySource);
	}
	return *this;
}

Session::~Session()
{
	if (TheSession.use_count() == 1 && DefaultSession == this)
	{
		DefaultSession = Sessions.empty() ? nullptr : Sessions[0];
	}
}

const ErrorStack Session::LastErrors() noexcept
{
	ErrorStack tmp{};
	tmp.swap(lasterrors);
	return tmp;
}

const bool Session::Connect(const SessionProtocols* Protocol)
{
	Close();
	const wchar_t* SelectedProtocol{ Protocol == nullptr ? nullptr : *Protocol == SessionProtocols::DCOM ? L"DCOM" : L"WINRM" };
	bool IsLocal{ true };
	if (!ComputerName.empty())
	{
		wstring LocalNamesFilter{ JoinString(L'|', GetLocalNamesAndIPs()) };
		LocalNamesFilter = regex_replace(LocalNamesFilter, wregex(L"\\."), L"\\.");
		IsLocal = regex_match(ComputerName, wregex(LocalNamesFilter, icase));
	}
	const wchar_t* TargetName{ ComputerName.empty() || IsLocal ? nullptr : ComputerName.c_str() };
	if (Options.overriddenoptions.size() || Options.HasCustomOptions())
	{
		MI_DestinationOptions DestinationOptions;
		MI_Result CreateDestinationOptions{ MI_Application_NewDestinationOptions(&TheCimApplication, &DestinationOptions) };
		if (CreateDestinationOptions == MI_RESULT_OK)
		{
			if (Options.CheckCACert.IsOverridden())
			{
				MI_DestinationOptions_SetCertCACheck(&DestinationOptions, Options.CheckCACert.get());
			}
		}
	}
	MI_Result Result{ MI_Application_NewSession(&TheCimApplication, SelectedProtocol, TargetName, NULL, NULL, NULL, TheSession.get()) };
	// TODO: error checking and reporting
	return Result == MI_RESULT_OK;
}

const bool Session::Connect()
{
	return Connect(nullptr);
}

const bool Session::Connect(const SessionProtocols Protocol)
{
	return Connect(&Protocol);
}

const bool Session::Close()
{
	// TODO: error checking
	MI_Session_Close(TheSession.get(), NULL, NULL);
	return true;
}

#pragma region Operators
const bool CIMitar::operator==(const Session& lhs, const Session& rhs) noexcept
{
	return false;
}

const bool CIMitar::operator!=(const Session& lhs, const Session& rhs) noexcept
{
	return !(lhs == rhs);
}
#pragma endregion

const bool Session::TestConnection() noexcept
{
	return Operation::TestConnection(TheSession.get());
}

Class Session::GetClass(const std::wstring& ClassName) noexcept { return GetClass(GetDefaultNamespace(), ClassName); }
Class Session::GetClass(const std::wstring& Namespace, const std::wstring& ClassName) noexcept
{
	list<Class> FoundClasses{ Operation::GetClass(TheSession.get(), Namespace, ClassName, nullptr, nullptr, nullptr) };
	if (FoundClasses.empty())
	{
		return Class{ static_cast<const MI_Class*>(nullptr) };
	}
	else
	{
		return FoundClasses.front();
	}
}

list<Class> Session::GetClasses(const std::wstring& Namespace, const bool NameOnly) noexcept
{
	return GetClasses(Namespace, L"", NameOnly);
}

list<Class> Session::GetClasses(const std::wstring& Namespace, const std::wstring& SourceClassName, const bool NameOnly) noexcept
{
	return Operation::EnumerateClasses(TheSession.get(), Namespace, SourceClassName, NameOnly, nullptr, nullptr, nullptr);
}

list<Class> Session::GetClasses(const Class& SourceClass, const bool NameOnly) noexcept
{
	return Operation::EnumerateClasses(TheSession.get(), SourceClass.Namespace(), SourceClass.Name(), NameOnly, nullptr, nullptr, nullptr);
}

list<Instance> Session::GetInstances(const wstring& ClassName, const bool KeysOnly) noexcept
{
	return GetInstances(GetDefaultNamespace(), ClassName, KeysOnly);
}

list<Instance> Session::GetInstances(const wstring& Namespace, const wstring& ClassName, const bool KeysOnly) noexcept
{
	return Operation::EnumerateInstances(TheSession.get(), Namespace.c_str(), ClassName.c_str(), KeysOnly, nullptr, nullptr, nullptr);
}
list<Instance> Session::GetInstances(const Class& SourceClass, const bool KeysOnly) noexcept
{
	return GetInstances(SourceClass.Namespace(), SourceClass.Name(), KeysOnly);
}

Instance Session::NewInstance(const std::wstring& ClassName) noexcept
{
	return NewInstance(GetDefaultNamespace(), ClassName);
}

Instance Session::NewInstance(const std::wstring& Namespace, const std::wstring& ClassName) noexcept
{
	Class InstanceClass{ GetClass(Namespace, ClassName) };
	MI_Instance* CreatedInstance;
	MI_Application_NewInstanceFromClass(&TheCimApplication, ClassName.c_str(), InstanceClass.cimclass.get(), &CreatedInstance);
	return Instance{ CreatedInstance, TheSession.get() };
}

Instance Session::NewInstance(const Instance& SourceInstance) noexcept
{
	return Operation::CreateInstance(TheSession.get(), SourceInstance.Namespace(), SourceInstance.ciminstance.get(), nullptr, nullptr, nullptr);
}

Session CIMitar::NewSession()
{
	return NewSession(L"");
}

Session CIMitar::NewSession(const std::wstring ComputerName)
{
	return NewSession(ComputerName, SessionOptions{});
}

Session CIMitar::NewSession(const SessionOptions& Options)
{
	return NewSession(L"", Options);
}

Session CIMitar::NewSession(const std::wstring ComputerName, const SessionOptions& Options)
{
	Session newsession{};
	lock_guard ApplicationGuard{ ApplicationMutex };
	if (Sessions.empty())
	{
		DefaultSession = &newsession;
		auto AppInitResult{ InitializeCIMitar() };
		if (AppInitResult.first == MI_Result::MI_RESULT_OK)
			TheCimApplication = AppInitResult.second;
		else
			TheCimApplication = MI_APPLICATION_NULL;	// TODO: this is not enough
	}
	newsession.ComputerName = ComputerName;
	Sessions.emplace(newsession.TheSession.get(), &newsession);
	return newsession;
}

std::list<CIMitar::Instance> Session::QueryInstances(const std::wstring& const Namespace, const std::wstring const& Query)
{
}
