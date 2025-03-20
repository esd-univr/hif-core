/// @file declarationUtils.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

// Local includes
#include "hif/classes/classes.hpp"
#include "hif/semantics/DeclarationOptions.hpp"
#include "hif/semantics/HIFSemantics.hpp"

namespace hif
{
namespace semantics
{

/// @name Retrieval of declaration.
/// Methods that help to get declaration objects.
///
///	@{

/// @brief Finds the declaration of a given object @p o starting from
/// @p location and potentially moving up the Hif tree.
/// This function works only with object that have a declaration member
/// (for example function calls, identifiers, etc.).
/// First of all, the function tries to find the declaration by reading
/// the delcaration member of the object. If this is not already set,
/// then the function tries to find declarations going up in scopes
/// starting from @p location.
/// If the declaration is found, the function sets the declaration object
/// member of @p o.
///
/// @param o The object of which the declaration is to be found.
/// @param sem The reference semantics. Default is HIF semantics.
/// @param opt The options.
/// @return The declaration if it exists, nullptr otherwise.
///

Declaration *getDeclaration(
    Object *o,
    hif::semantics::ILanguageSemantics *sem,
    const DeclarationOptions &opt = DeclarationOptions());
/// @brief Finds the declaration of a given object @p o starting from
/// @p location and potentially moving up the Hif tree.
/// This function works only with object that have a declaration member
/// (for example function calls, identifiers, etc.).
/// First of all, the function tries to find the declaration by reading
/// the declaration member of the object. If this is not already set,
/// then the function tries to find declarations going up in scopes
/// starting from @p location.
/// If the declaration is found, the function sets the declaration object
/// member of @p o.
///
/// @param o The object of which the declaration is to be found.
/// @param sem The reference semantics. Default is HIF semantics.
/// @param opt The options.
/// @return The declaration if it exists, nullptr otherwise.
///
template <typename T>
typename T::DeclarationType *
getDeclaration(T *o, hif::semantics::ILanguageSemantics *sem, const DeclarationOptions &opt = DeclarationOptions());

/// @brief Options related to getCandidates() method.
struct GetCandidatesOptions : public DeclarationOptions {
    GetCandidatesOptions();
    virtual ~GetCandidatesOptions();

    GetCandidatesOptions(const GetCandidatesOptions &other);
    GetCandidatesOptions(const DeclarationOptions &other);
    GetCandidatesOptions &operator=(DeclarationOptions other);
    GetCandidatesOptions &operator=(GetCandidatesOptions other);
    void swap(DeclarationOptions &other);
    void swap(GetCandidatesOptions &other);

    /// @brief If <tt>true</tt> returns all possible declarations,
    /// without checking their type.
    /// Default: False
    bool getAll;

    /// @brief If <tt>true</tt> and all the candidates have been
    /// discarded, the function will return the best candidate anyway.
    /// It is useful when the Hif tree is highly unstable.
    /// Default: False
    bool atLeastOne;

    /// @brief If <tt>true</tt>, get only assignable candidates.
    /// Default: False
    bool getAllAssignables;
};

/// @brief Returns all the declarations of a symbol visible
/// from a given object @p o.
/// If the declaration member of @p o is set, then only such
/// a declaration is returned.
///
/// @param list The list where to store declarations.
/// @param o The object of which the declaration is required.
/// @param sem The reference semantics.
/// @param opt The given options.
///

void getCandidates(
    std::list<Declaration *> &list,
    Object *o,
    hif::semantics::ILanguageSemantics *sem,
    const GetCandidatesOptions &opt = GetCandidatesOptions());
/// @brief Returns all the declarations of a symbol visible
/// from a given object @p o.
/// If the declaration member of @p o is set, then only such
/// a declaration is returned.
///
/// @param list The list where to store declarations.
/// @param o The object of which the declaration is required.
/// @param refSem The reference semantics.
/// @param opt The given options.
///
template <typename T>
void getCandidates(
    std::list<typename T::DeclarationType *> &list,
    T *o,
    hif::semantics::ILanguageSemantics *refSem,
    const GetCandidatesOptions &opt = GetCandidatesOptions());

} // namespace semantics
} // namespace hif
