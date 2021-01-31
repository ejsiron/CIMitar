#include <algorithm>
#include <map>
#include <type_traits>
#include "CIMitar.h"
#include "value.h"

using namespace std;
using namespace CIMitar;

constexpr size_t VAR_INT{ 0 };
constexpr size_t VAR_UINT{ 1 };
constexpr size_t VAR_INT64{ 2 };
constexpr size_t VAR_UINT64{ 3 };
constexpr size_t VAR_FP{ 4 };
constexpr size_t VAR_FP64{ 5 };
constexpr size_t VAR_CHAR{ 6 };
constexpr size_t VAR_STRING{ 7 };
constexpr size_t VAR_INSTANCE{ 8 };
constexpr size_t VAR_INTERVAL{ 9 };
constexpr size_t VAR_TIMESTAMP{ 10 };
constexpr size_t VAR_INT_A{ 11 };
constexpr size_t VAR_UINT_A{ 12 };
constexpr size_t VAR_INT64_A{ 13 };
constexpr size_t VAR_UINT64_A{ 14 };
constexpr size_t VAR_FP_A{ 15 };
constexpr size_t VAR_FP64_A{ 16 };
constexpr size_t VAR_CHAR_A{ 17 };
constexpr size_t VAR_STRING_A{ 18 };
constexpr size_t VAR_INSTANCE_A{ 19 };
constexpr size_t VAR_INTERVAL_A{ 20 };
constexpr size_t VAR_TIMESTAMP_A{ 21 };

constexpr size_t VAR_DATETIME{ 998 };
constexpr size_t VAR_DATETIME_A{ 999 };

static_assert (is_same_v<int, variant_alternative_t<VAR_INT, cimvaluevariant>>);
static_assert (is_same_v<unsigned int, variant_alternative_t<VAR_UINT, cimvaluevariant>>);
static_assert (is_same_v<long long, variant_alternative_t<VAR_INT64, cimvaluevariant>>);
static_assert (is_same_v<unsigned long long, variant_alternative_t<VAR_UINT64, cimvaluevariant>>);
static_assert (is_same_v<float, variant_alternative_t<VAR_FP, cimvaluevariant>>);
static_assert (is_same_v<double, variant_alternative_t<VAR_FP64, cimvaluevariant>>);
static_assert (is_same_v<wchar_t, variant_alternative_t<VAR_CHAR, cimvaluevariant>>);
static_assert (is_same_v<wstring, variant_alternative_t<VAR_STRING, cimvaluevariant>>);
static_assert (is_same_v<Instance, variant_alternative_t<VAR_INSTANCE, cimvaluevariant>>);
static_assert (is_same_v<Interval, variant_alternative_t<VAR_INTERVAL, cimvaluevariant>>);
static_assert (is_same_v<Timestamp, variant_alternative_t<VAR_TIMESTAMP, cimvaluevariant>>);
static_assert (is_same_v<vector<int>, variant_alternative_t<VAR_INT_A, cimvaluevariant>>);
static_assert (is_same_v<vector<unsigned int>, variant_alternative_t<VAR_UINT_A, cimvaluevariant>>);
static_assert (is_same_v<vector<long long>, variant_alternative_t<VAR_INT64_A, cimvaluevariant>>);
static_assert (is_same_v<vector<unsigned long long>, variant_alternative_t<VAR_UINT64_A, cimvaluevariant>>);
static_assert (is_same_v<vector<float>, variant_alternative_t<VAR_FP_A, cimvaluevariant>>);
static_assert (is_same_v<vector<double>, variant_alternative_t<VAR_FP64_A, cimvaluevariant>>);
static_assert (is_same_v<vector<wchar_t>, variant_alternative_t<VAR_CHAR_A, cimvaluevariant>>);
static_assert (is_same_v<vector<wstring>, variant_alternative_t<VAR_STRING_A, cimvaluevariant>>);
static_assert (is_same_v<vector<Instance>, variant_alternative_t<VAR_INSTANCE_A, cimvaluevariant>>);
static_assert (is_same_v<vector<Interval>, variant_alternative_t<VAR_INTERVAL_A, cimvaluevariant>>);
static_assert (is_same_v<vector<Timestamp>, variant_alternative_t<VAR_TIMESTAMP_A, cimvaluevariant>>);

