/// @file IntValue.hpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/ConstValue.hpp"

namespace hif
{

/// @brief Integer value.
/// @details
/// This class represents a value of integer type.
/// By default this value is assigned to 0.
class IntValue : public ConstValue
{
public:
    /// @brief Constructor.
    IntValue();

    /// @brief Constructor.
    /// @param v The integer value to be assigned. Default is 0.
    IntValue(long long v);

    /// @brief Constructor.
    /// @param v The integer value to be assigned. Default is 0.
    IntValue(unsigned long long v);

    /// @brief Constructor.
    /// @param v The integer value to be assigned. Default is 0.
    IntValue(int v);

    /// @brief Constructor.
    /// @param v The integer value to be assigned. Default is 0.
    IntValue(unsigned v);

    /// @brief Destructor.
    virtual ~IntValue();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the integer value.
    /// @return The integer value.
    long long getValue() const;

    /// @brief Sets the integer value.
    /// @param a The integer value to be set.
    void setValue(long long a);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    /// @brief The actual integer value.
    long long _value;
};

} // namespace hif
