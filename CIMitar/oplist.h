#pragma once

namespace CIMitar
{
	// provides error message context
	class CIMitarOps
	{
	public:
		enum class OperationCodes :const int
		{
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

		static const wchar_t* GetOperationMessage(OperationCodes OperationCode)
		{
			switch (OperationCode)
			{
			case OperationCodes::ApplicationInitialization:
				return L"Initializing application-to-management interface connection";
				break;
			case OperationCodes::LocalSystemInitialization:
				return L"Connecting to the local CIM interface";
				break;
			case OperationCodes::LoadingApplicationInstance:
				return L"Loading the local CIM interface";
				break;
			case OperationCodes::Connecting:
				return L"Connecting to the target computer";
				break;
			case OperationCodes::TestingConnection:
				return L"A test connection failed";
				break;
			case OperationCodes::Disconnecting:
				return L"Disconnecting from the target computer";
				break;
			case OperationCodes::VerifyingNamespace:
				return L"Verifying the existence of a namespace";
				break;
			case OperationCodes::RetrievingInstanceFromOp:
				return L"Retrieving an instance from a CIM operation";
				break;
			case OperationCodes::CompletingOperation:
				return L"Completing a CIM operation";
				break;
			case OperationCodes::LoadingCIMReferenceClass:
				return L"Loading reference copy of a CIM class";
				break;
			case OperationCodes::CloningCIMClass:
				return L"Cloning a CIM class";
				break;
			case OperationCodes::CloningCIMInstance:
				return L"Cloning a CIM instance";
				break;
			case OperationCodes::RetrievingElementValue:
				return L"Retrieving an element value";
				break;
			case OperationCodes::SettingElementValue:
				return L"Setting an element value";
				break;
			case OperationCodes::ApplicationShutdown:
				return L"Shutting down the application";
				break;
			case OperationCodes::ApplyingDestinationOption:
				return L"Applying a destination session option";
				break;
			default:
				return L"Unknown or unspecified operation";
				break;
			}
		}
	};
}
