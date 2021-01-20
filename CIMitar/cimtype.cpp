#include <algorithm>
#include <map>
#include <type_traits>
#include "CIMitar.h"
#include "value.h"

using namespace std;
using namespace CIMitar;

const bool Value::IsArray() const noexcept
{
	return isarray;
}

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

template <typename outtype, typename MIType>
vector<outtype> VectorizeMIArray(const MIType* SourceArray, size_t length) noexcept
{
	vector<outtype> OutVector{};
	OutVector.reserve(length);
	try
	{
		for (size_t i{ 0 }; i < length; ++i)
		{
			if constexpr (is_trivially_constructible_v<outtype, MIType>)
			{
				OutVector.emplace_back(SourceArray[i]);
			}
			else
				OutVector.emplace_back(outtype{ SourceArray[i] });
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
	case MI_BOOLEANA:	cimvalue = VectorizeMIArray<bool>(Val.booleana.data, length); break;
	case MI_CHAR16:	cimvalue = static_cast<wchar_t>(Val.char16); break;
	case MI_CHAR16A:	cimvalue = VectorizeMIArray<wchar_t>(Val.char16a.data, length); break;
	case MI_DATETIME:
		if (Val.datetime.isTimestamp)
		{
			cimvalue = CIMitar::Timestamp(Val.datetime.u.timestamp);
		}
		else
		{
			cimvalue = CIMitar::Interval(Val.datetime.u.interval);
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
	case MI_INSTANCE:	cimvalue = CIMitar::Instance(Val.instance); break;
	case MI_INSTANCEA:cimvalue = VectorizeMIArray<CIMitar::Instance>(Val.instancea.data, length); break;
	case MI_REAL32:	cimvalue = Val.real32;	break;
	case MI_REAL32A:	cimvalue = VectorizeMIArray<float>(Val.real32a.data, length); break;
	case MI_REAL64:	cimvalue = Val.real64;	break;
	case MI_REAL64A:	cimvalue = VectorizeMIArray<double>(Val.real64a.data, length); break;
	case MI_REFERENCE:cimvalue = CIMitar::Instance(Val.reference);	break;
	case MI_REFERENCEA:	cimvalue = VectorizeMIArray<CIMitar::Instance>(Val.referencea.data, length); break;
	case MI_SINT8:		cimvalue = Val.sint8;	break;
	case MI_SINT8A:	cimvalue = VectorizeMIArray<int>(Val.sint8a.data, length); break;
	case MI_SINT16:	cimvalue = Val.sint16;	break;
	case MI_SINT16A:	cimvalue = VectorizeMIArray<int>(Val.sint16a.data, length); break;
	case MI_SINT32:	cimvalue = Val.sint32;	break;
	case MI_SINT32A:	cimvalue = VectorizeMIArray<int>(Val.sint32a.data, length); break;
	case MI_SINT64:	cimvalue = Val.sint64;	break;
	case MI_SINT64A:	cimvalue = VectorizeMIArray<long long>(Val.sint64a.data, length); break;
	case MI_STRING:	cimvalue = Val.string;	break;
	case MI_STRINGA:	cimvalue = VectorizeMIArray<wstring>(Val.stringa.data, length); break;
	case MI_UINT8:		cimvalue = Val.uint8;	break;
	case MI_UINT8A:	cimvalue = VectorizeMIArray<unsigned int>(Val.uint8a.data, length); break;
	case MI_UINT16:	cimvalue = Val.uint16;	break;
	case MI_UINT16A:	cimvalue = VectorizeMIArray<unsigned int>(Val.uint16a.data, length); break;
	case MI_UINT32:	cimvalue = Val.uint32;	break;
	case MI_UINT32A:	cimvalue = VectorizeMIArray<unsigned int>(Val.uint32a.data, length); break;
	case MI_UINT64:	cimvalue = Val.uint64;	break;
	case MI_UINT64A:	cimvalue = VectorizeMIArray<unsigned long long>(Val.uint64a.data, length); break;
	default:				cimvalue = Val.boolean;	break;
	}
}

template <typename visitortype>
class BaseVisitor
{
private:
	virtual const visitortype DefaultValue() const noexcept = 0;
public:
	virtual const visitortype& operator()(const visitortype& value) const noexcept sealed { return value; }
	template <typename T>
	const visitortype operator()(const T&) { return DefaultValue(); }
	virtual ~BaseVisitor() = default;
};

class BoolVisitor : public BaseVisitor<bool>
{
private:
	const bool DefaultValue() const noexcept { return false; }
public:
	const bool operator()(const wchar_t ch) const noexcept { return !(ch == L'0' || ch == 0); }
	const bool operator()(const wstring& str) const noexcept
	{
		try
		{
			return str.size() > 0 && stold(str) != 0;
		}
		catch (...)
		{
			return false;
		}
	}
	const bool operator()(const vector<bool>& vec) const noexcept
	{
		return vec.size() && vec.at(0) == true ? true : false;
	}
	template <typename T>
	typename enable_if_t<is_integral_v<T> || is_floating_point_v<T>, const bool>
		operator()(const T& numeric) const noexcept { return numeric != 0; }
	using BaseVisitor::operator();
};

struct Char16Visitor :public BaseVisitor<wchar_t>
{
private:
	const wchar_t DefaultValue() const noexcept { return L' '; }
public:
	template <typename T>
	typename enable_if_t<is_integral_v<T> && sizeof(T) <= sizeof(wchar_t), const wchar_t>
		operator()(const T integral) const noexcept { return static_cast<wchar_t>(integral); }
	const wchar_t operator()(const wstring& str) const noexcept { return str.size() ? str[0] : DefaultValue(); }
	const wchar_t operator()(const vector<wstring>& strvec) const noexcept { return strvec.size() && strvec[0].size() ? strvec[0][0] : DefaultValue(); }
	using BaseVisitor::operator();
};

class DateTimeVisitor :public BaseVisitor<DateTime>
{
private:
	const DateTime DefaultValue() const noexcept { return DateTime{}; }
};

class InstanceVisitor : public BaseVisitor<Instance>
{
private:
	const Instance DefaultValue() const noexcept { return Instance::Empty(); }
};

class StringVisitor : public BaseVisitor<wstring>
{
private:
	const wstring DefaultValue() const noexcept { return wstring{}; }
public:
	template <typename T>
	typename enable_if_t<is_integral_v<T> && sizeof(T) <= sizeof(wchar_t), const wstring>
		operator()(const T integral) const noexcept { return to_wstring(integral); }
	// TODO: string conversions
	using BaseVisitor::operator();
};

template <typename T>
class NumericVisitor : public BaseVisitor<T>
{
private:
	const T DefaultValue() const noexcept { return 0; }
};

template <typename T, typename UnitConverter>
class BaseVisitorArray :BaseVisitor<vector<T>>
{
private:
	UnitConverter Transformer{};
	static vector<T> NewEmpty(const size_t size)
	{
		auto ne{ vector<T>() };
		ne.reserve(size);
		return ne;
	}
	const vector<T> DefaultValue() const noexcept
	{
		return NewEmpty(0);
	}
public:
	template <typename Source>
	const vector<T> operator()(const vector<Source>& sv) const noexcept
	{
		auto ov{ NewEmpty(sv.size()) };
		transform(sv.begin(), sv.end(), back_inserter(ov), Transformer);
		return ov;
	}
	virtual ~BaseVisitorArray() = default;
	using BaseVisitor<vector<T>>::operator();
};

class BoolAVisitor :public BaseVisitorArray<bool, BoolVisitor> {};
class Char16AVisitor :public BaseVisitorArray<wchar_t, Char16Visitor> {};
class DateTimeAVisitor :public BaseVisitorArray<DateTime, DateTimeVisitor> {};
template <typename T>
class NumericAVisitor :public BaseVisitorArray<T, NumericVisitor<T>> {};
class StringAVisitor :public BaseVisitorArray<wstring, StringVisitor> {};

const bool Value::Boolean() const noexcept
{
	return visit(BoolVisitor{}, cimvalue);
}

const vector<bool> Value::BooleanA() const noexcept
{
	return std::visit(BoolAVisitor{}, cimvalue);
}

const wchar_t Value::Char16() const noexcept
{
	return visit(Char16Visitor{}, cimvalue);
}

const vector<wchar_t> Value::Char16A() const noexcept
{
	return visit(Char16AVisitor{}, cimvalue);
}

const DateTime Value::DateTime() const noexcept
{
	return visit(DateTimeVisitor{}, cimvalue);
}

const vector<DateTime> Value::DateTimeA() const noexcept
{
	return visit(DateTimeAVisitor{}, cimvalue);
}

const float Value::Real32() const noexcept
{
	return visit(NumericVisitor<float>{}, cimvalue);
}

const vector<float> Value::Real32A() const noexcept
{
	return visit(NumericAVisitor<float>{}, cimvalue);
}

const double Value::Real64() const noexcept
{
	return visit(NumericVisitor<double>{}, cimvalue);
}

const vector<double> Value::Real64A() const noexcept
{
	return visit(NumericAVisitor<double>{}, cimvalue);
}

const int Value::SignedInt() const noexcept
{
	return visit(NumericVisitor<int>{}, cimvalue);
}

const vector<int> Value::SignedIntA() const noexcept
{
	return visit(NumericAVisitor<int>{}, cimvalue);
}

const long Value::SignedInt64() const noexcept
{
	return visit(NumericVisitor<long>{}, cimvalue);
}

const std::vector<long> Value::SignedInt64A() const noexcept
{
	return visit(NumericAVisitor<long>{}, cimvalue);
}

const wstring Value::String() const noexcept
{
	return visit(StringVisitor{}, cimvalue);
}

const vector<wstring> Value::StringA() const noexcept
{
	return visit(StringAVisitor{}, cimvalue);
}

const unsigned int Value::UnsignedInt() const noexcept
{
	return visit(NumericVisitor<unsigned int>{}, cimvalue);
}

const vector<unsigned int> Value::UnsignedIntA() const noexcept
{
	return visit(NumericAVisitor<unsigned int>{}, cimvalue);
}

const unsigned long Value::UnsignedInt64() const noexcept
{
	return visit(NumericVisitor<unsigned long>{}, cimvalue);
}

const vector<unsigned long> Value::UnsignedInt64A() const noexcept
{
	return visit(NumericAVisitor<unsigned long>{}, cimvalue);
}
