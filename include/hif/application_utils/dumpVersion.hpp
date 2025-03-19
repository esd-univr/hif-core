/// @file dumpVersion.hpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#pragma once

#include <string>

#include "hif/application_utils/portability.hpp"

namespace hif
{
namespace application_utils
{

/// @brief Returns the current HIF tag.

auto getHIFSuiteVersion() -> std::string;

/// @brief prints a banner with current HIFSuite infos.

void dumpVersion(std::string const &tool_name);

} // namespace application_utils
} // namespace hif
