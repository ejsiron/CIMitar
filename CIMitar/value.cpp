#include <algorithm>
#include <map>
#include <type_traits>
#include "CIMitar.h"
#include "value.h"

using namespace std;
using namespace CIMitar;

static_assert(is_same_v<int, variant_alternative_t<0, CIMValueVariant>>);
static_assert(is_same_v<long long, variant_alternative_t<1, CIMValueVariant>>);
static_assert(is_same_v<wstring, variant_alternative_t<2, CIMValueVariant>>);
static_assert(is_same_v<Instance, variant_alternative_t<3, CIMValueVariant>>);
static_assert(is_same_v<Interval, variant_alternative_t<4, CIMValueVariant>>);
static_assert(is_same_v<Timestamp, variant_alternative_t<5, CIMValueVariant>>);
static_assert(is_same_v<vector<int>, variant_alternative_t<6, CIMValueVariant>>);
static_assert(is_same_v<vector<long long>, variant_alternative_t<7, CIMValueVariant>>);
static_assert(is_same_v<vector<wstring>, variant_alternative_t<8, CIMValueVariant>>);
static_assert(is_same_v<vector<Instance>, variant_alternative_t<9, CIMValueVariant>>);
static_assert(is_same_v<vector<Interval>, variant_alternative_t<10, CIMValueVariant>>);
static_assert(is_same_v<vector<Timestamp>, variant_alternative_t<11, CIMValueVariant>>);

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
vector<outtype> VectorizeMIArray(const MIType* SourceArray, unsigned int length) noexcept
{
	vector<outtype> OutVector{};
	OutVector.reserve(length);
	try
	{
		for (unsigned int i{ 0 }; i < length; ++i)
		{
			//if constexpr (is_trivially_constructible_v<outtype, MIType>)
			//{
			OutVector.emplace_back(SourceArray[i]);
			//}
			//else
			//	OutVector.emplace_back(outtype{ SourceArray[i] });
		}
	}
	catch (...) {}
	return OutVector;
}

const bool Value::IsArray() const noexcept
{
	return isarray;
}

Value::Value(MI_Value& Val, const MI_Type Type) noexcept
{
	cimtype = CIMTypeIDTranslator(Type);
	auto length{ Type & MI_ARRAY ? Val.array.size : 0 };
	// This portion could throw if provided a malformed Val.
	// All Vals should come from an MI provider
	switch (Type)
	{
	case MI_BOOLEANA:	cimvalue = VectorizeMIArray<int>(Val.booleana.data, length); break;
	case MI_CHAR16:	cimvalue = static_cast<int>(Val.char16); break;
	case MI_CHAR16A:	cimvalue = VectorizeMIArray<int>(Val.char16a.data, length); break;
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
			cimvalue = VectorizeMIArray<CIMitar::Timestamp>(Val.datetimea.data, length);
		}
		else
		{
			cimvalue = VectorizeMIArray<CIMitar::Interval>(Val.datetimea.data, length);
		}
		break;
	case MI_INSTANCE:	cimvalue = CIMitar::Instance(Val.instance, nullptr); break;
	case MI_INSTANCEA:cimvalue = VectorizeMIArray<CIMitar::Instance>(Val.instancea.data, length); break;
	case MI_REAL32:	cimvalue = static_cast<int>(Val.real32);	break;
	case MI_REAL32A:	cimvalue = VectorizeMIArray<int>(Val.real32a.data, length); break;
		//case MI_REAL64:	cimvalue = Val.real64;	break;
	case MI_REAL64A:	cimvalue = VectorizeMIArray<double>(Val.real64a.data, length); break;
	case MI_REFERENCE:cimvalue = CIMitar::Instance(Val.reference, nullptr);	break;
	case MI_REFERENCEA:	cimvalue = VectorizeMIArray<CIMitar::Instance>(Val.referencea.data, length); break;
		//case MI_SINT8:		cimvalue = Val.sint8;	break;
	case MI_SINT8A:	cimvalue = VectorizeMIArray<int>(Val.sint8a.data, length); break;
		//case MI_SINT16:	cimvalue = Val.sint16;	break;
	case MI_SINT16A:	cimvalue = VectorizeMIArray<int>(Val.sint16a.data, length); break;
		//case MI_SINT32:	cimvalue = Val.sint32;	break;
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
};

