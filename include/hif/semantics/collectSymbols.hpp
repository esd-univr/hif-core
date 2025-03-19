/// @file collectSymbols.hpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/semantics/HIFSemantics.hpp"

namespace hif
{
namespace semantics
{

typedef std::list<Object *> SymbolList;

/// @brief Collects all symbols (i.e., all objects having a declaration member)
/// starting from @p root.
///
/// @param list The result list where to store all symbols.
/// @param root The starting root object.
/// @param sem The reference semantics.
/// @param skipStandardDeclarations If <tt>true</tt> skip standard declarations
/// and declarations inside standard LibraryDefs. default = false
///

void collectSymbols(
    SymbolList &list,
    Object *root,
    ILanguageSemantics *sem             = hif::semantics::HIFSemantics::getInstance(),
    const bool skipStandardDeclarations = false);

} // namespace semantics
} // namespace hif
