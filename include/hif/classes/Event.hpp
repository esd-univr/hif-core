/// @file Event.hpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/SimpleType.hpp"

namespace hif
{

///	@brief Event type.
///
/// @details
/// This class represents the event type.
class Event : public SimpleType
{
public:
    /// @brief Constructor.
    Event();

    /// @brief Destructor.
    virtual ~Event();

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
};

} // namespace hif
