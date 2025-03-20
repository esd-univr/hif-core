/// @file DeclarationsStack.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{
namespace semantics
{

/// @brief This class represents a stack of declarations.
/// It works as a data structure to store the current snapshot
/// of declarations.
class DeclarationsStack
{
public:
    typedef Object Symbol;
    typedef std::map<Symbol *, Declaration *> DeclarationsMap;
    typedef std::list<DeclarationsMap> Stack;

    /// @brief Constructor.
    /// @param root The root subtree from which to retrieve the snapshot of declarations.
    DeclarationsStack(Object *root);
    ~DeclarationsStack();

    /// @brief Inserts a snapshot of declarations.
    void push();

    /// @brief Removes a snapshot of declarations by restoring it.
    void pop();

private:
    Stack _stack;
    Object *_root;

    DeclarationsStack(const DeclarationsStack &);
    DeclarationsStack &operator=(const DeclarationsStack &);
};

} // namespace semantics
} // namespace hif
