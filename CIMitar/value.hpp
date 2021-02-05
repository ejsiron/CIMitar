#pragma once

#include <mi.h>
#include "CIMitar.hpp"

#ifndef CIMITAR_VALUE_H_INCLUDED
#define CIMITAR_VALUE_H_INCLUDED

const MI_Type CIMTypeIDTranslator(const CIMitar::CIMTypes Type) noexcept;
const CIMitar::CIMTypes CIMTypeIDTranslator(const MI_Type Type) noexcept;

#endif CIMITAR_VALUE_H_INCLUDED
