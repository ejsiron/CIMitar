#include <type_traits>
#include <iostream>
#include "CIMitar.h"
#include "value.h"

using namespace std;
using namespace CIMitar;

const MI_Type CIMTypeIDTranslator(const CIMTypes Type) noexcept
{
	switch (Type)
	{
	case CIMTypes::BooleanA:	return MI_BOOLEANA;
	case CIMTypes::Char16:		return MI_CHAR16;
	case CIMTypes::Char16A:		return MI_CHAR16A;
	case CIMTypes::DateTime:	return MI_DATETIME;
	case CIMTypes::DateTimeA:	return MI_DATETIMEA;
	case CIMTypes::Real32:		return MI_REAL32;
	case CIMTypes::Real32A:		return MI_REAL32A;
	case CIMTypes::Real64:		return MI_REAL64;
	case CIMTypes::Real64A:		return MI_REAL64A;
	case CIMTypes::SInt8:		return MI_SINT8;
	case CIMTypes::SInt8A:		return MI_SINT8A;
	case CIMTypes::SInt16:		return MI_SINT16;
	case CIMTypes::SInt16A:		return MI_SINT16A;
	case CIMTypes::SInt32:		return MI_SINT32;
	case CIMTypes::SInt32A:		return MI_SINT32A;
	case CIMTypes::SInt64:		return MI_SINT64;
	case CIMTypes::SInt64A:		return MI_SINT64A;
	case CIMTypes::String:		return MI_STRING;
	case CIMTypes::StringA:		return MI_STRINGA;
	case CIMTypes::UInt8:		return MI_UINT8;
	case CIMTypes::UInt8A:		return MI_UINT8A;
	case CIMTypes::UInt16:		return MI_UINT16;
	case CIMTypes::UInt16A:		return MI_UINT16A;
	case CIMTypes::UInt32:		return MI_UINT32;
	case CIMTypes::UInt32A:		return MI_UINT32A;
	case CIMTypes::UInt64:		return MI_UINT64;
	case CIMTypes::UInt64A:		return MI_UINT64A;
	default:							return MI_BOOLEAN;
	}
}

const CIMTypes CIMTypeIDTranslator(const MI_Type Type) noexcept
{
	switch (Type)
	{
	case MI_BOOLEANA:				return CIMTypes::BooleanA;
	case MI_CHAR16:				return CIMTypes::Char16;
	case MI_CHAR16A:				return CIMTypes::Char16A;
	case MI_DATETIME:				return CIMTypes::DateTime;
	case MI_DATETIMEA:			return CIMTypes::DateTimeA;
	case MI_REAL32:				return CIMTypes::Real32;
	case MI_REAL32A:				return CIMTypes::Real32A;
	case MI_REAL64:				return CIMTypes::Real64;
	case MI_REAL64A:				return CIMTypes::Real64A;
	case MI_SINT8:					return CIMTypes::SInt8;
	case MI_SINT8A:				return CIMTypes::SInt8A;
	case MI_SINT16:				return CIMTypes::SInt16;
	case MI_SINT16A:				return CIMTypes::SInt16A;
	case MI_SINT32:				return CIMTypes::SInt32;
	case MI_SINT32A:				return CIMTypes::SInt32A;
	case MI_SINT64:				return CIMTypes::SInt64;
	case MI_SINT64A:				return CIMTypes::SInt64A;
	case MI_STRING:				return CIMTypes::String;
	case MI_STRINGA:				return CIMTypes::StringA;
	case MI_UINT8:					return CIMTypes::UInt8;
	case MI_UINT8A:				return CIMTypes::UInt8A;
	case MI_UINT16:				return CIMTypes::UInt16;
	case MI_UINT16A:				return CIMTypes::UInt16A;
	case MI_UINT32:				return CIMTypes::UInt32;
	case MI_UINT32A:				return CIMTypes::UInt32A;
	case MI_UINT64:				return CIMTypes::UInt64;
	case MI_UINT64A:				return CIMTypes::UInt64A;
	default:							return CIMTypes::Boolean;
	}
}

const wstring CIMitar::Translate(const MI_Char* In) noexcept
{
	return In == nullptr ? wstring{} : wstring{ In };
}

