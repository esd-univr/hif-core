/// @file getNearestParent.cpp
/// @brief
/// @copyright (c) 2024-2025 Electronic Systems Design (ESD) Lab @ UniVR This
/// file is distributed under the BSD 2-Clause License. See LICENSE.md for
/// details.

#include "hif/hif_utils/getNearestParent.hpp"

#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{

namespace /*anon*/
{

} // namespace

template <typename T>
T *getNearestParent(Object *o, const bool matchStarting)
{
    if (o == nullptr) {
        assert(false);
        return nullptr;
    }

    Object *object = o;
    if (!matchStarting)
        object = o->getParent();
    while (object != nullptr) {
        if (dynamic_cast<T *>(object))
            break;
        object = object->getParent();
    }

    return static_cast<T *>(object);
}

/// @brief Defines a template method for retrieving the nearest parent object of a specific type.
#define HIF_TEMPLATE_METHOD(T) T *getNearestParent<T>(Object *, const bool)

HIF_INSTANTIATE_METHOD()

#undef HIF_TEMPLATE_METHOD

} // namespace hif
