/// @file IntValue.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/IntValue.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

IntValue::IntValue()
    : _value(0)
{
    // ntd
}

IntValue::IntValue(long long v)
    : _value(v)
{
    // ntd
}

IntValue::IntValue(unsigned long long v)
    : _value(static_cast<long long>(v))
{
    // ntd
}

IntValue::IntValue(int v)
    : _value(static_cast<long long>(v))
{
    // ntd
}

IntValue::IntValue(unsigned v)
    : _value(static_cast<long long>(v))
{
    // ntd
}

IntValue::~IntValue()
{
    // ntd
}

ClassId IntValue::getClassId() const { return CLASSID_INTVALUE; }

int IntValue::acceptVisitor(HifVisitor &vis) { return vis.visitIntValue(*this); }

void IntValue::_calculateFields() { ConstValue::_calculateFields(); }

long long IntValue::getValue() const { return _value; }

void IntValue::setValue(long long a) { _value = a; }

} // namespace hif