const map<CIMTypes, size_t> CIMTypeToVariantType = {
	{CIMTypes::Boolean, VAR_INT},
	{CIMTypes::BooleanA, VAR_INT_A },
	{CIMTypes::Char16, VAR_CHAR},
	{CIMTypes::Char16A, VAR_CHAR_A},
	{CIMTypes::DateTime, VAR_DATETIME},
	{CIMTypes::DateTimeA, VAR_DATETIME_A},
	{CIMTypes::Instance, VAR_INSTANCE},
	{CIMTypes::InstanceA, VAR_INSTANCE_A},
	{CIMTypes::Real32, VAR_FP},
	{CIMTypes::Real32A, VAR_FP_A},
	{CIMTypes::Real64, VAR_FP64},
	{CIMTypes::Real64A, VAR_FP64_A},
	{CIMTypes::Reference, VAR_INSTANCE},
	{CIMTypes::ReferenceA, VAR_INSTANCE_A},
	{CIMTypes::SInt8, VAR_INT},
	{CIMTypes::SInt8A, VAR_INT_A},
	{CIMTypes::SInt16, VAR_INT},
	{CIMTypes::SInt16A, VAR_INT_A},
	{CIMTypes::SInt32, VAR_INT},
	{CIMTypes::SInt32A, VAR_INT_A},
	{CIMTypes::SInt64, VAR_INT64},
	{CIMTypes::SInt64A, VAR_INT64_A},
	{CIMTypes::UInt8, VAR_UINT},
	{CIMTypes::UInt8A, VAR_UINT_A},
	{CIMTypes::UInt16, VAR_UINT},
	{CIMTypes::UInt16A, VAR_UINT_A},
	{CIMTypes::UInt32, VAR_UINT},
	{CIMTypes::UInt32A, VAR_UINT_A},
	{CIMTypes::UInt64, VAR_UINT64},
	{CIMTypes::UInt64A, VAR_UINT64_A}
};

const map<CIMTypes, _MI_Type> CIMTypeToMIType = {
	{CIMTypes::Boolean, MI_BOOLEAN},
	{CIMTypes::BooleanA, MI_BOOLEANA},
	{CIMTypes::Char16, MI_CHAR16},
	{CIMTypes::Char16A, MI_CHAR16A},
	{CIMTypes::DateTime, MI_DATETIME},
	{CIMTypes::DateTimeA, MI_DATETIMEA},
	{CIMTypes::Instance, MI_INSTANCE},
	{CIMTypes::InstanceA, MI_INSTANCEA},
	{CIMTypes::Real32, MI_REAL32},
	{CIMTypes::Real32A, MI_REAL32A},
	{CIMTypes::Real64, MI_REAL64},
	{CIMTypes::Real64A, MI_REAL64A},
	{CIMTypes::Reference, MI_REFERENCE},
	{CIMTypes::ReferenceA, MI_REFERENCEA},
	{CIMTypes::SInt8, MI_SINT8},
	{CIMTypes::SInt8A, MI_SINT8A},
	{CIMTypes::SInt16, MI_SINT16},
	{CIMTypes::SInt16A, MI_SINT16A},
	{CIMTypes::SInt32, MI_SINT32},
	{CIMTypes::SInt32A, MI_SINT32A},
	{CIMTypes::SInt64, MI_SINT64},
	{CIMTypes::SInt64A, MI_SINT64A},
	{CIMTypes::String, MI_STRING},
	{CIMTypes::StringA, MI_STRINGA},
	{CIMTypes::UInt8, MI_UINT8},
	{CIMTypes::UInt8A, MI_UINT8A},
	{CIMTypes::UInt16, MI_UINT16},
	{CIMTypes::UInt16A, MI_UINT16A},
	{CIMTypes::UInt32, MI_UINT32},
	{CIMTypes::UInt32A, MI_UINT32A},
	{CIMTypes::UInt64, MI_UINT64},
	{CIMTypes::UInt64A, MI_UINT64A}
};

