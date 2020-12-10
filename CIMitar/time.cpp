#include "CIMitar.h"

using namespace CIMitar;

constexpr Interval::Interval(MI_Interval* MIInterval) noexcept
{
	Days = MIInterval->days;
	Hours = MIInterval->hours;
	Minutes = MIInterval->minutes;
	Seconds = MIInterval->seconds;
	Microseconds = MIInterval->microseconds;
}

const MI_Interval& Interval::ToMIInterval() const noexcept
{
	return MI_Interval{ Days, Hours, Minutes, Seconds, Microseconds };
}

const MI_Timestamp& Interval::ToMITimestamp() const noexcept
{
	return MI_Timestamp{ 0, 0, Days, Hours, Minutes, Seconds, Microseconds, 0 };
}

const bool CIMitar::operator==(const MI_Interval& lhs, const MI_Interval& rhs) noexcept
{	// Could reinterpret_cast to MI_Timestamp& and make this a one-liner, but no guarantees that padding fields are 0.
	// We are not required to honor any value in padding fields, but this operator accepts them
	// as const and so should not change them. Cannot ignore them if casted. So, minor duplication of work.
	return lhs.days == rhs.days &&
		lhs.hours == rhs.hours &&
		lhs.minutes == rhs.minutes &&
		lhs.hours == rhs.hours &&
		lhs.seconds == rhs.seconds &&
		lhs.microseconds == rhs.microseconds;
}

const bool CIMitar::operator==(const Interval& lhs, const Interval& rhs) noexcept
{
	return lhs.ToMIInterval() == rhs.ToMIInterval();
}

constexpr Timestamp::Timestamp(MI_Timestamp* Timestamp) noexcept
{
	Year = Timestamp->year;
	Month = Timestamp->month;
	Day = Timestamp->day;
	Hour = Timestamp->hour;
	Minute = Timestamp->minute;
	Second = Timestamp->second;
	Microseconds = Timestamp->microseconds;
	UTCOffset = Timestamp->utc;
}

const MI_Timestamp& Timestamp::ToMITimestamp() const noexcept
{
	return MI_Timestamp{ Year, Month, Day, Hour, Minute, Second, Microseconds, UTCOffset };
}

const MI_Interval& Timestamp::ToMIInterval() const noexcept
{
	return MI_Interval{ Day, Hour, Minute, Second, Microseconds };
}

const bool CIMitar::operator==(const MI_Timestamp& lhs, const MI_Timestamp& rhs) noexcept
{
	return lhs.year == rhs.year &&
		lhs.month == rhs.month &&
		lhs.day == rhs.day &&
		lhs.hour == rhs.hour &&
		lhs.minute == rhs.minute &&
		lhs.second == rhs.second &&
		lhs.microseconds == rhs.microseconds &&
		lhs.utc == rhs.utc;
}


const bool CIMitar::operator==(const Timestamp& lhs, const Timestamp& rhs) noexcept
{
	return lhs.ToMITimestamp() == rhs.ToMITimestamp();
}


/*
Conversion functions and time utilies
Philosophy: working through FILETIME structures means the computer does the work
 instead of us needing to handroll a lot of specialized code. This library's goal
 is to make CIM interactions simpler, not handle bulk time calculations.
*/

// ensures that a TM structure will not cause crashes in related functions
constexpr void CleanTM(tm& TM) noexcept
{
#pragma(push)
#pragma warning(disable : 4804)
	0 <= TM.tm_mon <= 11 ? 0 : TM.tm_mon = 0;
	1 <= TM.tm_mday <= 31 ? 0 : TM.tm_mday = 1;
	0 <= TM.tm_hour <= 23 ? 0 : TM.tm_hour = 0;
	0 <= TM.tm_min <= 59 ? 0 : TM.tm_min = 0;
	0 <= TM.tm_sec <= 60 ? 0 : TM.tm_sec = 0;
	0 <= TM.tm_wday <= 6 ? 0 : TM.tm_wday = 0;
	0 <= TM.tm_yday <= 365 ? 0 : TM.tm_yday = 0;
#pragma warning(pop)
}

// todo: deal with UTC
tm CIMitar::TMFromCIMTime(const MI_Timestamp& Timestamp) noexcept
{
	tm TM{};
	TM.tm_year = Timestamp.year - 1900;
	TM.tm_mon = Timestamp.month - 1;
	TM.tm_mday = Timestamp.day;
	TM.tm_hour = Timestamp.hour;
	TM.tm_min = Timestamp.minute;
	TM.tm_sec = Timestamp.second;
	TM.tm_isdst = -1;
	CleanTM(TM);
	mktime(&TM);
	return TM;
}

// todo: deal with UTC
MI_Timestamp CIMitar::CIMTimeFromTM(const tm& TM) noexcept
{
	tm internaltm{ TM }; // to honor const tm&
	MI_Timestamp stamp{ 0 };
	CleanTM(internaltm);
	stamp.year = internaltm.tm_year + 1900;
	stamp.month = internaltm.tm_mon + 1;
	stamp.day = internaltm.tm_mday;
	stamp.hour = internaltm.tm_hour;
	stamp.minute = internaltm.tm_min;
	stamp.second = internaltm.tm_sec;
	return stamp;
}

// FILETIME multipliers -- 1 FILETIME == 100 NANOSECONDS
constexpr unsigned long long FileTime_Microsecond{ 10ull };
constexpr unsigned long long FileTime_Millisecond{ 1000ull * FileTime_Microsecond };
constexpr unsigned long long FileTime_Second{ 1000ull * FileTime_Millisecond };
constexpr unsigned long long FileTime_Minute{ 60ull * FileTime_Second };
constexpr unsigned long long FileTime_Hour{ 60ull * FileTime_Minute };
constexpr unsigned long long FileTime_Day{ 24ull * FileTime_Hour };
