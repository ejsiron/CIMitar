#pragma once
#ifndef CIMITAR_VALUE_H_INCLUDED
#define CIMITAR_VALUE_H_INCLUDED

#include <string>
#include <vector>
#include <mi.h> // caution: earlier versions of mi.h did not have a header guard

namespace CIMitar
{
	const std::wstring Translate(const MI_Char* In) noexcept;
	const MI_Char* Translate(const std::wstring& In) noexcept;
}

#endif CIMITAR_VALUE_H_INCLUDED
