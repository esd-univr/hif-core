/// @file isSubNode.hpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief Checks whether the passed object is in the subtree of @p parent.
/// If @p obj is @p parent, it returns <tt>true</tt>.
/// @param obj The passed object.
/// @param parent The parent tree.
/// @param matchStarting If <tt>true</tt> returns true also if @p obj is @p parent.
/// Default is <tt>true</tt>.
/// @return <tt>true</tt> if the passed object is in the sub tree of parent.
///

bool isSubNode(Object *obj, Object *parent, const bool matchStarting = true);

/// @brief Checks whether the passed object is in the subtree of @p parentList.
/// @param obj The passed object.
/// @param parentList The parent list.
/// @param matchStarting If <tt>true</tt> returns true also if @p obj is @p parent.
/// Default is <tt>true</tt>.
/// @return <tt>true</tt> if the passed object is in the sub tree of parentList.
///

bool isSubNode(Object *obj, BList<Object> &parentList, const bool matchStarting = true);

/// @brief Checks whether the passed object is in the subtree of @p parentList.
/// @param obj The passed object.
/// @param parentList The parent list.
/// @param matchStarting If <tt>true</tt> returns true also if @p obj is @p parent.
/// Default is <tt>true</tt>.
/// @return <tt>true</tt> if the passed object is in the sub tree of parentList.
///
template <typename T>
bool isSubNode(Object *obj, BList<T> &parentList, const bool matchStarting = true);

} // namespace hif
