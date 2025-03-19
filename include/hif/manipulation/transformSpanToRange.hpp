/// @file transformSpanToRange.hpp
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

/// @brief Transforms a span into the corresponding range.
///
/// @param r The span to be converted.
/// @param refLang The reference semantics.
/// @param isSigned <tt>true</tt> if the values represented by the span
/// returned are signed, <tt>false</tt> if they are unsigned.
/// @return The corresponding range.
///

Range *transformSpanToRange(Range *r, hif::semantics::ILanguageSemantics *refLang, const bool isSigned = true);

} // namespace manipulation
} // namespace hif
