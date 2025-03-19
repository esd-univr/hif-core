/// @file HifNodeRef.cpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#include "hif/HifNodeRef.hpp"
#include "hif/application_utils/Log.hpp"

namespace hif
{

HifNodeRef::HifNodeRef(hif::Object *o)
    : ref()
{
    // Property used to simplify debug.
    ref.addProperty("HifNodeRef");
    o->replace(&ref);
}

HifNodeRef::~HifNodeRef()
{
    messageDebugAssert(
        ref.getParent() == nullptr, "Found a HifNodeRef not replaced back in the tree.", nullptr, nullptr);
}

bool HifNodeRef::replace(hif::Object *o) { return ref.replace(o); }

} // namespace hif
