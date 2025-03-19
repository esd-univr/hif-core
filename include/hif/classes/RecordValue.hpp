/// @file RecordValue.hpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Value.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

///	@brief Record value constant.
///
/// @details
/// This class represents constant values of record types.
///
/// @see Record, RecordValueAlt
class RecordValue : public Value
{
public:
    /// @brief List of  RecordValueAlt to describe the values of the fields
    /// in the constant.
    BList<RecordValueAlt> alts;

    /// @brief Constructor.
    RecordValue();

    /// @brief Destructor.
    virtual ~RecordValue();

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

    /// @brief Returns the name of given BList w.r.t. this.
    virtual std::string _getBListName(const BList<Object> &list) const;
};

} // namespace hif
