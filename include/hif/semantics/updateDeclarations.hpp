/// @file updateDeclarations.hpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/semantics/DeclarationOptions.hpp"

namespace hif
{
namespace semantics
{

/// @brief Options related to getCandidates() method.
struct UpdateDeclarationOptions : public DeclarationOptions {
    UpdateDeclarationOptions();
    virtual ~UpdateDeclarationOptions();

    UpdateDeclarationOptions(const UpdateDeclarationOptions &other);
    UpdateDeclarationOptions &operator=(DeclarationOptions other);
    UpdateDeclarationOptions &operator=(UpdateDeclarationOptions other);
    void swap(DeclarationOptions &other);
    void swap(UpdateDeclarationOptions &other);

    /// @brief If a declaration is visible from the current scope, update it.
    /// Other declarations will be kept.
    /// Default is false.
    bool onlyVisible;

    /// @brief If current set declaration is not a subnode of root object,
    /// the declaration will be not updated. Usefull only with onlyVisible option.
    /// Default is <tt>nullptr</tt>.
    Object *root;
};

/// @brief Computes the declaration of objects in the subtree starting from
/// @p root according to provided options.
///
/// @param root The node from which to start the computation.
/// @param sem The reference semantics.
/// @param opt The options.
///

void updateDeclarations(
    Object *root,
    ILanguageSemantics *sem,
    const UpdateDeclarationOptions &opt = UpdateDeclarationOptions());

} // namespace semantics
} // namespace hif
