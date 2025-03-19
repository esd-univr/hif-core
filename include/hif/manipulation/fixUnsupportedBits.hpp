/// @file fixUnsupportedBits.hpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Options for fixUnsupportedBits.
struct FixUnsupportedBitsOptions {
    FixUnsupportedBitsOptions();
    ~FixUnsupportedBitsOptions();
    FixUnsupportedBitsOptions(const FixUnsupportedBitsOptions &other);
    FixUnsupportedBitsOptions &operator=(FixUnsupportedBitsOptions other);
    void swap(FixUnsupportedBitsOptions &other);

    /// @brief Transform all logic values to two-logic values.
    /// Default is false.
    bool onlyBinaryBits;
    /// @brief In case of onlyBinaryBits, the replace value for logic bits.
    /// Default is '0'.
    char xzReplaceValue;
    /// @brief When set, do not fix initial values. Default is false.
    bool skipInitialValues;
};

/// @brief Fix checkSem unsupported bits, by removing them and
/// by replacing them with the most compatible alternative.
///
/// @param root The root of manipulation.
/// @param sem The current tree semantics.
/// @param checkSem The reference semantics for the checks on bits.
/// @param opts The options.
/// @return True when at least one fix has been performed.
///

bool fixUnsupportedBits(
    Object *root,
    hif::semantics::ILanguageSemantics *sem,
    hif::semantics::ILanguageSemantics *checkSem,
    const FixUnsupportedBitsOptions &opts = FixUnsupportedBitsOptions());

} // namespace manipulation
} // namespace hif
