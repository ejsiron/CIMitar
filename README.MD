# CIMitar

C++ library to accelerate Common Information Model (CIM)/Windows Management Instrumentation (WMI) programming on Windows

## Library Goal

The CIMitar library seeks to approximate the ease of the [PowerShell CIM Cmdlets](https://docs.microsoft.com/en-us/powershell/module/cimcmdlets/?view=powershell-7.1) in C++. It also strives to expose *all* availability CIM client functionality. It does not work with the provider portions of the framework.

## CIM and Microsoft Implementation Overview

The [Distributed Management Task Force](https://www.dmtf.org/) owns and standardizes [CIM](https://www.dmtf.org/standards/cim). Vendors can implement providers as they see fit, but interfaces must follow the standard.

Microsoft exposes two CIM/WMI interfaces for C++ programmers. The [original WMI interface](https://docs.microsoft.com/en-us/windows/win32/wmisdk/about-wmi) still functions and Microsoft maintains its documentation. Microsoft has created what it calls its "next-generation WMI", known simply as "MI".

WMI requires knowledge of Component Object Model (COM) programming. If you have that knowledge, you will find WMI relatively straightforward to use. Most of the examples for it were written for scripting languages, but sufficient documentation exists to work with it. It works with components available in all Windows installations.

MI has a C interface. It does not require COM and exposes more CIM capabilities, but also needs more overall work. In unscientific testing, it appears faster. It requires an installation of the Windows Management Framework. Currently-supported versions of Windows and Windows Server have a functional installation by default. Some versions have a more recent downloadable version available.

The .Net framework exposes both techniques. The [System.Management](https://docs.microsoft.com/en-us/dotnet/api/system.management?view=netframework-4.8) namespace bears a great deal of similarity to WMI. It also has some bugs and performance problems. The [Microsoft.Management.Infrastructure](https://docs.microsoft.com/en-us/dotnet/api/microsoft.management.infrastructure?view=powershellsdk-7.0.0) namespace follows MI more closely. However, an examination of the PowerShell source code shows that perhaps the namespaces are merging.

Of interest, Microsoft's [OMI project](https://github.com/microsoft/omi), which implements MI on Linux, follows the Windows MI implementation very closely.

## Library Status

The CIMitar library is in early development condition. It has a great deal of unimplemented infrastructure.

### Working Components

The following functions have at least partial implementations:

* Remote connectivity
* Instance enumeration
* CIM-to-native type translation
* Class retrieval

### Non-working components

In general, anything not listed above does not work. However, some portions have sufficient scaffolding in the header to appear to function, but do not. This includes:

* Non-default connectivity options
* Username and password functionality
* Non-default operational options
* Queries and filtered item retrieval
* Error handling,tracking, and reporting

## Future Direction

At this time, the primary goal of the library maintainer is to complete all remaining client functionality.

Most importantly, be aware that **future versions of CIMitar will almost certainly include breaking changes**. No effort is being made to maintain any level of backward compatibility. In particular, the removal of the requirement to expose library consumers to mi.h remains a long-term priority.