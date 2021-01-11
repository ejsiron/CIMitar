#include <map>
#include <type_traits>
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

template <typename cpptype, typename MIType>
vector<cpptype> VectorizeMIArray(MIType* SourceArray, size_t length) noexcept
{
	vector<cpptype> OutVector(length);
	try
	{
		for (auto i{ 0 }; i < length; ++i)
		{
			OutVector.emplace_back(SourceArray[i]);
		}
	}
	catch (...) {}
	return OutVector;
}

Value::Value(MI_Value& Val, const MI_Type Type) noexcept
{
	cimtype = CIMTypeIDTranslator(Type);
	auto length{ Type & MI_ARRAY ? Val.array.size : 0 };
	// This portion could throw if provided a malformed Val.
	// All Vals should come from an MI provider
	switch (Type)
	{
	case MI_BOOLEANA:	value = VectorizeMIArray<bool>(Val.booleana.data, length); break;
	case MI_CHAR16:	value = Val.char16; break;
	case MI_CHAR16A:	value = VectorizeMIArray<wchar_t>(Val.char16a.data, length); break;
	case MI_DATETIME:
		if (Val.datetime.isTimestamp)
		{
			value = Timestamp(&Val.datetime.u.timestamp);
		}
		else
		{
			value = Interval(&Val.datetime.u.interval);
		}
		break;
	case MI_DATETIMEA:
		if (length > 0 && Val.datetimea.data->isTimestamp)
		{
			VectorizeMIArray <Timestamp>(Val.datetimea.data, length);
		}
		else
		{
			VectorizeMIArray<Interval>(Val.datetimea.data, length);
		}
		break;
	case MI_INSTANCE:	value = CIMitar::Instance(Val.instance); break;
	case MI_INSTANCEA:value = VectorizeMIArray<CIMitar::Instance>(&Val.instancea, length); break;
	case MI_REAL32:	value = Val.real32;	break;
	case MI_REAL32A:	value = VectorizeMIArray<float>(Val.real32a.data, length); break;
	case MI_REAL64:	value = Val.real64;	break;
	case MI_REAL64A:	value = VectorizeMIArray<double>(Val.real64a.data, length); break;
	case MI_REFERENCE:value = CIMitar::Instance(Val.reference);	break;
	case MI_REFERENCEA:	value = VectorizeMIArray<CIMitar::Instance>(&Val.referencea, length); break;
	case MI_SINT8:		value = Val.sint8;	break;
	case MI_SINT8A:	value = VectorizeMIArray<int>(Val.sint8a.data, length); break;
	case MI_SINT16:	value = Val.sint16;	break;
	case MI_SINT16A:	value = VectorizeMIArray<int>(Val.sint16a.data, length); break;
	case MI_SINT32:	value = Val.sint32;	break;
	case MI_SINT32A:	value = VectorizeMIArray<int>(Val.sint32a.data, length); break;
	case MI_SINT64:	value = Val.sint64;	break;
	case MI_SINT64A:	value = VectorizeMIArray<long long>(Val.sint64a.data, length); break;
	case MI_STRING:	value = Val.string;	break;
	case MI_STRINGA:	value = VectorizeMIArray<wstring>(Val.stringa.data, length); break;
	case MI_UINT8:		value = Val.uint8;	break;
	case MI_UINT8A:	value = VectorizeMIArray<unsigned int>(Val.uint8a.data, length); break;
	case MI_UINT16:	value = Val.uint16;	break;
	case MI_UINT16A:	value = VectorizeMIArray<unsigned int>(Val.uint16a.data, length); break;
	case MI_UINT32:	value = Val.uint32;	break;
	case MI_UINT32A:	value = VectorizeMIArray<unsigned int>(Val.uint32a.data, length); break;
	case MI_UINT64:	value = Val.uint64;	break;
	case MI_UINT64A:	value = VectorizeMIArray<unsigned long long>(Val.uint64a.data, length); break;
	default:				value = Val.boolean;	break;
	}
}

template <typename T>
class BoolVisitor
{
	const bool operator()(T& t, const bool isarray)
	{
		if (isarray)
		{
			return t[0];
		}
		else
		{
			return t;
		}
	}
};

const bool Value::Boolean() const noexcept
{
	auto y = 14;
	auto n = BoolVisitor(y, false);
}
