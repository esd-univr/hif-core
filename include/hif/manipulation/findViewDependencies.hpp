/// @file findViewDependencies.hpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Options for findViewDependencies
struct ViewDependencyOptions {
    ViewDependencyOptions();
    ~ViewDependencyOptions();
    ViewDependencyOptions(const ViewDependencyOptions &other);
    ViewDependencyOptions &operator=(ViewDependencyOptions other);
    void swap(ViewDependencyOptions &other);

    /// @brief If true skip standard views. Default false.
    bool skipStandardViews;
    /// @brief If true skip standard library. Default true.
    bool skipStandardLibraries;
    /// @brief If true skips RTL dependencies. Default false.
    bool skipRtlDependencies;
    /// @brief If true skips TLM dependencies. Default false.
    bool skipTlmDependencies;
    /// @brief If true skips C++ dependencies. Default false.
    bool skipCppDependencies;
    /// @brief If true skips C dependencies. Default false.
    bool skipCDependencies;
    /// @brief If true skips PSL dependencies. Default false.
    bool skipPslDependencies;
};

typedef std::set<View *> ViewDependenciesSet;
typedef std::map<View *, ViewDependenciesSet> ViewDependenciesMap;

/// @brief Visits the object @p obj to determine dependencies between the
/// views of design units (i.e., which view is sub-module of another one,
/// and which view is parent module of another one).
/// It is useful to understand the hierarchy between views in a system
/// description.
///
/// @param obj The root of the tree (usually the system object).
/// @param smm The map storing pointers to sub-module views.
/// @param pmm The map storing pointers to parent-module views.
/// @param sem The reference semantics.
/// @param opt The options.
///

void findViewDependencies(
    hif::Object *obj,
    ViewDependenciesMap &smm,
    ViewDependenciesMap &pmm,
    semantics::ILanguageSemantics *sem,
    const ViewDependencyOptions &opt = ViewDependencyOptions());

} // namespace manipulation
} // namespace hif
