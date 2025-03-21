/// @file Reference.hpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/CompositeType.hpp"

namespace hif
{

/// @brief Reference (in C++).
///
/// @details
/// This class represents a C++ reference.
class Reference : public CompositeType
{
public:
    /// @brief Constructor.
    Reference();

    /// @brief Destructor.
    virtual ~Reference();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    // K: disabled
    Reference(const Reference &);
    Reference &operator=(const Reference &);
};

} // namespace hif
