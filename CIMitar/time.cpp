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