class BoolVisitor : public BaseVisitor<bool>
{
private:
	const bool DefaultValue() const noexcept { return false; }
public:
	const bool operator()(const bool b) const noexcept { return b; }
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
	template <typename T>
	typename enable_if_t<!is_trivially_constructible_v<T>, const bool>
		operator()(const T&) const noexcept { return DefaultValue(); }
};

class Char16Visitor :public BaseVisitor<wchar_t>
{
private:
	const wchar_t DefaultValue() const noexcept { return L' '; }
public:
	const wchar_t operator()(const wchar_t wt) const noexcept { return wt; }
	const wchar_t operator()(const wstring& str) const noexcept { return str.size() ? str[0] : DefaultValue(); }
	const wchar_t operator()(const vector<wstring>& strvec) const noexcept { return strvec.size() && strvec[0].size() ? strvec[0][0] : DefaultValue(); }
	template <typename T>
	typename enable_if_t<is_trivially_constructible_v<T>, const wchar_t>
		operator()(const T) const noexcept { return DefaultValue(); }
	template <typename T>
	typename enable_if_t<!is_trivially_constructible_v<T>, const wchar_t>
		operator()(const T&) const noexcept { return DefaultValue(); }
};

class IntervalVisitor :public BaseVisitor<Interval>
{
private:
	const Interval DefaultValue() const noexcept { return Interval{}; }
public:
	const Interval& operator()(const Interval& interval) { return interval; }
	const Interval operator()(const Timestamp& timestamp) { return Interval{ timestamp }; }
	template <typename T>
	typename enable_if_t<is_trivially_constructible_v<T>, const Interval>
		operator()(const T) const noexcept { return DefaultValue(); }
	template <typename T>
	typename enable_if_t<!is_trivially_constructible_v<T>, const Interval>
		operator()(const T&) const noexcept { return DefaultValue(); }
};

class TimestampVisitor :public BaseVisitor<Timestamp>
{
private:
	const Timestamp DefaultValue() const noexcept { return Timestamp{}; }
public:
	const Timestamp& operator()(const Timestamp& timestamp) { return timestamp; }
	const Timestamp operator()(const Interval& interval) { return Timestamp{ interval }; }
	template <typename T>
	typename enable_if_t<is_trivially_constructible_v<T>, const Timestamp>
		operator()(const T) const noexcept { return DefaultValue(); }
	template <typename T>
	typename enable_if_t<!is_trivially_constructible_v<T>, const Timestamp>
		operator()(const T&) const noexcept { return DefaultValue(); }
};

class InstanceVisitor : public BaseVisitor<Instance>
{
private:
	const Instance DefaultValue() const noexcept { return Instance::Empty(); }
public:
	const Instance& operator()(const Instance& inst) const noexcept { return inst; }
	template <typename T>
	typename enable_if_t<is_trivially_constructible_v<T>, const Instance>
		operator()(const T) const noexcept { return DefaultValue(); }
	template <typename T>
	typename enable_if_t<!is_trivially_constructible_v<T>, const Instance>
		operator()(const T&) const noexcept { return DefaultValue(); }
};

class StringVisitor : public BaseVisitor<wstring>
{
private:
	const wstring DefaultValue() const noexcept { return wstring{}; }
public:
	const wstring& operator()(const wstring& value) { return value; }
	template <typename T>
	typename enable_if_t<(is_trivially_constructible_v<T>), const wstring>
		operator()(const T integral) const noexcept { return to_wstring(integral); }
	//template <typename T>
	//typename enable_if_t<is_trivially_copyable_v<T>, const wstring>
	//	operator()(const T) const noexcept { return DefaultValue(); }
	template <typename T>
	typename enable_if_t<!is_trivially_constructible_v<T>, const wstring>
		operator()(const T&) const noexcept { return DefaultValue(); }
	// TODO: string conversions
};

