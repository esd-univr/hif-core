/// @file Procedure.hpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/SubProgram.hpp"

namespace hif
{

///	@brief Procedure declaration.
///
/// @details
/// This class represents the declaration of a procedure.
class Procedure : public SubProgram
{
public:
    /// @brief Constructor.
    Procedure();

    /// @brief Destructor.
    virtual ~Procedure();

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
    Procedure(const Procedure &);
    Procedure &operator=(const Procedure &);
};

} // namespace hif
