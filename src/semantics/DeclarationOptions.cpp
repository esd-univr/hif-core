/// @file DeclarationOptions.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/DeclarationOptions.hpp"

#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace semantics
{

namespace /*anon*/
{

} // namespace

DeclarationOptions::DeclarationOptions()
    : location(nullptr)
    , forceRefresh(false)
    , dontSearch(false)
    , error(false)
    , looseTypeChecks(false)
{
    // ntd
}

DeclarationOptions::~DeclarationOptions()
{
    // ntd
}

DeclarationOptions::DeclarationOptions(const DeclarationOptions &other)
    : location(other.location)
    , forceRefresh(other.forceRefresh)
    , dontSearch(other.dontSearch)
    , error(other.error)
    , looseTypeChecks(other.looseTypeChecks)
{
    // ntd
}

DeclarationOptions &DeclarationOptions::operator=(DeclarationOptions other)
{
    swap(other);
    return *this;
}

void DeclarationOptions::swap(DeclarationOptions &other)
{
    std::swap(location, other.location);
    std::swap(forceRefresh, other.forceRefresh);
    std::swap(dontSearch, other.dontSearch);
    std::swap(error, other.error);
    std::swap(looseTypeChecks, other.looseTypeChecks);
}

} // namespace semantics
} // namespace hif
