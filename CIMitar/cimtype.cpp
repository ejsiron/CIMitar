#include "CIMitar.h"
#include <vector>

using namespace std;
using namespace CIMitar;

const wchar_t* CIMitar::CIMTypeName(CIMType Type) noexcept
{
	switch (Type)
	{
	case CIMType::BooleanA:		return L"BooleanArray";
	case CIMType::Char16:		return L"Char";
	case CIMType::Char16A:		return L"CharArray";
	case CIMType::Interval:		return L"Interval";
	case CIMType::IntervalA:	return L"IntervalArray";
	case CIMType::Real32:		return L"Real32";
	case CIMType::Real32A:		return L"Real32Array";
	case CIMType::Real64:		return L"Real64";
	case CIMType::Real64A:		return L"Real64Array";
	case CIMType::SInt8:			return L"SignedInteger8";
	case CIMType::SInt8A:		return L"SignedInteger8Array";
	case CIMType::SInt16:		return L"SignedInteger16";
	case CIMType::SInt16A:		return L"SignedInteger16Array";
	case CIMType::SInt32:		return L"SignedInteger32";
	case CIMType::SInt32A:		return L"SignedInteger32Array";
	case CIMType::SInt64:		return L"SignedInteger64";
	case CIMType::SInt64A:		return L"SignedInteger64Array";
	case CIMType::String:		return L"String";
	case CIMType::StringA:		return L"StringArray";
	case CIMType::Timestamp:	return L"Timestamp";
	case CIMType::TimestampA:	return L"TimestampArray";
	case CIMType::UInt8:			return L"UnsignedInteger8";
	case CIMType::UInt8A:		return L"UnsignedInteger8Array";
	case CIMType::UInt16:		return L"UnsignedInteger16";
	case CIMType::UInt16A:		return L"UnsignedInteger16Array";
	case CIMType::UInt32:		return L"UnsignedInteger32";
	case CIMType::UInt32A:		return L"UnsignedInteger32Array";
	case CIMType::UInt64:		return L"UnsignedInteger64";
	case CIMType::UInt64A:		return L"UnsignedInteger64Array";
	default:							return L"Boolean";
	}
}

static wstring FromMIString(const MI_Char* In) noexcept
{
	if (In)
	{
		return wstring{ In };
	}
	return wstring{};
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

CIMBooleanA::CIMBooleanA(const MI_BooleanA* Val) noexcept : CIMValue(CIMType::BooleanA)
{
	Value = FillCIMArray<bool, MI_BooleanA>(Val);
}

CIMChar16A::CIMChar16A(const MI_Char16A* Val) noexcept : CIMValue(CIMType::Char16A)
{
	Value = FillCIMArray<wchar_t, MI_Char16A>(Val);
}

CIMIntervalA::CIMIntervalA(const MI_DatetimeA* Val) noexcept : CIMValue(CIMType::IntervalA)
{
	Value = vector<MI_Interval>(Val->size);
	for (size_t i = 0; i != Val->size; i++)
	{
		Value[i] = Val->data[i].u.interval;
	}
}

CIMReal32A::CIMReal32A(const MI_Real32A* Val) noexcept : CIMValue(CIMType::Real32A)
{
	Value = FillCIMArray<float, MI_Real32A>(Val);
}

CIMReal64A::CIMReal64A(const MI_Real64A* Val) noexcept : CIMValue(CIMType::Real64A)
{
	Value = FillCIMArray<long double>(Val);
}

CIMSInt8A::CIMSInt8A(const MI_Sint8A* Val) noexcept :CIMValue(CIMType::SInt8A)
{
	Value = FillCIMArray<int>(Val);
}

CIMSInt16A::CIMSInt16A(const MI_Sint16A* Val) noexcept :CIMValue(CIMType::SInt16A)
{
	Value = FillCIMArray<int>(Val);
}

CIMSInt32A::CIMSInt32A(const MI_Sint32A* Val) noexcept : CIMValue(CIMType::SInt32A)
{
	Value = FillCIMArray<int>(Val);
}

CIMSInt64A::CIMSInt64A(const MI_Sint64A* Val) noexcept : CIMValue(CIMType::SInt64A)
{
	Value = FillCIMArray<long long>(Val);
}

CIMString::CIMString(const MI_Char* Val) noexcept : CIMValue(CIMType::String)
{
	Value = Val == nullptr ? wstring(L"") : wstring(Val);
}

CIMStringA::CIMStringA(const MI_StringA* Val) noexcept : CIMValue(CIMType::String)
{
	Value = FillCIMArray<wstring>(Val);
}

CIMTimestampA::CIMTimestampA(const MI_DatetimeA* Val) noexcept : CIMValue(CIMType::TimestampA)
{
	MI_Uint32 ValSize = Val == nullptr ? 0 : Val->size;
	Value = vector<MI_Timestamp>(ValSize);
	for (size_t i = 0; i != ValSize; i++)
	{
		Value[i] = Val->data[i].u.timestamp;
	}
}

CIMUInt8A::CIMUInt8A(const MI_Uint8A* Val) noexcept : CIMValue(CIMType::UInt8)
{
	Value = FillCIMArray<unsigned int>(Val);
}

CIMUInt16A::CIMUInt16A(const MI_Uint16A* Val) noexcept : CIMValue(CIMType::UInt16A)
{
	Value = FillCIMArray<unsigned int>(Val);
}

CIMUInt32A::CIMUInt32A(const MI_Uint32A* Val) noexcept : CIMValue(CIMType::UInt32A)
{
	Value = FillCIMArray<unsigned int>(Val);
}

CIMUInt64A::CIMUInt64A(const MI_Uint64A* Val) noexcept : CIMValue(CIMType::UInt64A)
{
	Value = FillCIMArray<unsigned long long>(Val);
}