const MI_Char* CIMitar::Translate(const wstring& In) noexcept
{
	return In.c_str();
}

template <typename T, typename U>
static auto FillCIMArray(const U* Val) noexcept
{
	MI_Uint32 OutSize{ Val == nullptr ? 0 : Val->size };
	vector<T> Out(OutSize);
	for (size_t i = 0; i != Val->size; i++)
		Out[i] = Val->data[i];
	return Out;
}
//
//CIMBooleanA::CIMBooleanA(const MI_BooleanA* Val) noexcept : CIMValue(CIMType::BooleanA)
//{
//	Value = FillCIMArray<bool, MI_BooleanA>(Val);
//}
//
//CIMChar16A::CIMChar16A(const MI_Char16A* Val) noexcept : CIMValue(CIMType::Char16A)
//{
//	Value = FillCIMArray<wchar_t, MI_Char16A>(Val);
//}
//
//CIMIntervalA::CIMIntervalA(const MI_DatetimeA* Val) noexcept : CIMValue(CIMType::IntervalA)
//{
//	Value = vector<MI_Interval>(Val->size);
//	for (size_t i = 0; i != Val->size; i++)
//	{
//		Value[i] = Val->data[i].u.interval;
//	}
//}
//
//CIMReal32A::CIMReal32A(const MI_Real32A* Val) noexcept : CIMValue(CIMType::Real32A)
//{
//	Value = FillCIMArray<float, MI_Real32A>(Val);
//}
//
//CIMReal64A::CIMReal64A(const MI_Real64A* Val) noexcept : CIMValue(CIMType::Real64A)
//{
//	Value = FillCIMArray<long double>(Val);
//}
//
//CIMSInt8A::CIMSInt8A(const MI_Sint8A* Val) noexcept :CIMValue(CIMType::SInt8A)
//{
//	Value = FillCIMArray<int>(Val);
//}
//
//CIMSInt16A::CIMSInt16A(const MI_Sint16A* Val) noexcept :CIMValue(CIMType::SInt16A)
//{
//	Value = FillCIMArray<int>(Val);
//}
//
//CIMSInt32A::CIMSInt32A(const MI_Sint32A* Val) noexcept : CIMValue(CIMType::SInt32A)
//{
//	Value = FillCIMArray<int>(Val);
//}
//
//CIMSInt64A::CIMSInt64A(const MI_Sint64A* Val) noexcept : CIMValue(CIMType::SInt64A)
//{
//	Value = FillCIMArray<long long>(Val);
//}
//
//CIMString::CIMString(const MI_Char* Val) noexcept : CIMValue(CIMType::String)
//{
//	Value = Val == nullptr ? wstring(L"") : wstring(Val);
//}
//
//CIMStringA::CIMStringA(const MI_StringA* Val) noexcept : CIMValue(CIMType::String)
//{
//	Value = FillCIMArray<wstring>(Val);
//}
//
//CIMTimestampA::CIMTimestampA(const MI_DatetimeA* Val) noexcept : CIMValue(CIMType::TimestampA)
//{
//	MI_Uint32 ValSize = Val == nullptr ? 0 : Val->size;
//	Value = vector<MI_Timestamp>(ValSize);
//	for (size_t i = 0; i != ValSize; i++)
//	{
//		Value[i] = Val->data[i].u.timestamp;
//	}
//}
//
//CIMUInt8A::CIMUInt8A(const MI_Uint8A* Val) noexcept : CIMValue(CIMType::UInt8)
//{
//	Value = FillCIMArray<unsigned int>(Val);
//}
//
//CIMUInt16A::CIMUInt16A(const MI_Uint16A* Val) noexcept : CIMValue(CIMType::UInt16A)
//{
//	Value = FillCIMArray<unsigned int>(Val);
//}
//
//CIMUInt32A::CIMUInt32A(const MI_Uint32A* Val) noexcept : CIMValue(CIMType::UInt32A)
//{
//	Value = FillCIMArray<unsigned int>(Val);
//}
//
//CIMUInt64A::CIMUInt64A(const MI_Uint64A* Val) noexcept : CIMValue(CIMType::UInt64A)
//{
//	Value = FillCIMArray<unsigned long long>(Val);
//}