const CIMTypes CIMTypeIDTranslator(const MI_Type Type) noexcept
{
	switch (Type)
	{
	case MI_BOOLEANA:				return CIMTypes::BooleanA;
	case MI_CHAR16:				return CIMTypes::Char16;
	case MI_CHAR16A:				return CIMTypes::Char16A;
	case MI_DATETIME:				return CIMTypes::DateTime;
	case MI_DATETIMEA:			return CIMTypes::DateTimeA;
	case MI_INSTANCE:				return CIMTypes::Instance;
	case MI_INSTANCEA:			return CIMTypes::InstanceA;
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

#pragma warning(push)
#pragma warning(disable: 4244)
template <typename out_num>
struct NumToNumVisitor
{
	template <typename in_num>
	typename enable_if_t <is_arithmetic_v<in_num> && !is_same_v<wchar_t, in_num>, const out_num>
		operator()(const in_num n) const noexcept
	{
		return n;
	}
};

#pragma region To wchar_t

// all numeric sources
// to prevent non-printable artifacts in wchar_t, return '0' for a 0 value and '1' for everything else
template <typename in_num>
typename enable_if_t <is_arithmetic_v<in_num> && !is_same_v<wchar_t, in_num>, const wchar_t>
to_stdwchar_t(const in_num n) noexcept
{
	return n ? L'1' : L'0';
}

// return a 'C' for all class types
template <class in_value>
typename enable_if_t<is_class_v<in_value>, const wchar_t>
to_stdwchar_t(const in_value&)
{
	return L'C';
}

#pragma endregion

#pragma region To numeric

// let C++ do whatever it wants for unlike numerics
template <typename out_num, typename in_charnum>
typename enable_if_t<is_arithmetic_v<in_charnum> && !is_same_v<out_num, in_charnum>
	, const out_num>
	to_stdnum(const in_charnum n) noexcept
{
	return n;
}

// return a 1 for all class types
template <typename out_num, class in_value>
typename enable_if_t<is_class_v<in_value>, const out_num>
to_stdnum(const in_value&)
{
	return 1;
}

#pragma endregion

#pragma region To wstring
// numeric to wstring
template <typename in_num>
typename enable_if_t<is_arithmetic_v<in_num> && !is_same_v<in_num, wchar_t>, const wstring>
to_stdwstring(const in_num n) noexcept
{
	return to_wstring(n);
}

const wstring to_stdwstring(const wchar_t c) noexcept
{
	return wstring{ c };
}

// todo: make into wstring conversion
const wstring to_stdwstring(const Instance& inst) noexcept
{
	return L"";
}

// todo: make into wstring conversion
const wstring to_stdwstring(const Interval& interval) noexcept
{
	return L"";
}

// todo: make into wstring conversion
const wstring to_stdwstring(const Timestamp& ts) noexcept
{
	return L"";
}

#pragma endregion

const bool Value::IsArray() const noexcept
{
	return isarray;
}

const bool Value::IsEmpty() const noexcept
{
	return isempty;
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
	case MI_BOOLEANA:	cimvalue = VectorizeMIArray<int>(Val.booleana.data, length); break;
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
			cimvalue = VectorizeMIArray<CIMitar::Timestamp>(Val.datetimea.data, length);
		}
		else
		{
			cimvalue = VectorizeMIArray<CIMitar::Interval>(Val.datetimea.data, length);
		}
		break;
	case MI_INSTANCE:	cimvalue = CIMitar::Instance(Val.instance, nullptr); break;
	case MI_INSTANCEA:cimvalue = VectorizeMIArray<CIMitar::Instance>(Val.instancea.data, length); break;
	case MI_REAL32:	cimvalue = Val.real32;	break;
	case MI_REAL32A:	cimvalue = VectorizeMIArray<float>(Val.real32a.data, length); break;
	case MI_REAL64:	cimvalue = Val.real64;	break;
	case MI_REAL64A:	cimvalue = VectorizeMIArray<double>(Val.real64a.data, length); break;
	case MI_REFERENCE:cimvalue = CIMitar::Instance(Val.reference, nullptr);	break;
	case MI_REFERENCEA:	cimvalue = VectorizeMIArray<CIMitar::Instance>(Val.referencea.data, length); break;
	case MI_SINT8:		cimvalue = Val.sint8;	break;
	case MI_SINT8A:	cimvalue = VectorizeMIArray<int>(Val.sint8a.data, length); break;
	case MI_SINT16:	cimvalue = Val.sint16;	break;
	case MI_SINT16A:	cimvalue = VectorizeMIArray<int>(Val.sint16a.data, length); break;
	case MI_SINT32:	cimvalue = Val.sint32;	break;
	case MI_SINT32A:	cimvalue = VectorizeMIArray<int>(Val.sint32a.data, length); break;
	case MI_SINT64:	cimvalue = static_cast<long long>(Val.sint64);	break;
	case MI_SINT64A:	cimvalue = VectorizeMIArray<long long>(Val.sint64a.data, length); break;
	case MI_STRING:	cimvalue = Val.string;	break;
	case MI_STRINGA:	cimvalue = VectorizeMIArray<wstring>(Val.stringa.data, length); break;
	case MI_UINT8:		cimvalue = static_cast<unsigned int>(Val.uint8);	break;
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

template <typename T, typename UnitConverter>
class BaseVisitorArray
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

class BoolAVisitor :public BaseVisitorArray<bool, NumericVisitor<const bool>> {};
class Char16AVisitor :public BaseVisitorArray<wchar_t, CharVisitor> {};
class IntervalAVisitor :public BaseVisitorArray<Interval, DateTimeVisitor<Interval>> {};
class TimestampAVisitor :public BaseVisitorArray<Timestamp, DateTimeVisitor<Timestamp>> {};
template <typename T>
class NumericAVisitor :public BaseVisitorArray<T, NumericVisitor<const T>> {};
class StringAVisitor :public BaseVisitorArray<wstring, StringVisitor> {};

const bool Value::Boolean() const noexcept
{
	switch (cimtype)
	{
	case CIMTypes::Boolean:	return get<bool>(cimvalue);
	case CIMTypes::Real32: [[fallthrough]];
	case CIMTypes::Real64: [[fallthrough]];
	case CIMTypes::SInt8: [[fallthrough]];
	case CIMTypes::SInt16: [[fallthrough]];
	case CIMTypes::SInt32: [[fallthrough]];
	case CIMTypes::SInt64: [[fallthrough]];
	case CIMTypes::UInt8: [[fallthrough]];
	case CIMTypes::UInt16: [[fallthrough]];
	case CIMTypes::UInt32: [[fallthrough]];
	case CIMTypes::UInt64: [[fallthrough]];
		return visit(NumToNumVisitor<bool>{}, cimvalue);

	case CIMTypes::DateTime:	return false;
	case CIMTypes::String:		return get<wstring>(cimvalue).size();
	case CIMTypes::StringA:		return MI_STRINGA;
	case CIMTypes::UInt8A:		return MI_UINT8A;
	case CIMTypes::UInt16A:		return MI_UINT16A;
	case CIMTypes::UInt32A:		return MI_UINT32A;
	case CIMTypes::UInt64A:		return MI_UINT64A;
	case CIMTypes::BooleanA:	return MI_BOOLEANA;
	case CIMTypes::Char16A:		return MI_CHAR16A;
	case CIMTypes::DateTimeA:	return MI_DATETIMEA;
	case CIMTypes::Real32A:		return MI_REAL32A;
	case CIMTypes::Real64A:		return MI_REAL64A;
	case CIMTypes::SInt8A:		return MI_SINT8A;
	case CIMTypes::SInt16A:		return MI_SINT16A;
	case CIMTypes::SInt32A:		return MI_SINT32A;
	case CIMTypes::SInt64A:		return MI_SINT64A;
	default:							return false;
	}
}

const vector<bool> Value::BooleanA() const noexcept
{
	return visit(BoolAVisitor{}, cimvalue);
}

const wchar_t Value::Char16() const noexcept
{
	return visit(CharVisitor{}, cimvalue);
}

const vector<wchar_t> Value::Char16A() const noexcept
{
	return visit(Char16AVisitor{}, cimvalue);
}

//const Interval Value::Interval() const noexcept
//{
//	return visit(DateTimeVisitor<Interval>{}, cimvalue);
//}

//const vector<Interval> Value::IntervalA() const noexcept
//{
//	return visit(IntervalAVisitor{}, cimvalue);
//}
//
//const Timestamp Value::Timestamp() const noexcept
//{
//	return visit(DateTimeVisitor<Timestamp>{}, cimvalue);
//}
//
//const vector<Timestamp> Value::TimestampA() const noexcept
//{
//	return visit(TimestampAVisitor{}, cimvalue);
//}

const float Value::Real32() const noexcept
{
	return visit(NumericVisitor<const float>{}, cimvalue);
}
//
//const vector<float> Value::Real32A() const noexcept
//{
//	return visit(NumericAVisitor<float>{}, cimvalue);
//}
//
const double Value::Real64() const noexcept
{
	return visit(NumericVisitor<const double>{}, cimvalue);
}
//
//const vector<double> Value::Real64A() const noexcept
//{
//	return visit(NumericAVisitor<double>{}, cimvalue);
//}
//
const int Value::SignedInt() const noexcept
{
	return visit(NumericVisitor<const int>{}, cimvalue);
}
//
//const vector<int> Value::SignedIntA() const noexcept
//{
//	return visit(NumericAVisitor<int>{}, cimvalue);
//}
//
const long long Value::SignedInt64() const noexcept
{
	return visit(NumericVisitor<const long long>{}, cimvalue);
}
//
//const std::vector<long long> Value::SignedInt64A() const noexcept
//{
//	return visit(NumericAVisitor<long>{}, cimvalue);
//}
//
const wstring Value::String() const noexcept
{
	//return visit(StringVisitor{}, cimvalue);
	return L"";
}
//
//const vector<wstring> Value::StringA() const noexcept
//{
//	return visit(StringAVisitor{}, cimvalue);
//}
//
const unsigned int Value::UnsignedInt() const noexcept
{
	return visit(NumericVisitor<const unsigned int>{}, cimvalue);
}
//
//const vector<unsigned int> Value::UnsignedIntA() const noexcept
//{
//	return visit(NumericAVisitor<unsigned int>{}, cimvalue);
//}
//
const unsigned long long Value::UnsignedInt64() const noexcept
{
	return visit(NumericVisitor<const unsigned long>{}, cimvalue);
}
//
//const vector<unsigned long long> Value::UnsignedInt64A() const noexcept
//{
//	return visit(NumericAVisitor<unsigned long>{}, cimvalue);
//}

#pragma warning(pop)
