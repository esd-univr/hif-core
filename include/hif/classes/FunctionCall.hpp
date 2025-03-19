/// @file FunctionCall.hpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Value.hpp"
#include "hif/classes/forwards.hpp"
#include "hif/features/INamedObject.hpp"
#include "hif/features/ISymbol.hpp"

namespace hif
{

///	@brief Call to a function.
///
/// @details
/// This class represents a call to a function.
/// The type associated to a function call is the return type of
/// the function in the corresponding declaration.
///
/// @see Function
class FunctionCall : public Value, public features::TemplateSymbolIf<Function>, public features::INamedObject
{
public:
    /// @brief Map to native call return type.
    typedef Value CallType;

    /// @brief List of template arguments (i.e., assignments to template formal function parameters).
    BList<TPAssign> templateParameterAssigns;

    /// @brief List of arguments (i.e., assignment to formal function parameters).
    BList<ParameterAssign> parameterAssigns;

    /// @brief Constructor.
    FunctionCall();

    /// @brief Destructor.
    virtual ~FunctionCall();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Sets the calling object for the function call.
    /// @param v The new calling object to be set.
    /// @return The old calling object.
    Value *setInstance(Value *v);

    /// @brief Returns the calling object for the function call.
    /// @return The calling object.
    Value *getInstance() const;

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

    /// @brief Returns this object as hif::Object.
    /// @return This object as hif::Object.
    virtual Object *toObject();

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given child w.r.t. this.
    virtual std::string _getFieldName(const Object *child) const;

    /// @brief Returns the name of given BList w.r.t. this.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    ///@brief The calling object.
    Value *_instance;

    // K: disabled
    FunctionCall(const FunctionCall &);
    FunctionCall &operator=(const FunctionCall &);
};

} // namespace hif
