/// @file IFeature.cpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#include "hif/features/IFeature.hpp"
#include "hif/application_utils/Log.hpp"

namespace hif
{
namespace features
{

IFeature::IFeature()
{
    // ntd
}

IFeature::~IFeature()
{
    // ntd
}

IFeature::IFeature(const hif::features::IFeature & /*other*/)
{
    // ntd
}

hif::features::IFeature &IFeature::operator=(const hif::features::IFeature &other)
{
    if (this == &other)
        return *this;
    return *this;
}

} // namespace features
} // namespace hif
