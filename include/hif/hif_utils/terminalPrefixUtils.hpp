/// @file terminalPrefixUtils.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief Options for the <tt>getTerminalPrefix</tt> function.
/// recurseInto* : True if the search must go into prefix of corresponding object.
/// e.g. with recurseIntoFieldRefs = false:
/// struct.field[index] -> return field, otherwise return struct.
///
/// Every member carries a default member initializer, so the documented
/// defaults below hold for each of the three ways this struct is built: the
/// default argument of getTerminalPrefix/setTerminalPrefix, a value-initialized
/// object, and a plain default-initialized local that then overrides only the
/// members it cares about. That last form is the common one at call sites, and
/// it is why the initializers are required rather than merely tidy: without
/// them the members a call site does not assign are indeterminate.
struct TerminalPrefixOptions {
    /// Recurse within field references. Default is true.
    bool recurseIntoFieldRefs{true};
    /// Recurse within member objects. Default is true.
    bool recurseIntoMembers{true};
    /// Recurse within slice objects. Default is true.
    bool recurseIntoSlices{true};
    /// Recurse within expressions having deref as operator. Default is false.
    bool recurseIntoDerefExpressions{false};
};

/// @brief This function returns the prefix of the given object.
/// For example with memb1[ memb2[ ] ].field, return memb1.
/// This function works with members, slices and field references.
/// @param obj The object from which the search will start.
/// @param opt The search options.
/// @return The terminal prefix if found, nullptr otherwise.
///

Value *getTerminalPrefix(Value *obj, const TerminalPrefixOptions &opt = TerminalPrefixOptions());

/// @brief This function sets the new prefix of a value.
/// @param obj The object on which a new prefix is to be set.
/// @param newPrefix The new prefix.
/// @param opt The options.
/// @return The old prefix.
///

Value *setTerminalPrefix(Value *obj, Value *newPrefix, const TerminalPrefixOptions &opt = TerminalPrefixOptions());

} // namespace hif
