# CIMitar

C++ library to accelerate Common Information Model (CIM)/Windows Management Instrumentation (WMI) programming on Windows

## Library Goal

The CIMitar library seeks to approximate the ease of the [PowerShell CIM Cmdlets](https://docs.microsoft.com/en-us/powershell/module/cimcmdlets/?view=powershell-7.1) in C++. It also strives to expose *all* availability CIM client functionality. It does not work with the provider portions of the framework.

## Overview

Microsoft exposes two CIM/WMI interfaces for C++ programmers. The [original WMI interface](https://docs.microsoft.com/en-us/windows/win32/wmisdk/about-wmi) still functions and Microsoft maintains its documentation. Microsoft has created what it calls its "next-generation WMI", known simply as "MI".

WMI requires knowledge of Component Object Model (COM) programming. If you have that knowledge, you will find WMI relatively straightforward to use. Most of the examples for it were written for scripting languages, but sufficient documentation exists to work with it.