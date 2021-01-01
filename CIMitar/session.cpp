#include "CIMitar.h"
#include "appinit.h"
#include "localaddresses.h"
#include "operation.h"
#include "stringformatters.h"
#include <mutex>
#include <regex>
#include <shared_mutex>

#include <iostream>

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

void MI_Class_Deleter(MI_Class* DoomedClass)
{
	MI_Class_Delete(DoomedClass);
}

unique_ptr<MI_Class, void(*)(MI_Class*)> CloneClass(const MI_Class* SourceClass) noexcept
{
	MI_Class* pClonedClass;
	MI_Class_Clone(SourceClass, &pClonedClass);
	return unique_ptr<MI_Class, void(*)(MI_Class*)>(pClonedClass, MI_Class_Deleter);
}

vector<Class> Session::GetClassCore(const wstring& Namespace, const wstring ClassName, const unsigned int MaximumResults) noexcept
{
	std::vector<Class> Classes{};
	if (TheSession != nullptr)
	{
		ClassOpPack Op{ MaximumResults };
		MI_Session_GetClass(TheSession.get(), 0, NULL, Namespace.c_str(), ClassName.c_str(), NULL, &Op.operation);
		do
		{
			MI_Operation_GetClass(&Op.operation, &Op.pRetrievedClass, &Op.MoreResults, &Op.Result, &Op.pErrorMessage, &Op.pErrorDetails);
			Classes.emplace_back(Class(Op.pRetrievedClass));
		} while (Op.More());
	}
	return Classes;
}

std::vector<Instance> Session::GetInstanceCore(const std::wstring& Namespace, const std::wstring ClassName, const unsigned int MaximumResults) noexcept
{
	std::vector<Instance> Instances{};
	if (TheSession != nullptr)
	{
		InstanceOpPack Op{ MaximumResults };
		MI_Session_QueryInstances(TheSession.get(), 0, NULL, Namespace.c_str(), QueryLanguage, L"", NULL, &Op.operation);
		do
		{
			MI_Operation_GetInstance(&Op.operation, &Op.pRetrievedInstance, &Op.MoreResults, &Op.Result, &Op.pErrorMessage, &Op.pErrorDetails);
			Instances.emplace_back(Instance(Op.pRetrievedInstance));
		} while (Op.More());
	}
	return Instances;
}

Class Session::GetClass(const std::wstring& ClassName) noexcept { return GetClass(GetDefaultNamespace(), ClassName); }
Class Session::GetClass(const std::wstring& Namespace, const std::wstring& ClassName) noexcept
{
	std::vector<Class> RetrievedClasses{ GetClassCore(Namespace, ClassName, 1) };
	if (RetrievedClasses.empty())
	{
		return Class{ static_cast<const MI_Class*>(nullptr) };
	}
	else
	{
		return RetrievedClasses[0];
	}
}

Instance Session::NewInstance(const std::wstring& ClassName) noexcept
{
	return NewInstance(GetDefaultNamespace(), ClassName);
}

Instance Session::NewInstance(const std::wstring& Namespace, const std::wstring& ClassName) noexcept
{
	InstanceOpPack Op{};
	Class InstanceClass{ GetClass(ClassName) };
	MI_Instance* CreatedInstance;
	MI_Application_NewInstanceFromClass(&TheCimApplication, ClassName.c_str(), InstanceClass.cimclass.get(), &CreatedInstance);
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
