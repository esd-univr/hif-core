/// @file Array.cpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#include "hif/classes/Array.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Range.hpp"

namespace hif
{

Array::Array()
    : _isSigned(false)
{
    // ntd
}

Array::~Array()
{
    // ntd
}

ClassId Array::getClassId() const { return CLASSID_ARRAY; }

int Array::acceptVisitor(HifVisitor &vis) { return vis.visitArray(*this); }

void Array::_calculateFields()
{
    CompositeType::_calculateFields();
    _addField(_span);
}

std::string Array::_getFieldName(const Object *child) const
{
    if (child == _span)
        return "span";
    return CompositeType::_getFieldName(child);
}

Range *Array::setSpan(Range *x) { return setChild(_span, x); }

bool Array::isSigned() const { return _isSigned; }

void Array::setSigned(const bool sign) { _isSigned = sign; }

Object *Array::toObject() { return this; }

} // namespace hif
