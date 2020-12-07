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

const MI_Interval& Interval::operator()() noexcept
{
	return MI_Interval{ Days, Hours, Minutes, Seconds, Microseconds };
}

const bool operator==(Interval& lhs, Interval& rhs) noexcept
{
	return
		lhs.Days == rhs.Days &&
		lhs.Hours == rhs.Hours &&
		lhs.Minutes == rhs.Minutes &&
		lhs.Seconds == rhs.Seconds &&
		lhs.Microseconds == rhs.Microseconds;
}