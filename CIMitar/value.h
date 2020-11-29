#pragma once

#include <mi.h> // caution: earlier versions of mi.h did not have a header guard

// wraps MI_Value
class Value sealed
{
private:
public:
	Value();
	~Value();
};