template <typename Numeric>
class NumericVisitor : public BaseVisitor<Numeric>
{
private:
	const Numeric DefaultValue() const noexcept { return 0; }
public:
	template <typename Input>
	const Numeric operator()(const Input value) const noexcept { return value; }
	template <typename Input>
	typename enable_if_t<!is_trivially_constructible_v<Input>, const Numeric>
		operator()(const Input&) const noexcept { return DefaultValue(); }
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
	const vector<T>& operator()(const vector<T>& v) const noexcept { return v; }
	template <typename Input>
	const vector<T> operator()(const vector<Input>& sv) const noexcept
	{
		auto ov{ NewEmpty(sv.size()) };
		transform(sv.begin(), sv.end(), back_inserter(ov), Transformer);
		return ov;
	}
	template <typename Input>
	typename enable_if_t<is_trivially_constructible_v<Input>, const vector<T>>
		operator()(const Input) const noexcept { return DefaultValue(); }
	template <typename Input>
	typename enable_if_t<!is_trivially_constructible_v<Input>, const vector<T>>
		operator()(const Input&) const noexcept { return DefaultValue(); }
	virtual ~BaseVisitorArray() = default;
};

class BoolAVisitor :public BaseVisitorArray<bool, BoolVisitor> {};
class Char16AVisitor :public BaseVisitorArray<wchar_t, Char16Visitor> {};
class IntervalAVisitor :public BaseVisitorArray<Interval, IntervalVisitor> {};
class TimestampAVisitor :public BaseVisitorArray<Timestamp, TimestampVisitor> {};
template <typename T>
class NumericAVisitor :public BaseVisitorArray<T, NumericVisitor<T>> {};
class StringAVisitor :public BaseVisitorArray<wstring, StringVisitor> {};

const bool Value::Boolean() const noexcept
{
	return visit(BoolVisitor{}, cimvalue);
}

//const vector<bool> Value::BooleanA() const noexcept
//{
//	return visit(BoolAVisitor{}, cimvalue);
//}

const wchar_t Value::Char16() const noexcept
{
	return visit(Char16Visitor{}, cimvalue);
}

//const vector<wchar_t> Value::Char16A() const noexcept
//{
//	return visit(Char16AVisitor{}, cimvalue);
//}

//const Interval Value::Interval() const noexcept
//{
//	return visit(IntervalVisitor{}, cimvalue);
//}

//const vector<Interval> Value::IntervalA() const noexcept
//{
//	return visit(IntervalAVisitor{}, cimvalue);
//}

//const Timestamp Value::Timestamp() const noexcept
//{
//	return visit(TimestampVisitor{}, cimvalue);
//}
//
//const vector<Timestamp> Value::TimestampA() const noexcept
//{
//	return visit(TimestampAVisitor{}, cimvalue);
//}

//const float Value::Real32() const noexcept
//{
//	return visit(NumericVisitor<float>{}, cimvalue);
//}
//
//const vector<float> Value::Real32A() const noexcept
//{
//	return visit(NumericAVisitor<float>{}, cimvalue);
//}
//
//const double Value::Real64() const noexcept
//{
//	return visit(NumericVisitor<double>{}, cimvalue);
//}
//
//const vector<double> Value::Real64A() const noexcept
//{
//	return visit(NumericAVisitor<double>{}, cimvalue);
//}
//
//const int Value::SignedInt() const noexcept
//{
//	return visit(NumericVisitor<int>{}, cimvalue);
//}
//
//const vector<int> Value::SignedIntA() const noexcept
//{
//	return visit(NumericAVisitor<int>{}, cimvalue);
//}
//
//const long Value::SignedInt64() const noexcept
//{
//	return visit(NumericVisitor<long>{}, cimvalue);
//}
//
//const std::vector<long> Value::SignedInt64A() const noexcept
//{
//	return visit(NumericAVisitor<long>{}, cimvalue);
//}
//
const wstring Value::String() const noexcept
{
	return visit(StringVisitor{}, cimvalue);
}
//
//const vector<wstring> Value::StringA() const noexcept
//{
//	return visit(StringAVisitor{}, cimvalue);
//}
//
//const unsigned int Value::UnsignedInt() const noexcept
//{
//	return visit(NumericVisitor<unsigned int>{}, cimvalue);
//}
//
//const vector<unsigned int> Value::UnsignedIntA() const noexcept
//{
//	return visit(NumericAVisitor<unsigned int>{}, cimvalue);
//}
//
//const unsigned long Value::UnsignedInt64() const noexcept
//{
//	return visit(NumericVisitor<unsigned long>{}, cimvalue);
//}
//
//const vector<unsigned long> Value::UnsignedInt64A() const noexcept
//{
//	return visit(NumericAVisitor<unsigned long>{}, cimvalue);
//}
